#ifndef QTTICONTROLBLOCKUNSET_H
#define QTTICONTROLBLOCKUNSET_H

#include "QtTIAbstractControlBlock.h"

//!
//! \brief The QtTBControlBlockUnset class
//!
//! === Control block:
//!     {% unset KEY %}
//!
//! === Example
//!     ![This is Lu and Bryu!]( {% set my_key = 'configure.svg' %} {{ image_path (my_key) }} "Lu and Bryu") {% unset my_key %}
//!
class QtTIControlBlockUnset : public QtTIAbstractControlBlock
{
public:
    QtTIControlBlockUnset(QtTIParser *parser);
    QtTIControlBlockUnset(QtTIParser *parser, const QString &blockCond, const int lineNum);
    virtual ~QtTIControlBlockUnset();

    QtTIAbstractControlBlock *makeBlock(const QString &blockCond, const int lineNum) final;
    QString blockCondition() const final;
    bool isBlockCondStart(const QString &blockCond) final;
    bool isBlockCondEnd(const QString &blockCond) final;
    std::tuple<bool/*isOk*/,QString/*res*/,QString/*err*/> evalBlock() final;

private:
    QString _blockCond;
};

#endif // QTTICONTROLBLOCKUNSET_H
