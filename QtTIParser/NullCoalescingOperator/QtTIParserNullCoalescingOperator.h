#ifndef QTTIPARSERNULLCOALESCINGOPERATOR_H
#define QTTIPARSERNULLCOALESCINGOPERATOR_H

#include <QString>
#include <QVariant>

#include "../QtTIParserArgs.h"
#include "../QtTIParserFunc.h"

class QtTIParserNullCoalescingOperator
{
public:
    QtTIParserNullCoalescingOperator() = default;
    ~QtTIParserNullCoalescingOperator() = default;

    static bool isNullCoalescingOperatorExpr(const QString &expr);
    static QVariant parseNullCoalescingOperator(const QString &expr,
                                                QtTIParserArgs *parserArgs,
                                                QtTIParserFunc *parserFunc,
                                                bool *isOk,
                                                QString &error);

private:
    static std::tuple<bool/*isOk*/,QVariant/*res*/,QString/*err*/> evalCond(const QString &str,
                                                                            QtTIParserArgs *parserArgs,
                                                                            QtTIParserFunc *parserFunc);

    static std::tuple<bool/*isOk*/,QVariant/*res*/,QString/*err*/> parseParamValue(const QString &str,
                                                                                   QtTIParserArgs *parserArgs,
                                                                                   QtTIParserFunc *parserFunc);
};

#endif // QTTIPARSERNULLCOALESCINGOPERATOR_H
