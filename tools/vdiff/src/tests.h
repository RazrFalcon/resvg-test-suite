#pragma once

#include <QVector>
#include <QHash>

#include "settings.h"

enum class Backend
{
    Chrome,
    ResvgCairo,
    ResvgQt,
    Inkscape,
    librsvg,
    QtSvg,
};

Q_DECL_PURE_FUNCTION inline uint qHash(const Backend &key, uint seed = 0)
{ return qHash((uint)key, seed); }

enum class TestState
{
    Unknown,
    Passed,
    Failed,
    Crashed,
};

struct TestItem
{
    QString path;
    QString fileName;
    QString name;
    QHash<Backend, TestState> state;
};

class Tests
{
public:
    static Tests load(const QString &path);
    void save(const QString &path);

    static void resync(const Settings &settings);

    QVector<TestItem>::const_iterator begin() const { return m_data.begin(); }
    QVector<TestItem>::const_iterator end() const { return m_data.end(); }

    TestItem& at(int row) { return m_data[row]; }
    const TestItem& at(int row) const { return m_data.at(row); }

    int size() const { return m_data.size(); }

private:
    QVector<TestItem> m_data;
};
