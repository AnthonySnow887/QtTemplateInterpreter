#ifndef QTTIABSTRACTPARSERARGS_H
#define QTTIABSTRACTPARSERARGS_H

#include <QString>
#include <QVariant>

class QtTIAbstractParserArgs
{
public:
    QtTIAbstractParserArgs() = default;
    virtual ~QtTIAbstractParserArgs() = default;

    virtual void appendParam(const QString &paramName, const QVariant &value) = 0;
    virtual void removeParam(const QString &paramName) = 0;
    virtual bool hasParam(const QString& paramName) = 0;
    virtual QVariant param(const QString& paramName) = 0;
    virtual void clearParams() = 0;

    virtual void appendTmpParam(const QString& paramName, const QVariant &paramValue) = 0;
    virtual void removeTmpParam(const QString& paramName) = 0;
    virtual bool hasTmpParam(const QString& paramName) = 0;
    virtual QVariant tmpParam(const QString& paramName) = 0;
    virtual void clearTmpParams() = 0;

    virtual QVariantList parseHelpFunctionArgs(const QString &args, const QChar &delimiter = QChar(',')) = 0;
    virtual QVariant prepareHelpFunctionArg(const QString &arg) = 0;
};

#endif // QTTIABSTRACTPARSERARGS_H
