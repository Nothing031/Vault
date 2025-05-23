#include "ExcludeChecker.hpp"

#include <QJsonArray>

ExcludeChecker ExcludeChecker::FromJsonObject(const QJsonObject& object)
{
    auto ParseArray = [](const QJsonArray& array) -> QVector<std::wstring>{
        QVector<std::wstring> list;
        for (auto& val : std::as_const(array)){
            if (val.isString() && !val.toString().isEmpty()){
                list.append(val.toString().toStdWString());
            }
        }
        return list;
    };

    ExcludeChecker checker;
    checker.enabled = object["ExcludeEnabled"].toBool(false);
    checker.ExactlySameAs = ParseArray(object["ExcludeExactlySameAs"].toArray());
    checker.Includes = ParseArray(object["ExcludeIncludes"].toArray());
    checker.StartsWith = ParseArray(object["ExcludeStartsWith"].toArray());
    checker.EndsWith = ParseArray(object["ExcludeEndsWith"].toArray());
    return checker;
}

QJsonObject ExcludeChecker::ToJsonObject()
{
    auto StringListToArray = [](const QVector<std::wstring>& list) -> QJsonArray{
        QJsonArray arr;
        for (auto& str : std::as_const(list)){
            if (str.size())
                arr.append(QString::fromStdWString(str));
        }
        return arr;
    };

    QJsonObject object;
    object["ExcludeExactlySameAs"] = StringListToArray(ExactlySameAs);
    object["ExcludeIncludes"] = StringListToArray(Includes);
    object["ExcludeStartsWith"] = StringListToArray(StartsWith);
    object["ExcludeEndsWith"] = StringListToArray(EndsWith);
    return object;
}

bool ExcludeChecker::GetEnabled() const { return enabled; }
void ExcludeChecker::SetEnabled(bool newEnabled) { enabled = newEnabled; }
QStringList ExcludeChecker::GetExactlySameAs() const {
    QStringList list;
    list.reserve(ExactlySameAs.size());
    for (auto& str : std::as_const(ExactlySameAs)){
        list.append(QString::fromStdWString(str));
    }
}
void ExcludeChecker::SetExactlySameAs(const QStringList &newExactlySameAs)
{
    ExactlySameAs.clear();
    ExactlySameAs.reserve(newExactlySameAs.size());
    for (auto& str : std::as_const(newExactlySameAs)){
        ExactlySameAs.append(str.toStdWString());
    }
}
QStringList ExcludeChecker::GetIncludes() const {
    QStringList list;
    list.reserve(Includes.size());
    for (const auto& str : std::as_const(Includes)) {
        list.append(QString::fromStdWString(str));
    }
    return list;
}
void ExcludeChecker::SetIncludes(const QStringList &newIncludes) {
    Includes.clear();
    Includes.reserve(newIncludes.size());
    for (const auto& str : std::as_const(newIncludes)) {
        Includes.append(str.toStdWString());
    }
}
QStringList ExcludeChecker::GetStartsWith() const {
    QStringList list;
    list.reserve(StartsWith.size());
    for (const auto& str : std::as_const(StartsWith)) {
        list.append(QString::fromStdWString(str));
    }
    return list;
}
void ExcludeChecker::SetStartsWith(const QStringList &newStartsWith) {
    StartsWith.clear();
    StartsWith.reserve(newStartsWith.size());
    for (const auto& str : std::as_const(newStartsWith)) {
        StartsWith.append(str.toStdWString());
    }
}
QStringList ExcludeChecker::GetEndsWith() const {
    QStringList list;
    list.reserve(EndsWith.size());
    for (const auto& str : std::as_const(EndsWith)) {
        list.append(QString::fromStdWString(str));
    }
    return list;
}
void ExcludeChecker::SetEndsWith(const QStringList &newEndsWith) {
    EndsWith.clear();
    EndsWith.reserve(newEndsWith.size());
    for (const auto& str : std::as_const(newEndsWith)) {
        EndsWith.append(str.toStdWString());
    }
}

