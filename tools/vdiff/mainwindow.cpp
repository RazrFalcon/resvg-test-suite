#include <QMessageBox>
#include <QTimer>
#include <QSettings>
#include <QFileInfo>
#include <QFileDialog>
#include <QDir>
#include <QScreen>
#include <QXmlStreamReader>
#include <QDebug>

#include "paths.h"
#include "settingsdialog.h"
#include "process.h"

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_render.setScale(qApp->screens().first()->devicePixelRatio());

    adjustSize();

    m_imgViews.insert(ImageType::Chrome, ui->imgViewChrome);
    m_imgViews.insert(ImageType::ResvgCairo, ui->imgViewResvgCairo);
    m_imgViews.insert(ImageType::ResvgQt, ui->imgViewResvgQt);
    m_imgViews.insert(ImageType::Inkscape, ui->imgViewInkscape);
    m_imgViews.insert(ImageType::Rsvg, ui->imgViewRsvg);
    m_imgViews.insert(ImageType::QtSvg, ui->imgViewQtSvg);

    m_diffViews.insert(ImageType::ResvgCairo, ui->imgViewResvgCairoDiff);
    m_diffViews.insert(ImageType::ResvgQt, ui->imgViewResvgQtDiff);
    m_diffViews.insert(ImageType::Inkscape, ui->imgViewInkscapeDiff);
    m_diffViews.insert(ImageType::Rsvg, ui->imgViewRsvgDiff);
    m_diffViews.insert(ImageType::QtSvg, ui->imgViewQtSvgDiff);

    m_diffLabels.insert(ImageType::ResvgCairo, ui->lblResvgCairoDiff);
    m_diffLabels.insert(ImageType::ResvgQt, ui->lblResvgQtDiff);
    m_diffLabels.insert(ImageType::Inkscape, ui->lblInkscapeDiff);
    m_diffLabels.insert(ImageType::Rsvg, ui->lblRsvgDiff);
    m_diffLabels.insert(ImageType::QtSvg, ui->lblQtSvgDiff);

    m_flagBoxes = {
        ui->cmbBoxChromeFlag,
        ui->cmbBoxResvgFlag,
        ui->cmbBoxInkscapeFlag,
        ui->cmbBoxLibrsvgFlag,
        ui->cmbBoxQtSvgFlag,
    };

    connect(&m_render, &Render::imageReady, this, &MainWindow::onImageReady);
    connect(&m_render, &Render::diffReady, this, &MainWindow::onDiffReady);
    connect(&m_render, &Render::diffStats, this, &MainWindow::onDiffStats);
    connect(&m_render, &Render::warning, this, &MainWindow::onRenderWarning);
    connect(&m_render, &Render::error, this, &MainWindow::onRenderError);
    connect(&m_render, &Render::finished, this, &MainWindow::onRenderFinished);

    for (QComboBox *cmbBox : m_flagBoxes) {
        cmbBox->addItem(QIcon(":/icons/unknown"), "Unknown");
        cmbBox->addItem(QIcon(":/icons/passed"), "Passed");
        cmbBox->addItem(QIcon(":/icons/failed"), "Failed");
        cmbBox->addItem(QIcon(":/icons/crashed"), "Crashed");

        connect(cmbBox, SIGNAL(activated(int)), this, SLOT(updatePassFlags()));
    }

    initDefaultSettings();

//    try {
//        if (DB::exists()) {
//            DB::open();
//        } else {
//            DB::init();
//        }
//    } catch (const QString &msg) {
//        QMessageBox::critical(this, "Error", msg);
//    }

    // TODO: check that convertors exists

    QTimer::singleShot(5, this, &MainWindow::onStart);
}

MainWindow::~MainWindow()
{
    m_tests.save();

    delete ui;
}

void MainWindow::initDefaultSettings()
{
    QSettings settings;
    if (!settings.contains("ResvgPath")) {
        settings.setValue("ResvgBuild", "debug");
        settings.setValue("InkscapePath", "inkscape");
        settings.setValue("RsvgPath", "rsvg-convert");

        m_render.loadSettings();
    }
}

void MainWindow::setGuiEnabled(bool flag)
{
    ui->btnSettings->setEnabled(flag);
    ui->cmbBoxFiles->setEnabled(flag);
    for (auto *cmbBox : m_flagBoxes) {
        cmbBox->setEnabled(flag);
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
        m_tests = Tests::load();
    } catch (const QString &msg) {
        QMessageBox::critical(this, "Error", msg);
        qApp->quit();
    }


    for (const auto &item : m_tests) {
        ui->cmbBoxFiles->addItem(item.path);
    }

    if (ui->cmbBoxFiles->count() != 0) {
        loadImage(ui->cmbBoxFiles->currentText());
    }

    ui->cmbBoxFiles->blockSignals(false);
    ui->cmbBoxFiles->setFocus();
}

void MainWindow::on_cmbBoxFiles_currentIndexChanged(int)
{
    loadImage(ui->cmbBoxFiles->currentText());
}

void MainWindow::loadImage(const QString &fileName)
{
    const auto path = Paths::testPath(fileName);

    setAnimationEnabled(true);
    getTitleAndDesc(path);
    resetImages();
    fillChBoxes();

    m_render.render(path);

    setGuiEnabled(false);
}

void MainWindow::setAnimationEnabled(bool flag)
{
    for (ImageView *view : m_imgViews) {
        view->setAnimationEnabled(flag);
    }

    for (ImageView *view : m_diffViews) {
        view->setAnimationEnabled(flag);
    }
}

void MainWindow::getTitleAndDesc(const QString &path)
{
    ui->lblTitle->clear();

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

        ui->cmbBoxChromeFlag->setCurrentIndex((int)item.chrome);
        ui->cmbBoxResvgFlag->setCurrentIndex((int)item.resvg);
        ui->cmbBoxInkscapeFlag->setCurrentIndex((int)item.inkscape);
        ui->cmbBoxLibrsvgFlag->setCurrentIndex((int)item.librsvg);
        ui->cmbBoxQtSvgFlag->setCurrentIndex((int)item.qtsvg);
    } catch (const QString &msg) {
        QMessageBox::critical(this, "Error", msg);
    }
}

void MainWindow::updatePassFlags()
{
    try {
        const auto idx = ui->cmbBoxFiles->currentIndex();
        auto &item = m_tests.at(idx);

        item.chrome = (TestState)ui->cmbBoxChromeFlag->currentIndex();
        item.resvg = (TestState)ui->cmbBoxResvgFlag->currentIndex();
        item.inkscape = (TestState)ui->cmbBoxInkscapeFlag->currentIndex();
        item.librsvg = (TestState)ui->cmbBoxLibrsvgFlag->currentIndex();
        item.qtsvg = (TestState)ui->cmbBoxQtSvgFlag->currentIndex();
    } catch (const QString &msg) {
        QMessageBox::critical(this, "Error", msg);
    }
}

void MainWindow::resetImages()
{
    for (ImageView *view : m_imgViews) {
        view->resetImage();
    }

    for (ImageView *view : m_diffViews) {
        view->resetImage();
    }
}

void MainWindow::setDiffText(QLabel *lbl, uint diff, float percent) const
{
    lbl->setText(QString::number(diff) + "/" + QString::number(percent, 'f', 2) + "%");
}

void MainWindow::onImageReady(const ImageType type, const QImage &img)
{
    Q_ASSERT(!img.isNull());

    const auto view = m_imgViews.value(type);
    view->setAnimationEnabled(false);
    view->setImage(img);
}

void MainWindow::onDiffReady(const ImageType type, const QImage &img)
{
    const auto view = m_diffViews.value(type);
    view->setAnimationEnabled(false);
    view->setImage(img);
}

void MainWindow::onDiffStats(const ImageType type, const uint value, const float percent)
{
    setDiffText(m_diffLabels.value(type), value, percent);
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

void MainWindow::on_btnSettings_clicked()
{
    SettingsDialog diag(this);
    if (diag.exec()) {
        m_render.loadSettings();
    }

    if (diag.isResynced()) {
        loadImageList();
    }
}
