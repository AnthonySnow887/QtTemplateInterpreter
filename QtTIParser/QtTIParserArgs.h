#ifndef QTTIPARSERARGS_H
#define QTTIPARSERARGS_H

#include <QHash>

#include "Abstract/QtTIAbstractParserArgs.h"
#include "Abstract/QtTIAbstractParserFunc.h"

class QtTIParserArgs : public QtTIAbstractParserArgs
{
    friend class QtTIParser;

public:
    QtTIParserArgs();
    QtTIParserArgs(QtTIAbstractParserFunc *parserFunc);
    virtual ~QtTIParserArgs();

    void appendParam(const QString &paramName, const QVariant &value) final;
    void removeParam(const QString &paramName) final;
    bool hasParam(const QString& paramName) final;
    QVariant param(const QString& paramName) final;
    void clearParams() final;

    void appendTmpParam(const QString& paramName, const QVariant &paramValue) final;
    void removeTmpParam(const QString& paramName) final;
    bool hasTmpParam(const QString& paramName) final;
    QVariant tmpParam(const QString& paramName) final;
    void clearTmpParams() final;

    QVariantList parseHelpFunctionArgs(const QString &args, const QChar &delimiter = QChar(',')) final;
    QVariant prepareHelpFunctionArg(const QString &arg) final;

protected:
    void setTIParserFunc(QtTIAbstractParserFunc *parserFunc) {
        _parserFunc = parserFunc;
    }

private:
    QStringList prepareArrayValues(const QString &arrayStr, const QChar &delimiter = QChar(','), const bool addQuotes = false);
    QMap<QString, QString> prepareMapKeysValues(const QString &mapStr, const QChar &delimiter = QChar(','));
    QPair<QString, QString> prepareMapKeyValue(const QString &mapKeyValueStr, const QString &delimiter = QString(":"));
    QString unescapeStr(const QString &value);

    QVariant paramValueRecursive(const QString &key, const QVariant &parent);
    QVariant evalParamMethod(QObject *object, const QMetaObject *mObj, const QString &funcName, const QVariantList &funcArgs);
    QVariant evalParamMethod(void *object, const QMetaObject *mObj, const QString &funcName, const QVariantList &funcArgs);

private:
    QtTIAbstractParserFunc *_parserFunc {nullptr};
    QHash<QString, QVariant> _params;
    QHash<QString, QVariant> _tmpParams;
};

#endif // QTTIPARSERARGS_H
