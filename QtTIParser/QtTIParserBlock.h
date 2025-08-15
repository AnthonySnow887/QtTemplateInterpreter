#ifndef QTTIPARSERBLOCK_H
#define QTTIPARSERBLOCK_H

#include "../QtTIDefines/QtTIRegExpDefines.h"
//#include "../QtTIControlBlock/ControlBlocks/QtTIAbstractControlBlock.h"

#include <QString>
#include <QPair>
#include <QRegExp>
#include <tuple>

class QtTIAbstractControlBlock;

class QtTIParserBlock
{
    friend class QtTIParser;

public:
    enum class Type {
        Invalid = -1,
        Base = 0,
        Control,
        Comment
    };

    QtTIParserBlock() = default;
    QtTIParserBlock(const QString &data)
        : _data(data)
    {}
    QtTIParserBlock(const QString &data,
                    QPair<int, int> startPos,
                    QPair<int, int> endPos)
        : _data(data)
        , _startPos(startPos)
        , _endPos(endPos)
    {}
    ~QtTIParserBlock()
    {
        clear();
    }

    QtTIParserBlock(const QtTIParserBlock& obj) {
        _data = obj.data();
        _startPos = obj.startPos();
        _endPos = obj.endPos();
        _hasDataBeforeBlock = obj.hasDataBeforeBlock();
        _controlBlock = obj.controlBlock();
    }

    QtTIParserBlock& operator=(const QtTIParserBlock& obj) {
        if (this == &obj)
            return *this;
        _data = obj.data();
        _startPos = obj.startPos();
        _endPos = obj.endPos();
        _hasDataBeforeBlock = obj.hasDataBeforeBlock();
        _controlBlock = obj.controlBlock();
        return *this;
    }

    bool isValid() const {
        return (!_data.isEmpty()
                && _startPos.first != -1
                && _startPos.second != -1
                && !isUnfinished()
                && isValidExpr());
    }

    bool hasBlockStart() const {
        return (!_data.isEmpty()
                && _startPos.first != -1
                && _startPos.second != -1
                && isUnfinished());
    }

    bool isUnfinished() const {
        return (!_data.isEmpty()
                && _startPos.first != -1
                && _startPos.second != -1
                && (_endPos.first == -1
                    || _endPos.second == -1));
    }

    bool isValidExpr() const {
        QRegExp rx(RX_BLOCK);
        if (rx.indexIn(_data) != 0)
            return false;
        if (rx.captureCount() < 3)
            return false;
        if (rx.cap(1) == "{" && rx.cap(3) == "}")
            return true;
        return (rx.cap(1) == rx.cap(3));
    }

    Type type() const {
        QRegExp rx(RX_BLOCK);
        if (rx.indexIn(_data) != 0)
            return Type::Invalid;
        if (rx.captureCount() < 3)
            return Type::Invalid;
        if (rx.cap(1) == "{" && rx.cap(3) == "}")
            return Type::Base;
        else if (rx.cap(1) == "%"
                 && rx.cap(1) == rx.cap(3))
            return Type::Control;
        else if (rx.cap(1) == "#"
                 && rx.cap(1) == rx.cap(3))
            return Type::Comment;
        return Type::Invalid;
    }

    Type potentialType() const {
        QRegExp rx(RX_BLOCK_START);
        if (rx.indexIn(_data) != 0)
            return Type::Invalid;
        if (rx.captureCount() < 2)
            return Type::Invalid;
        if (rx.cap(1) == "{")
            return Type::Base;
        else if (rx.cap(1) == "%")
            return Type::Control;
        else if (rx.cap(1) == "#")
            return Type::Comment;
        return Type::Invalid;
    }

    QString data() const { return _data; }
    const QString &data_ref() const { return _data; }

    QString body() const {
        QRegExp rx(RX_BLOCK);
        if (rx.indexIn(_data) != 0)
            return QString();
        if (rx.captureCount() < 3)
            return QString();
        return rx.cap(2);
    }

    QPair<int, int> startPos() const { return _startPos; }
    const QPair<int, int> &startPos_ref() const { return _startPos; }

    QPair<int, int> endPos() const { return _endPos; }
    const QPair<int, int> &endPos_ref() const { return _endPos; }   

    bool hasDataBeforeBlock() const { return _hasDataBeforeBlock; }

    QtTIAbstractControlBlock *controlBlock() const { return _controlBlock; }

//    //!
//    //! \brief Execute control block
//    //! \return
//    //!
//    std::tuple<bool/*isOk*/,QString/*res*/,QString/*err*/> evalBlock() {
//        bool isOk = false;
//        QString res, err;
//        std::tie(isOk, res, err) = _controlBlock->evalBlock();
//        blockCond = curBlock->blockCondition();
//        delete curBlock;
//        curBlock = nullptr;
//        if (!isOk) {
//            QString errFull = QString("Eval control block '%1' in line %2 failed! Error: %3")
//                              .arg(blockCond)
//                              .arg(curBlockLine)
//                              .arg(err);
//            return std::make_tuple(false, "", nullptr, "", errFull);
//        }
//    }

    void clear() {
        _data.clear();
        _startPos.first = -1;
        _startPos.second = -1;
        _endPos.first = -1;
        _endPos.second = -1;
        _hasDataBeforeBlock = true;
        // TODO
//        if (_controlBlock)
//            delete _controlBlock;
    }

protected:
    void setHasDataBeforeBlock(const bool hasDataBeforeBlock) {
        _hasDataBeforeBlock = hasDataBeforeBlock;
    }

    void setControlBlock(QtTIAbstractControlBlock *controlBlock) {
        // TODO
//        if (_controlBlock)
//            delete _controlBlock;
        _controlBlock = controlBlock;
    }

    void setEndPos(const QPair<int, int> &endPos) {
        _endPos = endPos;
    }

//private:
    QString _data;

    QPair<int /*line*/, int /*pos*/> _startPos {-1, -1};
    QPair<int /*line*/, int /*pos*/> _endPos {-1, -1};

    bool _hasDataBeforeBlock {true};

    QtTIAbstractControlBlock *_controlBlock {nullptr};
};

#endif // QTTIPARSERBLOCK_H
