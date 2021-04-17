#include <QFile>
#include <QDir>
#include <QDirIterator>
#include <QXmlStreamReader>
#include <QDebug>

#include "paths.h"
#include "settings.h"

#include "tests.h"

static TestState stateFormStr(const QStringRef &str)
{
    bool ok = false;
    const int idx = str.toInt(&ok);

    if (!ok) {
        throw QString("Invalid state ID: '%1'").arg(str.toString());
    }

    switch (idx) {
        case 0 : return TestState::Unknown;
        case 1 : return TestState::Passed;
        case 2 : return TestState::Failed;
        case 3 : return TestState::Crashed;
        default: {
            throw QString("Invalid state ID: %1").arg(idx);
        }
    }
}

static QString parseTitle(const QString &path)
{
    QFile file(path);
    if (!file.open(QFile::ReadOnly)) {
        return QString();
    }

    QString title;
    QXmlStreamReader reader(&file);
    while (!reader.atEnd() && !reader.hasError()) {
        if (reader.readNextStartElement()) {
            if (reader.name() == "title") {
                reader.readNext();
                title = reader.text().toString();
                break;
            }
        }
    }

    return title;
}

Tests Tests::load(const TestSuite testSuite, const QString &path, const QString &testsPath)
{
    QFile file(path);
    if (!file.open(QFile::ReadOnly)) {
        throw QString("Failed to open %1.").arg(path);
    }

    const QString text = file.readAll();

    // A minimal csv parser.
    //
    // We don't care about escape characters, because they are not used.

    Tests tests;

    int row = 1;
    for (const QStringRef &line : text.splitRef('\n')) {
        // Skip title.
        if (row == 1) {
            row++;
            continue;
        }

        if (line.isEmpty()) {
            break;
        }

        const auto items = line.split(',');

        if (items.size() != BackendsCount) {
            throw QString("Invalid columns count at row %1.").arg(row);
        }

        const auto testPath = testsPath + '/' + items.at(0).toString();

        TestItem item;
        item.path     = QFileInfo(testPath).absoluteFilePath();
        item.baseName = QFileInfo(testPath).completeBaseName();

        item.state.insert(Backend::Chrome,      stateFormStr(items.at(1)));
        item.state.insert(Backend::Firefox,     stateFormStr(items.at(2)));
        item.state.insert(Backend::Resvg,       stateFormStr(items.at(3)));
        item.state.insert(Backend::Batik,       stateFormStr(items.at(4)));
        item.state.insert(Backend::Inkscape,    stateFormStr(items.at(5)));
        item.state.insert(Backend::Librsvg,     stateFormStr(items.at(6)));
        item.state.insert(Backend::SvgNet,      stateFormStr(items.at(7)));
        item.state.insert(Backend::WxSvg,       stateFormStr(items.at(8)));
        item.state.insert(Backend::QtSvg,       stateFormStr(items.at(9)));

        if (testSuite == TestSuite::Own) {
            item.title = parseTitle(testPath);
        }

        tests.m_data << item;

        row++;
    }

    return tests;
}

Tests Tests::loadCustom(const QString &path)
{
    Tests tests;

    static const QStringList filesFilter = { "*.svg", "*.svgz" };

    QStringList paths;
    QDirIterator it(path, filesFilter, QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks,
                    QDirIterator::Subdirectories);
    while (it.hasNext()) {
        paths << it.next();
    }

    paths.sort();

    for (const QString &filePath : paths) {
        TestItem item;
        item.path     = filePath;
        item.baseName = QDir(path).relativeFilePath(filePath);
        tests.m_data << item;
    }

    return tests;
}

void Tests::save(const QString &path)
{
    QString text = "title,chrome,firefox,resvg,batik,inkscape,librsvg,svgnet,wxsvg,qtsvg\n";
    for (const TestItem &item : m_data) {
        text += QFileInfo(item.path).fileName() + ',';
        text += QString::number((int)item.state.value(Backend::Chrome))     + ',';
        text += QString::number((int)item.state.value(Backend::Firefox))    + ',';
        text += QString::number((int)item.state.value(Backend::Resvg))      + ',';
        text += QString::number((int)item.state.value(Backend::Batik))      + ',';
        text += QString::number((int)item.state.value(Backend::Inkscape))   + ',';
        text += QString::number((int)item.state.value(Backend::Librsvg))    + ',';
        text += QString::number((int)item.state.value(Backend::SvgNet))     + ',';
        text += QString::number((int)item.state.value(Backend::WxSvg))      + ',';
        text += QString::number((int)item.state.value(Backend::QtSvg))      + '\n';
    }

    QFile file(path);
    if (!file.open(QFile::WriteOnly)) {
        throw QString("Failed to open %1.").arg(path);
    }

    file.write(text.toUtf8());
}

void Tests::resync(const Settings &settings)
{
    auto oldTests = load(settings.testSuite, settings.resultsPath(), settings.testsPath());

    const auto files = QDir(settings.testsPath()).entryInfoList({ "*.svg" });
    for (const QFileInfo &fi : files) {
        const auto baseName = fi.completeBaseName();

        bool isExists = false;
        for (const TestItem &test : oldTests) {
            if (test.baseName == baseName) {
                isExists = true;
                break;
            }
        }

        if (!isExists) {
            TestItem item;
            item.path = fi.absoluteFilePath();

            oldTests.m_data << item;
        }
    }

    Tests newTests;

    const auto orderPath = Paths::order();
    QFile orderFile(orderPath);
    if (!orderFile.open(QFile::ReadOnly)) {
        throw QString("Failed to open %1.").arg(orderPath);
    }

    const QString pngDir = QFileInfo(settings.testsPath() + "/../png").absoluteFilePath();

    const QString text = orderFile.readAll();
    for (const QStringRef &line : text.splitRef('\n')) {
        if (line.isEmpty()) {
            break;
        }

        const auto pngPath = pngDir + "/" + QFileInfo(line.toString()).completeBaseName() + ".png";
        if (!QFile::exists(pngPath)) {
            throw QString("'%1' not found.").arg(pngPath);
        }

        for (const TestItem &test : oldTests) {
            if (QFileInfo(test.path).fileName() == line) {
                newTests.m_data << test;
                break;
            }
        }
    }

    newTests.save(settings.resultsPath());
}

static QString testSuiteToString(const TestSuite &t)
{
    switch (t) {
        case TestSuite::Own :       return "Own";
        case TestSuite::Custom :    return "Custom";
    }

    Q_UNREACHABLE();
}

QDebug operator<<(QDebug dbg, const TestSuite &t)
{
    return dbg << QString("TestSuite(%1)").arg(testSuiteToString(t));
}

QString backendToString(const Backend &t)
{
    switch (t) {
        case Backend::Reference :   return "Reference";
        case Backend::Chrome :      return "Chrome";
        case Backend::Firefox :     return "Firefox";
        case Backend::Resvg :       return "resvg";
        case Backend::Batik :       return "Batik";
        case Backend::Inkscape :    return "Inkscape";
        case Backend::Librsvg :     return "librsvg";
        case Backend::SvgNet :      return "SVG.NET";
        case Backend::WxSvg :       return "wxSVG";
        case Backend::QtSvg :       return "QtSvg";
    }

    Q_UNREACHABLE();
}

QDebug operator<<(QDebug dbg, const Backend &t)
{
    return dbg << QString("Backend(%1)").arg(backendToString(t));
}
