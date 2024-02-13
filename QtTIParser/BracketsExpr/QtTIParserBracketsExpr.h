#ifndef QTTIPARSERBRACKETSEXPR_H
#define QTTIPARSERBRACKETSEXPR_H

#include <QString>
#include <QStringList>

#include "QtTIBracketsNode.h"

class QtTIParserBracketsExpr
{
public:
    QtTIParserBracketsExpr() = default;
    ~QtTIParserBracketsExpr() = default;

    static QtTIBracketsNode parseBracketsExpression(const QString &string,
                                                    const QStringList &validStringsBeforeBracket,
                                                    QtTIParserArgs *parserArgs,
                                                    std::function<std::tuple<bool/*isOk*/,QVariant/*res*/,QString/*err*/>(const QString &nodeBody)> callback);
};

#endif // QTTIPARSERBRACKETSEXPR_H
