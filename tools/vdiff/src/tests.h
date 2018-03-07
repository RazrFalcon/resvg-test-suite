#pragma once

#include <QVector>

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
    TestState chrome    = TestState::Unknown;
    TestState resvg     = TestState::Unknown;
    TestState inkscape  = TestState::Unknown;
    TestState librsvg   = TestState::Unknown;
    TestState qtsvg     = TestState::Unknown;
};

class Tests
{
public:
    static Tests load();
    void save();

    static void resync();

    QVector<TestItem>::const_iterator begin() const { return m_data.begin(); }
    QVector<TestItem>::const_iterator end() const { return m_data.end(); }

    TestItem& at(int row) { return m_data[row]; }
    const TestItem& at(int row) const { return m_data.at(row); }

    int size() const { return m_data.size(); }

private:
    QVector<TestItem> m_data;
};
