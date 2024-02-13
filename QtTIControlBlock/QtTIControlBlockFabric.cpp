#include "QtTIControlBlockFabric.h"
#include "ControlBlocks/QtTIControlBlockSet.h"
#include "ControlBlocks/QtTIControlBlockUnset.h"
#include "ControlBlocks/QtTIControlBlockIf.h"
#include "ControlBlocks/QtTIControlBlockFor.h"

QtTIControlBlockFabric::QtTIControlBlockFabric(QtTIParser *parser)
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

//!
//! \brief Parse control block in line
//! \param line Line data
//! \param block Current control block pointer
//! \param lineNum Line number
//! \return
//!
std::tuple<bool, QString, QtTIAbstractControlBlock *, QString, QString> QtTIControlBlockFabric::parseBlock(const QString &line,
                                                                                                           QtTIAbstractControlBlock *block,
                                                                                                           const QString &unfinishedBlockCond,
                                                                                                           const int lineNum)
{
    QtTIAbstractControlBlock *curBlock = block;
    bool isBlockCond = !unfinishedBlockCond.isEmpty();
    bool isBlockBody = (curBlock != nullptr);
    QString blockCond = unfinishedBlockCond;
    QString tmpLine;
    QString tmpLinePart;
    for (int i = 0; i < line.size(); i++) {
        const QChar ch = line[i];
        QChar chPrev;
        if (i > 0)
            chPrev = line[i - 1];
        QChar chNext;
        if (i < line.size() - 1)
            chNext = line[i + 1];

        // check start block
        if (ch == '{' && chNext == '%') {
            isBlockCond = true;
            isBlockBody = false;
            bool isOk = false;
            QString res, err;
            std::tie(isOk, res, err) = _parser->parseLine(tmpLinePart, lineNum);
            if (!isOk)
                return std::make_tuple(false, "", nullptr, "", err);
            tmpLine += res;
            tmpLinePart.clear();
            blockCond.clear();
            continue;
        }
        // check end block
        if (ch == '}' && chPrev == '%') {
            isBlockCond = false;
            blockCond = blockCond.mid(1, blockCond.size() - 2).trimmed();
            if (curBlock && curBlock->isBlockCondIntermediate(blockCond)) {
                curBlock->appendBlockCondIntermediate(blockCond);
                blockCond.clear();
                isBlockBody = (curBlock != nullptr);
                continue;
            } else if (curBlock && curBlock->isBlockCondEnd(blockCond)) {
                if (!curBlock->parentBlock()) {
                    bool isOk = false;
                    QString res, err;
                    std::tie(isOk, res, err) = curBlock->evalBlock();
                    blockCond = curBlock->blockCondition();
                    delete curBlock;
                    curBlock = nullptr;
                    if (!isOk) {
                        QString errFull = QString("Eval control block '%1' in line %2 failed! Error: %3")
                                          .arg(blockCond)
                                          .arg(lineNum)
                                          .arg(err);
                        return std::make_tuple(false, "", nullptr, "", errFull);
                    }
                    tmpLine += res;
                } else {
                    curBlock = curBlock->parentBlock();
                }
                isBlockBody = (curBlock != nullptr);
                blockCond.clear();
                continue;
            }
            QtTIAbstractControlBlock *tmpBlock = createBlock(blockCond, lineNum);
            if (!tmpBlock) {
                QString err = QString("Unsupported control block '%1' in line %2")
                              .arg(blockCond)
                              .arg(lineNum);
                return std::make_tuple(false, "", nullptr, "", err);
            }
            if (curBlock)
                curBlock->appendSubBlock(tmpBlock);
            curBlock = tmpBlock;
            isBlockBody = true;
            if (curBlock->isBlockCondEnd(blockCond)) {
                if (!curBlock->parentBlock()) {
                    bool isOk = false;
                    QString res, err;
                    std::tie(isOk, res, err) = curBlock->evalBlock();
                    blockCond = curBlock->blockCondition();
                    delete curBlock;
                    curBlock = nullptr;
                    if (!isOk) {
                        QString errFull = QString("Eval control block '%1' in line %2 failed! Error: %3")
                                          .arg(blockCond)
                                          .arg(lineNum)
                                          .arg(err);
                        return std::make_tuple(false, "", nullptr, "", errFull);
                    }
                    tmpLine += res;
                } else {
                    curBlock = curBlock->parentBlock();
                }
                isBlockBody = (curBlock != nullptr);
                blockCond.clear();
                continue;
            }
            blockCond.clear();
            continue;
        }
        // check
        if (isBlockCond)
            blockCond += ch;
        else if (isBlockBody && curBlock)
            curBlock->appendBlockBody(ch);
        else
            tmpLinePart += ch;
    }
    if (!tmpLinePart.isEmpty())
        tmpLine += tmpLinePart;
    return std::make_tuple(true, tmpLine, curBlock, blockCond.trimmed(), "");
}
