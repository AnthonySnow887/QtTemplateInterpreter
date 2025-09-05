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

    QtTIAbstractControlBlock *createBlock(const QString &blockCond, const int lineNum);

    std::tuple<bool/*isOk*/,
               QString/*res*/,
               QtTIAbstractControlBlock*/*curBlock*/,
               QString/*unfinishedBlockCond*/,
               QString/*err*/> parseBlock(const QString &line,
                                          QtTIAbstractControlBlock *block,
                                          const QString &unfinishedBlockCond,
                                          const int lineNum);

private:
    QtTIAbstractParser *_parser {nullptr};
    QList<QtTIAbstractControlBlock*> _blocks;
};

#endif // QTTICONTROLBLOCKFABRIC_H
