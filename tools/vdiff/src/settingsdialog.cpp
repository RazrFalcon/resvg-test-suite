#include <QMessageBox>
#include <QFileDialog>

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

    ui->buttonBox->setFocus();
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::loadSettings()
{
    ui->rBtnSuiteOfficial->setChecked(m_settings->testSuite == TestSuite::Official);
    ui->rBtnRelease->setChecked(m_settings->buildType == BuildType::Release);
    ui->lineEditResvg->setText(m_settings->resvgDir);

    ui->chBoxUseInkscape->setChecked(m_settings->useInkscape);
    ui->lineEditInkscape->setText(m_settings->inkscapePath);

    ui->chBoxUseLibrsvg->setChecked(m_settings->useLibrsvg);
    ui->lineEditRsvg->setText(m_settings->librsvgPath);

    ui->chBoxUseQtSvg->setChecked(m_settings->useQtSvg);
}

void SettingsDialog::on_buttonBox_accepted()
{
    m_settings->testSuite = ui->rBtnSuiteOfficial->isChecked()
                    ? TestSuite::Official
                    : TestSuite::Own;

    m_settings->buildType = ui->rBtnRelease->isChecked()
                    ? BuildType::Release
                    : BuildType::Debug;

    m_settings->useInkscape = ui->chBoxUseInkscape->isChecked();
    m_settings->useLibrsvg = ui->chBoxUseLibrsvg->isChecked();
    m_settings->useQtSvg = ui->chBoxUseQtSvg->isChecked();

    m_settings->resvgDir = ui->lineEditResvg->text();
    m_settings->inkscapePath = ui->lineEditInkscape->text();
    m_settings->librsvgPath = ui->lineEditRsvg->text();

    m_settings->save();
}

void SettingsDialog::on_btnSelectResvg_clicked()
{
    const auto path = QFileDialog::getExistingDirectory(this, "resvg source path");
    if (!path.isEmpty()) {
        ui->lineEditResvg->setText(path);
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
