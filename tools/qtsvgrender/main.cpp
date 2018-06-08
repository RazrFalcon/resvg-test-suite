#include <QGuiApplication>
#include <QSvgRenderer>
#include <QPainter>
#include <QFile>

#include <cmath>

int main(int argc, char *argv[])
{
    if (!(argc == 3 || argc == 4)) {
        printf("Usage:\n"
               "  qtsvgrender in.svg out.png\n"
               "  qtsvgrender in.svg out.png 500\n");
        return 1;
    }

    QFile file(argv[1]);
    if (!file.open(QFile::ReadOnly)) {
        printf("Error: Failed to open an input file.\n");
        return 1;
    }

    const QByteArray svgData = file.readAll();

    const bool isGuiRequired = svgData.contains("<text");

    // QGuiApplication initialization is very slow and only needed to render text,
    // so avoid it if possible.
    QScopedPointer<QCoreApplication> app(isGuiRequired ? new QGuiApplication(argc, argv)
                                                       : new QCoreApplication(argc, argv));

    QSvgRenderer render(svgData);

    if (!render.isValid()) {
        printf("Error: Invalid SVG data.\n");
        return 1;
    }

    QSize imgSize = render.viewBox().size();

    // Scale to width.
    if (argc == 4) {
        const int w = QString(argv[3]).toUInt();
        if (w != 0) {
            imgSize.setHeight(std::ceil(double(w) * imgSize.height() / imgSize.width()));
            imgSize.setWidth(w);
        }
    }

    QImage img(imgSize, QImage::Format_ARGB32);
    img.fill(Qt::transparent);

    QPainter p(&img);
    render.render(&p);
    p.end();

    img.save(argv[2]);

    return 0;
}
