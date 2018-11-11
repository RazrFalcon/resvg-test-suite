#include <QButtonGroup>
#include <QFileDialog>
#include <QMessageBox>

#include "settings.h"

#include "settingsdialog.h"
#include "ui_settingsdialog.h"

SettingsDialog::SettingsDialog(Settings *settings, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::SettingsDialog)
    , m_settings(settings)
{
    ui->setupUi(this);

    loadSettings();
    setMinimumWidth(600);
    adjustSize();

    auto suiteGroup = new QButtonGroup(this);
    suiteGroup->addButton(ui->rBtnSuiteResvg);
    suiteGroup->addButton(ui->rBtnSuiteOfficial);
    suiteGroup->addButton(ui->rBtnSuiteCustom);
    connect(suiteGroup, SIGNAL(buttonToggled(QAbstractButton*,bool)),
            this, SLOT(prepareTestsPathWidgets()));

    // TODO: implement
    ui->rBtnSuiteCustom->hide();

    ui->buttonBox->setFocus();
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::loadSettings()
{
    ui->rBtnSuiteOfficial->setChecked(m_settings->testSuite == TestSuite::Official);
    ui->rBtnSuiteCustom->setChecked(m_settings->testSuite == TestSuite::Custom);
    ui->rBtnRelease->setChecked(m_settings->buildType == BuildType::Release);
    ui->lineEditTestsPath->setText(m_settings->customTestsPath);
    ui->lineEditResvg->setText(m_settings->resvgDir);

    ui->chBoxUseChrome->setChecked(m_settings->useChrome);

    ui->chBoxUseFirefox->setChecked(m_settings->useFirefox);
    ui->lineEditFirefox->setText(m_settings->firefoxPath);

    ui->chBoxUseBatik->setChecked(m_settings->useBatik);
    ui->lineEditBatik->setText(m_settings->batikPath);

    ui->chBoxUseInkscape->setChecked(m_settings->useInkscape);
    ui->lineEditInkscape->setText(m_settings->inkscapePath);

    ui->chBoxUseLibrsvg->setChecked(m_settings->useLibrsvg);
    ui->lineEditRsvg->setText(m_settings->librsvgPath);

    ui->chBoxUseQtSvg->setChecked(m_settings->useQtSvg);

    prepareTestsPathWidgets();
}

void SettingsDialog::prepareTestsPathWidgets()
{
    ui->lineEditTestsPath->setVisible(ui->rBtnSuiteCustom->isChecked());
    ui->btnSelectTest->setVisible(ui->rBtnSuiteCustom->isChecked());
}

void SettingsDialog::on_buttonBox_accepted()
{
    auto suite = TestSuite::Own;
    if (ui->rBtnSuiteOfficial->isChecked()) {
        suite = TestSuite::Official;
    } else if (ui->rBtnSuiteCustom->isChecked()) {
        suite = TestSuite::Custom;
    }
    m_settings->testSuite = suite;
    m_settings->customTestsPath = ui->lineEditTestsPath->text();

    int viewSize = Settings::ViewSizeOwn;
    if (suite == TestSuite::Official) {
        viewSize = Settings::ViewSizeOfficial;
    }
    m_settings->viewSize = viewSize / 2;

    m_settings->buildType = ui->rBtnRelease->isChecked()
                    ? BuildType::Release
                    : BuildType::Debug;

    m_settings->useChrome = ui->chBoxUseChrome->isChecked();
    m_settings->useFirefox = ui->chBoxUseFirefox->isChecked();
    m_settings->useBatik = ui->chBoxUseBatik->isChecked();
    m_settings->useInkscape = ui->chBoxUseInkscape->isChecked();
    m_settings->useLibrsvg = ui->chBoxUseLibrsvg->isChecked();
    m_settings->useQtSvg = ui->chBoxUseQtSvg->isChecked();

    m_settings->resvgDir = ui->lineEditResvg->text();
    m_settings->firefoxPath = ui->lineEditFirefox->text();
    m_settings->batikPath = ui->lineEditBatik->text();
    m_settings->inkscapePath = ui->lineEditInkscape->text();
    m_settings->librsvgPath = ui->lineEditRsvg->text();

    m_settings->save();
}

void SettingsDialog::on_btnSelectTest_clicked()
{
    const auto path = QFileDialog::getExistingDirectory(this, "Custom tests path",
                                                        ui->lineEditTestsPath->text());
    if (!path.isEmpty()) {
        ui->lineEditTestsPath->setText(path);
    }
}

void SettingsDialog::on_btnSelectResvg_clicked()
{
    const auto path = QFileDialog::getExistingDirectory(this, "resvg source path");
    if (!path.isEmpty()) {
        ui->lineEditResvg->setText(path);
    }
}

void SettingsDialog::on_btnSelectFirefox_clicked()
{
    const auto path = QFileDialog::getOpenFileName(this, "Firefox exe path");
    if (!path.isEmpty()) {
        ui->lineEditFirefox->setText(path);
    }
}

void SettingsDialog::on_btnSelectBatik_clicked()
{
    const auto path = QFileDialog::getOpenFileName(this, "batik-rasterizer exe path");
    if (!path.isEmpty()) {
        ui->lineEditBatik->setText(path);
    }
}

void SettingsDialog::on_btnSelectInkscape_clicked()
{
    const auto path = QFileDialog::getOpenFileName(this, "Inkscape exe path");
    if (!path.isEmpty()) {
        ui->lineEditInkscape->setText(path);
    }
}

void SettingsDialog::on_btnSelectRsvg_clicked()
{
    const auto path = QFileDialog::getOpenFileName(this, "rsvg-convert exe path");
    if (!path.isEmpty()) {
        ui->lineEditRsvg->setText(path);
    }
}
