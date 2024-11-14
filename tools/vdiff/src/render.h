#pragma once

#include <QObject>
#include <QFutureWatcher>
#include <QImage>

#include "imagecache.h"
#include "settings.h"

struct RenderData
{
    Backend type;
    int viewSize;
    QSize imageSize;
    QString imgPath;
    QString convPath;
    TestSuite testSuite;
};

struct RenderResult
{
    Backend type;
    QImage img;
};

struct DiffData
{
    Backend type;
    QImage img1;
    QImage img2;
};

struct DiffOutput
{
    Backend type;
    QImage img;
};

Q_DECLARE_METATYPE(RenderResult)
Q_DECLARE_METATYPE(DiffOutput)

class Render : public QObject
{
    Q_OBJECT

public:
    explicit Render(QObject *parent = nullptr);

    void setScale(qreal s);

    void render(const QString &path);

    void setSettings(Settings *settings) { m_settings = settings; }

signals:
    void imageReady(Backend, QImage);
    void diffReady(Backend, QImage);
    void finished();

private:
    void renderImages();

    static QImage loadImage(const QString &path);
    static QImage renderReference(const RenderData &data);
    static QImage renderViaChrome(const RenderData &data);
    static QImage renderViaFirefox(const RenderData &data);
    static QImage renderViaSafari(const RenderData &data);
    static QImage renderViaResvg(const RenderData &data);
    static QImage renderViaSvgNet(const RenderData &data);
    static QImage renderViaBatik(const RenderData &data);
    static QImage renderViaInkscape(const RenderData &data);
    static QImage renderViaRsvg(const RenderData &data);
    static QImage renderViaQtSvg(const RenderData &data);
    static QImage renderViaLadybird(const RenderData &data);
    static RenderResult renderImage(const RenderData &data);
    static DiffOutput diffImage(const DiffData &data);

private slots:
    void onImageRendered(const int idx);
    void onImagesRendered();
    void onDiffResult(const int idx);
    void onDiffFinished();

private:
    Settings *m_settings = nullptr;
    ImageCache m_imgCache;
    int m_viewSize = 300;
    qreal m_dpiScale = 1.0;
    QFutureWatcher<RenderResult> m_watcher1;
    QFutureWatcher<DiffOutput> m_watcher2;
    QString m_imgPath;
    QHash<Backend, QImage> m_imgs;
};
