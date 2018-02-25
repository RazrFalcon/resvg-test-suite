#pragma once

#include <QString>

class Paths
{
public:
    static QString results() noexcept;
    static QString testsPath() noexcept;
    static QString testPath(const QString &fileName) noexcept;
    static QString order() noexcept;
};
