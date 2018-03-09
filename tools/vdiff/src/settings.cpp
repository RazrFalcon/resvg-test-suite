#include <QSettings>
#include <QFileInfo>

#include "settings.h"

namespace Key {
    static const QString TestSuite = "TestSuite";
    static const QString ResvgBuild = "ResvgBuild";
    static const QString ResvgDir = "ResvgDir";
    static const QString InkscapePath = "InkscapePath";
    static const QString RsvgPath = "RsvgPath";
}

static QString testSuiteToStr(TestSuite t)
{
    switch (t) {
        case TestSuite::Own      : return "results";
        case TestSuite::Official : return "official";
    }
}

static QString buildTypeToStr(BuildType t)
{
    switch (t) {
        case BuildType::Debug   : return "debug";
        case BuildType::Release : return "release";
    }
}

Settings Settings::load()
{
    QSettings appSettings;
    Settings d;

    d.testSuite = appSettings.value(Key::TestSuite).toString() == "official"
                    ? TestSuite::Official
                    : TestSuite::Own;

    d.buildType = appSettings.value(Key::ResvgBuild).toString() == "release"
                    ? BuildType::Release
                    : BuildType::Debug;

    d.resvgDir = appSettings.value(Key::ResvgDir).toString();
    d.inkscapePath = appSettings.value(Key::InkscapePath).toString();
    d.librsvgPath = appSettings.value(Key::RsvgPath).toString();

    return d;
}

void Settings::save()
{
    QSettings appSettings;
    appSettings.setValue(Key::TestSuite, testSuiteToStr(this->testSuite));
    appSettings.setValue(Key::ResvgBuild, buildTypeToStr(this->buildType));
    appSettings.setValue(Key::ResvgDir, this->resvgDir);
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
    }

    Q_ASSERT(QFile::exists(path));
    return QFileInfo(path).absoluteFilePath();
}

QString Settings::testPath(const QString &fileName) const noexcept
{
    return testsPath() + '/' + fileName;
}
