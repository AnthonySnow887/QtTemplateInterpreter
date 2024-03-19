#ifndef QTTIABSTRACTCONTROLBLOCK_H
#define QTTIABSTRACTCONTROLBLOCK_H

#include <QUuid>
#include <QString>
#include <QList>
#include <QTextStream>
#include <QCryptographicHash>
#include <QMap>
#include "../../QtTIParser/QtTIParser.h"
#include "../../QtTIParser/TernaryOperator/QtTIParserTernaryOperator.h"
#include "../../QtTIParser/NullCoalescingOperator/QtTIParserNullCoalescingOperator.h"
#include "../../QtTIParser/Logic/QtTIParserLogic.h"
#include "../../QtTIParser/Math/QtTIParserMath.h"

//!
//! \brief The QtTIAbstractControlBlock class
//!
class QtTIAbstractControlBlock
{
public:
    //!
    //! \brief QtTIAbstractControlBlock
    //! \param parser QtTIParser pointer
    //! \param lineNum Line number
    //!
    QtTIAbstractControlBlock(QtTIParser *parser, const int lineNum)
        : _uuid(QUuid::createUuid().toString().mid(1,36))
        , _uuidHash(QCryptographicHash::hash(_uuid.toUtf8(), QCryptographicHash::Sha1).toHex().constData())
        , _lineNum(lineNum)
        , _parser(parser)
    {}

    virtual ~QtTIAbstractControlBlock() {
        for (const QtTIAbstractControlBlock *b : qAsConst(_subBlocks)) {
            // remove hlp tmp func
            _parser->parserFunc()->removeHelpFunction(QString("eval_block_%1").arg(b->uuidHash()), QVariantList());
            delete b;
        }
        _subBlocks.clear();
        _parentBlock = nullptr;
        _parser = nullptr;
    }

    //!
    //! \brief Get control block uuid
    //! \return
    //!
    QString uuid() const {
        return _uuid;
    }

    //!
    //! \brief Get control block uuid SHA1 hash
    //! \return
    //!
    QString uuidHash() const {
        return _uuidHash;
    }

    //!
    //! \brief Get control block line humber
    //! \return
    //!
    int lineNum() const {
        return _lineNum;
    }

    //!
    //! \brief Get parent control block pointer for this control block
    //! \return
    //!
    QtTIAbstractControlBlock *parentBlock() const {
        return _parentBlock;
    }

    //!
    //! \brief Append nested control block
    //! \param block Control block pointer
    //!
    void appendSubBlock(QtTIAbstractControlBlock *block) {
        if (!block)
            return;
        block->_parentBlock = this;
        _subBlocks.append(block);

        // append hlp tmp func
        _parser->parserFunc()->appendHelpFunction(new QtTIHelperFunction<>(QString("eval_block_%1").arg(block->uuidHash()), [block]() {
            return block->evalBlock();
        }));
        // append sub block function macros
        appendBlockBody(QString("{{ eval_block_%1() }}").arg(block->uuidHash()), block->lineNum());
    }

    //!
    //! \brief Make control block
    //! \param blockCond Control block condition
    //! \param lineNum Line number
    //! \return
    //!
    virtual QtTIAbstractControlBlock *makeBlock(const QString &blockCond, const int lineNum) = 0;

    //!
    //! \brief Get control block condition
    //! \return
    //!
    virtual QString blockCondition() const = 0;

    //!
    //! \brief Check is control block start
    //! \param blockCond Control block condition
    //! \return
    //!
    virtual bool isBlockCondStart(const QString &blockCond) = 0;

    //!
    //! \brief Check is control block end
    //! \param blockCond Control block condition
    //! \return
    //!
    virtual bool isBlockCondEnd(const QString &blockCond) = 0;

    //!
    //! \brief Execute control block
    //! \return
    //!
    virtual std::tuple<bool/*isOk*/,QString/*res*/,QString/*err*/> evalBlock() = 0;

    //!
    //! \brief Check is intermediate part of control block
    //! \param blockCond Control block condition
    //! \return
    //!
    virtual bool isBlockCondIntermediate(const QString &blockCond) {
        Q_UNUSED(blockCond)
        return false;
    }

    //!
    //! \brief Append intermediate part of control block
    //! \param blockCond Control block condition
    //!
    virtual void appendBlockCondIntermediate(const QString &blockCond) {
        Q_UNUSED(blockCond)
        return;
    }

    //!
    //! \brief Append control block body
    //! \param blockBody Control block body
    //! \param lineNum Control block body line number
    //!
    virtual void appendBlockBody(const QString &blockBody, const int lineNum) {
        Q_UNUSED(blockBody)
        Q_UNUSED(lineNum)
    }

protected:
    //!
    //! \brief Get QtTIParser
    //! \return
    //!
    QtTIParser *parser() const {
        return _parser;
    }

    //!
    //! \brief Get nested control blocks list
    //! \return
    //!
    QList<QtTIAbstractControlBlock*> subBlocks() const {
        return _subBlocks;
    }

    //!
    //! \brief Build control block body
    //! \param data Control block body
    //! \return
    //!
    //! NOTE: This method calculates line numbers from the line number of the beginning of the block.
    //!
    std::tuple<bool/*isOk*/,QString/*res*/,QString/*err*/> buildBlockBody(QString data)
    {
        QTextStream in(&data);
        int lineNum = _lineNum - 1;
        QString tmpData;
        while (!in.atEnd()) {
            if (!tmpData.isEmpty()
                && tmpData[tmpData.size() - 1] != '\n')
                tmpData += "\r\n";

            lineNum++;
            QString line = in.readLine();

            // search params & functions
            bool isOk = false;
            QString error;
            std::tie(isOk, line, error) = _parser->parseLine(line, lineNum);
            if (!isOk)
                return std::make_tuple(false, "", error);

            tmpData += line;
        }
        return std::make_tuple(true, tmpData, "");
    }

    //!
    //! \brief Build control block body
    //! \param data Control block body
    //! \return
    //!
    std::tuple<bool/*isOk*/,QString/*res*/,QString/*err*/> buildBlockBody(const QMap<int/*lineNum*/,QString/*lineData*/> &data)
    {
        QString tmpData;
        QMapIterator<int,QString> it(data);
        while (it.hasNext()) {
            it.next();
            if (!tmpData.isEmpty()
                && tmpData[tmpData.size() - 1] != '\n')
                tmpData += "\r\n";

            QString line = it.value();
            if (it.key() == lineNum())
                line = QtTIAbstractControlBlock::lstrip(line); // trim left

            // search params & functions
            bool isOk = false;
            QString error;
            std::tie(isOk, line, error) = _parser->parseLine(line, it.key());
            if (!isOk)
                return std::make_tuple(false, "", error);

            tmpData += line;
        }
        return std::make_tuple(true, tmpData, "");
    }

    //!
    //! \brief Parse parameter value from string
    //! \param str Parameter value string view
    //! \return
    //!
    std::tuple<bool/*isOk*/,QVariant/*res*/,QString/*err*/> parseParamValue(const QString &str)
    {
        if (str.isEmpty())
            return std::make_tuple(false, QVariant(), "Parse value failed (empty string passed)");

        // ternary
        if (QtTIParserTernaryOperator::isTernaryOperatorExpr(str)) {
            bool isOk = false;
            QString error;
            QVariant result = QtTIParserTernaryOperator::parseTernaryOperator(str, parser()->parserArgs(), parser()->parserFunc(), &isOk, error);
            if (!isOk)
                return std::make_tuple(false, QVariant(), error);

            return std::make_tuple(true, result, "");
        }

        // null-coalescing
        if (QtTIParserNullCoalescingOperator::isNullCoalescingOperatorExpr(str)) {
            bool isOk = false;
            QString error;
            QVariant result = QtTIParserNullCoalescingOperator::parseNullCoalescingOperator(str, parser()->parserArgs(), parser()->parserFunc(), &isOk, error);
            if (!isOk)
                return std::make_tuple(false, QVariant(), error);

            return std::make_tuple(true, result, "");
        }

        // logic
        if (QtTIParserLogic::isLogicExpr(str)) {
            bool isOk = false;
            QString error;
            QVariant result = QtTIParserLogic::parseLogic(str, parser()->parserArgs(), parser()->parserFunc(), &isOk, error);
            if (!isOk)
                return std::make_tuple(false, QVariant(), error);

            return std::make_tuple(true, result, "");
        }

        // math
        if (QtTIParserMath::isMathExpr(str)) {
            bool isOk = false;
            QString error;
            QVariant result = QtTIParserMath::parseMath(str, parser()->parserArgs(), parser()->parserFunc(), &isOk, error);
            if (!isOk)
                return std::make_tuple(false, QVariant(), error);

            return std::make_tuple(true, result, "");
        }

        // function
        QRegExp rxFunc("^(\\s*([\\w]+)\\s*\\(\\s*([A-Za-z0-9_\\ \\+\\-\\*\\,\\.\\'\\\"\\{\\}\\[\\]\\(\\)\\:\\/\\^\\$\\\\\\@\\#\\!\\<\\>\\=\\&\\%\\|\\;\\~]*)\\s*\\)\\s*)");
        if (rxFunc.indexIn(str) != -1) {
            QString funcName = rxFunc.cap(2).trimmed();
            QVariantList funcArgs = parser()->parserArgs()->parseHelpFunctionArgs(rxFunc.cap(3).trimmed());
            return parser()->parserFunc()->evalHelpFunction(funcName, funcArgs);
        }

        // arg
        QVariantList tmp = parser()->parserArgs()->parseHelpFunctionArgs(str);
        if (tmp.isEmpty())
            return std::make_tuple(false, QVariant(), "Parse value failed");
        if (tmp.size() > 1)
            return std::make_tuple(false, QVariant(), "Parse value failed (more than one argument is given)");

        return std::make_tuple(true, tmp[0], "");
    }

    static QString lstrip(const QString& str)
    {
        int n = 0;
        for (; n < str.size(); ++n) {
            if (!str.at(n).isSpace())
                return str.right(n);
        }
        return "";
    }

    static QString rstrip(const QString& str)
    {
        int n = str.size() - 1;
        for (; n >= 0; --n) {
            if (!str.at(n).isSpace())
                return str.left(n + 1);
        }
        return "";
    }

private:
    QString _uuid;                                      //!< control block uuid
    QString _uuidHash;                                  //!< control block uuid SHA1 hash
    int _lineNum {-1};                                  //!< control block line number
    QtTIParser *_parser {nullptr};                      //!< parser point
    QtTIAbstractControlBlock *_parentBlock {nullptr};   //!< parent control block pointer for this control block
    QList<QtTIAbstractControlBlock*> _subBlocks;        //!< nested control blocks list
};

#endif // QTTIABSTRACTCONTROLBLOCK_H
