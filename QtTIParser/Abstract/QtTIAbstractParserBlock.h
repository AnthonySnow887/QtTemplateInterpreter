#ifndef QTTIABSTRACTPARSERBLOCK_H
#define QTTIABSTRACTPARSERBLOCK_H

#include <QString>
#include <QPair>

class QtTIAbstractParserBlock
{   
public:
    //!
    //! \brief Abstract block types
    //!
    enum class Type {
        Invalid = -1,   //!< invalid block
        Base = 0,       //!< base block `{{ ... }}`
        Control,        //!< control block `{% ... %}`
        Comment         //!< comment block `{# ... #}`
    };

    QtTIAbstractParserBlock() = default;
    virtual ~QtTIAbstractParserBlock() = default;

    //!
    //! \brief Check block is empty (no data & positions)
    //! \return
    //!
    virtual bool isEmpty() const = 0;

    //!
    //! \brief Check block is valid (data is not empty, start & end postions are set, block type is valid)
    //! \return
    //!
    virtual bool isValid() const = 0;

    //!
    //! \brief Check has block is started (data is not empty, start postions are set, end position is not set)
    //! \return
    //!
    virtual bool hasBlockStart() const = 0;

    //!
    //! \brief Check block is unfinished (start postions are set, end position is not set)
    //! \return
    //!
    virtual bool isUnfinished() const = 0;

    //!
    //! \brief Check block data is valid regex expression
    //! \return
    //!
    virtual bool isValidExpr() const = 0;

    //!
    //! \brief Get block type
    //! \return
    //!
    virtual Type type() const = 0;

    //!
    //! \brief Get potential block type (determined by the first characters of the expression)
    //! \return
    //!
    virtual Type potentialType() const = 0;

    //!
    //! \brief Get block data (including control characters)
    //! \return
    //!
    virtual QString data() const = 0;
    virtual const QString &data_ref() const = 0;

    //!
    //! \brief Get block body (content within control characters (control characters are not included))
    //! \return
    //!
    virtual QString body() const = 0;

    //!
    //! \brief Get block start position
    //! \return
    //!
    //! QPair<int, int>:
    //! - first     - line number
    //! - second    - position number in line
    //!
    virtual QPair<int, int> startPos() const = 0;
    virtual const QPair<int, int> &startPos_ref() const = 0;

    //!
    //! \brief Get block end position
    //! \return
    //!
    //! QPair<int, int>:
    //! - first     - line number
    //! - second    - position number in line
    //!
    virtual QPair<int, int> endPos() const = 0;
    virtual const QPair<int, int> &endPos_ref() const = 0;

    //!
    //! \brief Check has data before block
    //! \return
    //!
    virtual bool hasDataBeforeBlock() const = 0;

    //!
    //! \brief Clear block
    //!
    virtual void clear() = 0;
};

#endif // QTTIABSTRACTPARSERBLOCK_H
