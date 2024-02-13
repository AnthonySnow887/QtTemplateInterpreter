#ifndef QTTICONTROLBLOCKSET_H
#define QTTICONTROLBLOCKSET_H

#include "QtTIAbstractControlBlock.h"

//!
//! \brief The QtTBControlBlockSet class
//!
//! === Control block (ver.1):
//!     {% set KEY = VALUE %}
//!
//! === Control block (ver.2):
//!     {% set KEY = FUNCTION() %}
//!
//! === Control block (ver.3):
//!     {% set KEY = (MATH OPERATIONS) %}
//!
//! === Control block (ver.4):
//!     {% set KEY = (LOGIC OPERATIONS) %}
//!
//! === Example 1
//!     ![This is Lu and Bryu!]( {% set my_key = 'configure.svg' %} {{ image_path (my_key) }} "Lu and Bryu")
//!
//! === Example 2
//!     {% set tt2 = my_test_array () %}
//!
//! === Example 3
//!     {% set tt2 = ((1 + 5) * 2) %}
//!
//! === Example 4
//!     {% set tt2 = ((a + b) > 5 || a < 10) %}
//!
class QtTIControlBlockSet : public QtTIAbstractControlBlock
{
public:
    QtTIControlBlockSet(QtTIParser *parser);
    QtTIControlBlockSet(QtTIParser *parser, const QString &blockCond, const int lineNum);
    virtual ~QtTIControlBlockSet();

    QtTIAbstractControlBlock *makeBlock(const QString &blockCond, const int lineNum) final;
    QString blockCondition() const final;
    bool isBlockCondStart(const QString &blockCond) final;
    bool isBlockCondEnd(const QString &blockCond) final;

    std::tuple<bool/*isOk*/,QString/*res*/,QString/*err*/> evalBlock() final;

private:
    QString _blockCond;
};

#endif // QTTICONTROLBLOCKSET_H
