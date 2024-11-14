#include <QSettings>
#include <QFileInfo>

#include "settings.h"

namespace Key {
    static const QString TestSuite          = "TestSuite";
    static const QString CustomTestsPath    = "CustomTestsPath";
    static const QString ResvgBuild         = "ResvgBuild";
    static const QString ResvgDir           = "ResvgDir";
    static const QString FirefoxPath        = "FirefoxPath";
    static const QString BatikPath          = "BatikPath";
    static const QString InkscapePath       = "InkscapePath";
    static const QString RsvgPath           = "RsvgPath";
    static const QString LadybirdPath       = "LadybirdPath";
    static const QString UseChrome          = "UseChrome";
    static const QString UseFirefox         = "UseFirefox";
    static const QString UseSafari          = "UseSafari";
    static const QString UseBatik           = "UseBatik";
    static const QString UseInkscape        = "UseInkscape";
    static const QString UseSvgNet          = "UseSvgNet";
    static const QString UseLibrsvg         = "UseLibrsvg";
    static const QString UseQtSvg           = "UseQtSvg";
    static const QString UseLadybird        = "UseLadybird";
    static const QString ViewSize           = "ViewSize";
}

static QString testSuiteToStr(TestSuite t) noexcept
{
    switch (t) {
        case TestSuite::Own      : return "results";
        case TestSuite::Custom   : return "custom";
    }

    Q_UNREACHABLE();
}

static TestSuite testSuiteFromStr(const QString &str) noexcept
{
    if (str == "custom") {
        return TestSuite::Custom;
    } else {
        return TestSuite::Own;
    }

    Q_UNREACHABLE();
}

static QString buildTypeToStr(BuildType t) noexcept
{
    switch (t) {
        case BuildType::Debug   : return "debug";
        case BuildType::Release : return "release";
    }

    Q_UNREACHABLE();
}

void Settings::load() noexcept
{
    QSettings appSettings;
    this->testSuite = testSuiteFromStr(appSettings.value(Key::TestSuite).toString());
    this->customTestsPath = appSettings.value(Key::CustomTestsPath).toString();

    this->buildType = appSettings.value(Key::ResvgBuild).toString() == "release"
                        ? BuildType::Release
                        : BuildType::Debug;

    this->useChrome = appSettings.value(Key::UseChrome, true).toBool();
    this->useFirefox = appSettings.value(Key::UseFirefox).toBool();
    this->useSafari = appSettings.value(Key::UseSafari).toBool();
    this->useBatik = appSettings.value(Key::UseBatik).toBool();
    this->useInkscape = appSettings.value(Key::UseInkscape).toBool();
    this->useLibrsvg = appSettings.value(Key::UseLibrsvg).toBool();
    this->useSvgNet = appSettings.value(Key::UseSvgNet).toBool();
    this->useQtSvg = appSettings.value(Key::UseQtSvg).toBool();
    this->useLadybird = appSettings.value(Key::UseLadybird).toBool();

    this->resvgDir = appSettings.value(Key::ResvgDir).toString();
    this->firefoxPath = appSettings.value(Key::FirefoxPath).toString();
    this->batikPath = appSettings.value(Key::BatikPath).toString();
    this->inkscapePath = appSettings.value(Key::InkscapePath).toString();
    this->librsvgPath = appSettings.value(Key::RsvgPath).toString();
    this->ladybirdPath = appSettings.value(Key::LadybirdPath).toString();
}

void Settings::save() const noexcept
{
    QSettings appSettings;
    appSettings.setValue(Key::TestSuite, testSuiteToStr(this->testSuite));
    appSettings.setValue(Key::CustomTestsPath, this->customTestsPath);
    appSettings.setValue(Key::ResvgBuild, buildTypeToStr(this->buildType));
    appSettings.setValue(Key::ViewSize, this->viewSize);
    appSettings.setValue(Key::UseChrome, this->useChrome);
    appSettings.setValue(Key::UseFirefox, this->useFirefox);
    appSettings.setValue(Key::UseSafari, this->useSafari);
    appSettings.setValue(Key::UseBatik, this->useBatik);
    appSettings.setValue(Key::UseInkscape, this->useInkscape);
    appSettings.setValue(Key::UseLibrsvg, this->useLibrsvg);
    appSettings.setValue(Key::UseSvgNet, this->useSvgNet);
    appSettings.setValue(Key::UseQtSvg, this->useQtSvg);
    appSettings.setValue(Key::UseLadybird, this->useLadybird);
    appSettings.setValue(Key::ResvgDir, this->resvgDir);
    appSettings.setValue(Key::FirefoxPath, this->firefoxPath);
    appSettings.setValue(Key::BatikPath, this->batikPath);
    appSettings.setValue(Key::InkscapePath, this->inkscapePath);
    appSettings.setValue(Key::RsvgPath, this->librsvgPath);
    appSettings.setValue(Key::LadybirdPath, this->ladybirdPath);
}

QString Settings::resvgPath() const noexcept
{
    return QString("%1/target/%2/resvg").arg(this->resvgDir, buildTypeToStr(this->buildType));
}

QString Settings::resultsPath() const noexcept
{
    Q_ASSERT(!QString(SRCDIR).isEmpty());
    Q_ASSERT(this->testSuite != TestSuite::Custom);

    const auto path = QString("%1/../../%2.csv").arg(SRCDIR).arg(testSuiteToStr(this->testSuite));

    Q_ASSERT(QFile::exists(path));

    return QFileInfo(path).absoluteFilePath();
}

QString Settings::testsPath() const noexcept
{
    QString path;
    switch (this->testSuite) {
        case TestSuite::Own      : path = QString("%1/../../tests").arg(SRCDIR); break;
        case TestSuite::Custom   : Q_UNREACHABLE();
    }

    Q_ASSERT(QFile::exists(path));
    return QFileInfo(path).absoluteFilePath();
}
