#ifndef QTTICONTROLBLOCKFOR_H
#define QTTICONTROLBLOCKFOR_H

#include "QtTIAbstractControlBlock.h"

//!
//! \brief The QtTBControlBlockFor class
//!
//! === Control block (ver.1):
//!     {% for VALUE in LIST %}
//!         ...
//!     {% endfor %}
//!
//! === Control block (ver.2):
//!     {% for KEY, VALUE in MAP %}
//!         ...
//!     {% endfor %}
//!
//! === Control block (ver.3):
//!     {% for VALUE in MAP %}
//!         ...
//!     {% endfor %}
//!
//! === Example 1
//!     {% set set my_arr = [ 1, 2, 3, 4 ] %}
//!     {% for a in my_arr %}
//!      --- A: {{ a }} ---
//!     {% endfor %}
//!
//! === Example 2
//!     {% set set my_map = { a: 1, b: 2, c: 3, d: 4 } %}
//!     {% for key, val in my_map %}
//!      --- {{ key }}: {{ val }} ---
//!     {% endfor %}
//!
//! === Example 3
//!     {% set set my_map = { a: 1, b: 2, c: 3, d: 4 } %}
//!     {% for val in my_map %}
//!      --- VALUE: {{ val }} ---
//!     {% endfor %}
//!
//! === Example 4
//!     {# function 'my_test_map ()' return map '{ a: 1, b: 11, c: 111 }' #}
//!     {% for key, val in my_test_map () %}
//!      --- {{ key }}: {{ val }} ---
//!     {% endfor %}
//!
class QtTIControlBlockFor : public QtTIAbstractControlBlock
{
public:
    QtTIControlBlockFor(QtTIParser *parser);
    QtTIControlBlockFor(QtTIParser *parser, const QString &blockCond, const int lineNum);
    virtual ~QtTIControlBlockFor();

    QtTIAbstractControlBlock *makeBlock(const QString &blockCond, const int lineNum) final;
    QString blockCondition() const final;
    bool isBlockCondStart(const QString &blockCond) final;
    bool isBlockCondEnd(const QString &blockCond) final;
    void appendBlockBody(const QString &blockBody, const int lineNum) final;

    std::tuple<bool/*isOk*/,QString/*res*/,QString/*err*/> evalBlock() final;

private:
    QString _blockCond;
    QMap<int,QString> _blockBody;

    std::tuple<bool/*isOk*/,QString/*res*/,QString/*err*/> evalFor(const QStringList &args, const QVariant &container);
    std::tuple<bool/*isOk*/,QString/*res*/,QString/*err*/> evalList(const QString &arg, const QVariant &container);
    std::tuple<bool/*isOk*/,QString/*res*/,QString/*err*/> evalMap(const QStringList &args, const QVariant &container);
};

#endif // QTTICONTROLBLOCKFOR_H
