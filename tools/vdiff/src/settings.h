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
    void load() noexcept;
    void save() const noexcept;

    QString resvgPath() const noexcept;
    QString resultsPath() const noexcept;
    QString testsPath() const noexcept;
    QString testPath(const QString &fileName) const noexcept;

public:
    TestSuite testSuite = TestSuite::Own;
    BuildType buildType = BuildType::Debug;
    int viewSize = 300;
    bool useBatik = true;
    bool useInkscape = true;
    bool useLibrsvg = true;
    bool useQtSvg = true;
    QString resvgDir; // it's a dir, not a path
    QString batikPath;
    QString inkscapePath;
    QString librsvgPath;
};
