#pragma once

#include <QWidget>

#include "tests.h"

class QLabel;
class QComboBox;

class ImageView;

class BackendWidget : public QWidget
{
    Q_OBJECT

public:
    explicit BackendWidget(const Backend backend, QWidget *parent = nullptr);

    void setTitle(const QString &title);
    void setImage(const QImage &img);
    void setDiffImage(const QImage &img);
    void setDiffVisible(bool flag);
    void setDiffStats(uint diff, float percent);
    void setAnimationEnabled(bool flag);
    void setViewSize(const QSize &size);

    void resetImages();

    Backend backend() const { return m_backend; }

    TestState testState() const;
    void setTestState(const TestState state);
    void setTestStateVisible(bool flag);

signals:
    void testStateChanged();

private:
    const Backend m_backend;
    QLabel * const m_lblTitle;
    ImageView * const m_imageView;
    ImageView * const m_diffView;
    QLabel * const m_lblStats;
    QComboBox * const m_cmbBoxState;
};
