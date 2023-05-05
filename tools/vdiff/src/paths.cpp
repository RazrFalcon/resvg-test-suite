#include <QCoreApplication>
#include <QFileInfo>

#include "paths.h"

QString Paths::workDir() noexcept
{
#ifdef Q_OS_MAC
    return qApp->applicationDirPath() + "/../../../";
#else
    return qApp->applicationDirPath();
#endif
}
