#include <QGuiApplication>
#include <QSvgRenderer>
#include <QPainter>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    // Do not use QCommandLineParser because it too slow.

    const QStringList args = app.arguments();

    QSvgRenderer render(args.at(1));

    QImage img(render.defaultSize(), QImage::Format_ARGB32);
    img.fill(Qt::transparent);

    QPainter p(&img);
    render.render(&p);
    p.end();

    img.save(args.at(2));

    return 0;
}
