#ifndef QTTIPARSERNULLCOALESCINGOPERATOR_H
#define QTTIPARSERNULLCOALESCINGOPERATOR_H

#include <QString>
#include <QVariant>

#include "../Abstract/QtTIAbstractParserArgs.h"
#include "../Abstract/QtTIAbstractParserFunc.h"

class QtTIParserNullCoalescingOperator
{
public:
    QtTIParserNullCoalescingOperator() = default;
    ~QtTIParserNullCoalescingOperator() = default;

    static bool isNullCoalescingOperatorExpr(const QString &expr);
    static QVariant parseNullCoalescingOperator(const QString &expr,
                                                QtTIAbstractParserArgs *parserArgs,
                                                QtTIAbstractParserFunc *parserFunc,
                                                bool *isOk,
                                                QString &error);

private:
    static std::tuple<bool/*isOk*/,QVariant/*res*/,QString/*err*/> evalCond(const QString &str,
                                                                            QtTIAbstractParserArgs *parserArgs,
                                                                            QtTIAbstractParserFunc *parserFunc);

    static std::tuple<bool/*isOk*/,QVariant/*res*/,QString/*err*/> parseParamValue(const QString &str,
                                                                                   QtTIAbstractParserArgs *parserArgs,
                                                                                   QtTIAbstractParserFunc *parserFunc);
};

#endif // QTTIPARSERNULLCOALESCINGOPERATOR_H
