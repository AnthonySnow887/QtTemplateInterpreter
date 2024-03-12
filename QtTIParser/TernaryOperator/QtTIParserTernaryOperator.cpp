#include "QtTIParserTernaryOperator.h"
#include "../BracketsExpr/QtTIParserBracketsExpr.h"
#include "../Logic/QtTIParserLogic.h"
#include "../Math/QtTIParserMath.h"
#include <QRegExp>

//!
//! \brief Check is ternary operator expression
//! \param expr
//! \return
//!
bool QtTIParserTernaryOperator::isTernaryOperatorExpr(const QString &expr)
{
    QRegExp rx("^\\s{0,}([\\w\\.\\,\\+\\-\\/\\%\\*\\(\\)\\ \\_\\'\\\"\\{\\}\\[\\]\\:\\<\\>\\=\\!\\&\\|]+)\\s{1,}\\?\\s{1,}(([\\w\\.\\,\\+\\-\\/\\%\\*\\(\\)\\ \\_\\'\\\"\\{\\}\\[\\]\\:\\<\\>\\=\\!\\&\\|]+)\\s{1,}\\:\\s{1,}([\\w\\.\\,\\+\\-\\/\\%\\*\\(\\)\\ \\_\\'\\\"\\{\\}\\[\\]\\:\\<\\>\\=\\!\\&\\|]+)\\s{0,})$");
    QRegExp rx_2("^\\s{0,}([\\w\\.\\,\\+\\-\\/\\%\\*\\(\\)\\ \\_\\'\\\"\\{\\}\\[\\]\\:\\<\\>\\=\\!\\&\\|]+)\\s{1,}\\?\\:\\s{1,}([\\w\\.\\,\\+\\-\\/\\%\\*\\(\\)\\ \\_\\'\\\"\\{\\}\\[\\]\\:\\<\\>\\=\\!\\&\\|]+)\\s{0,}$");
    QRegExp rx_3("^\\s{0,}([\\w\\.\\,\\+\\-\\/\\%\\*\\(\\)\\ \\_\\'\\\"\\{\\}\\[\\]\\:\\<\\>\\=\\!\\&\\|]+)\\s{1,}\\?\\s{1,}([\\w\\.\\,\\+\\-\\/\\%\\*\\(\\)\\ \\_\\'\\\"\\{\\}\\[\\]\\:\\<\\>\\=\\!\\&\\|]+)\\s{0,}$");
    return ((rx.indexIn(expr) != -1)
            || (rx_2.indexIn(expr) != -1)
            || (rx_3.indexIn(expr) != -1));
}

//!
//! \brief Parse ternary operator and exec (with brackets)
//! \param expr
//! \param parserArgs
//! \param parserFunc
//! \param isOk
//! \param error
//! \return
//!
//! === Ternary operator:
//!         {{ foo ? 'yes' : 'no' }}
//!         {{ foo ?: 'no' }} is the same as {{ foo ? foo : 'no' }}
//!         {{ foo ? 'yes' }} is the same as {{ foo ? 'yes' : '' }}
//!
//! === Brackets support:
//!     Supported grouping expressions with brackets '(...expr...)':
//!         {% set a = 10.5 %}
//!         {% set b = 2 %}
//!         {% set aa = { a:1, b:2, c:3 } %}
//!
//!         {{ ((a + aa.b) > 5 && (aa.b < 2 || aa.c >= 3)) ? 'yes' : 'no' }} is 'yes'
//!         {{ ((a + aa.b) > 5 && (aa.b < 2 || aa.c >= 3)) ? (a + aa.b) : (a - aa.b) }} is 12.5
//!
//!         {{ ((a + aa.b) > 5 && (aa.b < 2 || aa.c >= 3)) ?: 'no' }} is true
//!         {{ ((a + aa.b) > 5 && (aa.b < 2 || aa.c >= 3)) ?: (a - aa.b) }} is true
//!
//!         {{ ((a + aa.b) > 5 && (aa.b < 2 || aa.c >= 3)) ? 'yes' }} is 'yes'
//!         {{ ((a + aa.b) > 5 && (aa.b < 2 || aa.c >= 3)) ? (a + aa.b) }} is 12.5
//!
QVariant QtTIParserTernaryOperator::parseTernaryOperator(const QString &expr,
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
    if (!QtTIParserTernaryOperator::isTernaryOperatorExpr(expr)) {
        error = QString("Incorrect ternary operator expression '%1'").arg(expr);
        return QVariant();
    }

    QRegExp rx("^\\s{0,}([\\w\\.\\,\\+\\-\\/\\%\\*\\(\\)\\ \\_\\'\\\"\\{\\}\\[\\]\\:\\<\\>\\=\\!\\&\\|]+)\\s{1,}\\?\\s{1,}(([\\w\\.\\,\\+\\-\\/\\%\\*\\(\\)\\ \\_\\'\\\"\\{\\}\\[\\]\\:\\<\\>\\=\\!\\&\\|]+)\\s{1,}\\:\\s{1,}([\\w\\.\\,\\+\\-\\/\\%\\*\\(\\)\\ \\_\\'\\\"\\{\\}\\[\\]\\:\\<\\>\\=\\!\\&\\|]+)\\s{0,})$");
    QRegExp rx_2("^\\s{0,}([\\w\\.\\,\\+\\-\\/\\%\\*\\(\\)\\ \\_\\'\\\"\\{\\}\\[\\]\\:\\<\\>\\=\\!\\&\\|]+)\\s{1,}\\?\\:\\s{1,}([\\w\\.\\,\\+\\-\\/\\%\\*\\(\\)\\ \\_\\'\\\"\\{\\}\\[\\]\\:\\<\\>\\=\\!\\&\\|]+)\\s{0,}$");
    QRegExp rx_3("^\\s{0,}([\\w\\.\\,\\+\\-\\/\\%\\*\\(\\)\\ \\_\\'\\\"\\{\\}\\[\\]\\:\\<\\>\\=\\!\\&\\|]+)\\s{1,}\\?\\s{1,}([\\w\\.\\,\\+\\-\\/\\%\\*\\(\\)\\ \\_\\'\\\"\\{\\}\\[\\]\\:\\<\\>\\=\\!\\&\\|]+)\\s{0,}$");

    // check conditions
    if (rx.indexIn(expr) != -1) {
        QString ternCond = rx.cap(1).trimmed();
        QPair<QString, QString> ternResults = QtTIParserTernaryOperator::parseLeftRight(rx.cap(2).trimmed());

        bool calcIsOk = false;
        QVariant ternCondVal;
        std::tie(calcIsOk, ternCondVal, error) = QtTIParserTernaryOperator::evalCond(ternCond, parserArgs, parserFunc);
        if (!calcIsOk) {
            if (isOk)
                *isOk = false;
            return QVariant();
        }

        QVariant ternResultTrue;
        std::tie(calcIsOk, ternResultTrue, error) = QtTIParserTernaryOperator::evalCond(ternResults.first, parserArgs, parserFunc);
        if (!calcIsOk) {
            if (isOk)
                *isOk = false;
            return QVariant();
        }

        QVariant ternResultFalse;
        std::tie(calcIsOk, ternResultFalse, error) = QtTIParserTernaryOperator::evalCond(ternResults.second, parserArgs, parserFunc);
        if (!calcIsOk) {
            if (isOk)
                *isOk = false;
            return QVariant();
        }
        if (isOk)
            *isOk = true;

        if (ternCondVal.type() == QVariant::Bool)
            return (ternCondVal.toBool() ? ternResultTrue : ternResultFalse);

        return (!ternCondVal.isNull() ? ternResultTrue : ternResultFalse);

    } else if (rx_2.indexIn(expr) != -1) {
        QString ternCond = rx_2.cap(1).trimmed();
        QString ternCondFalse = rx_2.cap(2).trimmed();

        bool calcIsOk = false;
        QVariant ternCondVal;
        std::tie(calcIsOk, ternCondVal, error) = QtTIParserTernaryOperator::evalCond(ternCond, parserArgs, parserFunc);
        if (!calcIsOk) {
            if (isOk)
                *isOk = false;
            return QVariant();
        }

        QVariant ternResultFalse;
        std::tie(calcIsOk, ternResultFalse, error) = QtTIParserTernaryOperator::evalCond(ternCondFalse, parserArgs, parserFunc);
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

    } else if (rx_3.indexIn(expr) != -1) {
        QString ternCond = rx_3.cap(1).trimmed();
        QString ternCondTrue = rx_3.cap(2).trimmed();

        bool calcIsOk = false;
        QVariant ternCondVal;
        std::tie(calcIsOk, ternCondVal, error) = QtTIParserTernaryOperator::evalCond(ternCond, parserArgs, parserFunc);
        if (!calcIsOk) {
            if (isOk)
                *isOk = false;
            return QVariant();
        }

        QVariant ternResultTrue;
        std::tie(calcIsOk, ternResultTrue, error) = QtTIParserTernaryOperator::evalCond(ternCondTrue, parserArgs, parserFunc);
        if (!calcIsOk) {
            if (isOk)
                *isOk = false;
            return QVariant();
        }
        if (isOk)
            *isOk = true;

        if (ternCondVal.type() == QVariant::Bool)
            return (ternCondVal.toBool() ? ternResultTrue : QVariant());

        return (!ternCondVal.isNull() ? ternResultTrue : QVariant());
    }

    error = QString("Could not recognize ternary operator for expression '%1'").arg(expr);
    return QVariant();
}

//!
//! \brief Parse left and right expressions for view: {{ foo ? 'yes' : 'no' }}
//! \param str
//! \return
//!
QPair<QString, QString> QtTIParserTernaryOperator::parseLeftRight(const QString &str)
{
    if (str.isEmpty())
        return QPair<QString, QString>();
    QPair<QString, QString> tmpResult;
    QString tmpValue;
    bool isEescaping = false;
    int openBrackets = 0;
    for (int i = 0; i < str.size(); i++) {
        const QChar ch = str[i];
        QChar chPrev;
        if (i > 0)
            chPrev = str[i - 1];

        if (ch == ":" && !isEescaping && openBrackets == 0) {
            tmpResult.first = tmpValue.trimmed();
            tmpValue.clear();
            continue;
        }
        if (ch == "'" || ch == "\"") {
            if (chPrev == "\\") {
                tmpValue += ch;
                continue;
            }
            tmpValue += ch;
            isEescaping = !isEescaping;
            continue;
        }
        if (ch == "{" && !isEescaping) {
            openBrackets++;
            tmpValue += ch;
            continue;
        }
        if (ch == "}" && !isEescaping) {
            openBrackets--;
            tmpValue += ch;
            continue;
        }
        tmpValue += ch;
    }
    tmpResult.second = tmpValue.trimmed();
    return tmpResult;
}

//!
//! \brief Evaluate condition
//! \param str
//! \param parserArgs
//! \param parserFunc
//! \return
//!
std::tuple<bool, QVariant, QString> QtTIParserTernaryOperator::evalCond(const QString &str,
                                                                        QtTIParserArgs *parserArgs,
                                                                        QtTIParserFunc *parserFunc)
{
    if (str.isEmpty())
        return std::make_tuple(false, QVariant(), "Eval condition failed (empty string passed)");

    QStringList validStringsBeforeBracket({ "+", "-", "/", "%", "//", "*", "**", "and", "or", "not", "&&", "||" });
    QtTIBracketsNode brNode = QtTIParserBracketsExpr::parseBracketsExpression(str, validStringsBeforeBracket, parserArgs, [parserArgs, parserFunc](const QString &nodeBody) {
        return QtTIParserTernaryOperator::parseParamValue(nodeBody, parserArgs, parserFunc);
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
std::tuple<bool, QVariant, QString> QtTIParserTernaryOperator::parseParamValue(const QString &str,
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
    QRegExp rxFunc("^(\\s*([\\w]+)\\s*\\(\\s*([A-Za-z0-9_\\ \\+\\-\\*\\,\\.\\'\\\"\\{\\}\\[\\]\\(\\)\\:\\/\\^\\$\\\\\\@\\#\\!\\<\\>\\=\\&\\%\\|\\;\\~]*)\\s*\\)\\s*)");
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
