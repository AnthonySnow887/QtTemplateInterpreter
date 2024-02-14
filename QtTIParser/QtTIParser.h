#ifndef QTTIPARSER_H
#define QTTIPARSER_H

#include <QString>
#include <tuple>

#include "QtTIParserArgs.h"
#include "QtTIParserFunc.h"
#include "../QtTIHelperFunction/QtTIHelperFunction.h"

class QtTIParser
{
public:
    QtTIParser();
    ~QtTIParser();

    QtTIParserArgs *parserArgs();
    QtTIParserFunc *parserFunc();

    QString removeComments(const QString &line, bool *isMultiline);

    std::tuple<bool/*isOk*/,QString/*res*/,QString/*err*/> parseLine(const QString &line, const int lineNum);

    std::tuple<bool/*isOk*/,QString/*res*/,QString/*err*/> parseHelpFunctions(const QString &line, const int lineNum);
    QMap<QString, QVariant> parseAndExecHelpFunctions(const QString &line, const int lineNum, bool *isOk, QString &error);

    std::tuple<bool/*isOk*/,QString/*res*/,QString/*err*/> parseHelpParams(const QString &line, const int lineNum);
    QMap<QString, QVariant> parseAndExecHelpParams(const QString &line, const int lineNum, bool *isOk, QString &error);

private:
    QString evalHelpParam(const QString& paramName);
    QString evalHelpTmpParam(const QString& paramName);

    std::tuple<bool/*isOk*/,QVariant/*res*/,QString/*err*/> evalHelpFunction(const QString& funcName, const QVariantList &args) const;

    QString validParamExpr(const QString &str, const QString &ending) const;

private:
    QtTIParserArgs _parserArgs;
    QtTIParserFunc _parserFunc;
};

#endif // QTTIPARSER_H
