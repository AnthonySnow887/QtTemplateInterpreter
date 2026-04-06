#ifndef QTTIABSTRACTCONTROLBLOCK_H
#define QTTIABSTRACTCONTROLBLOCK_H

#include <QString>
#include <QList>
#include <QTextStream>
#include <QMap>
#include "../../Abstract/QtTIAbstractParser.h"
#include "../../TernaryOperator/QtTIParserTernaryOperator.h"
#include "../../NullCoalescingOperator/QtTIParserNullCoalescingOperator.h"
#include "../../Logic/QtTIParserLogic.h"
#include "../../Math/QtTIParserMath.h"
#include "../../../QtTIDefines/QtTIRegExpDefines.h"
#include "../../../QtTIHelperFunction/QtTIHelperFunction.h"

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
    QtTIAbstractControlBlock(QtTIAbstractParser *parser, const int lineNum, const int linePos)
        : _lineNum(lineNum)
        , _linePos(linePos)
        , _parser(parser)
    {}

    virtual ~QtTIAbstractControlBlock() {
        _parser = nullptr;
    }

    //!
    //! \brief Get control block line humber
    //! \return
    //!
    int lineNum() const {
        return _lineNum;
    }

    //!
    //! \brief Get control block position number in line
    //! \return
    //!
    int linePos() const {
        return _linePos;
    }

    //!
    //! \brief Make control block
    //! \param blockCond Control block condition
    //! \param lineNum Line number
    //! \param linePos Position number in line
    //! \return
    //!
    virtual QtTIAbstractControlBlock *makeBlock(const QString &blockCond, const int lineNum, const int linePos) = 0;

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

    //!
    //! \brief Set control block body
    //! \param blockBody Control block body
    //! \param lineNum Control block body line number
    //!
    virtual void setBlockBody(const QString &blockBody, const int lineNum) {
        Q_UNUSED(blockBody)
        Q_UNUSED(lineNum)
    }

    //!
    //! \brief Get control block body
    //! \param lineNum Control block body line number
    //! \return
    //!
    virtual QString blockBody(const int lineNum) const {
        Q_UNUSED(lineNum)
        return QString();
    }

protected:
    //!
    //! \brief Get QtTIParser
    //! \return
    //!
    QtTIAbstractParser *parser() const {
        return _parser;
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
        QtTIAbstractParserBlock *block = nullptr;
        int lineNum = _lineNum - 1;
        QString tmpData;
        while (!in.atEnd()) {
            if (!tmpData.isEmpty()
                && tmpData[tmpData.size() - 1] != '\n')
                tmpData += "\r\n";

            lineNum++;
            QString line = in.readLine();

            // parse
            bool isOk = false;
            QString error;
            std::tie(isOk, line, error) = _parser->parseLine(line, lineNum, block);
            if (!isOk) {
                if (block)
                    delete block;
                return std::make_tuple(false, "", error);
            }
            tmpData += line;
        }
        // check block
        if (block && block->isUnfinished()) {
            QString error = QString("Unfinished block in line %1!").arg(block->startPos().first);
            delete block;
            return std::make_tuple(false, "", error);
        }
        if (block)
            delete block;
        return std::make_tuple(true, tmpData, "");
    }

    //!
    //! \brief Build control block body
    //! \param data Control block body
    //! \return
    //!
    std::tuple<bool/*isOk*/,QString/*res*/,QString/*err*/> buildBlockBody(const QMap<int/*lineNum*/,QString/*lineData*/> &data)
    {
        QtTIAbstractParserBlock *block = nullptr;
        QString tmpData;
        QMapIterator<int,QString> it(data);
        while (it.hasNext()) {
            it.next();
            if (!tmpData.isEmpty()
                && tmpData[tmpData.size() - 1] != '\n')
                tmpData += "\r\n";

            QString line = it.value();
            if (it.key() == lineNum())
                line = QtTIAbstractParser::lstrip(line); // trim left

            // parse
            bool isOk = false;
            QString error;
            std::tie(isOk, line, error) = _parser->parseLine(line, it.key(), block);
            if (!isOk) {
                if (block)
                    delete block;
                return std::make_tuple(false, "", error);
            }
            tmpData += line;
        }
        // check block
        if (block && block->isUnfinished()) {
            QString error = QString("Unfinished block in line %1!").arg(block->startPos().first);
            delete block;
            return std::make_tuple(false, "", error);
        }
        if (block)
            delete block;
        return std::make_tuple(true, tmpData, "");
    }

    //!
    //! \brief Parse parameter value from string
    //! \param str Parameter value string view
    //! \return
    //!
    std::tuple<bool/*isOk*/,QVariant/*res*/,QString/*err*/> parseParamValue(const QString &str,
                                                                            const int lineNum,
                                                                            const int linePos)
    {
        if (str.isEmpty())
            return std::make_tuple(false, QVariant(), "Parse value failed (empty string passed)");

        return _parser->parseAndExecBlockData(str, {lineNum, linePos});
    }

    //!
    //! \brief Search for the control block condition body
    //! \param data Input string data
    //! \param pos Position to start searching
    //! \return
    //!
    std::tuple<QString/*all*/,QString/*only cond*/,int/*start pos*/> parseBlockCondition(const QString &data,
                                                                                         const int &pos = 0) const
    {
        if (pos >= data.size())
            return std::make_tuple("", "", -1);

        bool isString = false;
        bool isBlock = false;
        QString tmpCond;
        int tmpCondStart = -1;
        for (int i = pos; i < data.size(); i++) {
            const int chPos = i;
            const QChar ch = data[chPos];

            QChar chPrev;
            if (i > 0)
                chPrev = data[i - 1];
            QChar chNext;
            if (i < data.size() - 1)
                chNext = data[i + 1];

            // check is string
            if ((ch == '"' || ch == '\'')
                && chPrev != '\\'
                && isBlock)
                isString = !isString;

            // check start block
            if (!isString
                && !isBlock
                && ch == '{'
                && chNext == '%') {
                tmpCond += ch;
                tmpCondStart = chPos;
                isBlock = true;
                continue;
            }
            // check end block
            if (!isString
                && isBlock
                && ch == '}'
                && chPrev == '%') {
                tmpCond += ch;

                // return found result
                QStringRef onlyCond = tmpCond.midRef(2);
                onlyCond = onlyCond.mid(0, onlyCond.size() - 2);
                return std::make_tuple(tmpCond, onlyCond.toString(), tmpCondStart);
            }

            // else is block
            if (isBlock)
                tmpCond += ch;
        }
        return std::make_tuple("", "", -1);
    }

private:
    int _lineNum {-1};                                  //!< control block line number
    int _linePos {-1};                                  //!< control block position number in line
    QtTIAbstractParser *_parser {nullptr};              //!< parser pointer
};

#endif // QTTIABSTRACTCONTROLBLOCK_H
