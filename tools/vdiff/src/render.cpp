#include <QFile>
#include <QPainter>
#include <QImageReader>
#include <QSvgRenderer>
#include <QtConcurrent/QtConcurrentMap>
#include <QtConcurrent/QtConcurrentRun>

#include <cmath>

#include "process.h"

#include "render.h"

namespace ImgName {
    static const QString Chrome     = "chrome.png";
    static const QString ResvgCairo = "resvg-cairo.png";
    static const QString ResvgQt    = "resvg-qt.png";
    static const QString Batik      = "batik.png";
    static const QString Inkscape   = "ink.png";
    static const QString Rsvg       = "rsvg.png";
}

Render::Render(QObject *parent)
    : QObject(parent)
{
    qRegisterMetaType<RenderResult>("RenderResult");
    qRegisterMetaType<DiffOutput>("DiffOutput");

    connect(&m_watcher1, &QFutureWatcher<RenderResult>::resultReadyAt,
            this, &Render::onImageRendered);
    connect(&m_watcher1, &QFutureWatcher<RenderResult>::finished,
            this, &Render::onImagesRendered);

    connect(&m_watcher2, &QFutureWatcher<DiffOutput>::resultReadyAt,
            this, &Render::onDiffResult);
    connect(&m_watcher2, &QFutureWatcher<DiffOutput>::finished,
            this, &Render::onDiffFinished);
}

void Render::setScale(qreal s)
{
    m_viewSize = m_settings->viewSize * s;
}

void Render::render(const QString &path)
{
    m_imgPath = path;
    m_imgs.clear();
    renderImages();
}

QString Render::backendName(const Backend t)
{
    switch (t) {
        case Backend::Chrome : return "Chrome";
        case Backend::ResvgCairo : return "Resvg (cairo)";
        case Backend::ResvgQt : return "Resvg (Qt)";
        case Backend::Batik : return "Batik";
        case Backend::Inkscape : return "Inkscape";
        case Backend::Librsvg : return "rsvg";
        case Backend::QtSvg : return "QtSvg";
    }
}

QImage Render::renderViaChrome(const RenderData &data)
{
    const QString out = Process::run("node", {
        QString(SRCDIR) + "../chrome-svgrender/svgrender.js",
        data.imgPath,
        ImgName::Chrome,
        QString::number(data.viewSize)
    }, true);

    if (!out.isEmpty()) {
        qDebug().noquote() << "chrome:" << out;
    }

    return loadImage(ImgName::Chrome);
}

QImage Render::renderViaResvg(const RenderData &data)
{
    const QString outPath = (data.type == Backend::ResvgCairo) ? ImgName::ResvgCairo
                                                               : ImgName::ResvgQt;

    const QString out = Process::run(data.convPath, {
        data.imgPath,
        outPath,
        "-w", QString::number(data.viewSize),
        "--background=white",
        QString("--backend=") + ((data.type == Backend::ResvgCairo) ? "cairo" : "qt")
    }, true);

    if (!out.isEmpty()) {
        qDebug().noquote() << "resvg:" << out;
    }

    return loadImage(outPath);
}

QImage Render::renderViaBatik(const RenderData &data)
{
    int w = data.viewSize;
    int h = data.viewSize;

    // TODO: fix this temporary hack
    if (data.testSuite == TestSuite::Official) {
        h *= 0.75;
    }

    const QString out = Process::run(data.convPath, {
        "-scriptSecurityOff",
        data.imgPath,
        "-d", ImgName::Batik,
        "-w", QString::number(w),
        "-h", QString::number(h),
        "-bg", "255.255.255.255"
    }, true);

    if (!out.contains("success")) {
        qDebug().noquote() << "batik:" << out;
    }

    return loadImage(ImgName::Batik);
}

QImage Render::renderViaInkscape(const RenderData &data)
{
    /*const QString out = */Process::run(data.convPath,
      {
        data.imgPath,
        "--export-background=white",
        "-w", QString::number(data.viewSize),
        "--export-png=" + ImgName::Inkscape
      }
    );

//    if (!out.isEmpty()) {
//        qDebug().noquote() << "inkscape:" << out;
//    }

    return loadImage(ImgName::Inkscape);
}

QImage Render::renderViaRsvg(const RenderData &data)
{
    const QString out = Process::run(data.convPath,
      {
        "-f", "png",
        "-w", QString::number(data.viewSize),
        "--background-color=white",
        data.imgPath,
        "-o", ImgName::Rsvg
      }
    );

    if (!out.isEmpty()) {
        qDebug().noquote() << "rsvg:" << out;
    }

    return loadImage(ImgName::Rsvg);
}

QImage Render::renderViaQtSvg(const RenderData &data)
{
    // TODO: move to a separate exe

    QSvgRenderer render(data.imgPath);
    const QSize s = render.viewBox().size()
                          .scaled(data.viewSize, data.viewSize, Qt::KeepAspectRatio);
    QImage img(s, QImage::Format_RGB32);
    img.fill(Qt::white);

    QPainter p(&img);
    render.render(&p);
    p.end();

    return img;
}

void Render::renderImages()
{
    const auto ts = m_settings->testSuite;

    QVector<RenderData> list;
    list.append({ Backend::Chrome, m_viewSize, m_imgPath, QString(), ts });
    list.append({ Backend::ResvgCairo, m_viewSize, m_imgPath, m_settings->resvgPath(), ts });
    list.append({ Backend::ResvgQt, m_viewSize, m_imgPath, m_settings->resvgPath(), ts });

    if (m_settings->useBatik) {
        list.append({ Backend::Batik, m_viewSize, m_imgPath, m_settings->batikPath, ts });
    }

    if (m_settings->useInkscape) {
        list.append({ Backend::Inkscape, m_viewSize, m_imgPath, m_settings->inkscapePath, ts });
    }

    if (m_settings->useLibrsvg) {
        list.append({ Backend::Librsvg, m_viewSize, m_imgPath, m_settings->librsvgPath, ts });
    }

    if (m_settings->useQtSvg) {
        list.append({ Backend::QtSvg, m_viewSize, m_imgPath, QString(), ts });
    }

    const auto future = QtConcurrent::mapped(list, &Render::renderImage);
    m_watcher1.setFuture(future);
}

QImage Render::loadImage(const QString &path)
{
    const QImage img(path);
    if (img.isNull()) {
        throw QString("Invalid image: %1").arg(path);
    }

    QFile::remove(path);
    return img;
}

RenderResult Render::renderImage(const RenderData &data)
{
    try {
        QImage img;
        switch (data.type) {
            case Backend::Chrome     : img = renderViaChrome(data); break;
            case Backend::ResvgCairo : img = renderViaResvg(data); break;
            case Backend::ResvgQt    : img = renderViaResvg(data); break;
            case Backend::Batik      : img = renderViaBatik(data); break;
            case Backend::Inkscape   : img = renderViaInkscape(data); break;
            case Backend::Librsvg    : img = renderViaRsvg(data); break;
            case Backend::QtSvg      : img = renderViaQtSvg(data); break;
        }

        return {{ data.type, img }};
    } catch (const QString &s) {
        return { s };
    } catch (const std::exception &e) {
        return { QString(e.what()) };
    } catch (...) {
        Q_UNREACHABLE();
    }
}

int colorDistance(const QColor &color1, const QColor &color2)
{
    const int rd = std::pow(color1.red() - color2.red(), 2);
    const int gd = std::pow(color1.green() - color2.green(), 2);
    const int bd = std::pow(color1.blue() - color2.blue(), 2);
    return std::sqrt(rd + gd + bd);
}

DiffOutput Render::diffImage(const DiffData &data)
{
    if (data.img1.size() != data.img2.size()) {
        QString msg = QString("Images size mismatch: %1x%2 != %3x%4 Chrome vs %5")
            .arg(data.img1.width()).arg(data.img1.height())
            .arg(data.img2.width()).arg(data.img2.height())
            .arg(backendName(data.type));

        qWarning() << msg;
    }

    uint diffValue = 0;

    const int w = qMin(data.img1.width(), data.img2.width());
    const int h = qMin(data.img1.height(), data.img2.height());

    QImage diffImg(data.img1.size(), QImage::Format_RGB32);
    diffImg.fill(Qt::green);

    for (int y = 0; y < h; ++y) {
        QRgb *s1 = (QRgb*)(data.img1.scanLine(y));
        QRgb *s2 = (QRgb*)(data.img2.scanLine(y));
        QRgb *s3 = (QRgb*)(diffImg.scanLine(y));

        for (int x = 0; x < w; ++x) {
            QRgb c1 = *s1;
            QRgb c2 = *s2;

            if (colorDistance(c1, c2) > 5) {
                diffValue++;

                *s3 = qRgb(255, 0, 0);
            } else {
                *s3 = qRgb(255, 255, 255);
            }

            s1++;
            s2++;
            s3++;
        }
    }

    const float percent = (double)diffValue / (w * h) * 100.0;

    return { data.type, diffValue, percent, diffImg };
}

void Render::onImageRendered(const int idx)
{
    const auto res = m_watcher1.resultAt(idx);
    if (res.is1st()) {
        const auto v = res.as1st();
        m_imgs.insert(v.type, v.img);
        emit imageReady(v.type, v.img);
    } else {
        const auto v = res.as2nd();
        emit warning(v);
    }
}

void Render::onImagesRendered()
{
    if (!m_imgs.contains(Backend::Chrome)) {
        emit error("Image must be rendered via Chrome to calculate diff images.");
        emit finished();
        return;
    }

    const QImage chromeImg = m_imgs.value(Backend::Chrome);

    QVector<DiffData> list;
    const auto append = [&](const Backend type){
        if (m_imgs.contains(type) && type != Backend::Chrome) {
            list.append({ type, chromeImg, m_imgs.value(type) });
        }
    };

    for (int t = (int)Backend::ResvgCairo; t <= (int)Backend::QtSvg; ++t) {
        append((Backend)t);
    }

    const auto future = QtConcurrent::mapped(list, &Render::diffImage);
    m_watcher2.setFuture(future);
}

void Render::onDiffResult(const int idx)
{
    const auto v = m_watcher2.resultAt(idx);
    emit diffReady(v.type, v.img);
    emit diffStats(v.type, v.value, v.percent);
}

void Render::onDiffFinished()
{
    emit finished();
}

QDebug operator<<(QDebug dbg, const Backend &t)
{
    return dbg << QString("ImageType(%1)").arg(Render::backendName(t));
}
