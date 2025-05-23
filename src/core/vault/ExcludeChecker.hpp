#pragma once

#include <QStringList>
#include <QJsonObject>
#include <QVector>
#include <string>

class ExcludeChecker{
public:
    static ExcludeChecker FromJsonObject(const QJsonObject&);

    QJsonObject ToJsonObject();

    bool GetEnabled() const;
    void SetEnabled(bool newEnabled);
    QStringList GetExactlySameAs() const;
    void SetExactlySameAs(const QStringList &newExactlySameAs);
    QStringList GetIncludes() const;
    void SetIncludes(const QStringList &newIncludes);
    QStringList GetStartsWith() const;
    void SetStartsWith(const QStringList &newStartsWith);
    QStringList GetEndsWith() const;
    void SetEndsWith(const QStringList &newEndsWith);

private:
    bool enabled;
    QVector<std::wstring> ExactlySameAs;
    QVector<std::wstring> Includes;
    QVector<std::wstring> StartsWith;
    QVector<std::wstring> EndsWith;
};







