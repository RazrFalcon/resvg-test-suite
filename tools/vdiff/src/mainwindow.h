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
    void loadImage(const QString &fileName);
    void setAnimationEnabled(bool flag);
    void getTitleAndDesc(const QString &path);
    void fillChBoxes();

private slots:
    void onStart();
    void on_cmbBoxFiles_currentIndexChanged(int index);
    void onImageReady(const Backend type, const QImage &img);
    void onDiffReady(const Backend type, const QImage &img);
    void onDiffStats(const Backend type, const uint value, const float percent);
    void onRenderWarning(const QString &msg);
    void onRenderError(const QString &msg);
    void onRenderFinished();
    void updatePassFlags();
    void on_btnResync_clicked();
    void on_btnSettings_clicked();
    void on_cmbBoxNames_currentIndexChanged(const QString &text);

private:
    Ui::MainWindow * const ui;

    QHash<Backend, BackendWidget*> m_backendWidges;

    Settings m_settings;
    Tests m_tests;
    Render m_render;
};
