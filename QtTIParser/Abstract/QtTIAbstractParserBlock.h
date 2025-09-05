#ifndef QTTIABSTRACTPARSERBLOCK_H
#define QTTIABSTRACTPARSERBLOCK_H

#include <QString>
#include <QPair>

class QtTIAbstractParserBlock
{   
public:
    enum class Type {
        Invalid = -1,
        Base = 0,
        Control,
        Comment
    };

    QtTIAbstractParserBlock() = default;
    virtual ~QtTIAbstractParserBlock() = default;

    virtual bool isValid() const = 0;

    virtual bool hasBlockStart() const = 0;

    virtual bool isUnfinished() const = 0;

    virtual bool isValidExpr() const = 0;
    virtual Type type() const = 0;

    virtual Type potentialType() const = 0;

    virtual QString data() const = 0;
    virtual const QString &data_ref() const = 0;

    virtual QString body() const = 0;

    virtual QPair<int, int> startPos() const = 0;
    virtual const QPair<int, int> &startPos_ref() const = 0;

    virtual QPair<int, int> endPos() const = 0;
    virtual const QPair<int, int> &endPos_ref() const = 0;

    virtual bool hasDataBeforeBlock() const = 0;

    virtual void clear() = 0;
};

#endif // QTTIABSTRACTPARSERBLOCK_H
