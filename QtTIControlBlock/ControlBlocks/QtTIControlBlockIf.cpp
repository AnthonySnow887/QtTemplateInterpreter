#include "QtTIControlBlockIf.h"
#include "../QtTIControlBlockFabric.h"
#include "../../QtTIParser/BracketsExpr/QtTIParserBracketsExpr.h"
#include "../../QtTIParser/TernaryOperator/QtTIParserTernaryOperator.h"
#include "../../QtTIParser/NullCoalescingOperator/QtTIParserNullCoalescingOperator.h"
#include "../../QtTIParser/Logic/QtTIParserLogic.h"
#include "../../QtTIParser/Math/QtTIParserMath.h"

QtTIControlBlockIf::QtTIControlBlockIf(QtTIParser *parser)
    : QtTIAbstractControlBlock(parser, -1)
{}

QtTIControlBlockIf::QtTIControlBlockIf(QtTIParser *parser, const QString &blockCond, const int lineNum)
    : QtTIAbstractControlBlock(parser, lineNum)
    , _ifCond(blockCond)
{}

QtTIControlBlockIf::~QtTIControlBlockIf()
{
}

//!
//! \brief Make control block
//! \param blockCond Control block condition
//! \param lineNum Line number
//! \return
//!
QtTIAbstractControlBlock *QtTIControlBlockIf::makeBlock(const QString &blockCond, const int lineNum)
{
    return new QtTIControlBlockIf(parser(), blockCond, lineNum);
}

//!
//! \brief Get control block condition
//! \return
//!
QString QtTIControlBlockIf::blockCondition() const
{
    return _ifCond;
}

//!
//! \brief Check is control block start
//! \param blockCond Control block condition
//! \return
//!
bool QtTIControlBlockIf::isBlockCondStart(const QString &blockCond)
{
    QRegExp rx("(if\\s+(.*))");
    QRegExp rxElseIf("(\\s{0,}elseif\\s+(.*))");
    return ((rx.indexIn(blockCond) != -1)
            && (rxElseIf.indexIn(blockCond) == -1));
}

//!
//! \brief Check is intermediate part of control block
//! \param blockCond Control block condition
//! \return
//!
bool QtTIControlBlockIf::isBlockCondIntermediate(const QString &blockCond)
{
    QRegExp rxElseIf("(\\s{0,}elseif\\s+(.*))");
    if ((rxElseIf.indexIn(blockCond) != -1) && _elseCond.isEmpty())
        return true;
    QRegExp rxElse("^(else)$");
    if (rxElse.indexIn(blockCond) != -1 && _elseCond.isEmpty())
        return true;
    return false;
}

//!
//! \brief Append intermediate part of control block
//! \param blockCond Control block condition
//!
void QtTIControlBlockIf::appendBlockCondIntermediate(const QString &blockCond)
{
    QRegExp rxElseIf("(\\s{0,}elseif\\s+(.*))");
    if (rxElseIf.indexIn(blockCond) != -1) {
        _elseIfConds.append(blockCond);
        _elseIfBodys.append(QMap<int,QString>());
        _bodyPos = BodyPosition::ElseIf;
        return;
    }
    QRegExp rxElse("^(else)$");
    if (rxElse.indexIn(blockCond) != -1) {
        _elseCond = blockCond;
        _bodyPos = BodyPosition::Else;
        return;
    }
}

//!
//! \brief Check is control block end
//! \param blockCond Control block condition
//! \return
//!
bool QtTIControlBlockIf::isBlockCondEnd(const QString &blockCond)
{
    QRegExp rx("^(endif)$");
    return (rx.indexIn(blockCond) != -1);
}

//!
//! \brief Execute control block
//! \return
//!
std::tuple<bool, QString, QString> QtTIControlBlockIf::evalBlock()
{
    // check IF
    QRegExp rxIf("(if\\s+(.*))");
    if (rxIf.indexIn(_ifCond) != -1) {
        QString ifConf = rxIf.cap(2).trimmed();
        bool isOk = false;
        QString error;
        QVariant result;
        std::tie(isOk, result, error) = evalCond(ifConf, parser()->parserArgs(), parser()->parserFunc());
        if (!isOk)
            return std::make_tuple(false, "", error);
        if (result.type() == QVariant::Bool
            && result.toBool())
            return buildBlockBody(_ifBody);
        else if (result.type() != QVariant::Bool
                 && !result.isNull())
            return buildBlockBody(_ifBody);
    } else {
        return std::make_tuple(false, "", "Incorrect conditions are specified for the IF block");
    }

    // check ELSEIF
    if (!_elseIfConds.isEmpty()) {
        for (int i = 0; i < _elseIfConds.size(); i++) {
            const QString elseIfCond = _elseIfConds[i];
            const QMap<int,QString> elseIfBody = _elseIfBodys[i];
            // check IF
            QRegExp rxElseIf("(\\s{0,}elseif\\s+(.*))");
            if (rxElseIf.indexIn(elseIfCond) != -1) {
                QString ifConf = rxElseIf.cap(2).trimmed();
                bool isOk = false;
                QString error;
                QVariant result;
                std::tie(isOk, result, error) = evalCond(ifConf, parser()->parserArgs(), parser()->parserFunc());
                if (!isOk)
                    return std::make_tuple(false, "", error);
                if (result.type() == QVariant::Bool
                    && result.toBool())
                    return buildBlockBody(elseIfBody);
                else if (result.type() != QVariant::Bool
                         && !result.isNull())
                    return buildBlockBody(elseIfBody);
            }
        }
    }

    // check ELSE
    QRegExp rxElse("(\\s{0,}else\\s{0,})");
    if (rxElse.indexIn(_elseCond) != -1)
        return buildBlockBody(_elseBody);

    // none of the conditions matched, we return an empty result
    return std::make_tuple(true, "", "");
}

//!
//! \brief Append control block body
//! \param blockBody Control block body
//!
void QtTIControlBlockIf::appendBlockBody(const QString &blockBody, const int lineNum)
{
    switch (_bodyPos) {
        case BodyPosition::If: {
            _ifBody[lineNum].append(blockBody);
            break;
        }
        case BodyPosition::Else: {
            _elseBody[lineNum].append(blockBody);
            break;
        }
        case BodyPosition::ElseIf: {
            _elseIfBodys[_elseIfBodys.size() - 1][lineNum].append(blockBody);
            break;
        }
        default:
            break;
    }
}

//!
//! \brief Evaluate condition
//! \param str
//! \param parserArgs
//! \param parserFunc
//! \return
//!
std::tuple<bool, QVariant, QString> QtTIControlBlockIf::evalCond(const QString &str,
                                                                 QtTIParserArgs *parserArgs,
                                                                 QtTIParserFunc *parserFunc)
{
    if (str.isEmpty())
        return std::make_tuple(false, QVariant(), "Eval condition failed (empty string passed)");

    QStringList validStringsBeforeBracket({ "+", "-", "/", "%", "//", "*", "**", "and", "or", "not", "&&", "||" });
    QtTIBracketsNode brNode = QtTIParserBracketsExpr::parseBracketsExpression(str, validStringsBeforeBracket, parserArgs, [parserArgs, parserFunc](const QString &nodeBody) {
        return QtTIControlBlockIf::parseParamValue(nodeBody, parserArgs, parserFunc);
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
std::tuple<bool, QVariant, QString> QtTIControlBlockIf::parseParamValue(const QString &str,
                                                                        QtTIParserArgs *parserArgs,
                                                                        QtTIParserFunc *parserFunc)
{
    if (str.isEmpty())
        return std::make_tuple(false, QVariant(), "Parse value failed (empty string passed)");

    // ternary
    if (QtTIParserTernaryOperator::isTernaryOperatorExpr(str)) {
        bool isOk = false;
        QString error;
        QVariant result = QtTIParserTernaryOperator::parseTernaryOperator(str, parserArgs, parserFunc, &isOk, error);
        if (!isOk)
            return std::make_tuple(false, QVariant(), error);

        return std::make_tuple(true, result, "");
    }

    // null-coalescing
    if (QtTIParserNullCoalescingOperator::isNullCoalescingOperatorExpr(str)) {
        bool isOk = false;
        QString error;
        QVariant result = QtTIParserNullCoalescingOperator::parseNullCoalescingOperator(str, parserArgs, parserFunc, &isOk, error);
        if (!isOk)
            return std::make_tuple(false, QVariant(), error);

        return std::make_tuple(true, result, "");
    }

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

    QString tmpStr = str;
    QRegExp rx("not\\s+(.*)");
    const bool useNot = rx.exactMatch(str);
    if (useNot)
        tmpStr = rx.cap(1);

    // function
    QRegExp rxFunc("^(\\s*([\\w]+)\\s*\\(\\s*([A-Za-z0-9_\\ \\+\\-\\*\\,\\.\\'\\\"\\{\\}\\[\\]\\(\\)\\:\\/\\^\\$\\\\\\@\\#\\!\\<\\>\\=\\&\\%\\|\\;\\~]*)\\s*\\)\\s*)");
    if (rxFunc.indexIn(tmpStr) != -1) {
        QString funcName = rxFunc.cap(2).trimmed();
        QVariantList funcArgs = parserArgs->parseHelpFunctionArgs(rxFunc.cap(3).trimmed());
        bool isOk = false;
        QString error;
        QVariant result;
        std::tie(isOk, result, error) = parserFunc->evalHelpFunction(funcName, funcArgs);
        if (!isOk)
            return std::make_tuple(false, QVariant(), error);
        if (useNot) {
            if (result.type() == QVariant::Bool)
                return std::make_tuple(true, !result.toBool(), "");
            else if (result.isNull())
                return std::make_tuple(true, true, "");
            else
                return std::make_tuple(true, false, "");
        }
        return std::make_tuple(true, result, "");
    }

    // arg
    QVariantList tmp = parserArgs->parseHelpFunctionArgs(tmpStr);
    if (tmp.isEmpty())
        return std::make_tuple(false, QVariant(), "Parse value failed");
    if (tmp.size() > 1)
        return std::make_tuple(false, QVariant(), "Parse value failed (more than one argument is given)");

    if (useNot) {
        if (tmp[0].type() == QVariant::Bool)
            return std::make_tuple(true, !tmp[0].toBool(), "");
        else if (tmp[0].isNull())
            return std::make_tuple(true, true, "");
        else
            return std::make_tuple(true, false, "");
    }
    return std::make_tuple(true, tmp[0], "");
}
