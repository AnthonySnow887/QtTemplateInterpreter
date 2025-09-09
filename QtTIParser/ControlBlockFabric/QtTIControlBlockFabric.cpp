#include "QtTIControlBlockFabric.h"
#include "ControlBlocks/QtTIControlBlockSet.h"
#include "ControlBlocks/QtTIControlBlockUnset.h"
#include "ControlBlocks/QtTIControlBlockIf.h"
#include "ControlBlocks/QtTIControlBlockFor.h"

QtTIControlBlockFabric::QtTIControlBlockFabric(QtTIAbstractParser *parser)
    : _parser(parser)
{
    _blocks.append(new QtTIControlBlockSet(_parser));
    _blocks.append(new QtTIControlBlockUnset(_parser));
    _blocks.append(new QtTIControlBlockIf(_parser));
    _blocks.append(new QtTIControlBlockFor(_parser));
}

QtTIControlBlockFabric::~QtTIControlBlockFabric()
{
    qDeleteAll(_blocks);
    _blocks.clear();
    _parser = nullptr;
}

//!
//! \brief Create control block by block condition
//! \param blockCond Control block condition
//! \param lineNum Line number
//! \return
//!
QtTIAbstractControlBlock *QtTIControlBlockFabric::createBlock(const QString &blockCond, const int lineNum)
{
    for (QtTIAbstractControlBlock *b : qAsConst(_blocks)) {
        if (b->isBlockCondStart(blockCond))
            return b->makeBlock(blockCond, lineNum);
    }
    return nullptr;
}
