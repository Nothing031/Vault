#include "ExcludeChecker.hpp"

#include <QJsonArray>

#ifndef NO_EXCLUDE_CHECKER

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
    checker.exactlySameAs = ParseArray(object["ExcludeExactlySameAs"].toArray());
    checker.includes = ParseArray(object["ExcludeIncludes"].toArray());
    checker.startsWith = ParseArray(object["ExcludeStartsWith"].toArray());
    checker.endsWith = ParseArray(object["ExcludeEndsWith"].toArray());
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
    object["ExcludeExactlySameAs"] = StringListToArray(exactlySameAs);
    object["ExcludeIncludes"] = StringListToArray(includes);
    object["ExcludeStartsWith"] = StringListToArray(startsWith);
    object["ExcludeEndsWith"] = StringListToArray(endsWith);
    return object;
}
bool ExcludeChecker::Enabled() const { return enabled; }
void ExcludeChecker::SetEnabled(bool newEnabled) { enabled = newEnabled; }
QStringList ExcludeChecker::ExactlySameAs() const {
    QStringList list;
    list.reserve(exactlySameAs.size());
    for (auto& str : std::as_const(exactlySameAs)){
        list.append(QString::fromStdWString(str));
    }
    return list;
}
void ExcludeChecker::SetExactlySameAs(const QStringList &newExactlySameAs)
{
    exactlySameAs.clear();
    exactlySameAs.reserve(newExactlySameAs.size());
    for (auto& str : std::as_const(newExactlySameAs)){
        exactlySameAs.append(str.toStdWString());
    }
}
QStringList ExcludeChecker::Includes() const {
    QStringList list;
    list.reserve(includes.size());
    for (const auto& str : std::as_const(includes)) {
        list.append(QString::fromStdWString(str));
    }
    return list;
}
void ExcludeChecker::SetIncludes(const QStringList &newIncludes) {
    includes.clear();
    includes.reserve(newIncludes.size());
    for (const auto& str : std::as_const(newIncludes)) {
        includes.append(str.toStdWString());
    }
}
QStringList ExcludeChecker::StartsWith() const {
    QStringList list;
    list.reserve(startsWith.size());
    for (const auto& str : std::as_const(startsWith)) {
        list.append(QString::fromStdWString(str));
    }
    return list;
}
void ExcludeChecker::SetStartsWith(const QStringList &newStartsWith) {
    startsWith.clear();
    startsWith.reserve(newStartsWith.size());
    for (const auto& str : std::as_const(newStartsWith)) {
        startsWith.append(str.toStdWString());
    }
}
QStringList ExcludeChecker::EndsWith() const {
    QStringList list;
    list.reserve(endsWith.size());
    for (const auto& str : std::as_const(endsWith)) {
        list.append(QString::fromStdWString(str));
    }
    return list;
}
void ExcludeChecker::SetEndsWith(const QStringList &newEndsWith) {
    endsWith.clear();
    endsWith.reserve(newEndsWith.size());
    for (const auto& str : std::as_const(newEndsWith)) {
        endsWith.append(str.toStdWString());
    }
}
bool ExcludeChecker::Checker(const std::filesystem::path &path)
{
    // exactlySameAs
    for (auto& str : std::as_const(exactlySameAs)){

    }

    // includes
    for (auto& str : std::as_const(includes)){

    }
    // includes
    for (auto& str : std::as_const(includes)){
        startsWith;
    }
    // includes
    for (auto& str : std::as_const(includes)){
        endsWith;
    }




}

#endif
