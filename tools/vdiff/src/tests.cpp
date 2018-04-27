#include <QFile>
#include <QDir>

#include "paths.h"

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

Tests Tests::load(const QString &path)
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

        const auto testPath = items.at(0).toString();

        QString testName = testPath;
        testName.remove(0, 2); // remove prefix
        testName.remove(QRegExp("-[0-9]+\\.svg"));

        TestItem item;
        item.path       = testPath;
        item.fileName   = QFileInfo(testPath).completeBaseName();
        item.name       = testName;
        item.state.insert(Backend::Chrome,      stateFormStr(items.at(1)));
        item.state.insert(Backend::ResvgCairo,  stateFormStr(items.at(2)));
        item.state.insert(Backend::Batik,       stateFormStr(items.at(3)));
        item.state.insert(Backend::Inkscape,    stateFormStr(items.at(4)));
        item.state.insert(Backend::Librsvg,     stateFormStr(items.at(5)));
        item.state.insert(Backend::QtSvg,       stateFormStr(items.at(6)));

        tests.m_data << item;

        row++;
    }

    return tests;
}

void Tests::save(const QString &path)
{
    QString text = "title,chrome,resvg,batik,inkscape,librsvg,qtsvg\n";
    for (const auto &item : m_data) {
        text += item.path + ',';
        text += QString::number((int)item.state.value(Backend::Chrome))     + ',';
        text += QString::number((int)item.state.value(Backend::ResvgCairo)) + ',';
        text += QString::number((int)item.state.value(Backend::Batik))      + ',';
        text += QString::number((int)item.state.value(Backend::Inkscape))   + ',';
        text += QString::number((int)item.state.value(Backend::Librsvg))    + ',';
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
    auto oldTests = load(settings.resultsPath());

    const auto files = QDir(settings.testsPath()).entryInfoList({ "*.svg" });
    for (const auto &fi : files) {
        const auto fileName = fi.fileName();

        bool isExists = false;
        for (const auto &test : oldTests) {
            if (test.path == fileName) {
                isExists = true;
                break;
            }
        }

        if (!isExists) {
            TestItem item;
            item.path = fileName;

            oldTests.m_data << item;
        }
    }

    Tests newTests;

    const auto orderPath = Paths::order();
    QFile orderFile(orderPath);
    if (!orderFile.open(QFile::ReadOnly)) {
        throw QString("Failed to open %1.").arg(orderPath);
    }

    const QString text = orderFile.readAll();
    for (const QStringRef &line : text.splitRef('\n')) {
        if (line.isEmpty()) {
            break;
        }

        for (const auto &test : oldTests) {
            if (test.path == line) {
                newTests.m_data << test;
                break;
            }
        }
    }

//    if (oldTests.size() != newTests.size()) {
//        throw QString("order.txt has a different amount of tests.").arg(orderPath);
//    }

    newTests.save(settings.resultsPath());
}
