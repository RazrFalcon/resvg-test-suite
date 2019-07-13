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
    static const QString UseCairoBackend    = "UseCairoBackend";
    static const QString UseQtBackend       = "UseQtBackend";
    static const QString UseRaqoteBackend   = "UseRaqoteBackend";
    static const QString UseSkiaBackend   = "UseSkiaBackend";
    static const QString UseChrome          = "UseChrome";
    static const QString UseFirefox         = "UseFirefox";
    static const QString UseBatik           = "UseBatik";
    static const QString UseInkscape        = "UseInkscape";
    static const QString UseLibrsvg         = "UseLibrsvg";
    static const QString UseQtSvg           = "UseQtSvg";
    static const QString ViewSize           = "ViewSize";
}

static QString testSuiteToStr(TestSuite t) noexcept
{
    switch (t) {
        case TestSuite::Own      : return "results";
        case TestSuite::Official : return "official";
        case TestSuite::Custom   : return "custom";
    }

    Q_UNREACHABLE();
}

static TestSuite testSuiteFromStr(const QString &str) noexcept
{
    if (str == "official") {
        return TestSuite::Official;
    } else if (str == "custom") {
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

    this->viewSize = appSettings.value(Key::ViewSize, ViewSizeOwn / 2).toUInt();

    // TODO: check for available backends
    this->useResvgCairo = appSettings.value(Key::UseCairoBackend, true).toBool();
    this->useResvgQt = appSettings.value(Key::UseQtBackend, true).toBool();
    this->useResvgRaqote = appSettings.value(Key::UseRaqoteBackend).toBool();
    this->useResvgSkia = appSettings.value(Key::UseSkiaBackend).toBool();
    this->useChrome = appSettings.value(Key::UseChrome, true).toBool();
    this->useFirefox = appSettings.value(Key::UseFirefox).toBool();
    this->useBatik = appSettings.value(Key::UseBatik).toBool();
    this->useInkscape = appSettings.value(Key::UseInkscape).toBool();
    this->useLibrsvg = appSettings.value(Key::UseLibrsvg).toBool();
    this->useQtSvg = appSettings.value(Key::UseQtSvg).toBool();

    this->resvgDir = appSettings.value(Key::ResvgDir).toString();
    this->firefoxPath = appSettings.value(Key::FirefoxPath).toString();
    this->batikPath = appSettings.value(Key::BatikPath).toString();
    this->inkscapePath = appSettings.value(Key::InkscapePath).toString();
    this->librsvgPath = appSettings.value(Key::RsvgPath).toString();
}

void Settings::save() const noexcept
{
    QSettings appSettings;
    appSettings.setValue(Key::TestSuite, testSuiteToStr(this->testSuite));
    appSettings.setValue(Key::CustomTestsPath, this->customTestsPath);
    appSettings.setValue(Key::ResvgBuild, buildTypeToStr(this->buildType));
    appSettings.setValue(Key::ViewSize, this->viewSize);
    appSettings.setValue(Key::UseCairoBackend, this->useResvgCairo);
    appSettings.setValue(Key::UseQtBackend, this->useResvgQt);
    appSettings.setValue(Key::UseRaqoteBackend, this->useResvgRaqote);
    appSettings.setValue(Key::UseSkiaBackend, this->useResvgSkia);
    appSettings.setValue(Key::UseChrome, this->useChrome);
    appSettings.setValue(Key::UseFirefox, this->useFirefox);
    appSettings.setValue(Key::UseBatik, this->useBatik);
    appSettings.setValue(Key::UseInkscape, this->useInkscape);
    appSettings.setValue(Key::UseLibrsvg, this->useLibrsvg);
    appSettings.setValue(Key::UseQtSvg, this->useQtSvg);
    appSettings.setValue(Key::ResvgDir, this->resvgDir);
    appSettings.setValue(Key::FirefoxPath, this->firefoxPath);
    appSettings.setValue(Key::BatikPath, this->batikPath);
    appSettings.setValue(Key::InkscapePath, this->inkscapePath);
    appSettings.setValue(Key::RsvgPath, this->librsvgPath);
}

QString Settings::resvgPath() const noexcept
{
    return QString("%1/target/%2/rendersvg").arg(this->resvgDir, buildTypeToStr(this->buildType));
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
        case TestSuite::Own      : path = QString("%1/../../svg").arg(SRCDIR); break;
        case TestSuite::Official : path = QString("%1/../../official_test_suite/svg").arg(SRCDIR); break;
        case TestSuite::Custom   : Q_UNREACHABLE();
    }

    Q_ASSERT(QFile::exists(path));
    return QFileInfo(path).absoluteFilePath();
}
