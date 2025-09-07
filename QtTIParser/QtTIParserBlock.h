#ifndef QTTIPARSERBLOCK_H
#define QTTIPARSERBLOCK_H

#include "Abstract/QtTIAbstractParserBlock.h"
#include "../QtTIDefines/QtTIRegExpDefines.h"
#include "../QtTIControlBlock/ControlBlocks/QtTIAbstractControlBlock.h"

#include <QString>
#include <QPair>
#include <QRegExp>
#include <tuple>
#include <memory>

class QtTIParserBlock : public QtTIAbstractParserBlock
{
    friend class QtTIParser;

public:
    QtTIParserBlock()
        : QtTIAbstractParserBlock()
    {}
    QtTIParserBlock(const QString &data)
        : QtTIAbstractParserBlock()
        , _data(data)
    {}
    QtTIParserBlock(const QString &data,
                            QPair<int, int> startPos,
                            QPair<int, int> endPos)
        : QtTIAbstractParserBlock()
        , _data(data)
        , _startPos(startPos)
        , _endPos(endPos)
    {}
    virtual ~QtTIParserBlock()
    {}

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

    virtual bool isEmpty() const final {
        return (_data.isEmpty()
        && _startPos.first == -1
        && _startPos.second == -1);
    }

    bool isValid() const final {
        return (!_data.isEmpty()
        && _startPos.first != -1
        && _startPos.second != -1
        && !isUnfinished()
        && isValidExpr());
    }

    bool hasBlockStart() const final {
        return (!_data.isEmpty()
        && _startPos.first != -1
        && _startPos.second != -1
        && isUnfinished());
    }

    bool isUnfinished() const final {
        return (!_data.isEmpty()
        && _startPos.first != -1
        && _startPos.second != -1
        && (_endPos.first == -1
        || _endPos.second == -1));
    }

    bool isValidExpr() const final {
        QRegExp rx(RX_BLOCK);
        if (rx.indexIn(_data) != 0)
            return false;
        if (rx.captureCount() < 3)
            return false;
        if (rx.cap(1) == "{" && rx.cap(3) == "}")
            return true;
        return (rx.cap(1) == rx.cap(3));
    }

    Type type() const final {
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

    Type potentialType() const final {
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

    QString data() const final { return _data; }
    const QString &data_ref() const final { return _data; }

    QString body() const final {
        QRegExp rx(RX_BLOCK);
        if (rx.indexIn(_data) != 0)
            return QString();
        if (rx.captureCount() < 3)
            return QString();
        return rx.cap(2);
    }

    QPair<int, int> startPos() const final { return _startPos; }
    const QPair<int, int> &startPos_ref() const final { return _startPos; }

    QPair<int, int> endPos() const final { return _endPos; }
    const QPair<int, int> &endPos_ref() const final { return _endPos; }

    bool hasDataBeforeBlock() const final { return _hasDataBeforeBlock; }

    std::shared_ptr<QtTIAbstractControlBlock> controlBlock() const { return _controlBlock; }

    virtual void clear() final {
        _data.clear();
        _startPos.first = -1;
        _startPos.second = -1;
        _endPos.first = -1;
        _endPos.second = -1;
        _hasDataBeforeBlock = true;

        if (_controlBlock) {
            _controlBlock.reset();
            _controlBlock = nullptr;
        }
    }

protected:
    void setHasDataBeforeBlock(const bool hasDataBeforeBlock) {
        _hasDataBeforeBlock = hasDataBeforeBlock;
    }

    void setControlBlock(QtTIAbstractControlBlock *controlBlock) {
        _controlBlock.reset(controlBlock);
    }

    void setEndPos(const QPair<int, int> &endPos) {
        _endPos = endPos;
    }

    QString _data;

    QPair<int /*line*/, int /*pos*/> _startPos {-1, -1};
    QPair<int /*line*/, int /*pos*/> _endPos {-1, -1};

    bool _hasDataBeforeBlock {true};

    std::shared_ptr<QtTIAbstractControlBlock> _controlBlock {nullptr};
};

#endif // QTTIPARSERBLOCK_H
