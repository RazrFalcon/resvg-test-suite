#include <QMessageBox>
#include <QTimer>
#include <QFileInfo>
#include <QFileDialog>
#include <QDir>
#include <QScreen>
#include <QXmlStreamReader>
#include <QSortFilterProxyModel>
#include <QStandardItemModel>
#include <QDebug>

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

    m_settings = Settings::load();

    m_render.loadSettings(m_settings);
    m_render.setScale(qApp->screens().first()->devicePixelRatio());

    prepareBackends();

    adjustSize();

    connect(&m_render, &Render::imageReady, this, &MainWindow::onImageReady);
    connect(&m_render, &Render::diffReady, this, &MainWindow::onDiffReady);
    connect(&m_render, &Render::diffStats, this, &MainWindow::onDiffStats);
    connect(&m_render, &Render::warning, this, &MainWindow::onRenderWarning);
    connect(&m_render, &Render::error, this, &MainWindow::onRenderError);
    connect(&m_render, &Render::finished, this, &MainWindow::onRenderFinished);

    auto *model = new QStandardItemModel(0, 1);
    auto *sortMmodel = new QSortFilterProxyModel();
    sortMmodel->setSourceModel(model);
    ui->cmbBoxFiles->setModel(sortMmodel);

    // TODO: this
    ui->cmbBoxNames->hide();

    // TODO: check that convertors exists

    QTimer::singleShot(5, this, &MainWindow::onStart);
}

MainWindow::~MainWindow()
{
    m_tests.save(m_settings.resultsPath());

    delete ui;
}

void MainWindow::prepareBackends()
{
    const QVector<Backend> backends = {
        Backend::Chrome,
        Backend::ResvgCairo,
        Backend::ResvgQt,
        Backend::Inkscape,
        Backend::librsvg,
        Backend::QtSvg,
    };

    for (const Backend backend : backends) {
        auto w = new BackendWidget(backend);
        w->setTitle(Render::backendName(backend));
        m_backendWidges.insert(backend, w);

        ui->layBackends->addWidget(w);
    }

    m_backendWidges.value(Backend::Chrome)->setDiffVisible(false);
    m_backendWidges.value(Backend::ResvgQt)->setTestStateVisible(false);
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

    ui->cmbBoxNames->blockSignals(true);
    ui->cmbBoxNames->clear();

    try {
        m_tests = Tests::load(m_settings.resultsPath());
    } catch (const QString &msg) {
        QMessageBox::critical(this, "Error", msg);
        qApp->quit();
    }


    QSet<QString> namesSet;
    int idx = 0;
    for (const auto &item : m_tests) {
        ui->cmbBoxFiles->addItem(item.fileName, idx);
        namesSet.insert(item.name);
        idx++;
    }

    QStringList namesList = namesSet.toList();
    namesList.sort();
    ui->cmbBoxNames->addItem("");
    ui->cmbBoxNames->addItems(namesList);

    if (ui->cmbBoxFiles->count() != 0) {
        loadImage(ui->cmbBoxFiles->currentText() + ".svg");
    }

    ui->cmbBoxFiles->blockSignals(false);
    ui->cmbBoxFiles->setFocus();

    ui->cmbBoxNames->blockSignals(false);
}

void MainWindow::on_cmbBoxFiles_currentIndexChanged(int)
{
    loadImage(ui->cmbBoxFiles->currentText() + ".svg");
}

void MainWindow::loadImage(const QString &fileName)
{
    const auto path = m_settings.testPath(fileName);

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
        const auto idx = ui->cmbBoxFiles->currentData().toUInt();
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
        const auto idx = ui->cmbBoxFiles->currentData().toUInt();
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
    SettingsDialog diag(this);
    if (diag.exec()) {
        const auto oldTs = m_settings.testSuite;

        m_settings = Settings::load();
        m_render.loadSettings(m_settings);

        if (m_settings.testSuite != oldTs) {
            loadImageList();
        }
    }
}

void MainWindow::on_cmbBoxNames_currentIndexChanged(const QString &text)
{
    auto *model = static_cast<QSortFilterProxyModel*>(ui->cmbBoxFiles->model());
    model->setFilterFixedString(text);
}
