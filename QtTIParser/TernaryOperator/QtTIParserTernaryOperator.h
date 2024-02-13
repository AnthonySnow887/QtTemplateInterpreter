#ifndef QTTIPARSERTERNARYOPERATOR_H
#define QTTIPARSERTERNARYOPERATOR_H

#include <QString>
#include <QVariant>
#include <QPair>

#include "../QtTIParserArgs.h"
#include "../QtTIParserFunc.h"

class QtTIParserTernaryOperator
{
public:
    QtTIParserTernaryOperator() = default;
    ~QtTIParserTernaryOperator() = default;

    static bool isTernaryOperatorExpr(const QString &expr);
    static QVariant parseTernaryOperator(const QString &expr,
                                         QtTIParserArgs *parserArgs,
                                         QtTIParserFunc *parserFunc,
                                         bool *isOk,
                                         QString &error);

private:
    static QPair<QString, QString> parseLeftRight(const QString &str);

    static std::tuple<bool/*isOk*/,QVariant/*res*/,QString/*err*/> evalCond(const QString &str,
                                                                            QtTIParserArgs *parserArgs,
                                                                            QtTIParserFunc *parserFunc);

    static std::tuple<bool/*isOk*/,QVariant/*res*/,QString/*err*/> parseParamValue(const QString &str,
                                                                                   QtTIParserArgs *parserArgs,
                                                                                   QtTIParserFunc *parserFunc);
};

#endif // QTTIPARSERTERNARYOPERATOR_H
