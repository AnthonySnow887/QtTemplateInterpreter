#ifndef QTTIPARSERLOGIC_H
#define QTTIPARSERLOGIC_H

#include <QString>
#include <QVariant>

#include "../Abstract/QtTIAbstractParserArgs.h"
#include "../Abstract/QtTIAbstractParserFunc.h"

class QtTIParserLogic
{
public:
    enum class LogicalOperation {
        Invalid = 0,
        Equally,       //!< равно (==)
        NotEqually,    //!< не равно (!=)
        Larger,        //!< больше (>)
        Less,          //!< меньше (<)
        LargerOrEqual, //!< больше или равно (>=)
        LessOrEqual,   //!< меньше или равно (<=)
    };

    QtTIParserLogic() = default;
    ~QtTIParserLogic() = default;

    static bool isLogicExpr(const QString &expr);
    static QVariant parseLogic(const QString &expr,
                               QtTIAbstractParserArgs *parserArgs,
                               QtTIAbstractParserFunc *parserFunc,
                               bool *isOk,
                               QString &error);

private:
    static QVariant parseLogicWithoutBrackets(const QString &expr,
                                              QtTIAbstractParserArgs *parserArgs,
                                              QtTIAbstractParserFunc *parserFunc,
                                              bool *isOk,
                                              QString &error);

    static std::tuple<bool/*isOk*/,bool/*res*/,QString/*err*/> compareBlockCondition(const QString &str,
                                                                                     QtTIAbstractParserArgs *parserArgs,
                                                                                     QtTIAbstractParserFunc *parserFunc);

    static std::tuple<bool/*isOk*/,bool/*res*/,QString/*err*/> compareExpression(const QString &str,
                                                                                 QtTIAbstractParserArgs *parserArgs,
                                                                                 QtTIAbstractParserFunc *parserFunc);

    static std::tuple<bool/*isOk*/,QVariant/*res*/,QString/*err*/> parseParamValue(const QString &str,
                                                                                   QtTIAbstractParserArgs *parserArgs,
                                                                                   QtTIAbstractParserFunc *parserFunc);

    static bool compare(const QVariant &left);
    static bool compare(const QVariant &left, const QVariant &right, const QString &cond);

    template<typename T>
    static bool compare(const T &left,
                        const T &right,
                        const LogicalOperation &operation);

    static LogicalOperation strToOperation(const QString &cond);
};

#endif // QTTIPARSERLOGIC_H
