#ifndef QTTIPARSERTERNARYOPERATOR_H
#define QTTIPARSERTERNARYOPERATOR_H

#include <QString>
#include <QVariant>
#include <QPair>

#include "../Abstract/QtTIAbstractParserArgs.h"
#include "../Abstract/QtTIAbstractParserFunc.h"

class QtTIParserTernaryOperator
{
public:
    QtTIParserTernaryOperator() = default;
    ~QtTIParserTernaryOperator() = default;

    static bool isTernaryOperatorExpr(const QString &expr);
    static QVariant parseTernaryOperator(const QString &expr,
                                         QtTIAbstractParserArgs *parserArgs,
                                         QtTIAbstractParserFunc *parserFunc,
                                         bool *isOk,
                                         QString &error);

private:
    static QPair<QString, QString> parseLeftRight(const QString &str);

    static std::tuple<bool/*isOk*/,QVariant/*res*/,QString/*err*/> evalCond(const QString &str,
                                                                            QtTIAbstractParserArgs *parserArgs,
                                                                            QtTIAbstractParserFunc *parserFunc);

    static std::tuple<bool/*isOk*/,QVariant/*res*/,QString/*err*/> parseParamValue(const QString &str,
                                                                                   QtTIAbstractParserArgs *parserArgs,
                                                                                   QtTIAbstractParserFunc *parserFunc);
};

#endif // QTTIPARSERTERNARYOPERATOR_H
