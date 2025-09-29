#ifndef QTTICONTROLBLOCKFABRIC_H
#define QTTICONTROLBLOCKFABRIC_H

#include <QString>

#include "ControlBlocks/QtTIAbstractControlBlock.h"

//!
//! \brief The QtTIControlBlockFabric class
//!
class QtTIControlBlockFabric
{
public:
    explicit QtTIControlBlockFabric(QtTIAbstractParser *parser);
    ~QtTIControlBlockFabric();

    QtTIAbstractControlBlock *createBlock(const QString &blockCond,
                                          const int lineNum,
                                          const int linePos);

private:
    QtTIAbstractParser *_parser {nullptr};
    QList<QtTIAbstractControlBlock*> _blocks;
};

#endif // QTTICONTROLBLOCKFABRIC_H
