#pragma once

#include <QObject>
#include <QFutureWatcher>
#include <QImage>

#include "tests.h"
#include "either.h"
#include "settings.h"

struct RenderData
{
    Backend type;
    int viewSize;
    QString imgPath;
    QString convPath;
    TestSuite testSuite;
};

struct RenderOutput
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
    uint value;
    float percent;
    QImage img;
};

using RenderResult = Either<RenderOutput, QString>;

Q_DECLARE_METATYPE(RenderOutput)
Q_DECLARE_METATYPE(DiffOutput)

class Render : public QObject
{
    Q_OBJECT

public:
    explicit Render(QObject *parent = nullptr);

    void setScale(qreal s);

    void render(const QString &path);

    void setSettings(Settings *settings) { m_settings = settings; }

    static QString backendName(const Backend t);

signals:
    void imageReady(Backend, QImage);
    void diffReady(Backend, QImage);
    void diffStats(Backend, uint, float);
    void warning(QString);
    void error(QString);
    void finished();

private:
    void renderImages();

    static QImage loadImage(const QString &path);
    static QImage renderViaChrome(const RenderData &data);
    static QImage renderViaResvg(const RenderData &data);
    static QImage renderViaBatik(const RenderData &data);
    static QImage renderViaInkscape(const RenderData &data);
    static QImage renderViaRsvg(const RenderData &data);
    static QImage renderViaQtSvg(const RenderData &data);
    static RenderResult renderImage(const RenderData &data);
    static DiffOutput diffImage(const DiffData &data);

private slots:
    void onImageRendered(const int idx);
    void onImagesRendered();
    void onDiffResult(const int idx);
    void onDiffFinished();

private:
    Settings *m_settings = nullptr;
    int m_viewSize = 300;
    QFutureWatcher<RenderResult> m_watcher1;
    QFutureWatcher<DiffOutput> m_watcher2;
    QString m_imgPath;
    QHash<Backend, QImage> m_imgs;
};

QDebug operator<<(QDebug dbg, const Backend &t);
