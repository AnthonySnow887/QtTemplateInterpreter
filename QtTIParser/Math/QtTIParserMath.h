#ifndef QTTIPARSERMATH_H
#define QTTIPARSERMATH_H

#include <QString>
#include <QVariant>

#include "../QtTIParserArgs.h"

class QtTIParserMath
{
public:
    QtTIParserMath() = default;
    ~QtTIParserMath() = default;

    static bool isMathExpr(const QString &expr);
    static QVariant parseMath(const QString &expr, QtTIParserArgs *parserArgs, bool *isOk, QString &error);

private:
    static QVariant parseMathWithoutBrackets(const QString &expr, QtTIParserArgs *parserArgs, bool *isOk, QString &error);

    static std::tuple<bool/*isOk*/,QVariant/*res*/,QString/*err*/> calcMathOperation(const QVariant &left, const QVariant &right, const QString &op);

    template<typename T>
    static std::tuple<bool/*isOk*/,QVariant/*res*/,QString/*err*/> calcMathOperation_t_int(const T &left, const T &right, const QString &op);

    template<typename T>
    static std::tuple<bool/*isOk*/,QVariant/*res*/,QString/*err*/> calcMathOperation_t_real(const T &left, const T &right, const QString &op);
};

#endif // QTTIPARSERMATH_H
