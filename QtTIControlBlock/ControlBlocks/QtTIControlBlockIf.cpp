#include "QtTIControlBlockIf.h"
#include "../QtTIControlBlockFabric.h"
#include "../../QtTIParser/Logic/QtTIParserLogic.h"

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
        _elseIfBodys.append(QString());
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
        QVariant result = QtTIParserLogic::parseLogic(ifConf, parser()->parserArgs(), parser()->parserFunc(), &isOk, error);
        if (!isOk)
            return std::make_tuple(false, "", error);
        if (result.toBool())
            return buildBlockBody(_ifBody);
    }

    // check ELSEIF
    if (!_elseIfConds.isEmpty()) {
        for (int i = 0; i < _elseIfConds.size(); i++) {
            const QString elseIfCond = _elseIfConds[i];
            const QString elseIfBody = _elseIfBodys[i];
            // check IF
            QRegExp rxElseIf("(\\s{0,}elseif\\s+(.*))");
            if (rxElseIf.indexIn(elseIfCond) != -1) {
                QString ifConf = rxElseIf.cap(2).trimmed();
                bool isOk = false;
                QString error;
                QVariant result = QtTIParserLogic::parseLogic(ifConf, parser()->parserArgs(), parser()->parserFunc(), &isOk, error);
                if (!isOk)
                    return std::make_tuple(false, "", error);
                if (result.toBool())
                    return buildBlockBody(elseIfBody);
            }
        }
    }

    // check ELSE
    QRegExp rxElse("(\\s{0,}else\\s{0,})");
    if (rxElse.indexIn(_elseCond) != -1)
        return buildBlockBody(_elseBody);

    return std::make_tuple(false, "", "Incorrect conditions are specified for the IF block");
}

//!
//! \brief Append control block body
//! \param blockBody Control block body
//!
void QtTIControlBlockIf::appendBlockBody(const QString &blockBody)
{
    switch (_bodyPos) {
        case BodyPosition::If: {
            _ifBody += blockBody;
            break;
        }
        case BodyPosition::Else: {
            _elseBody += blockBody;
            break;
        }
        case BodyPosition::ElseIf: {
            _elseIfBodys[_elseIfBodys.size() - 1] += blockBody;
            break;
        }
        default:
            break;
    }
}
