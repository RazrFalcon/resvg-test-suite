#pragma once

#include <QMainWindow>

#include "settings.h"
#include "tests.h"
#include "render.h"

namespace Ui {
class MainWindow;
}

class QLabel;
class QComboBox;

class BackendWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void prepareBackends();
    void setGuiEnabled(bool flag);
    void loadImageList();
    void resetImages();
    void loadTest(const int idx);
    void setAnimationEnabled(bool flag);
    void fillChBoxes();
    void save();

private slots:
    void onStart();
    void on_cmbBoxFiles_currentIndexChanged(int idx);
    void onImageReady(const Backend type, const QImage &img);
    void onDiffReady(const Backend type, const QImage &img);
    void onDiffStats(const Backend type, const uint value, const float percent);
    void onRenderFinished();
    void updatePassFlags();
    void on_btnSync_clicked();
    void on_btnSettings_clicked();
    void on_btnPrint_clicked();

private:
    Ui::MainWindow * const ui;
    QTimer * const m_autosaveTimer;

    QHash<Backend, BackendWidget*> m_backendWidges;

    Settings m_settings;
    Tests m_tests;
    Render m_render;
};
