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
        const auto idx = ui->cmbBoxFiles->currentData().toUInt();
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
