#include <QVBoxLayout>
#include <QLabel>
#include <QComboBox>

#include "imageview.h"

#include "backendwidget.h"

BackendWidget::BackendWidget(const Backend backend, QWidget *parent)
    : QWidget(parent)
    , m_backend(backend)
    , m_lblTitle(new QLabel)
    , m_imageView(new ImageView)
    , m_diffView(new ImageView)
    , m_cmbBoxState(new QComboBox)
{
    auto lay = new QVBoxLayout(this);
    lay->setContentsMargins(QMargins());
    lay->addWidget(m_lblTitle);
    lay->addWidget(m_imageView);
    lay->addWidget(m_diffView);
    lay->addWidget(m_cmbBoxState, 0, Qt::AlignHCenter);
    lay->addStretch();

    m_lblTitle->setAlignment(Qt::AlignCenter);

    m_imageView->setFixedSize(300, 300);
    m_diffView->setFixedSize(300, 300);

    m_cmbBoxState->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    m_cmbBoxState->addItem(QIcon(":/icons/unknown.svgz"), "Unknown");
    m_cmbBoxState->addItem(QIcon(":/icons/passed.svgz"), "Passed");
    m_cmbBoxState->addItem(QIcon(":/icons/failed.svgz"), "Failed");
    m_cmbBoxState->addItem(QIcon(":/icons/crashed.svgz"), "Crashed");
    connect(m_cmbBoxState, SIGNAL(activated(int)), this, SIGNAL(testStateChanged()));
}

QString BackendWidget::title() const
{
    return m_lblTitle->text();
}

void BackendWidget::setTitle(const QString &title)
{
    m_lblTitle->setText(title);
}

QImage BackendWidget::image() const
{
    return m_imageView->image();
}

void BackendWidget::setImage(const QImage &img)
{
    m_imageView->setAnimationEnabled(false);
    m_imageView->setImage(img);
}

void BackendWidget::setDiffImage(const QImage &img)
{
    m_diffView->setAnimationEnabled(false);
    m_diffView->setImage(img);
}

void BackendWidget::setDiffVisible(bool flag)
{
    m_diffView->setVisible(flag);
}

void BackendWidget::setAnimationEnabled(bool flag)
{
    m_imageView->setAnimationEnabled(flag);
    m_diffView->setAnimationEnabled(flag);
}

void BackendWidget::setViewSize(const QSize &size)
{
    m_imageView->setFixedSize(size);
    m_diffView->setFixedSize(size);
}

void BackendWidget::resetImages()
{
    m_imageView->resetImage();
    m_diffView->resetImage();
}

TestState BackendWidget::testState() const
{
    return (TestState)m_cmbBoxState->currentIndex();
}

void BackendWidget::setTestState(const TestState state)
{
    m_cmbBoxState->setCurrentIndex((int)state);
}

void BackendWidget::setTestStateVisible(bool flag)
{
    m_cmbBoxState->setVisible(flag);
}
