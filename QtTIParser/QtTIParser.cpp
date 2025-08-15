#include "QtTIParser.h"
#include "Math/QtTIParserMath.h"
#include "Logic/QtTIParserLogic.h"
#include "TernaryOperator/QtTIParserTernaryOperator.h"
#include "NullCoalescingOperator/QtTIParserNullCoalescingOperator.h"
#include "../QtTIControlBlock/QtTIControlBlockFabric.h"
#include "../QtTIDefines/QtTIRegExpDefines.h"

QtTIParser::QtTIParser()
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
QtTIParserArgs *QtTIParser::parserArgs()
{
    return &_parserArgs;
}

//!
//! \brief Select parser func object pointer
//! \return
//!
QtTIParserFunc *QtTIParser::parserFunc()
{
    return &_parserFunc;
}

//!
//! \brief Remove all comments in line
//! \param line Line data
//! \param isMultiline Check is multi-line comment
//! \return
//!
QString QtTIParser::removeComments(const QString &line, bool *isMultiline)
{
    bool isComment = false;
    if (isMultiline)
        isComment = *isMultiline;
    QString tmpLine;
    for (int i = 0; i < line.size(); i++) {
        const QChar ch = line[i];
        QChar chPrev;
        if (i > 0)
            chPrev = line[i - 1];
        QChar chNext;
        if (i < line.size() - 1)
            chNext = line[i + 1];

        // check start comment
        if (ch == '{' && chNext == '#') {
            isComment = true;
            continue;
        }
        // check end comment
        if (ch == '}' && chPrev == '#') {
            isComment = false;
            continue;
        }
        // check
        if (isComment)
            continue;
        // append char
        tmpLine += ch;
    }
    if (isMultiline)
        *isMultiline = isComment;
    return tmpLine;
}

//!
//! \brief Parse line data (parse help functions and help parameters)
//! \param line Line data
//! \param lineNum Line number
//! \return
//!
std::tuple<bool, QString, QString> QtTIParser::parseLine(const QString &line, const int lineNum)
{
    if (line.isEmpty())
        return std::make_tuple(true, line, "");
    bool isOk = false;
    QString newLine = line;
    QString error;

    // check functions
    std::tie(isOk, newLine, error) = parseHelpFunctions(newLine, lineNum);
    if (!isOk)
        return std::make_tuple(false, "", error);

    // check params
    std::tie(isOk, newLine, error) = parseHelpParams(newLine, lineNum);
    if (!isOk)
        return std::make_tuple(false, "", error);

    return std::make_tuple(true, newLine, "");
}

std::tuple<bool, QString, QString, QtTIParserBlock*> QtTIParser::parseLine_v2(const QString &line, const int lineNum, QtTIParserBlock *block)
{
    QString blockData;
    QPair<int, int> blockStartPos(-1, -1);
    QPair<int, int> blockEndPos(-1, -1);
    bool isBlock = false;
    if (block)
        isBlock = block->isUnfinished();
//    if (isBlock) {
//        blockData = block.data();
//        blockStartPos = block.startPos();
//    }

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
        if (ch == '"' || ch == '\'')
            isString = !isString;

        // check start block
        if (!isString
            && ch == '{'
            && (chNext == '{'
                || chNext == '%'
                || chNext == '#')) {
            blockData += ch;
            blockStartPos.first = lineNum;
            blockStartPos.second = chPos;
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
            blockData += ch;
            blockEndPos.first = lineNum;
            blockEndPos.second = chPos;

            // check is block end
            bool isBlockCondEnd = false;
            if (block
                && block->type() == QtTIParserBlock::Type::Control
                && block->controlBlock()) {
                QtTIParserBlock tmpBlock(blockData, blockStartPos, blockEndPos);
                if (block->controlBlock()->isBlockCondIntermediate(tmpBlock.body().trimmed())) {
                    block->controlBlock()->appendBlockCondIntermediate(tmpBlock.body().trimmed());
                } else if (block->controlBlock()->isBlockCondEnd(tmpBlock.body().trimmed())) {
                    isBlockCondEnd = true;
                    // update end pos
                    block->_endPos = tmpBlock.endPos_ref();
                } else {
                    block->controlBlock()->appendBlockBody(tmpBlock.data_ref(), tmpBlock.startPos().first);
                }
            } else {
                if (block) {
                    block->_data += blockData;
                    block->_endPos = blockEndPos;
                } else {
                    block = new QtTIParserBlock(blockData, blockStartPos, blockEndPos);
                    block->setHasDataBeforeBlock(!tmpLine.trimmed().isEmpty());
                }

                qDebug() << "IS BLOCK: "
                         << block->isValid()
                         << static_cast<int>(block->type())
                         << block->startPos()
                         << block->endPos()
                         << block->data_ref()
                         << block->body();

                // check is valid block
                if (!block->isValid()) {
                    QString err = QString("Invalid block '%1' in line %2")
                                  .arg(block->data_ref())
                                  .arg(block->startPos().first);
                    delete block;
                    block = nullptr;
                    return std::make_tuple(false, "", err, nullptr);
                }

                // check block type
                if (block->type() == QtTIParserBlock::Type::Control) {
                    QtTIControlBlockFabric bf(this); // TODO
                    QtTIAbstractControlBlock *bfObject = bf.createBlock(block->body().trimmed(), block->startPos().first);
                    if (!bfObject) {
                        QString err = QString("Unsupported control block '%1' in line %2")
                                      .arg(block->data_ref())
                                      .arg(block->startPos().first);
                        delete block;
                        block = nullptr;
                        return std::make_tuple(false, "", err, nullptr);
                    }
                    block->setControlBlock(bfObject);
                    // check is block ended
                    isBlockCondEnd = block->controlBlock()->isBlockCondEnd(block->body().trimmed());
                }
            }

            // check is valid block
            if (block->type() == QtTIParserBlock::Type::Invalid) {
                QString err = QString("Invalid block '%1' in line %2")
                              .arg(block->data_ref())
                              .arg(block->startPos().first);
                delete block;
                block = nullptr;
                return std::make_tuple(false, "", err, nullptr);
            }

            // eval block
            if (block->type() == QtTIParserBlock::Type::Base) {
                // TODO parse function or param
                QString res = QString("START{{ %1 }}END").arg(block->body());

                // add to tmpLine
                tmpLine += res;
            } else if (block->type() == QtTIParserBlock::Type::Control
                       && isBlockCondEnd) {
                bool isOk = false;
                QString res, err;
                std::tie(isOk, res, err) = block->controlBlock()->evalBlock();
                if (!isOk) {
                    QString errFull = QString("Eval control block '%1' in line %2 failed! Error: %3")
                                      .arg(block->data_ref())
                                      .arg(block->startPos().first)
                                      .arg(err);
                    delete block;
                    block = nullptr;
                    return std::make_tuple(false, "", errFull, nullptr);
                }
                // add to tmpLine,
                tmpLine += res;
            }
            // clear block object
            delete block;
            block = nullptr;
            // clear
            blockData.clear();
            isBlock = false;
            continue;
        }
        // check
        if (isBlock)
            blockData += ch;
        else if (!isBlock
                 && block
                 && block->type() == QtTIParserBlock::Type::Control
                 && block->controlBlock())
            block->controlBlock()->appendBlockBody(ch, lineNum);
        else
            tmpLine += ch;
    }
    if (isBlock) {
        if (block) {
            if (block->type() == QtTIParserBlock::Type::Control
                && block->controlBlock()) {
                QtTIParserBlock tmpBlock(blockData, blockStartPos, blockEndPos);
                block->controlBlock()->appendBlockBody(tmpBlock.data_ref(), tmpBlock.startPos().first);
            } else {
                block->_data += blockData;
            }
        } else {
            block = new QtTIParserBlock(blockData, blockStartPos, blockEndPos);
            block->setHasDataBeforeBlock(!tmpLine.trimmed().isEmpty()); // TODO ...
        }
        if (block->potentialType() == QtTIParserBlock::Type::Control
            || block->type() == QtTIParserBlock::Type::Comment)
            tmpLine = tmpLine.trimmed();
    }
    if (block
        && !block->hasDataBeforeBlock()
        && (block->type() == QtTIParserBlock::Type::Control
         || block->type() == QtTIParserBlock::Type::Comment)) {
        const int lPos = block->endPos_ref().second + 1;
        const QString lineEnd = line.mid(lPos);
        if (lineEnd.trimmed().isEmpty()) {
            tmpLine = QtTIParser::rstrip(tmpLine);
            if (block->controlBlock()) {
                QString cBlockBody = block->controlBlock()->blockBody(lineNum);
                block->controlBlock()->setBlockBody(QtTIParser::rstrip(cBlockBody), lineNum);
            }
        }
    }
    return std::make_tuple(true, tmpLine, "", block);
}


//!
//! \brief Parse help functions in line
//! \param line Line data
//! \param lineNum Line number
//! \return
//!
//! === Example
//!     ![This is Lu and Bryu!]( {{ image_path ('configure.svg') }} "Lu and Bryu")
//!
//! === Example 2:
//!     ![This is Lu and Bryu!]( {{ image_path_b (b'configure.svg') }} \"Lu and Bryu\") --- {{ image_path_2 ([ui1, ul2, l3, 4, [5.2, -6]]) }}  {{ image_path_3 ({a:f1.2, b:2, c : +f3.5, ff : h{d : 4, e:-5}, f:-6.2}) }}
//!
std::tuple<bool, QString, QString> QtTIParser::parseHelpFunctions(const QString &line, const int lineNum)
{
    QString tmpLine = line;
    bool isOk = false;
    QString error;
    QMap<QString, QVariant> funcRes = parseAndExecHelpFunctions(line, lineNum, &isOk, error);
    if (!isOk)
        return std::make_tuple(false, "", error);

    QMapIterator<QString, QVariant> it(funcRes);
    while (it.hasNext()) {
        it.next();
        tmpLine = tmpLine.replace(it.key(), it.value().toString());
    }
    return std::make_tuple(true, tmpLine, "");
}

//!
//! \brief Parse and exec help functions in line
//! \param line Line data
//! \param lineNum Line number
//! \param[in,out] isOk Check is success
//! \param[in,out] error Error string
//! \return
//!
QMap<QString, QVariant> QtTIParser::parseAndExecHelpFunctions(const QString &line, const int lineNum, bool *isOk, QString &error)
{
    error.clear();
    QMap<QString, QVariant> tmpRes;
    QRegExp rxFunc(RX_BLOCK_FUNC);
    int pos = 0;
    while ((pos = rxFunc.indexIn(line, pos)) != -1) {
        pos += rxFunc.matchedLength();
        if (rxFunc.captureCount() < 5)
            continue;
        QString replaceStr = rxFunc.cap(1);
        const bool isRawValue = rxFunc.cap(2) == "rv";
        QVariant funcRes;
        if (isRawValue) {
            funcRes = QString(replaceStr).remove(0, 2);
        } else {
            QString tagOpen = rxFunc.cap(3);
            QString helpFunc = rxFunc.cap(4);
            QVariantList helpFuncArgs = _parserArgs.parseHelpFunctionArgs(rxFunc.cap(5));
            QString tagClose = rxFunc.cap(6);
            // check
            if (tagOpen == "{" && tagClose != "}") {
                if (isOk)
                    *isOk = false;
                error = QString("Invalid closed symbol (not '}') in line %1").arg(lineNum);
                return QMap<QString, QVariant>();
            }
            if (tagOpen == "#" && tagClose != "#") {
                if (isOk)
                    *isOk = false;
                error = QString("Invalid closed symbol (not '#') in line %1").arg(lineNum);
                return QMap<QString, QVariant>();
            }
            if (!_parserFunc.hasHelpFunction(helpFunc, helpFuncArgs)) {
                if (isOk)
                    *isOk = false;
                error = QString("Unsupported help function '%1 (%2)' in line %3")
                        .arg(helpFunc,
                             QtTIAbstractHelperFunction::typesToStr(QtTIAbstractHelperFunction::vListArgsTypes(helpFuncArgs)))
                        .arg(lineNum);
                return QMap<QString, QVariant>();
            }
            if (tagOpen != "#") {
                bool isOkEval = false;
                QString err;
                std::tie(isOkEval, funcRes, err) = evalHelpFunction(helpFunc, helpFuncArgs);

                // check
                if (!isOkEval) {
                    if (isOk)
                        *isOk = false;
                    error = QString("Eval help function '%1 (%2)' in line %3 failed! Error: '%4'")
                            .arg(helpFunc,
                                 QtTIAbstractHelperFunction::typesToStr(QtTIAbstractHelperFunction::vListArgsTypes(helpFuncArgs)))
                            .arg(lineNum)
                            .arg(err);
                    return QMap<QString, QVariant>();
                }
            }
        }
        tmpRes.insert(replaceStr, funcRes);
    }
    if (isOk)
        *isOk = true;
    return tmpRes;
}

//!
//! \brief Parse help parameters in line
//! \param line Line data
//! \param lineNum Line number
//! \return
//!
//! === Example
//!       Key: {{ my_key }}
//!     Key 2: {{ my_key_2 }}
//!     Key 3: {{ my_key3 }}
//!
//! === Variables:
//!     The application passes variables to the templates for manipulation in the template.
//!     Variables may have attributes or elements you can access, too.
//!     The visual representation of a variable depends heavily on the application providing it.
//!     Use a dot (.) to access attributes of a variable (properties of a object, or items of a array):
//!             {{ foo.bar }}
//!
//! === Math operators (only for numbers):
//!     The following operators are supported:
//!      + : Adds two numbers together (the operands are casted to numbers):
//!             {{ 1 + 1 }} is 2
//!
//!      - : Subtracts the second number from the first one:
//!             {{ 3 - 2 }} is 1
//!
//!      / : Divides two numbers. The returned value will be a floating point number:
//!             {{ 1 / 2 }} is {{ 0.5 }}
//!
//!      % : Calculates the remainder of an integer division:
//!             {{ 11 % 7 }} is 4
//!
//!     // : Divides two numbers and returns the floored integer result:
//!             {{ 20 // 7 }} is 2, {{ -20  // 7 }} is -3 (this is just syntactic sugar for the round filter)
//!
//!      * : Multiplies the left operand with the right one:
//!             {{ 2 * 2 }} would return 4
//!
//!     ** : Raises the left operand to the power of the right operand:
//!             {{ 2 ** 3 }} would return 8
//!
//! === Other operators:
//!     ?: : The ternary operator:
//!             {{ foo ? 'yes' : 'no' }}
//!             {{ foo ?: 'no' }} is the same as {{ foo ? foo : 'no' }}
//!             {{ foo ? 'yes' }} is the same as {{ foo ? 'yes' : '' }}
//!
//!     ?? : The null-coalescing operator:
//!             {# returns the value of foo if it is defined and not null, 'no' otherwise #}
//!             {{ foo ?? 'no' }}
//!
std::tuple<bool, QString, QString> QtTIParser::parseHelpParams(const QString &line, const int lineNum)
{
    QString tmpLine = line;
    bool isOk = false;
    QString error;
    QMap<QString, QVariant> funcRes = parseAndExecHelpParams(line, lineNum, &isOk, error);
    if (!isOk)
        return std::make_tuple(false, "", error);

    QMapIterator<QString, QVariant> it(funcRes);
    while (it.hasNext()) {
        it.next();
        tmpLine = tmpLine.replace(it.key(), it.value().toString());
    }
    return std::make_tuple(true, tmpLine, "");
}

//!
//! \brief Parse and exec help parameters in line
//! \param line Line data
//! \param lineNum Line number
//! \param[in,out] isOk Check is success
//! \param[in,out] error Error string
//! \return
//!
QMap<QString, QVariant> QtTIParser::parseAndExecHelpParams(const QString &line, const int lineNum, bool *isOk, QString &error)
{
    error.clear();
    QMap<QString, QVariant> tmpRes;
    QRegExp rx(RX_BLOCK_PARAMS);
    QRegExp rxParam(RX_PARAM);
    int pos = 0;
    while ((pos = rx.indexIn(line, pos)) != -1) {
        pos += rx.matchedLength();
        if (rx.captureCount() < 4)
            continue;
        QString replaceStr = rx.cap(1);
        QString tagOpen = rx.cap(3);

        // get valif expr
        replaceStr = validParamExpr(replaceStr, QString("%1}").arg((tagOpen == "{") ? "}" : "#" ));
        // re-calculate pos
        pos -= (rx.matchedLength() - replaceStr.size());
        // re-select regex data
        if (rx.indexIn(replaceStr, 0) == -1)
            continue;
        const bool isRawValue = rx.cap(2) == "rv";
        QString helpCondition = rx.cap(4).trimmed();
        QString tagClose = rx.cap(5);

        // check
        if (tagOpen == "{" && tagClose != "}") {
            if (isOk)
                *isOk = false;
            error = QString("Invalid closed symbol (not '}') in line %1").arg(lineNum);
            return QMap<QString, QVariant>();
        }
        if (tagOpen == "#" && tagClose != "#") {
            if (isOk)
                *isOk = false;
            error = QString("Invalid closed symbol (not '#') in line %1").arg(lineNum);
            return QMap<QString, QVariant>();
        }

        // check condition
        QVariant resultValue;
        if (isRawValue) {
            resultValue = QString(replaceStr).remove(0, 2);
        } else if (tagOpen != "#" && QtTIParserTernaryOperator::isTernaryOperatorExpr(helpCondition)) {
            bool calcIsOk = false;
            QString calcErr;
            QVariant calcRes = QtTIParserTernaryOperator::parseTernaryOperator(helpCondition, &_parserArgs, &_parserFunc, &calcIsOk, calcErr);
            if (!calcIsOk) {
                if (isOk)
                    *isOk = false;
                error = QString("%1 in line %2").arg(calcErr).arg(lineNum);
                return QMap<QString, QVariant>();
            }
            resultValue = calcRes;

        } else if (tagOpen != "#" && QtTIParserNullCoalescingOperator::isNullCoalescingOperatorExpr(helpCondition)) {
            bool calcIsOk = false;
            QString calcErr;
            QVariant calcRes = QtTIParserNullCoalescingOperator::parseNullCoalescingOperator(helpCondition, &_parserArgs, &_parserFunc, &calcIsOk, calcErr);
            if (!calcIsOk) {
                if (isOk)
                    *isOk = false;
                error = QString("%1 in line %2").arg(calcErr).arg(lineNum);
                return QMap<QString, QVariant>();
            }
            resultValue = calcRes;

        } else if (tagOpen != "#" && QtTIParserLogic::isLogicExpr(helpCondition)) {
            bool calcIsOk = false;
            QString calcErr;
            QVariant calcRes = QtTIParserLogic::parseLogic(helpCondition, &_parserArgs, &_parserFunc, &calcIsOk, calcErr);
            if (!calcIsOk) {
                if (isOk)
                    *isOk = false;
                error = QString("%1 in line %2").arg(calcErr).arg(lineNum);
                return QMap<QString, QVariant>();
            }
            resultValue = calcRes;

        } else if (tagOpen != "#" && QtTIParserMath::isMathExpr(helpCondition)) {
            bool calcIsOk = false;
            QString calcErr;
            QVariant calcRes = QtTIParserMath::parseMath(helpCondition, &_parserArgs, &_parserFunc, &calcIsOk, calcErr);
            if (!calcIsOk) {
                if (isOk)
                    *isOk = false;
                error = QString("%1 in line %2").arg(calcErr).arg(lineNum);
                return QMap<QString, QVariant>();
            }
            resultValue = calcRes;

        } else if (tagOpen != "#" && rxParam.indexIn(helpCondition) != -1) {
            if (_parserArgs.hasParam(helpCondition)) {
                resultValue = evalHelpParam(helpCondition);
            } else if (_parserArgs.hasTmpParam(helpCondition)) {
                resultValue = evalHelpTmpParam(helpCondition);
            } else {
                if (isOk)
                    *isOk = false;
                error = QString("Unsupported help parameter '%1' in line %2").arg(helpCondition).arg(lineNum);
                return QMap<QString, QVariant>();
            }
        }
        tmpRes.insert(replaceStr, resultValue);
    }
    if (isOk)
        *isOk = true;
    return tmpRes;
}

QString QtTIParser::lstrip(const QString &str)
{
    int n = 0;
    for (; n < str.size(); ++n) {
        if (!str.at(n).isSpace())
            return str.right(str.size() - n);
    }
    return "";
}

QString QtTIParser::rstrip(const QString &str)
{
    int n = str.size() - 1;
    for (; n >= 0; --n) {
        if (!str.at(n).isSpace())
            return str.left(n + 1);
    }
    return "";
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

//!
//! \brief Get valid parameter expression
//! \param str String for select
//! \param ending Substring that should end the original string
//! \return
//!
//! === Example
//!           Str: "{{ foo ?: 'no' }} is the same as {{ foo ? foo : 'no' }}"
//!        Ending: "}}"
//!        Result: "{{ foo ?: 'no' }}"
//!
QString QtTIParser::validParamExpr(const QString &str, const QString &ending) const
{
    if (str.isEmpty() || ending.isEmpty())
        return str;
    QString tmpValue;
    bool isEescaping = false;
    for (int i = 0; i < str.size(); i++) {
        if (tmpValue.endsWith(ending) && !isEescaping)
            return tmpValue;

        const QChar ch = str[i];
        QChar chPrev;
        if (i > 0)
            chPrev = str[i - 1];

        if (ch == "'" || ch == "\"") {
            if (chPrev == "\\") {
                tmpValue += ch;
                continue;
            }
            tmpValue += ch;
            isEescaping = !isEescaping;
            continue;
        }
        tmpValue += ch;
    }
    return str;
}
