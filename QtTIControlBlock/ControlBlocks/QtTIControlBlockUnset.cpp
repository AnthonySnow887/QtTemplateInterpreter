#include "QtTIControlBlockUnset.h"

QtTIControlBlockUnset::QtTIControlBlockUnset(QtTIParser *parser)
    : QtTIAbstractControlBlock(parser, -1)
{}

QtTIControlBlockUnset::QtTIControlBlockUnset(QtTIParser *parser, const QString &blockCond, const int lineNum)
    : QtTIAbstractControlBlock(parser, lineNum)
    , _blockCond(blockCond)
{}

QtTIControlBlockUnset::~QtTIControlBlockUnset()
{}

//!
//! \brief Make control block
//! \param blockCond Control block condition
//! \param lineNum Line number
//! \return
//!
QtTIAbstractControlBlock *QtTIControlBlockUnset::makeBlock(const QString &blockCond, const int lineNum)
{
    return new QtTIControlBlockUnset(parser(), blockCond, lineNum);
}

//!
//! \brief Get control block condition
//! \return
//!
QString QtTIControlBlockUnset::blockCondition() const
{
    return _blockCond;
}

//!
//! \brief Check is control block start
//! \param blockCond Control block condition
//! \return
//!
bool QtTIControlBlockUnset::isBlockCondStart(const QString &blockCond)
{
    QRegExp rx("(unset\\s+([\\w]+))");
    return (rx.indexIn(blockCond) != -1);
}

//!
//! \brief Check is control block end
//! \param blockCond Control block condition
//! \return
//!
bool QtTIControlBlockUnset::isBlockCondEnd(const QString &blockCond)
{
    return (_blockCond == blockCond);
}

//!
//! \brief Execute control block
//! \return
//!
std::tuple<bool, QString, QString> QtTIControlBlockUnset::evalBlock()
{
    QRegExp rx("(unset\\s+([\\w]+))");
    if (rx.indexIn(_blockCond) != -1) {
        QString paramName = rx.cap(2).trimmed();
        // check
        if (paramName.isEmpty())
            return std::make_tuple(false, "", QString("Invalid parameter name (empty) in line %1").arg(lineNum()));
        if (parser()->parserArgs()->hasTmpParam(paramName))
            parser()->parserArgs()->removeTmpParam(paramName);

        return std::make_tuple(true, "", "");
    }
    return std::make_tuple(false, "", QString("Unsupported block condition '%1' in line %2").arg(_blockCond).arg(lineNum()));
}

