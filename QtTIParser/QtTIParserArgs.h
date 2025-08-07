#ifndef QTTIPARSERARGS_H
#define QTTIPARSERARGS_H

#include <QHash>
#include <QString>
#include <QVariant>

class QtTIParser;
class QtTIParserFunc;

class QtTIParserArgs
{
    friend class QtTIParser;

public:
    QtTIParserArgs();
    QtTIParserArgs(QtTIParserFunc *parserFunc);
    ~QtTIParserArgs();

    void appendParam(const QString &paramName, const QVariant &value);
    void removeParam(const QString &paramName);
    bool hasParam(const QString& paramName);
    QVariant param(const QString& paramName);
    void clearParams();

    void appendTmpParam(const QString& paramName, const QVariant &paramValue);
    void removeTmpParam(const QString& paramName);
    bool hasTmpParam(const QString& paramName);
    QVariant tmpParam(const QString& paramName);
    void clearTmpParams();

    QVariantList parseHelpFunctionArgs(const QString &args, const QChar &delimiter = QChar(','));
    QVariant prepareHelpFunctionArg(const QString &arg);

protected:
    void setTIParserFunc(QtTIParserFunc *parserFunc) {
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
    QtTIParserFunc *_parserFunc {nullptr};
    QHash<QString, QVariant> _params;
    QHash<QString, QVariant> _tmpParams;
};

#endif // QTTIPARSERARGS_H
