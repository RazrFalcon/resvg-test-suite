#pragma once

#include <QString>

class Paths
{
public:
    static QString results() noexcept;
    static QString testPath(const QString &fileName) noexcept;
};
