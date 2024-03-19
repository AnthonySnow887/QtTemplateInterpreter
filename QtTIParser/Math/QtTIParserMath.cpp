#include "QtTIParserMath.h"

#include "../BracketsExpr/QtTIParserBracketsExpr.h"

#include <QRegExp>
#include <qmath.h>

//!
//! \brief Check is math expression
//! \param expr
//! \return
//!
bool QtTIParserMath::isMathExpr(const QString &expr)
{
    QRegExp rx("^\\s{0,}([\\w\\_\\.\\,\\+\\-\\(\\) \\'\\\"\\[\\]\\{\\}\\:]+)(\\s{0,}(\\+|-|/|//|%|\\*|\\*\\*)\\s{0,}([\\w\\_\\.\\,\\+\\-\\(\\) \\'\\\"\\[\\]\\{\\}\\:]+)\\s{0,})+$");
    return (rx.indexIn(expr) != -1);
}

//!
//! \brief Parse math expression and calculate (with brackets)
//! \param expr
//! \param parserArgs
//! \param parserFunc
//! \param[in,out] isOk
//! \param[in,out] error
//! \return
//!
//! === Math operators (only for numbers):
//!     The following operators are supported:
//!      + : Adds two numbers together (the operands are casted to numbers):
//!             {{ 1 + 1 }} is 2
//!
//!      - : Subtracts the second number from the first one:
//!             {{ 3 - 2 }} is 1
//!
//!      / : Divides two numbers. The returned value will be a floating point number:
//!             {{ 1 / 2 }} is {{ 0.5 }}
//!
//!      % : Calculates the remainder of an integer division:
//!             {{ 11 % 7 }} is 4
//!
//!     // : Divides two numbers and returns the floored integer result:
//!             {{ 20 // 7 }} is 2, {{ -20  // 7 }} is -3 (this is just syntactic sugar for the round filter)
//!
//!      * : Multiplies the left operand with the right one:
//!             {{ 2 * 2 }} would return 4
//!
//!     ** : Raises the left operand to the power of the right operand:
//!             {{ 2 ** 3 }} would return 8
//!
//! === Brackets support:
//!     Supported grouping expressions with brackets '(...expr...)':
//!         {{ ( 10.5 + 2 * 2 ) / 3 + 10 - 1 + 2 ** 2 }} is 17.833333333333332
//!         {{ ( 10.5 + 2 * (2 + 5)) / (3 + 10 * 2) - (1 + 2) ** 2 }} is -7.934782608695652
//!
QVariant QtTIParserMath::parseMath(const QString &expr, QtTIParserArgs *parserArgs, QtTIParserFunc *parserFunc, bool *isOk, QString &error)
{
    if (isOk)
        *isOk = false;
    error.clear();
    if (!parserArgs) {
        error = QString("QtTIParserArgs is NULL");
        return QVariant();
    }
    if (!QtTIParserMath::isMathExpr(expr)) {
        error = QString("Incorrect math expression '%1'").arg(expr);
        return QVariant();
    }

    QStringList validStringsBeforeBracket({ "+", "-", "/", "%", "//", "*", "**" });
    QtTIBracketsNode brNode = QtTIParserBracketsExpr::parseBracketsExpression(expr, validStringsBeforeBracket, parserArgs, [parserArgs, parserFunc](const QString &nodeBody) {
        bool isOk = false;
        QString error;
        QVariant result = QtTIParserMath::parseMathWithoutBrackets(nodeBody, parserArgs, parserFunc, &isOk, error);
        return std::make_tuple(isOk, result, error);
    });
    if (!brNode.isValid()) {
        error = QString("Invalid brackets node for expression '%1'").arg(expr);
        return QVariant();
    }
    bool isOkLocal = false;
    QVariant result;
    std::tie(isOkLocal, result, error) = brNode.evalNodeBody();
    if (isOk)
        *isOk = isOkLocal;
    return result;
}

//!
//! \brief Parse math expression without brackets and calculate
//! \param expr
//! \param parserArgs
//! \param parserFunc
//! \param[in,out] isOk
//! \param[in,out] error
//! \return
//!
QVariant QtTIParserMath::parseMathWithoutBrackets(const QString &expr, QtTIParserArgs *parserArgs, QtTIParserFunc *parserFunc, bool *isOk, QString &error)
{
    if (isOk)
        *isOk = false;
    error.clear();
    if (!parserArgs) {
        error = QString("QtTIParserArgs is NULL");
        return QVariant();
    }
    if (!QtTIParserMath::isMathExpr(expr)) {
        error = QString("Incorrect math expression '%1'").arg(expr);
        return QVariant();
    }

    QList<QtTIMathAction> actions;
    bool parseIsOk = false;
    parseLR(expr, &actions, parserArgs, parserFunc, &parseIsOk, error);
    if (!parseIsOk)
        return QVariant();

    // calc math operations
    int pos = 0;
    int opPriority = 0;
    while (true) {
        if (pos == actions.size()) {
            if (opPriority == MAX_OP_PRIORITY)
                break;
            opPriority++;
            pos = 0;
            continue;
        }
        QtTIMathAction actLeft;
        if (pos > 0)
            actLeft = actions[pos - 1];
        QtTIMathAction actRight;
        if (pos < actions.size() - 1)
            actRight = actions[pos + 1];
        QtTIMathAction act = actions[pos];
        if (act.opPriority() != opPriority) {
            pos++;
            continue;
        }
        bool calcIsOk = false;
        QVariant calcRes;
        std::tie(calcIsOk, calcRes, error) = calcMathOperation(act.left(), act.right(), act.op());
        if (!calcIsOk)
            return QVariant();

        if (actLeft.isValid())
            actLeft.setRight(calcRes);
        if (actRight.isValid())
            actRight.setLeft(calcRes);

        // update
        if (pos > 0)
            actions.replace(pos - 1, actLeft);
        if (pos < actions.size() - 1)
            actions.replace(pos + 1, actRight);

        actions.removeAt(pos);
        if (actions.isEmpty()) {
            if (isOk)
                *isOk = true;
            return calcRes;
        }
    }
    error = QString("Incorrect execution of a math expression '%1'").arg(expr);
    return QVariant();
}

//!
//! \brief Parse left and right values
//! \param expr
//! \param actions
//! \param parserArgs
//! \param parserFunc
//! \param[in,out] isOk
//! \param[in,out] error
//!
void QtTIParserMath::parseLR(const QString &expr, QList<QtTIMathAction> *actions, QtTIParserArgs *parserArgs, QtTIParserFunc *parserFunc, bool *isOk, QString &error)
{
    if (isOk)
        *isOk = false;
    error.clear();
    QRegExp rxMath("\\s{0,}([\\w\\_\\.\\,\\+\\-\\(\\) \\'\\\"\\[\\]\\{\\}\\:]+)\\s{0,}(\\+|-|/|//|%|\\*|\\*\\*)\\s{0,}([\\w\\_\\.\\,\\+\\-\\(\\) \\'\\\"\\[\\]\\{\\}\\:]+)\\s{0,}");
    if (rxMath.indexIn(expr, 0) != -1) {
        if (rxMath.captureCount() < 3)
            return;

        QString condLeft = rxMath.cap(1);
        QString condOp = rxMath.cap(2);
        QString condRight = rxMath.cap(3);
        QVariant condLeftVal;
        QVariant condRightVal;
        bool parseIsOk = false;
        if (rxMath.indexIn(condLeft) != -1) {
            parseLR(condLeft.trimmed(), actions, parserArgs, parserFunc, &parseIsOk, error);
            if (!parseIsOk) {
                error = QString("Parse condition left value failed (condition: '%1')!").arg(condLeft.trimmed());
                return;
            }
            const int pos = actions->size() - 1;
            if (pos < 0)
                return;
            condLeftVal = actions->at(pos).right();
        } else {
            std::tie(parseIsOk, condLeftVal, error) = QtTIParserMath::parseParamValue(condLeft.trimmed(), parserArgs, parserFunc);
            if (!parseIsOk) {
                error = QString("Parse condition left value failed (condition: '%1')!").arg(condLeft.trimmed());
                return;
            }
        }

        const int rPos = actions->size();
        if (rxMath.indexIn(condRight) != -1) {
            parseLR(condRight.trimmed(), actions, parserArgs, parserFunc, &parseIsOk, error);
            if (!parseIsOk) {
                error = QString("Parse condition right value failed (condition: '%1')!").arg(condRight.trimmed());
                return;
            }
            if (rPos >= actions->size())
                return;
            condRightVal = actions->at(rPos).left();
        } else {
            std::tie(parseIsOk, condRightVal, error) = QtTIParserMath::parseParamValue(condRight.trimmed(), parserArgs, parserFunc);
            if (!parseIsOk) {
                error = QString("Parse condition right value failed (condition: '%1')!").arg(condRight.trimmed());
                return;
            }
        }
        actions->insert(rPos, QtTIMathAction(condLeftVal, condRightVal, condOp));
        if (isOk)
            *isOk = true;
    }
}

//!
//! \brief Parse parameter value from string
//! \param str Parameter value string view
//! \return
//!
std::tuple<bool, QVariant, QString> QtTIParserMath::parseParamValue(const QString &str, QtTIParserArgs *parserArgs, QtTIParserFunc *parserFunc)
{
    if (str.isEmpty())
        return std::make_tuple(false, QVariant(), "Parse value failed (empty string passed)");

    // math
    if (QtTIParserMath::isMathExpr(str)) {
        bool isOk = false;
        QString error;
        QVariant result = QtTIParserMath::parseMath(str, parserArgs, parserFunc, &isOk, error);
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

//!
//! \brief Calculate math operation
//! \param left
//! \param right
//! \param op
//! \return
//!
std::tuple<bool, QVariant, QString> QtTIParserMath::calcMathOperation(const QVariant &left, const QVariant &right, const QString &op)
{
    QList<int> supTypes({QVariant::Int,
                         QVariant::UInt,
                         QVariant::Double,
                         QMetaType::Float,
                         QVariant::LongLong,
                         QVariant::ULongLong});

    if (!supTypes.contains(static_cast<int>(left.type())))
        return std::make_tuple(false, QVariant(), QString("Unsupported left value type '%1' for operator '%2'").arg(left.typeName()).arg(op));
    if (!supTypes.contains(static_cast<int>(right.type())))
        return std::make_tuple(false, QVariant(), QString("Unsupported right value type '%1' for operator '%2'").arg(right.typeName()).arg(op));

    switch (static_cast<int>(left.type())) {
        case QVariant::Int:
            return calcMathOperation_t_int<int>(left.toInt(), right.toInt(), op);
        case QVariant::UInt:
            return calcMathOperation_t_int<uint>(left.toUInt(), right.toUInt(), op);
        case QVariant::Double:
            return calcMathOperation_t_real<double>(left.toDouble(), right.toDouble(), op);
        case QMetaType::Float:
            return calcMathOperation_t_real<float>(left.toFloat(), right.toFloat(), op);
        case QVariant::LongLong:
            return calcMathOperation_t_int<qlonglong>(left.toLongLong(), right.toLongLong(), op);
        case QVariant::ULongLong:
            return calcMathOperation_t_int<qulonglong>(left.toULongLong(), right.toULongLong(), op);
        default:
            break;
    }
    return std::make_tuple(false, QVariant(), QString("Unsupported value type '%1' for operator '%2'").arg(left.typeName()).arg(op));
}

//!
//! \brief Template method for calculate math operation for integer numbers
//! \param left
//! \param right
//! \param op
//! \return
//!
template<typename T>
std::tuple<bool, QVariant, QString> QtTIParserMath::calcMathOperation_t_int(const T &left, const T &right, const QString &op)
{
    if (op == "+") {
        return std::make_tuple(true, left + right, "");
    } else if (op == "-") {
        return std::make_tuple(true, left - right, "");
    } else if (op == "/") {
        if (right == 0)
            return std::make_tuple(false, QVariant(), "Division by zero!");
        return std::make_tuple(true, left / right, "");
    } else if (op == "*") {
        return std::make_tuple(true, left * right, "");
    } else if (op == "%") {
        if (right == 0)
            return std::make_tuple(false, QVariant(), "Division by zero!");
        return std::make_tuple(true, left % right, "");
    } else if (op == "//") {
        if (right == 0)
            return std::make_tuple(false, QVariant(), "Division by zero!");
        return std::make_tuple(true, static_cast<int>(left / right), "");
    } else if (op == "**") {
        return std::make_tuple(true, std::pow(left, right), "");
    }
    return std::make_tuple(false, QVariant(), QString("Unsupported math operator '%1'").arg(op));
}

//!
//! \brief Template method for calculate math operation for real numbers
//! \param left
//! \param right
//! \param op
//! \return
//!
template<typename T>
std::tuple<bool, QVariant, QString> QtTIParserMath::calcMathOperation_t_real(const T &left, const T &right, const QString &op)
{
    if (op == "+") {
        return std::make_tuple(true, left + right, "");
    } else if (op == "-") {
        return std::make_tuple(true, left - right, "");
    } else if (op == "/") {
        if (right == 0.0)
            return std::make_tuple(false, QVariant(), "Division by zero!");
        return std::make_tuple(true, left / right, "");
    } else if (op == "*") {
        return std::make_tuple(true, left * right, "");
    } else if (op == "%") {
        if (right == 0.0)
            return std::make_tuple(false, QVariant(), "Division by zero!");
        return std::make_tuple(true, std::fmod(left, right), "");
    } else if (op == "//") {
        if (right == 0.0)
            return std::make_tuple(false, QVariant(), "Division by zero!");
        return std::make_tuple(true, static_cast<int>(left / right), "");
    } else if (op == "**") {
        return std::make_tuple(true, std::pow(left, right), "");
    }

    return std::make_tuple(false, QVariant(), QString("Unsupported math operator '%1'").arg(op));
}
