#pragma once

#include <QVector>
#include <QHash>

class Settings;

enum class TestSuite
{
    Own,
    Custom,
};

QDebug operator<<(QDebug dbg, const TestSuite &t);

enum class Backend
{
    Reference,
    Chrome,
    Firefox,
    Safari,
    Resvg,
    Batik,
    Inkscape,
    Librsvg,
    SvgNet,
    QtSvg,
    Ladybird,
};

QString backendToString(const Backend &t);
QDebug operator<<(QDebug dbg, const Backend &t);

constexpr int BackendsCount = 11;

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
    QString baseName;
    QString title;
    QHash<Backend, TestState> state;
};

class Tests
{
public:
    static Tests load(const TestSuite testSuite, const QString &path, const QString &testsPath);
    static Tests loadCustom(const QString &path);
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
