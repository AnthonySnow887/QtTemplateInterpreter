#include "QtTIParser.h"
#include "QtTIParserBlock.h"
#include "QtTIParserArgs.h"
#include "QtTIParserFunc.h"
#include "Math/QtTIParserMath.h"
#include "Logic/QtTIParserLogic.h"
#include "TernaryOperator/QtTIParserTernaryOperator.h"
#include "NullCoalescingOperator/QtTIParserNullCoalescingOperator.h"
#include "../QtTIDefines/QtTIRegExpDefines.h"
#include "../QtTIHelperFunction/QtTIHelperFunction.h"

QtTIParser::QtTIParser()
    : QtTIAbstractParser()
{
    _parserArgs.setTIParserFunc(&_parserFunc);
}

QtTIParser::~QtTIParser()
{
    _parserFunc.clearFunctions();
    _parserArgs.clearParams();
    _parserArgs.clearTmpParams();
}

//!
//! \brief Select parser args object pointer
//! \return
//!
QtTIAbstractParserArgs *QtTIParser::parserArgs()
{
    return &_parserArgs;
}

//!
//! \brief Select parser func object pointer
//! \return
//!
QtTIAbstractParserFunc *QtTIParser::parserFunc()
{
    return &_parserFunc;
}

//!
//! \brief Parse line data (parse help functions and help parameters)
//! \param line Line data
//! \param lineNum Line number
//! \param block Abstract parser block object
//! \return
//!
std::tuple<bool, QString, QString> QtTIParser::parseLine(const QString &line,
                                                         const int lineNum,
                                                         QtTIAbstractParserBlock *&block)
{
    QtTIParserBlock *rBlock = dynamic_cast<QtTIParserBlock*>(block);
    QtTIParserBlock lastBlock;
    if (rBlock)
        lastBlock = *rBlock;
    QtTIParserBlock tmpBlock;
    bool isBlock = rBlock ? rBlock->isUnfinished() : false;
    bool isString = false;
    QString tmpLine;
    for (int i = 0; i < line.size(); i++) {
        const int chPos = i;
        const QChar ch = line[chPos];

        QChar chPrev;
        if (i > 0)
            chPrev = line[i - 1];
        QChar chNext;
        if (i < line.size() - 1)
            chNext = line[i + 1];

        // check is string
        if ((ch == '"' || ch == '\'')
            && chPrev != '\\'
            && isBlock)
            isString = !isString;

        // check start block
        if (!isString
            && !isBlock
            && ch == '{'
            && (chNext == '{'
                || chNext == '%'
                || chNext == '#')) {
            tmpBlock._data += ch;
            tmpBlock._startPos.first = lineNum;
            tmpBlock._startPos.second = chPos;
            tmpBlock._hasDataBeforeBlock = !tmpLine.trimmed().isEmpty();
            isBlock = true;
            continue;
        }
        // check end block
        if (!isString
            && isBlock
            && ch == '}'
            && (chPrev == '}'
                || chPrev == '%'
                || chPrev == '#')) {
            tmpBlock._data += ch;
            tmpBlock._endPos.first = lineNum;
            tmpBlock._endPos.second = chPos;

            // check is block end
            bool isBlockCondEnd = false;
            if (rBlock
                && rBlock->type() == QtTIParserBlock::Type::Control
                && rBlock->controlBlock()) {
                if (rBlock->controlBlock()->isBlockCondIntermediate(tmpBlock.body().trimmed())) {
                    rBlock->controlBlock()->appendBlockCondIntermediate(tmpBlock.body().trimmed());
                } else if (rBlock->controlBlock()->isBlockCondEnd(tmpBlock.body().trimmed())) {
                    isBlockCondEnd = true;
                    // update end pos
                    rBlock->_endPos = tmpBlock.endPos_ref();
                } else {
                    rBlock->controlBlock()->appendBlockBody(tmpBlock.data_ref(), tmpBlock.startPos_ref().first);
                }
            } else {               
                if (!rBlock) {
                    rBlock = new QtTIParserBlock();
                    rBlock->_data = tmpBlock.data_ref();
                    rBlock->_startPos = tmpBlock.startPos_ref();
                    rBlock->_endPos = tmpBlock.endPos_ref();
                    rBlock->_hasDataBeforeBlock = tmpBlock.hasDataBeforeBlock();
                } else if (rBlock->isUnfinished()) {
                    rBlock->_data += tmpBlock.data_ref();
                    rBlock->_endPos = tmpBlock.endPos_ref();
                } else {
                    rBlock->clear();
                    rBlock->_data = tmpBlock.data_ref();
                    rBlock->_startPos = tmpBlock.startPos_ref();
                    rBlock->_endPos = tmpBlock.endPos_ref();
                    rBlock->_hasDataBeforeBlock = tmpBlock.hasDataBeforeBlock();
                }

//                qDebug() << "IS BLOCK: "
//                         << rBlock->isValid()
//                         << static_cast<int>(rBlock->type())
//                         << rBlock->startPos()
//                         << rBlock->endPos()
//                         << rBlock->data_ref()
//                         << rBlock->body();

                // check is valid block
                if (!rBlock->isValid()) {
                    rBlock->_endPos.first = -1;
                    rBlock->_endPos.second = -1;
                    tmpBlock.clear();
                    isBlock = false;
                    continue;
                }

                // check block type
                if (rBlock->type() == QtTIParserBlock::Type::Control) {
                    QtTIAbstractControlBlock *bfObject = _blockFabric.createBlock(rBlock->body().trimmed(),
                                                                                  rBlock->startPos_ref().first,
                                                                                  rBlock->startPos_ref().second);
                    if (!bfObject) {
                        QString err = QString("Unsupported control block '%1' in line %2 (position %3)")
                                      .arg(rBlock->data_ref())
                                      .arg(rBlock->startPos_ref().first)
                                      .arg(rBlock->startPos_ref().second);
                        rBlock->clear();
                        delete rBlock;
                        block = rBlock = nullptr;
                        return std::make_tuple(false, "", err);
                    }
                    rBlock->setControlBlock(bfObject);
                    // check is block ended
                    isBlockCondEnd = rBlock->controlBlock()->isBlockCondEnd(rBlock->body().trimmed());
                }
            }

            // check is valid block
            if (rBlock->type() == QtTIParserBlock::Type::Invalid) {
                QString err = QString("Invalid block '%1' in line %2 (position %3)")
                              .arg(rBlock->data_ref())
                              .arg(rBlock->startPos_ref().first)
                              .arg(rBlock->startPos_ref().second);
                rBlock->clear();
                delete rBlock;
                block = rBlock = nullptr;
                return std::make_tuple(false, "", err);
            }

            lastBlock = *rBlock;

            // eval block
            if (rBlock->type() == QtTIParserBlock::Type::Base) {
                bool isOk = false;
                QVariant res;
                QString err;
                std::tie(isOk, res, err) = parseAndExecBlockData(rBlock);
                if (!isOk) {
                    QString errFull = QString("Eval control block '%1' in line %2 (position %3) failed! Error: %4")
                                      .arg(rBlock->data_ref())
                                      .arg(rBlock->startPos_ref().first)
                                      .arg(rBlock->startPos_ref().second)
                                      .arg(err);
                    rBlock->clear();
                    delete rBlock;
                    block = rBlock = nullptr;
                    return std::make_tuple(false, "", errFull);
                }

                // clear block object
                rBlock->clear();
                // add to tmpLine
                tmpLine += res.toString();
            } else if (rBlock->type() == QtTIParserBlock::Type::Control
                       && isBlockCondEnd) {
                bool isOk = false;
                QString res, err;
                std::tie(isOk, res, err) = rBlock->controlBlock()->evalBlock();
                if (!isOk) {
                    QString errFull = QString("Eval control block '%1' in line %2 (position %3) failed! Error: %4")
                                      .arg(rBlock->data_ref())
                                      .arg(rBlock->startPos_ref().first)
                                      .arg(rBlock->startPos_ref().second)
                                      .arg(err);
                    rBlock->clear();
                    delete rBlock;
                    block = rBlock = nullptr;
                    return std::make_tuple(false, "", errFull);
                }
                // clear block object
                rBlock->clear();
                // add to tmpLine,
                tmpLine += res;
            } else if (rBlock->type() == QtTIParserBlock::Type::Comment) {
                // clear block object
                rBlock->clear();
            }
            // clear
            tmpBlock.clear();
            isBlock = false;
            continue;
        }
        // check
        if (isBlock)
            tmpBlock._data += ch;
        else if (!isBlock
                 && rBlock
                 && rBlock->type() == QtTIParserBlock::Type::Control
                 && rBlock->controlBlock())
            rBlock->controlBlock()->appendBlockBody(ch, lineNum);
        else
            tmpLine += ch;
    }
    if (isBlock) {
        if (rBlock
            && rBlock->type() == QtTIParserBlock::Type::Control
            && rBlock->controlBlock()) {
            rBlock->controlBlock()->appendBlockBody(tmpBlock.data_ref(), tmpBlock.startPos_ref().first);
        } else {
            if (!rBlock) {
                rBlock = new QtTIParserBlock();
                rBlock->_data = tmpBlock.data_ref();
                rBlock->_startPos = tmpBlock.startPos_ref();
                rBlock->_endPos = tmpBlock.endPos_ref();
                rBlock->_hasDataBeforeBlock = tmpBlock.hasDataBeforeBlock();
            } else if (rBlock->isUnfinished()) {
                rBlock->_data += tmpBlock.data_ref();
                rBlock->_endPos = tmpBlock.endPos_ref();
            } else {
                rBlock->clear();
                rBlock->_data = tmpBlock.data_ref();
                rBlock->_startPos = tmpBlock.startPos_ref();
                rBlock->_endPos = tmpBlock.endPos_ref();
                rBlock->_hasDataBeforeBlock = tmpBlock.hasDataBeforeBlock();
            }
        }
        lastBlock = *rBlock;
    }
    if (!lastBlock.hasDataBeforeBlock()) {
        if (!lastBlock.isUnfinished()
            && (lastBlock.type() == QtTIParserBlock::Type::Control
                || lastBlock.type() == QtTIParserBlock::Type::Comment)) {
            const int lPos = lastBlock.endPos_ref().second + 1;
            const QString lineEnd = line.mid(lPos);
            if (lineEnd.trimmed().isEmpty()) {
                tmpLine = QtTIParser::rstrip(tmpLine);
                if (lastBlock.controlBlock()) {
                    QString cBlockBody = lastBlock.controlBlock()->blockBody(lineNum);
                    lastBlock.controlBlock()->setBlockBody(QtTIParser::rstrip(cBlockBody), lineNum);
                }
            }
        } else if (lastBlock.isUnfinished()
                   && (lastBlock.potentialType() == QtTIParserBlock::Type::Control
                       || lastBlock.potentialType() == QtTIParserBlock::Type::Comment)) {
            tmpLine = QtTIParser::rstrip(tmpLine);
        }
    }
    if (rBlock && rBlock->isEmpty()) {
        delete rBlock;
        rBlock = nullptr;
    }
    block = rBlock;
    return std::make_tuple(true, tmpLine, "");
}

//!
//! \brief Parse and execute abstract block data
//! \param block Abstract parser block object
//! \return
//!
std::tuple<bool, QVariant, QString> QtTIParser::parseAndExecBlockData(QtTIAbstractParserBlock *block)
{
    if (!block || !block->isValid())
        return std::make_tuple(false, QVariant(), "Invalid block object!");

    return parseAndExecBlockData(block->body(), block->startPos_ref());
}

//!
//! \brief Parse and execute abstract block data
//! \param data Parser block data
//! \param startPos Parser block start position in line
//! \return
//!
std::tuple<bool, QVariant, QString> QtTIParser::parseAndExecBlockData(const QString &data, const QPair<int, int> &startPos)
{
    if (data.isEmpty())
        return std::make_tuple(false, QVariant(), "Block data is Empty!");
    if (startPos.first == -1 || startPos.second == -1)
        return std::make_tuple(false, QVariant(), "Invalid block data position!");

    // check condition
    QVariant resultValue;
    if (QtTIParserTernaryOperator::isTernaryOperatorExpr(data)) {
        bool calcIsOk = false;
        QString calcErr;
        const QVariant calcRes = QtTIParserTernaryOperator::parseTernaryOperator(data, &_parserArgs, &_parserFunc, &calcIsOk, calcErr);
        if (!calcIsOk) {
            const QString error = QString("%1 in line %2 (position %3)")
                                  .arg(calcErr)
                                  .arg(startPos.first)
                                  .arg(startPos.second);
            return std::make_tuple(false, QVariant(), error);
        }
        resultValue = calcRes;

    } else if (QtTIParserNullCoalescingOperator::isNullCoalescingOperatorExpr(data)) {
        bool calcIsOk = false;
        QString calcErr;
        const QVariant calcRes = QtTIParserNullCoalescingOperator::parseNullCoalescingOperator(data, &_parserArgs, &_parserFunc, &calcIsOk, calcErr);
        if (!calcIsOk) {
            const QString error = QString("%1 in line %2 (position %3)")
                                  .arg(calcErr)
                                  .arg(startPos.first)
                                  .arg(startPos.second);
            return std::make_tuple(false, QVariant(), error);
        }
        resultValue = calcRes;

    } else if (QtTIParserLogic::isLogicExpr(data)) {
        bool calcIsOk = false;
        QString calcErr;
        const QVariant calcRes = QtTIParserLogic::parseLogic(data, &_parserArgs, &_parserFunc, &calcIsOk, calcErr);
        if (!calcIsOk) {
            const QString error = QString("%1 in line %2 (position %3)")
                                  .arg(calcErr)
                                  .arg(startPos.first)
                                  .arg(startPos.second);
            return std::make_tuple(false, QVariant(), error);
        }
        resultValue = calcRes;

    } else if (QtTIParserMath::isMathExpr(data)) {
        bool calcIsOk = false;
        QString calcErr;
        const QVariant calcRes = QtTIParserMath::parseMath(data, &_parserArgs, &_parserFunc, &calcIsOk, calcErr);
        if (!calcIsOk) {
            const QString error = QString("%1 in line %2 (position %3)")
                                  .arg(calcErr)
                                  .arg(startPos.first)
                                  .arg(startPos.second);
            return std::make_tuple(false, QVariant(), error);
        }
        resultValue = calcRes;

    } else {
        resultValue = _parserArgs.prepareHelpFunctionArg(data);
        if (!resultValue.isValid()) {
            const QString error = QString("Unsupported help parameter '%1' in line %2 (position %3)")
                                  .arg(data)
                                  .arg(startPos.first)
                                  .arg(startPos.second);
            return std::make_tuple(false, QVariant(), error);
        }
    }
    return std::make_tuple(true, resultValue, "");
}

//!
//! \brief Convert help parameter value to string
//! \param paramName Parameter name
//! \return
//!
QString QtTIParser::evalHelpParam(const QString &paramName)
{
    return _parserArgs.param(paramName).toString();
}

//!
//! \brief Convert help tmp parameter value to string
//! \param paramName Tmp parameter name
//! \return
//!
QString QtTIParser::evalHelpTmpParam(const QString &paramName)
{
    return _parserArgs.tmpParam(paramName).toString();
}

//!
//! \brief Execute help function
//! \param funcName Function name
//! \param args Function arguments
//! \return
//!
std::tuple<bool, QVariant, QString> QtTIParser::evalHelpFunction(const QString &funcName, const QVariantList &args) const
{
    return _parserFunc.evalHelpFunction(funcName, args);
}
