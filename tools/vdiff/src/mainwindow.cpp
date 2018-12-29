#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QPainter>
#include <QScreen>
#include <QScrollBar>
#include <QShortcut>
#include <QTimer>

#include "backendwidget.h"
#include "paths.h"
#include "process.h"
#include "settingsdialog.h"

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_autosaveTimer(new QTimer(this))
{
    ui->setupUi(this);

    m_settings.load();

    m_render.setSettings(&m_settings);
    m_render.setScale(qApp->screens().first()->devicePixelRatio());

    prepareBackends();

    connect(&m_render, &Render::imageReady, this, &MainWindow::onImageReady);
    connect(&m_render, &Render::diffReady, this, &MainWindow::onDiffReady);
    connect(&m_render, &Render::finished, this, &MainWindow::onRenderFinished);

    connect(m_autosaveTimer, &QTimer::timeout, this, &MainWindow::save);
    m_autosaveTimer->setInterval(10000); // 10 sec
    m_autosaveTimer->start();

    auto shortcut = new QShortcut(QKeySequence("Ctrl+R"), this);
    connect(shortcut, &QShortcut::activated, [this]() {
        const auto idx = ui->cmbBoxFiles->currentIndex();
        if (idx >= 0) {
            loadTest(idx);
        }
    });

    // TODO: check that convertors exists

    QTimer::singleShot(5, this, &MainWindow::onStart);
}

MainWindow::~MainWindow()
{
    save();

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
        Backend::ResvgCairo,
        Backend::ResvgQt,
    };

    if (m_settings.testSuite != TestSuite::Custom) {
        backends.prepend(Backend::Reference);
    }

    if (m_settings.useChrome) {
        backends << Backend::Chrome;
    }

    if (m_settings.useFirefox) {
        backends << Backend::Firefox;
    }

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
        w->setTitle(backendToString(backend));
        w->setViewSize(QSize(m_settings.viewSize, m_settings.viewSize));
        connect(w, &BackendWidget::testStateChanged, this, &MainWindow::updatePassFlags);
        m_backendWidges.insert(backend, w);

        ui->layBackends->addWidget(w);
    }

    if (m_settings.testSuite != TestSuite::Custom) {
        m_backendWidges.value(Backend::Reference)->setDiffVisible(false);
        m_backendWidges.value(Backend::Reference)->setTestStateVisible(false);
    }

    m_backendWidges.value(Backend::ResvgQt)->setTestStateVisible(false);

    if (m_settings.testSuite == TestSuite::Custom) {
        for (auto *w : m_backendWidges.values()) {
            w->setTestStateVisible(false);
        }
    }

    ui->btnSync->setVisible(m_settings.testSuite == TestSuite::Own);

    QTimer::singleShot(50, this, [this](){
        ui->scrollAreaWidgetContents->adjustSize();

        const auto w = (m_settings.viewSize + ui->layBackends->spacing())
                       * qMin(6.5, (double)m_backendWidges.size());
        ui->scrollArea->setMinimumWidth(w);
        ui->scrollArea->setMinimumHeight(  ui->scrollAreaWidgetContents->height()
                                         + ui->scrollArea->horizontalScrollBar()->height() + 5);

        adjustSize();
        QTimer::singleShot(50, this, [this](){
            adjustSize();
        });
    });
}

void MainWindow::setGuiEnabled(bool flag)
{
    ui->btnSettings->setEnabled(flag);
    ui->btnSync->setEnabled(flag);
    ui->btnPrint->setEnabled(flag);
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
            m_tests = Tests::load(m_settings.testSuite, m_settings.resultsPath(),
                                  m_settings.testsPath());
        }
    } catch (const QString &msg) {
        QMessageBox::critical(this, "Error", msg);
        qApp->quit();
    }


    for (const TestItem &item : m_tests) {
        QString title = item.baseName;
        if (m_settings.testSuite == TestSuite::Own) {
            title += " - " + item.title;
            title.replace('`', '\'');
        }

        ui->cmbBoxFiles->addItem(title);
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

void MainWindow::save()
{
    if (m_settings.testSuite != TestSuite::Custom) {
        m_tests.save(m_settings.resultsPath());
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

void MainWindow::onRenderFinished()
{
    setGuiEnabled(true);
    ui->cmbBoxFiles->setFocus();

    setAnimationEnabled(false);
}

void MainWindow::on_btnSync_clicked()
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
        QMessageBox::critical(this, "Error", msg);
    }
}

void MainWindow::on_btnSettings_clicked()
{
    // Save in case of any changes.
    save();

    SettingsDialog diag(&m_settings, this);
    if (diag.exec()) {
        m_autosaveTimer->stop();

        m_render.setScale(qApp->screens().first()->devicePixelRatio());

        for (auto *w : m_backendWidges.values()) {
            w->setViewSize(QSize(m_settings.viewSize, m_settings.viewSize));
        }

        prepareBackends();
        loadImageList();
        m_autosaveTimer->start();
    }
}

void MainWindow::on_btnPrint_clicked()
{
    const auto order = {
        Backend::ResvgCairo,
        Backend::ResvgQt,
        Backend::Chrome,
        Backend::Firefox,
        Backend::Batik,
        Backend::Inkscape,
        Backend::Librsvg,
        Backend::QtSvg,
    };

    const int backend = m_backendWidges.size() - 1; // -Reference
    const int scale = (int)qApp->screens().first()->devicePixelRatio();
    const int titleHeight = 20;
    const int spacing = 5;
    const int fullWidth = m_settings.viewSize * backend + spacing * (backend + 1);
    const int fullHeight = titleHeight + m_settings.viewSize + spacing * 2;

    QImage image(fullWidth * scale, fullHeight * scale, QImage::Format_ARGB32);
    image.fill(Qt::white);
    image.setDevicePixelRatio(scale);

    QPainter p(&image);
    p.setFont(QFont("Arial", 12));

    int x = spacing;
    int y = spacing;
    for (const auto backend : order) {
        if (!m_backendWidges.contains(backend)) {
            continue;
        }

        auto w = m_backendWidges.value(backend);

        const auto textRect = QRect(x, y, m_settings.viewSize, titleHeight - 3);
        p.drawText(textRect, Qt::AlignCenter, w->title());

        auto img = w->image();
        p.drawImage(x, y + titleHeight, img);

        x += m_settings.viewSize + spacing;
    }

    p.end();

    const auto path = QFileDialog::getSaveFileName(this, tr("Save As"),
                                                   QDir::homePath(), tr("Images (*.png)"));
    if (!path.isEmpty()) {
        image.save(path);
    }
}
