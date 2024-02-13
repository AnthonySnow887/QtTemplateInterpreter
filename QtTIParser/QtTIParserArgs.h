#ifndef QTTIPARSERARGS_H
#define QTTIPARSERARGS_H

#include <QHash>
#include <QString>
#include <QVariant>

class QtTIParserArgs
{
public:
    QtTIParserArgs() = default;
    ~QtTIParserArgs();

    void appendParam(const QString &paramName, const QVariant &value);
    void removeParam(const QString &paramName);
    bool hasParam(const QString& paramName) const;
    QVariant param(const QString& paramName) const;
    void clearParams();

    void appendTmpParam(const QString& paramName, const QVariant &paramValue);
    void removeTmpParam(const QString& paramName);
    bool hasTmpParam(const QString& paramName) const;
    QVariant tmpParam(const QString& paramName) const;
    void clearTmpParams();

    QVariantList parseHelpFunctionArgs(const QString &args, const QChar &delimiter = QChar(','));
    QVariant prepareHelpFunctionArg(const QString &arg);

private:
    QStringList prepareArrayValues(const QString &arrayStr, const QChar &delimiter = QChar(','), const bool addQuotes = false);
    QMap<QString, QString> prepareMapKeysValues(const QString &mapStr, const QChar &delimiter = QChar(','));
    QPair<QString, QString> prepareMapKeyValue(const QString &mapKeyValueStr, const QString &delimiter = QString(":"));
    QString unescapeStr(const QString &value);

    QVariant paramValueRecursive(const QString &key, const QVariant &parent) const;

private:
    QHash<QString, QVariant> _params;
    QHash<QString, QVariant> _tmpParams;
};

#endif // QTTIPARSERARGS_H
