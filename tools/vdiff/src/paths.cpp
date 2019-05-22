#include <QCoreApplication>
#include <QFileInfo>

#include "paths.h"

QString Paths::order() noexcept
{
    const auto path = QString("%1/../../order.txt").arg(SRCDIR);

    Q_ASSERT(QFile::exists(path));

    return QFileInfo(path).absoluteFilePath();
}

QString Paths::workDir() noexcept
{
#ifdef Q_OS_MAC
    return qApp->applicationDirPath() + "/../../../";
#else
    return qApp->applicationDirPath();
#endif
}
