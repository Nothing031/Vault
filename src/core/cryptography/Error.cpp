#include "Error.hpp"

Error::Error() : m_code(NO_ERROR), m_what(""), m_path(""){}

Error::Error(const ErrorCode code, const QString what, const QString path) : m_code(code), m_what(what), m_path(path){}

void Error::Set(const ErrorCode code, const QString what, const QString path){
    m_code = code;
    m_what = what;
    m_path = path;
}

const Error::ErrorCode& Error::code(){
    return m_code;
}

const QString& Error::what(){
    return m_what;
}

const QString& Error::path(){
    return m_path;
}

