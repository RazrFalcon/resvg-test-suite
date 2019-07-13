#pragma once

#include <QString>

#include "tests.h"

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

public:
    static constexpr int ViewSizeOwn = 500;
    static constexpr int ViewSizeOfficial = 480;

    TestSuite testSuite = TestSuite::Own;
    BuildType buildType = BuildType::Debug;
    QString customTestsPath;
    int viewSize = ViewSizeOwn / 2;
    bool useResvgCairo = true;
    bool useResvgQt = true;
    bool useResvgRaqote = true;
    bool useResvgSkia = true;
    bool useChrome = true;
    bool useFirefox = true;
    bool useBatik = true;
    bool useInkscape = true;
    bool useLibrsvg = true;
    bool useQtSvg = true;
    QString resvgDir; // it's a dir, not a path
    QString firefoxPath;
    QString batikPath;
    QString inkscapePath;
    QString librsvgPath;
};
