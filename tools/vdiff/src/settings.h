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
    TestSuite testSuite = TestSuite::Own;
    BuildType buildType = BuildType::Debug;
    QString customTestsPath;
    int viewSize = 250;
    bool useChrome = true;
    bool useFirefox = true;
    bool useSafari = true;
    bool useBatik = true;
    bool useInkscape = true;
    bool useLibrsvg = true;
    bool useSvgNet = true;
    bool useQtSvg = true;
    bool useLadybird = true;
    QString resvgDir; // it's a dir, not a path
    QString firefoxPath;
    QString batikPath;
    QString inkscapePath;
    QString librsvgPath;
    QString ladybirdPath;
};
