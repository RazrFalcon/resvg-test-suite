#include <QSettings>
#include <QMessageBox>
#include <QFileDialog>
#include <QDebug>

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
    QSettings settings;
    ui->rBtnRelease->setChecked(settings.value("ResvgBuild").toString() == "release");

    ui->lineEditResvg->setText(settings.value("ResvgDir").toString());
    ui->lineEditInkscape->setText(settings.value("InkscapePath").toString());
    ui->lineEditRsvg->setText(settings.value("RsvgPath").toString());
}

void SettingsDialog::on_buttonBox_accepted()
{
    QSettings settings;

    const auto resvgBuild = ui->rBtnDebug->isChecked() ? "debug" : "release";
    settings.setValue("ResvgBuild", resvgBuild);

    const auto resvgPath = QString("%1/target/%2/rendersvg")
                             .arg(ui->lineEditResvg->text(), resvgBuild);

    settings.setValue("ResvgPath", resvgPath);
    settings.setValue("ResvgDir", ui->lineEditResvg->text());
    settings.setValue("InkscapePath", ui->lineEditInkscape->text());
    settings.setValue("RsvgPath", ui->lineEditRsvg->text());
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
