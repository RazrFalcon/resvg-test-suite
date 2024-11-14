#pragma once

#include <QDialog>

namespace Ui {
class SettingsDialog;
}

class Settings;

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(Settings *settings, QWidget *parent = nullptr);
    ~SettingsDialog();

private:
    void loadSettings();

private slots:
    void on_buttonBox_accepted();
    void on_btnSelectInkscape_clicked();
    void on_btnSelectRsvg_clicked();
    void on_btnSelectResvg_clicked();
    void on_btnSelectFirefox_clicked();
    void on_btnSelectBatik_clicked();
    void on_btnSelectTest_clicked();
    void on_btnSelectLadybird_clicked();
    void prepareTestsPathWidgets();

private:
    Ui::SettingsDialog * const ui;
    Settings * const m_settings;
};
