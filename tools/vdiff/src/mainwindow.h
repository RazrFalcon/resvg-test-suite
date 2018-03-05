#pragma once

#include <QMainWindow>

#include "tests.h"
#include "render.h"

namespace Ui {
class MainWindow;
}

class QLabel;
class QComboBox;

class ImageView;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void initDefaultSettings();
    void setGuiEnabled(bool flag);
    void loadImageList();
    void resetImages();
    void loadImage(const QString &fileName);
    void setAnimationEnabled(bool flag);
    void getTitleAndDesc(const QString &path);
    void fillChBoxes();

    void setDiffText(QLabel *lbl, uint diff, float percent) const;

private slots:
    void onStart();
    void on_cmbBoxFiles_currentIndexChanged(int index);
    void onImageReady(const ImageType type, const QImage &img);
    void onDiffReady(const ImageType type, const QImage &img);
    void onDiffStats(const ImageType type, const uint value, const float percent);
    void onRenderWarning(const QString &msg);
    void onRenderError(const QString &msg);
    void onRenderFinished();
    void updatePassFlags();
    void on_btnResync_clicked();
    void on_btnSettings_clicked();
    void on_cmbBoxNames_currentIndexChanged(const QString &text);

private:
    Ui::MainWindow * const ui;

    QHash<ImageType, ImageView*> m_imgViews;
    QHash<ImageType, ImageView*> m_diffViews;
    QHash<ImageType, QLabel*> m_diffLabels;
    QVector<QComboBox*> m_flagBoxes;

    Tests m_tests;
    Render m_render;
};
