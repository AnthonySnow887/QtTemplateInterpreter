#ifndef QTTIPARSERBLOCK_H
#define QTTIPARSERBLOCK_H

#include "../QtTIDefines/QtTIRegExpDefines.h"
#include "Abstract/QtTIAbstractParserBlock.h"
#include "ControlBlockFabric/ControlBlocks/QtTIAbstractControlBlock.h"

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
                    const QPair<int, int> &startPos,
                    const QPair<int, int> &endPos)
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

    //!
    //! \brief Check block is empty (no data & positions)
    //! \return
    //!
    bool isEmpty() const final {
        return (_data.isEmpty()
                && _startPos.first == -1
                && _startPos.second == -1);
    }

    //!
    //! \brief Check block is valid (data is not empty, start & end postions are set, block type is valid)
    //! \return
    //!
    bool isValid() const final {
        return (!_data.isEmpty()
                && !isUnfinished()
                && isValidExpr());
    }

    //!
    //! \brief Check has block is started (data is not empty, start postions are set, end position is not set)
    //! \return
    //!
    bool hasBlockStart() const final {
        return (!_data.isEmpty()
                && isUnfinished());
    }

    //!
    //! \brief Check block is unfinished (start postions are set, end position is not set)
    //! \return
    //!
    bool isUnfinished() const final {
        return (_startPos.first != -1
                && _startPos.second != -1
                && (_endPos.first == -1
                    || _endPos.second == -1));
    }

    //!
    //! \brief Check block data is valid regex expression
    //! \return
    //!
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

    //!
    //! \brief Get block type
    //! \return
    //!
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

    //!
    //! \brief Get potential block type (determined by the first characters of the expression)
    //! \return
    //!
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

    //!
    //! \brief Get block data (including control characters)
    //! \return
    //!
    QString data() const final { return _data; }
    const QString &data_ref() const final { return _data; }

    //!
    //! \brief Get block body (content within control characters (control characters are not included))
    //! \return
    //!
    QString body() const final {
        QRegExp rx(RX_BLOCK);
        if (rx.indexIn(_data) != 0)
            return QString();
        if (rx.captureCount() < 3)
            return QString();
        return rx.cap(2);
    }

    //!
    //! \brief Get block start position
    //! \return
    //!
    //! QPair<int, int>:
    //! - first     - line number
    //! - second    - position number in line
    //!
    QPair<int, int> startPos() const final { return _startPos; }
    const QPair<int, int> &startPos_ref() const final { return _startPos; }

    //!
    //! \brief Get block end position
    //! \return
    //!
    //! QPair<int, int>:
    //! - first     - line number
    //! - second    - position number in line
    //!
    QPair<int, int> endPos() const final { return _endPos; }
    const QPair<int, int> &endPos_ref() const final { return _endPos; }

    //!
    //! \brief Check has data before block
    //! \return
    //!
    bool hasDataBeforeBlock() const final { return _hasDataBeforeBlock; }

    //!
    //! \brief Get control block object pointer
    //! \return
    //!
    std::shared_ptr<QtTIAbstractControlBlock> controlBlock() const { return _controlBlock; }

    //!
    //! \brief Clear block
    //!
    void clear() final {
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
    //!
    //! \brief Set has data before block
    //! \param hasDataBeforeBlock
    //!
    void setHasDataBeforeBlock(const bool hasDataBeforeBlock) {
        _hasDataBeforeBlock = hasDataBeforeBlock;
    }

    //!
    //! \brief Set control block object pointer
    //! \param controlBlock
    //!
    void setControlBlock(QtTIAbstractControlBlock *controlBlock) {
        _controlBlock.reset(controlBlock);
    }

    //!
    //! \brief Set end position
    //! \param endPos
    //!
    void setEndPos(const QPair<int, int> &endPos) {
        _endPos = endPos;
    }

    QString _data;                                                      //!< block data

    QPair<int /*line*/, int /*pos*/> _startPos {-1, -1};                //!< block start position
    QPair<int /*line*/, int /*pos*/> _endPos {-1, -1};                  //!< block end position

    bool _hasDataBeforeBlock {true};                                    //!< has data before block

    std::shared_ptr<QtTIAbstractControlBlock> _controlBlock {nullptr};  //!< control block object pointer
};

#endif // QTTIPARSERBLOCK_H
