#include "QtTIControlBlockSet.h"

QtTIControlBlockSet::QtTIControlBlockSet(QtTIParser *parser)
    : QtTIAbstractControlBlock(parser, -1)
{}

QtTIControlBlockSet::QtTIControlBlockSet(QtTIParser *parser, const QString &blockCond, const int lineNum)
    : QtTIAbstractControlBlock(parser, lineNum)
    , _blockCond(blockCond)
{}

QtTIControlBlockSet::~QtTIControlBlockSet()
{}

//!
//! \brief Make control block
//! \param blockCond Control block condition
//! \param lineNum Line number
//! \return
//!
QtTIAbstractControlBlock *QtTIControlBlockSet::makeBlock(const QString &blockCond, const int lineNum)
{
    return new QtTIControlBlockSet(parser(), blockCond, lineNum);
}

//!
//! \brief Get control block condition
//! \return
//!
QString QtTIControlBlockSet::blockCondition() const
{
    return _blockCond;
}

//!
//! \brief Check is control block start
//! \param blockCond Control block condition
//! \return
//!
bool QtTIControlBlockSet::isBlockCondStart(const QString &blockCond)
{
    QRegExp rx("(set\\s+([\\w]+)\\s{0,}\\=\\s{0,}([A-Za-z0-9_\\ \\+\\-\\%\\*\\,\\.\\'\\\"\\{\\}\\[\\]\\:\\/\\!\\=\\<\\>\\(\\)\\&\\|\\^\\$]+))");
    return (rx.indexIn(blockCond) != -1);
}

//!
//! \brief Check is control block end
//! \param blockCond Control block condition
//! \return
//!
bool QtTIControlBlockSet::isBlockCondEnd(const QString &blockCond)
{
    return (_blockCond == blockCond);
}

//!
//! \brief Execute control block
//! \return
//!
std::tuple<bool, QString, QString> QtTIControlBlockSet::evalBlock()
{
    QRegExp rx("(set\\s+([\\w]+)\\s{0,}\\=\\s{0,}([A-Za-z0-9_\\ \\+\\-\\%\\*\\,\\.\\'\\\"\\{\\}\\[\\]\\:\\/\\!\\=\\<\\>\\(\\)\\&\\|\\^\\$\\\\\\@\\#\\%\\;\\~]+))");
    if (rx.indexIn(_blockCond) != -1) {
        QString paramName = rx.cap(2).trimmed();
        QString paramExpr = rx.cap(3).trimmed();
        bool isOk = false;
        QVariant paramValue;
        QString error;
        std::tie(isOk, paramValue, error) = parseParamValue(paramExpr);
        if (!isOk)
            return std::make_tuple(false, "", QString("Parse parameter value in block 'set ...' in line %1 failed! Error: %2").arg(lineNum()).arg(error));
        if (paramName.isEmpty())
            return std::make_tuple(false, "", QString("Invalid parameter name (empty) in line %1").arg(lineNum()));
        if (paramValue.isNull() && paramValue.type() != QVariant::Type::String)
            return std::make_tuple(false, "", QString("Invalid parameter value (Null) in line %1").arg(lineNum()));
        if (parser()->parserArgs()->hasParam(paramName))
            return std::make_tuple(false, "", QString("Parameter with name '%1' in line %2 already declared in the global parameter list").arg(paramName).arg(lineNum()));

        parser()->parserArgs()->appendTmpParam(paramName, paramValue);
        return std::make_tuple(true, "", "");
    }
    return std::make_tuple(false, "", QString("Unsupported block condition '%1' in line %2").arg(_blockCond).arg(lineNum()));
}
