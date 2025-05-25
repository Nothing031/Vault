#pragma once

#include <QStringList>
#include <QJsonObject>
#include <QVector>
#include <string>

class ExcludeChecker{
public:
    static ExcludeChecker FromJsonObject(const QJsonObject&);

    QJsonObject ToJsonObject();

    bool Enabled() const;
    void SetEnabled(bool newEnabled);
    QStringList ExactlySameAs() const;
    void SetExactlySameAs(const QStringList &newExactlySameAs);
    QStringList Includes() const;
    void SetIncludes(const QStringList &newIncludes);
    QStringList StartsWith() const;
    void SetStartsWith(const QStringList &newStartsWith);
    QStringList EndsWith() const;
    void SetEndsWith(const QStringList &newEndsWith);

private:
    bool enabled;
    QVector<std::wstring> exactlySameAs;
    QVector<std::wstring> includes;
    QVector<std::wstring> startsWith;
    QVector<std::wstring> endsWith;
};







