#pragma once

#include <QString>

enum class TestSuite
{
    Own,
    Official,
};

enum class BuildType
{
    Debug,
    Release,
};

class Settings
{
public:
    static Settings load();
    void save();

    QString resvgPath() const noexcept;
    QString resultsPath() const noexcept;
    QString testsPath() const noexcept;
    QString testPath(const QString &fileName) const noexcept;

public:
    TestSuite testSuite = TestSuite::Own;
    BuildType buildType = BuildType::Debug;
    QString resvgDir; // it's dir, not path
    QString inkscapePath;
    QString librsvgPath;
};
