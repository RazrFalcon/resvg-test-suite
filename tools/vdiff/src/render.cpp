#include <QFile>
#include <QFileInfo>
#include <QPainter>
#include <QImageReader>
#include <QtConcurrent/QtConcurrentMap>
#include <QtConcurrent/QtConcurrentRun>

#include <cmath>

#include "process.h"

#include "render.h"

namespace ImgName {
    static const QString Chrome     = "chrome.png";
    static const QString Firefox    = "firefox.png";
    static const QString ResvgCairo = "resvg-cairo.png";
    static const QString ResvgQt    = "resvg-qt.png";
    static const QString Batik      = "batik.png";
    static const QString Inkscape   = "ink.png";
    static const QString Rsvg       = "rsvg.png";
    static const QString QtSvg      = "qtsvg.png";
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
        case Backend::Reference :   return "Reference";
        case Backend::Chrome :      return "Chrome";
        case Backend::Firefox :     return "Firefox";
        case Backend::ResvgCairo :  return "resvg (cairo)";
        case Backend::ResvgQt :     return "resvg (Qt)";
        case Backend::Batik :       return "Batik";
        case Backend::Inkscape :    return "Inkscape";
        case Backend::Librsvg :     return "librsvg";
        case Backend::QtSvg :       return "QtSvg";
    }

    Q_UNREACHABLE();
}

QImage Render::renderReference(const RenderData &data)
{
    const QFileInfo fi(data.imgPath);
    const QString path = fi.absolutePath() + "/../png/" + fi.completeBaseName() + ".png";

    Q_ASSERT(QFile(path).exists());

    const QSize targetSize(data.viewSize, data.viewSize);

    QImage img(path);
    if (img.size() != targetSize) {
        img = img.scaled(targetSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }

    return img.convertToFormat(QImage::Format_ARGB32);
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

QImage Render::renderViaFirefox(const RenderData &data)
{
    int w = data.viewSize;
    int h = data.viewSize;

    // TODO: fix this temporary hack
    if (data.testSuite == TestSuite::Official) {
        h *= 0.75;
    }

    const QString out = Process::run(data.convPath, {
        "--headless",
        "--window-size",
        QString("%1,%2").arg(w).arg(h),
        "--screenshot",
        QFileInfo(ImgName::Firefox).absoluteFilePath(),
        data.imgPath,
    }, true);

    if (!out.isEmpty()) {
        if (out.simplified() != "*** You are running in headless mode.") {
            qDebug().noquote() << "firefox:" << out;
        }
    }

    return loadImage(ImgName::Firefox);
}

QImage Render::renderViaResvg(const RenderData &data)
{
    const QString outPath = (data.type == Backend::ResvgCairo) ? ImgName::ResvgCairo
                                                               : ImgName::ResvgQt;

    const QString out = Process::run(data.convPath, {
        data.imgPath,
        outPath,
        "-w", QString::number(data.viewSize),
        QString("--backend=") + ((data.type == Backend::ResvgCairo) ? "cairo" : "qt")
    }, true);

    if (!out.isEmpty()) {
        qDebug().noquote() << "resvg:" << out;
    }

    // TODO: convertToFormat is a temporary hack for e-radialGradient-031.svg + cairo backend
    // for some reasons, it creates an RGB image, not RGBA.
    return loadImage(outPath).convertToFormat(QImage::Format_ARGB32);
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
    }, true);

    if (!out.contains("success")) {
        qDebug().noquote() << "batik:" << out;
    }

    return loadImage(ImgName::Batik);
}

QImage Render::renderViaInkscape(const RenderData &data)
{
    /*const QString out = */Process::run(data.convPath, {
        data.imgPath,
        "-w", QString::number(data.viewSize),
        "--export-png=" + ImgName::Inkscape
    });

//    if (!out.isEmpty()) {
//        qDebug().noquote() << "inkscape:" << out;
//    }

    return loadImage(ImgName::Inkscape);
}

QImage Render::renderViaRsvg(const RenderData &data)
{
//    qDebug() << Process::run(data.convPath, { "-v" });

    const QString out = Process::run(data.convPath, {
        "-f", "png",
        "-w", QString::number(data.viewSize),
        data.imgPath,
        "-o", ImgName::Rsvg
    });

    if (!out.isEmpty()) {
        qDebug().noquote() << "rsvg:" << out;
    }

    // TODO: convertToFormat is a temporary hack for e-radialGradient-031.svg
    // for some reasons, it creates an RGB image, not RGBA.
    return loadImage(ImgName::Rsvg).convertToFormat(QImage::Format_ARGB32);
}

QImage Render::renderViaQtSvg(const RenderData &data)
{
    const QString out = Process::run(QString(SRCDIR) + "../qtsvgrender/qtsvgrender", {
        data.imgPath,
        ImgName::QtSvg,
        QString::number(data.viewSize)
    }, true);

    if (!out.isEmpty()) {
        qDebug().noquote() << "qtsvg:" << out;
    }

    return loadImage(ImgName::QtSvg);
}

void Render::renderImages()
{
    const auto ts = m_settings->testSuite;

    QVector<RenderData> list;

    if (ts != TestSuite::Custom) {
        list.append({ Backend::Reference, m_viewSize, m_imgPath, QString(), ts });
    }

    list.append({ Backend::ResvgCairo, m_viewSize, m_imgPath, m_settings->resvgPath(), ts });
    list.append({ Backend::ResvgQt, m_viewSize, m_imgPath, m_settings->resvgPath(), ts });

    if (m_settings->useChrome) {
        list.append({ Backend::Chrome, m_viewSize, m_imgPath, QString(), ts });
    }

    if (m_settings->useFirefox) {
        list.append({ Backend::Firefox, m_viewSize, m_imgPath, m_settings->firefoxPath, ts });
    }

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
            case Backend::Reference  : img = renderReference(data); break;
            case Backend::Chrome     : img = renderViaChrome(data); break;
            case Backend::Firefox    : img = renderViaFirefox(data); break;
            case Backend::ResvgCairo : img = renderViaResvg(data); break;
            case Backend::ResvgQt    : img = renderViaResvg(data); break;
            case Backend::Batik      : img = renderViaBatik(data); break;
            case Backend::Inkscape   : img = renderViaInkscape(data); break;
            case Backend::Librsvg    : img = renderViaRsvg(data); break;
            case Backend::QtSvg      : img = renderViaQtSvg(data); break;
        }

        return { data.type, img };
    } catch (const QString &s) {
        QImage img(data.viewSize, data.viewSize, QImage::Format_ARGB32);
        img.fill(Qt::white);

        QPainter p(&img);
        auto f = p.font();
        f.setPointSize(26);
        p.setFont(f);
        p.drawText(QRect(0, 0, data.viewSize, data.viewSize),
                   Qt::AlignLeft | Qt::AlignTop | Qt::TextWordWrap,
                   s);
        p.end();

        return { data.type, img };
    } catch (...) {
        Q_UNREACHABLE();
    }
}

static QImage toRGBFormat(const QImage &img, const QColor &bg)
{
    QImage newImg(img.size(), QImage::Format_RGB32);
    newImg.fill(bg);

    QPainter p(&newImg);
    p.drawImage(0, 0, img);
    p.end();
    return newImg;
}

static int colorDistance(const QColor &color1, const QColor &color2)
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

    Q_ASSERT(data.img1.format() == data.img2.format());

    const int w = qMin(data.img1.width(), data.img2.width());
    const int h = qMin(data.img1.height(), data.img2.height());

    // We have to convert ARGB images to RGB one with a white background,
    // because colorDistance() doesn't work with alpha.
    //
    // TODO: remove, because expensive.
    const auto img1 = toRGBFormat(data.img1, Qt::white);
    const auto img2 = toRGBFormat(data.img2, Qt::white);

    QImage diffImg(data.img1.size(), QImage::Format_RGB32);
    diffImg.fill(Qt::red);

    for (int y = 0; y < h; ++y) {
        auto s1 = (QRgb*)(img1.constScanLine(y));
        auto s2 = (QRgb*)(img2.constScanLine(y));
        auto s3 = (QRgb*)(diffImg.scanLine(y));

        for (int x = 0; x < w; ++x) {
            QRgb c1 = *s1;
            QRgb c2 = *s2;

            if (colorDistance(c1, c2) > 5) {
                *s3 = qRgb(255, 0, 0);
            } else {
                *s3 = qRgb(255, 255, 255);
            }

            s1++;
            s2++;
            s3++;
        }
    }

    return { data.type, diffImg };
}

void Render::onImageRendered(const int idx)
{
    const auto res = m_watcher1.resultAt(idx);
    m_imgs.insert(res.type, res.img);
    emit imageReady(res.type, res.img);
}

void Render::onImagesRendered()
{
    if (m_settings->testSuite == TestSuite::Custom) {
        // Use Chrome as a reference.

        const QImage refImg = m_imgs.value(Backend::Chrome);

        QVector<DiffData> list;
        const auto append = [&](const Backend type){
            if (m_imgs.contains(type) && type != Backend::Chrome) {
                list.append({ type, refImg, m_imgs.value(type) });
            }
        };

        for (int t = (int)Backend::Firefox; t <= (int)Backend::QtSvg; ++t) {
            append((Backend)t);
        }

        const auto future = QtConcurrent::mapped(list, &Render::diffImage);
        m_watcher2.setFuture(future);
    } else {
        const QImage refImg = m_imgs.value(Backend::Reference);

        QVector<DiffData> list;
        const auto append = [&](const Backend type){
            if (m_imgs.contains(type) && type != Backend::Reference) {
                list.append({ type, refImg, m_imgs.value(type) });
            }
        };

        for (int t = (int)Backend::Chrome; t <= (int)Backend::QtSvg; ++t) {
            append((Backend)t);
        }

        const auto future = QtConcurrent::mapped(list, &Render::diffImage);
        m_watcher2.setFuture(future);
    }
}

void Render::onDiffResult(const int idx)
{
    const auto v = m_watcher2.resultAt(idx);
    emit diffReady(v.type, v.img);
}

void Render::onDiffFinished()
{
    emit finished();
}

QDebug operator<<(QDebug dbg, const Backend &t)
{
    return dbg << QString("ImageType(%1)").arg(Render::backendName(t));
}
