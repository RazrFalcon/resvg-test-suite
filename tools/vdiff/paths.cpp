#include <QFileInfo>

#include "paths.h"

QString Paths::results() noexcept
{
    Q_ASSERT(!QString(SRCDIR).isEmpty());

    const auto path = QString("%1/../../results.csv").arg(SRCDIR);

    Q_ASSERT(QFile::exists(path));

    return QFileInfo(path).absoluteFilePath();
}

QString Paths::testsPath() noexcept
{
    const auto path = QString("%1/../../svg").arg(SRCDIR);

    Q_ASSERT(QFile::exists(path));

    return QFileInfo(path).absoluteFilePath();
}

QString Paths::testPath(const QString &fileName) noexcept
{
    Q_ASSERT(!fileName.contains('/'));

    const auto path = QString("%1/../../svg/%2").arg(SRCDIR).arg(fileName);

    Q_ASSERT(QFile::exists(path));

    return QFileInfo(path).absoluteFilePath();
}
