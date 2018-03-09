#include <QFileInfo>

#include "paths.h"

QString Paths::order() noexcept
{
    const auto path = QString("%1/../../order.txt").arg(SRCDIR);

    Q_ASSERT(QFile::exists(path));

    return QFileInfo(path).absoluteFilePath();
}
