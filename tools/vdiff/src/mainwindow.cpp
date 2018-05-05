#include <QMessageBox>
#include <QTimer>
#include <QFileInfo>
#include <QFileDialog>
#include <QDir>
#include <QScreen>
#include <QXmlStreamReader>
#include <QSortFilterProxyModel>
#include <QStandardItemModel>

#include "paths.h"
#include "settingsdialog.h"
#include "process.h"
#include "backendwidget.h"

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_settings.load();

    m_render.setSettings(&m_settings);
    m_render.setScale(qApp->screens().first()->devicePixelRatio());

    prepareBackends();

    connect(&m_render, &Render::imageReady, this, &MainWindow::onImageReady);
    connect(&m_render, &Render::diffReady, this, &MainWindow::onDiffReady);
    connect(&m_render, &Render::diffStats, this, &MainWindow::onDiffStats);
    connect(&m_render, &Render::warning, this, &MainWindow::onRenderWarning);
    connect(&m_render, &Render::error, this, &MainWindow::onRenderError);
    connect(&m_render, &Render::finished, this, &MainWindow::onRenderFinished);

    // TODO: check that convertors exists

    QTimer::singleShot(5, this, &MainWindow::onStart);
}

MainWindow::~MainWindow()
{
    if (m_settings.testSuite != TestSuite::Custom) {
        m_tests.save(m_settings.resultsPath());
    }

    delete ui;
}

void MainWindow::prepareBackends()
{
    while (ui->layBackends->count() > 0) {
        auto item = ui->layBackends->takeAt(0);
        delete item->widget();
        delete item;
    }
    m_backendWidges.clear();

    QVector<Backend> backends = {
        Backend::Chrome,
        Backend::ResvgCairo,
        Backend::ResvgQt,
    };

    if (m_settings.useBatik) {
        backends << Backend::Batik;
    }

    if (m_settings.useInkscape) {
        backends << Backend::Inkscape;
    }

    if (m_settings.useLibrsvg) {
        backends << Backend::Librsvg;
    }

    if (m_settings.useQtSvg) {
        backends << Backend::QtSvg;
    }


    for (const Backend backend : backends) {
        auto w = new BackendWidget(backend);
        w->setTitle(Render::backendName(backend));
        w->setViewSize(QSize(m_settings.viewSize, m_settings.viewSize));
        connect(w, &BackendWidget::testStateChanged, this, &MainWindow::updatePassFlags);
        m_backendWidges.insert(backend, w);

        ui->layBackends->addWidget(w);
    }

    m_backendWidges.value(Backend::Chrome)->setDiffVisible(false);
    m_backendWidges.value(Backend::ResvgQt)->setTestStateVisible(false);

    if (m_settings.testSuite == TestSuite::Custom) {
        for (auto *w : m_backendWidges.values()) {
            w->setTestStateVisible(false);
        }
    }

    ui->btnResync->setVisible(m_settings.testSuite != TestSuite::Custom);

    QTimer::singleShot(50, this, [this](){ adjustSize(); });
}

void MainWindow::setGuiEnabled(bool flag)
{
    ui->btnSettings->setEnabled(flag);
    ui->cmbBoxFiles->setEnabled(flag);
    for (auto *w : m_backendWidges.values()) {
        w->setEnabled(flag);
    }
}

void MainWindow::onStart()
{
    loadImageList();
}

void MainWindow::loadImageList()
{
    ui->cmbBoxFiles->blockSignals(true);
    ui->cmbBoxFiles->clear();

    try {
        if (m_settings.testSuite == TestSuite::Custom) {
            m_tests = Tests::loadCustom(m_settings.customTestsPath);
        } else {
            m_tests = Tests::load(m_settings.resultsPath(), m_settings.testsPath());
        }
    } catch (const QString &msg) {
        QMessageBox::critical(this, "Error", msg);
        qApp->quit();
    }


    for (const TestItem &item : m_tests) {
        ui->cmbBoxFiles->addItem(item.baseName);
    }

    if (ui->cmbBoxFiles->count() != 0) {
        loadTest(0);
    }

    ui->cmbBoxFiles->blockSignals(false);
    ui->cmbBoxFiles->setFocus();
}

void MainWindow::on_cmbBoxFiles_currentIndexChanged(int idx)
{
    loadTest(idx);
}

void MainWindow::loadTest(const int idx)
{
    const auto path = m_tests.at(idx).path;

    setAnimationEnabled(true);
    getTitleAndDesc(path);
    resetImages();
    fillChBoxes();

    m_render.render(path);

    setGuiEnabled(false);
}

void MainWindow::setAnimationEnabled(bool flag)
{
    for (auto *w : m_backendWidges.values()) {
        w->setAnimationEnabled(flag);
    }
}

void MainWindow::getTitleAndDesc(const QString &path)
{
    ui->lblTitle->clear();

    if (m_settings.testSuite != TestSuite::Own) {
        return;
    }

    QFile file(path);
    if (!file.open(QFile::ReadOnly)) {
        qWarning() << "failed to open:" << path;
        return;
    }

    QString title;

    QXmlStreamReader reader(&file);
    while (!reader.atEnd() && !reader.hasError()) {
        if (reader.readNextStartElement()) {
            if (reader.name() == "title") {
                reader.readNext();
                title += reader.text().toString();
            }

            if (reader.name() == "desc") {
                reader.readNext();

                title += ". " + reader.text().toString();
            }
        }
    }

    title += '.';

    ui->lblTitle->setText(title);
}

void MainWindow::fillChBoxes()
{
    try {
        const auto idx = ui->cmbBoxFiles->currentIndex();
        const auto &item = m_tests.at(idx);

        for (auto *w : m_backendWidges.values()) {
            w->setTestState(item.state.value(w->backend()));
        }
    } catch (const QString &msg) {
        QMessageBox::critical(this, "Error", msg);
    }
}

void MainWindow::updatePassFlags()
{
    try {
        const auto idx = ui->cmbBoxFiles->currentIndex();
        auto &item = m_tests.at(idx);

        for (auto *w : m_backendWidges.values()) {
            item.state.insert(w->backend(), w->testState());
        }
    } catch (const QString &msg) {
        QMessageBox::critical(this, "Error", msg);
    }
}

void MainWindow::resetImages()
{
    for (auto *w : m_backendWidges.values()) {
        w->resetImages();
    }
}

void MainWindow::onImageReady(const Backend type, const QImage &img)
{
    Q_ASSERT(!img.isNull());

    const auto view = m_backendWidges.value(type);
    view->setImage(img);
}

void MainWindow::onDiffReady(const Backend type, const QImage &img)
{
    const auto view = m_backendWidges.value(type);
    view->setDiffImage(img);
}

void MainWindow::onDiffStats(const Backend type, const uint value, const float percent)
{
    const auto view = m_backendWidges.value(type);
    view->setDiffStats(value, percent);
}

void MainWindow::onRenderWarning(const QString &msg)
{
    QMessageBox::warning(this, "Warning", msg);
}

void MainWindow::onRenderError(const QString &msg)
{
    QMessageBox::critical(this, "Error", msg);
}

void MainWindow::onRenderFinished()
{
    setGuiEnabled(true);
    ui->cmbBoxFiles->setFocus();

    setAnimationEnabled(false);
}

void MainWindow::on_btnResync_clicked()
{
    if (m_settings.testSuite != TestSuite::Own) {
        QMessageBox::warning(this, "Warning", "The official SVG test suite cannot be resynced.");
        return;
    }

    const auto ans = QMessageBox::question(this, "Resync?", "Reload test files?",
                                           QMessageBox::Yes | QMessageBox::No);

    if (ans != QMessageBox::Yes) {
        return;
    }

    try {
        Tests::resync(m_settings);
        loadImageList();

        QMessageBox::information(this, "Info", "Tests was successfully synced.");
    } catch (const QString &msg) {
        QMessageBox::critical(this, "Error", msg + "\n\nApplication will close now.");
        qApp->quit();
    }
}

void MainWindow::on_btnSettings_clicked()
{
    SettingsDialog diag(&m_settings, this);
    if (diag.exec()) {
        m_render.setScale(qApp->screens().first()->devicePixelRatio());

        for (auto *w : m_backendWidges.values()) {
            w->setViewSize(QSize(m_settings.viewSize, m_settings.viewSize));
        }

        prepareBackends();
        loadImageList();
    }
}
