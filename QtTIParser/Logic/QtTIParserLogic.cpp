#include "QtTIParserLogic.h"
#include "../BracketsExpr/QtTIParserBracketsExpr.h"
#include "../Math/QtTIParserMath.h"
#include <QRegExp>

//!
//! \brief Check is logic expression
//! \param expr
//! \return
//!
bool QtTIParserLogic::isLogicExpr(const QString &expr)
{
    QRegExp rx("^\\s{0,}([\\w\\.\\,\\+\\-\\/\\%\\*\\(\\)\\ \\_\\'\\\"\\{\\}\\[\\]\\:\\<\\>\\=\\!]+)(\\s{1,}(and|or|&&|\\|\\||not)\\s{1,}([\\w\\.\\,\\+\\-\\/\\%\\*\\(\\)\\ \\_\\'\\\"\\{\\}\\[\\]\\:\\<\\>\\=\\!]+)\\s{0,})+$");
    QRegExp rx_2("^\\s{0,}([\\w\\.\\,\\+\\-\\/\\%\\*\\(\\)\\ \\_\\'\\\"\\{\\}\\[\\]\\:]+)(\\s{1,}([\\<\\>\\=\\!]+)\\s{1,}([\\w\\.\\,\\+\\-\\/\\%\\*\\(\\)\\ \\_\\'\\\"\\{\\}\\[\\]\\:]+)\\s{0,})+$");
    return ((rx.indexIn(expr) != -1)
            || (rx_2.indexIn(expr) != -1));
}

//!
//! \brief Parse logic expression and exec (with brackets)
//! \param expr
//! \param parserArgs
//! \param parserFunc
//! \param isOk
//! \param error
//! \return
//!
//! === Comparison operations:
//!     ==: equally
//!     !=: not equally
//!      >: larger
//!      <: less
//!     >=: larger or equal
//!     <=: less or equal
//!
//! === Logic:
//!     You can combine multiple expressions with the following operators:
//!      - and: Returns true if the left and the right operands are both true.
//!      - or: Returns true if the left or the right operand is true.
//!      - not: Negates a statement.
//!      - &&: equivalent of method 'and'
//!      - ||: equivalent of method 'or'
//!
//! === Brackets support:
//!     Supported grouping expressions with brackets '(...expr...)':
//!         {% set a = 10.5 %}
//!         {% set b = 2 %}
//!         {% set aa = { a:1, b:2, c:3 } %}
//!         {{ (a + aa.b) > 5 && (aa.b < 2 || aa.c >= 3) }} is true
//!         {{ my_test_array () && a <= 5 }} is false
//!
QVariant QtTIParserLogic::parseLogic(const QString &expr,
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
    if (!QtTIParserLogic::isLogicExpr(expr)) {
        error = QString("Incorrect logic expression '%1'").arg(expr);
        return QVariant();
    }

    QStringList validStringsBeforeBracket({ "and", "or", "not", "&&", "||" });
    QtTIBracketsNode brNode = QtTIParserBracketsExpr::parseBracketsExpression(expr, validStringsBeforeBracket, parserArgs, [parserArgs, parserFunc](const QString &nodeBody) {
        bool isOk = false;
        QString error;
        QVariant result = QtTIParserLogic::parseLogicWithoutBrackets(nodeBody, parserArgs, parserFunc, &isOk, error);
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
//! \brief Parse logic expression without brackets and exec
//! \param expr
//! \param parserArgs
//! \param parserFunc
//! \param isOk
//! \param error
//! \return
//!
QVariant QtTIParserLogic::parseLogicWithoutBrackets(const QString &expr,
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

    // check is math expr
    if (QtTIParserMath::isMathExpr(expr))
        return QtTIParserMath::parseMath(expr, parserArgs, isOk, error);

    // check is logic expr
    if (!QtTIParserLogic::isLogicExpr(expr)) {
        error = QString("Incorrect logic expression '%1'").arg(expr);
        return QVariant();
    }

    bool isOkLocal = false;
    bool result = false;
    std::tie(isOkLocal, result, error) = QtTIParserLogic::compareBlockCondition(expr, parserArgs, parserFunc);
    if (isOk)
        *isOk = isOkLocal;
    return result;
}

//!
//! \brief Parse logic expression and compare (with: and, or, &&, ||)
//! \param str
//! \param parserArgs
//! \param parserFunc
//! \return
//!
//! === Example
//!         a > 5 and b <= 2
//!
std::tuple<bool, bool, QString> QtTIParserLogic::compareBlockCondition(const QString &str,
                                                                       QtTIParserArgs *parserArgs,
                                                                       QtTIParserFunc *parserFunc)
{
    QString expression = str.trimmed();
    QRegExp rxInvalid("^(&&|and|\\|\\||or)\\s+");
    if (rxInvalid.indexIn(expression) != -1)
        return std::make_tuple(false, false, "The condition is set incorrectly: condition cannot start with '&&', '||', 'and', 'or'");

    QRegExp rxAnd("\\s+(&&|and)\\s+");
    QRegExp rxOr("\\s+(\\|\\||or)\\s+");
    if (rxOr.indexIn(expression) != -1) {
        expression = expression.replace(" || ", " or ");
        bool exResult = false;
        const QStringList exSections = expression.split(" or ");
        for (const QString &e : exSections) {
            bool isOk = false;
            bool res = false;
            QString err;
            std::tie(isOk, res, err) = compareBlockCondition(e, parserArgs, parserFunc);
            if (!isOk)
                return std::make_tuple(false, false, err);
            // If 'true', then you don’t have to check further and return the result
            if (res)
                return std::make_tuple(true, true, "");
            exResult |= res;
        }
        return std::make_tuple(true, exResult, "");
    } else if (rxAnd.indexIn(expression) != -1) {
        expression = expression.replace(" && ", " and ");
        bool exResult = true;
        const QStringList exSections = expression.split(" and ");
        for (const QString &e : exSections) {
            bool isOk = false;
            bool res = false;
            QString err;
            std::tie(isOk, res, err) = compareBlockCondition(e, parserArgs, parserFunc);
            if (!isOk)
                return std::make_tuple(false, false, err);
            // If 'false', then you don’t have to check further and return the result
            if (!res)
                return std::make_tuple(true, false, "");
            exResult &= res;
        }
        return std::make_tuple(true, exResult, "");
    }
    return compareExpression(expression, parserArgs, parserFunc);
}

//!
//! \brief Parse logic expression and compare (without: and, or, &&, ||)
//! \param str
//! \param parserArgs
//! \param parserFunc
//! \return
//!
//! === Example
//!         a > 5
//!
std::tuple<bool, bool, QString> QtTIParserLogic::compareExpression(const QString &str,
                                                                   QtTIParserArgs *parserArgs,
                                                                   QtTIParserFunc *parserFunc)
{
    QRegExp rxLeftRight("((not\\s+)?([A-Za-z0-9_\\ \\+\\-\\,\\.\\'\\\"\\{\\}\\[\\]\\:\\/\\(\\)]*)\\s+([\\<\\>\\=\\!]+)\\s+([A-Za-z0-9_\\ \\+\\-\\,\\.\\'\\\"\\{\\}\\[\\]\\:\\/\\(\\)]*))");
    QRegExp rxOne("((not\\s+)?([A-Za-z0-9_\\ \\+\\-\\,\\.\\'\\\"\\{\\}\\[\\]\\:\\/\\(\\)]*))");
    if (rxLeftRight.indexIn(str) != -1) {
        QString notVal = rxLeftRight.cap(2).trimmed();
        QString left = rxLeftRight.cap(3).trimmed();
        if (left.isEmpty())
            return std::make_tuple(false, false, "Invalid exptression left parameter (not specified)");
        QString cond = rxLeftRight.cap(4).trimmed();
        if (cond.isEmpty())
            return std::make_tuple(false, false, "Invalid exptression condition (not specified)");
        QString right = rxLeftRight.cap(5).trimmed();
        if (right.isEmpty())
            return std::make_tuple(false, false, "Invalid exptression right parameter (not specified)");
        QVariant vLeft;
        QVariant vRight;
        // parse
        bool isOk = false;
        QString error;
        std::tie(isOk, vLeft, error) = parseParamValue(left, parserArgs, parserFunc);
        if (!isOk)
            return std::make_tuple(false, false, error);

        std::tie(isOk, vRight, error) = parseParamValue(right, parserArgs, parserFunc);
        if (!isOk)
            return std::make_tuple(false, false, error);

        // compare
        bool res = compare(vLeft, vRight, cond);
        if (!notVal.isEmpty())
            res = !res;
        return std::make_tuple(true, res, "");
    } else if (rxOne.indexIn(str) != -1) {
        QString notVal = rxOne.cap(2).trimmed();
        QString left = rxOne.cap(3).trimmed();
        if (left.isEmpty())
            return std::make_tuple(false, false, "Invalid exptression parameter (not specified)");
        QVariant vLeft;
        // parse
        bool isOk = false;
        QString error;
        std::tie(isOk, vLeft, error) = parseParamValue(left, parserArgs, parserFunc);
        if (!isOk)
            return std::make_tuple(false, false, error);

        // compare
        bool res = compare(vLeft);
        if (!notVal.isEmpty())
            res = !res;
        return std::make_tuple(true, res, "");
    }
    return std::make_tuple(false, false, QString("Invalid exptression '%1'").arg(str));
}

//!
//! \brief Parse parameter value from string
//! \param str Parameter value string view
//! \param parserArgs
//! \param parserFunc
//! \return
//!
std::tuple<bool/*isOk*/,QVariant/*res*/,QString/*err*/> QtTIParserLogic::parseParamValue(const QString &str,
                                                                                         QtTIParserArgs *parserArgs,
                                                                                         QtTIParserFunc *parserFunc)
{
    if (str.isEmpty())
        return std::make_tuple(false, QVariant(), "Parse value failed (empty string passed)");

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

//!
//! \brief Compare value
//! \param left
//! \return
//!
//! If 'left' type is bool - return bool value.
//! If 'left' type is not bool - return check result value is not NULL.
//!
bool QtTIParserLogic::compare(const QVariant &left)
{
    if (left.type() == QVariant::Bool)
        return left.toBool();

    return !left.isNull();
}

//!
//! \brief Compare values
//! \param left
//! \param right
//! \param cond
//! \return
//!
bool QtTIParserLogic::compare(const QVariant &left, const QVariant &right, const QString &cond)
{
    switch (static_cast<int>(left.type())) {
        case QVariant::Int:
            return compare(left.toInt(), right.toInt(), strToOperation(cond));
        case QVariant::UInt:
            return compare(left.toUInt(), right.toUInt(), strToOperation(cond));
        case QVariant::Double:
            return compare(left.toDouble(), right.toDouble(), strToOperation(cond));
        case QMetaType::Float:
            return compare(left.toFloat(), right.toFloat(), strToOperation(cond));
        case QVariant::LongLong:
            return compare(left.toLongLong(), right.toLongLong(), strToOperation(cond));
        case QVariant::ULongLong:
            return compare(left.toULongLong(), right.toULongLong(), strToOperation(cond));
        case QVariant::Bool:
            return compare(left.toBool(), right.toBool(), strToOperation(cond));
        case QVariant::String:
            return compare(left.toString(), right.toString(), strToOperation(cond));
        case QVariant::ByteArray:
            return compare(left.toByteArray(), right.toByteArray(), strToOperation(cond));
        default:
            break;
    }
    return false;
}

//!
//! \brief Template method for compare values
//! \param left
//! \param right
//! \param operation
//! \return
//!
template<typename T>
bool QtTIParserLogic::compare(const T &left, const T &right, const LogicalOperation &operation)
{
    switch (operation) {
        case LogicalOperation::Equally:
            return (left == right);
        case LogicalOperation::NotEqually:
            return (left != right);
        case LogicalOperation::Larger:
            return (left > right);
        case LogicalOperation::Less:
            return (left < right);
        case LogicalOperation::LargerOrEqual:
            return (left >= right);
        case LogicalOperation::LessOrEqual:
            return (left <= right);
        default:
            break;
    }
    return false;
}

//!
//! \brief Convert string view operation to enum 'QtTIParserLogic::LogicalOperation' value
//! \param cond
//! \return
//!
QtTIParserLogic::LogicalOperation QtTIParserLogic::strToOperation(const QString &cond)
{
    if (cond == "==")
        return LogicalOperation::Equally;
    else if (cond == "!=" || cond == "<>")
        return LogicalOperation::NotEqually;
    else if (cond == ">")
        return LogicalOperation::Larger;
    else if (cond == "<")
        return LogicalOperation::Less;
    else if (cond == ">=")
        return LogicalOperation::LargerOrEqual;
    else if (cond == "<=")
        return LogicalOperation::LessOrEqual;

    return LogicalOperation::Invalid;
}
