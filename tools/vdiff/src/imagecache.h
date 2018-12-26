#pragma once

#include <QImage>

#include "tests.h"

class ImageCache
{
public:
    ImageCache();
    ~ImageCache();

    QImage getImage(const Backend backend, const QString &svgPath);
    void setImage(const Backend backend, const QString &svgPath, const QImage &img);

private:
    Q_DISABLE_COPY(ImageCache)
};
