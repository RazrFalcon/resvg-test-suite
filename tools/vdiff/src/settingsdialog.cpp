#include <QMessageBox>
#include <QFileDialog>
#include <QDebug>

#include "settings.h"

#include "settingsdialog.h"
#include "ui_settingsdialog.h"

SettingsDialog::SettingsDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);

    loadSettings();
    setMinimumWidth(600);
    adjustSize();
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::loadSettings()
{
    const auto settings = Settings::load();
    ui->rBtnSuiteOfficial->setChecked(settings.testSuite == TestSuite::Official);
    ui->rBtnRelease->setChecked(settings.buildType == BuildType::Release);
    ui->lineEditResvg->setText(settings.resvgDir);
    ui->lineEditInkscape->setText(settings.inkscapePath);
    ui->lineEditRsvg->setText(settings.librsvgPath);
}

void SettingsDialog::on_buttonBox_accepted()
{
    Settings d;

    d.testSuite = ui->rBtnSuiteOfficial->isChecked()
                    ? TestSuite::Official
                    : TestSuite::Own;

    d.buildType = ui->rBtnRelease->isChecked()
                    ? BuildType::Release
                    : BuildType::Debug;

    d.resvgDir = ui->lineEditResvg->text();
    d.inkscapePath = ui->lineEditInkscape->text();
    d.librsvgPath = ui->lineEditRsvg->text();

    d.save();
}

void SettingsDialog::on_btnSelectResvg_clicked()
{
    const auto path = QFileDialog::getExistingDirectory(this, "rendersvg source path");
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
