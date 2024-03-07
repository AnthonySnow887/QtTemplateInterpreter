#include "QtTIParserNullCoalescingOperator.h"
#include "../BracketsExpr/QtTIParserBracketsExpr.h"
#include "../Logic/QtTIParserLogic.h"
#include "../Math/QtTIParserMath.h"
#include <QRegExp>

//!
//! \brief Check is null-coalescing operator expression
//! \param expr
//! \return
//!
bool QtTIParserNullCoalescingOperator::isNullCoalescingOperatorExpr(const QString &expr)
{
    QRegExp rx("^\\s{0,}([\\w\\.\\,\\+\\-\\/\\%\\*\\(\\)\\ \\_\\'\\\"\\{\\}\\[\\]\\:\\<\\>\\=\\!\\&\\|]+)\\s{1,}\\?\\?\\s{1,}([\\w\\.\\,\\+\\-\\/\\%\\*\\(\\)\\ \\_\\'\\\"\\{\\}\\[\\]\\:\\<\\>\\=\\!\\&\\|]+)\\s{0,}$");
    return (rx.indexIn(expr) != -1);
}

//!
//! \brief Parse null-coalescing operator and exec (with brackets)
//! \param expr
//! \param parserArgs
//! \param parserFunc
//! \param isOk
//! \param error
//! \return
//!
//! === The null-coalescing operator:
//!         {# returns the value of foo if it is defined and not null, 'no' otherwise #}
//!         {{ foo ?? 'no' }}
//!
//! === Brackets support:
//!     Supported grouping expressions with brackets '(...expr...)':
//!         {% set a = 10.5 %}
//!         {% set b = 2 %}
//!         {% set aa = { a:1, b:2, c:3 } %}
//!
//!         {{ ((a + aa.b) > 5 && (aa.b < 2 || aa.c >= 3)) ?? 'no' }} is 'true'
//!         {{ ((a + aa.b) > 5 && (aa.b < 2 || aa.c >= 3)) ?? (a - aa.b) }} is 'true'
//!
QVariant QtTIParserNullCoalescingOperator::parseNullCoalescingOperator(const QString &expr,
                                                                       QtTIParserArgs *parserArgs,
                                                                       QtTIParserFunc *parserFunc,
                                                                       bool *isOk,
                                                                       QString &error)
{
    if (isOk)
        *isOk = false;
    error.clear();
    if (!parserArgs) {
        error = QString("QtTIParserArgs is NULL");
        return QVariant();
    }
    if (!parserFunc) {
        error = QString("QtTIParserFunc is NULL");
        return QVariant();
    }
    if (!QtTIParserNullCoalescingOperator::isNullCoalescingOperatorExpr(expr)) {
        error = QString("Incorrect null-coalescing operator expression '%1'").arg(expr);
        return QVariant();
    }

    QRegExp rx("^\\s{0,}([\\w\\.\\,\\+\\-\\/\\%\\*\\(\\)\\ \\_\\'\\\"\\{\\}\\[\\]\\:\\<\\>\\=\\!\\&\\|]+)\\s{1,}\\?\\?\\s{1,}([\\w\\.\\,\\+\\-\\/\\%\\*\\(\\)\\ \\_\\'\\\"\\{\\}\\[\\]\\:\\<\\>\\=\\!\\&\\|]+)\\s{0,}$");
    if (rx.indexIn(expr) != -1) {
        QString ternCond = rx.cap(1).trimmed();
        QString ternCondFalse = rx.cap(2).trimmed();

        bool calcIsOk = false;
        QVariant ternCondVal;
        std::tie(calcIsOk, ternCondVal, error) = QtTIParserNullCoalescingOperator::evalCond(ternCond, parserArgs, parserFunc);
        if (!calcIsOk) {
            if (isOk)
                *isOk = false;
            return QVariant();
        }

        QVariant ternResultFalse;
        std::tie(calcIsOk, ternResultFalse, error) = QtTIParserNullCoalescingOperator::evalCond(ternCondFalse, parserArgs, parserFunc);
        if (!calcIsOk) {
            if (isOk)
                *isOk = false;
            return QVariant();
        }
        if (isOk)
            *isOk = true;

        if (ternCondVal.type() == QVariant::Bool)
            return (ternCondVal.toBool() ? ternCondVal : ternResultFalse);

        return (!ternCondVal.isNull() ? ternCondVal : ternResultFalse);
    }
    error = QString("Could not recognize null-coalescing operator for expression '%1'").arg(expr);
    return QVariant();
}

//!
//! \brief Evaluate condition
//! \param str
//! \param parserArgs
//! \param parserFunc
//! \return
//!
std::tuple<bool, QVariant, QString> QtTIParserNullCoalescingOperator::evalCond(const QString &str,
                                                                               QtTIParserArgs *parserArgs,
                                                                               QtTIParserFunc *parserFunc)
{
    if (str.isEmpty())
        return std::make_tuple(false, QVariant(), "Eval condition failed (empty string passed)");

    QStringList validStringsBeforeBracket({ "+", "-", "/", "%", "//", "*", "**", "and", "or", "not", "&&", "||" });
    QtTIBracketsNode brNode = QtTIParserBracketsExpr::parseBracketsExpression(str, validStringsBeforeBracket, parserArgs, [parserArgs, parserFunc](const QString &nodeBody) {
        return QtTIParserNullCoalescingOperator::parseParamValue(nodeBody, parserArgs, parserFunc);
    });
    if (!brNode.isValid())
        return std::make_tuple(false, QVariant(), QString("Invalid brackets node for expression '%1'").arg(str));

    return brNode.evalNodeBody();
}

//!
//! \brief Parse parameter value from string
//! \param str Parameter value string view
//! \param parserArgs
//! \param parserFunc
//! \return
//!
std::tuple<bool, QVariant, QString> QtTIParserNullCoalescingOperator::parseParamValue(const QString &str,
                                                                                      QtTIParserArgs *parserArgs,
                                                                                      QtTIParserFunc *parserFunc)
{
    if (str.isEmpty())
        return std::make_tuple(false, QVariant(), "Parse value failed (empty string passed)");

    // logic
    if (QtTIParserLogic::isLogicExpr(str)) {
        bool isOk = false;
        QString error;
        QVariant result = QtTIParserLogic::parseLogic(str, parserArgs, parserFunc, &isOk, error);
        if (!isOk)
            return std::make_tuple(false, QVariant(), error);

        return std::make_tuple(true, result, "");
    }

    // math
    if (QtTIParserMath::isMathExpr(str)) {
        bool isOk = false;
        QString error;
        QVariant result = QtTIParserMath::parseMath(str, parserArgs, &isOk, error);
        if (!isOk)
            return std::make_tuple(false, QVariant(), error);

        return std::make_tuple(true, result, "");
    }

    // function
    QRegExp rxFunc("^(\\s*([\\w]+)\\s*\\(\\s*([A-Za-z0-9_\\ \\+\\-\\*\\,\\.\\'\\\"\\{\\}\\[\\]\\(\\)\\:\\/\\^\\$\\\\]*)\\s*\\)\\s*)");
    if (rxFunc.indexIn(str) != -1) {
        QString funcName = rxFunc.cap(2).trimmed();
        QVariantList funcArgs = parserArgs->parseHelpFunctionArgs(rxFunc.cap(3).trimmed());
        return parserFunc->evalHelpFunction(funcName, funcArgs);
    }

    // arg
    QVariantList tmp = parserArgs->parseHelpFunctionArgs(str);
    if (tmp.isEmpty())
        return std::make_tuple(false, QVariant(), "Parse value failed");
    if (tmp.size() > 1)
        return std::make_tuple(false, QVariant(), "Parse value failed (more than one argument is given)");

    return std::make_tuple(true, tmp[0], "");
}
