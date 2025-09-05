#ifndef QTTIPARSER_H
#define QTTIPARSER_H

#include <QString>
#include <tuple>

#include "Abstract/QtTIAbstractParser.h"
#include "QtTIParserArgs.h"
#include "QtTIParserFunc.h"

class QtTIParser : public QtTIAbstractParser
{
public:
    QtTIParser();
    virtual ~QtTIParser();

    QtTIAbstractParserArgs *parserArgs() final;
    QtTIAbstractParserFunc *parserFunc() final;

    QString removeComments(const QString &line, bool *isMultiline);

    std::tuple<bool/*isOk*/,QString/*res*/,QString/*err*/> parseLine(const QString &line, const int lineNum) final;

    std::tuple<bool/*isOk*/,QString/*res*/,QString/*err*/> parseLine_v2(const QString &line,
                                                                        const int lineNum,
                                                                        QtTIAbstractParserBlock *&block) final;

    std::tuple<bool/*isOk*/,QString/*res*/,QString/*err*/> parseHelpFunctions(const QString &line, const int lineNum);
    QMap<QString, QVariant> parseAndExecHelpFunctions(const QString &line, const int lineNum, bool *isOk, QString &error);

    std::tuple<bool/*isOk*/,QString/*res*/,QString/*err*/> parseHelpParams(const QString &line, const int lineNum);
    QMap<QString, QVariant> parseAndExecHelpParams(const QString &line, const int lineNum, bool *isOk, QString &error);

    std::tuple<bool/*isOk*/,QString/*res*/,QString/*err*/> parseAndExecBlockData(QtTIAbstractParserBlock *block);

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
