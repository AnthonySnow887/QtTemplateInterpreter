#ifndef QTTICONTROLBLOCKIF_H
#define QTTICONTROLBLOCKIF_H

#include "QtTIAbstractControlBlock.h"

//!
//! \brief The QtTBControlBlockIf class
//!
//! === Control block (ver. 1):
//!     {% if LEFT CONDITION RIGHT %}
//!         ...
//!     {% endif %}
//!
//! === Control block (ver. 2):
//!     {% if OBJECT %}
//!         ...
//!     {% endif %}
//!
//! === Control block (ver. 3):
//!     {% if LEFT CONDITION RIGHT %}
//!         ...
//!     {% elseif LEFT CONDITION RIGHT %}
//!         ...
//!     {% elseif OBJECT %}
//!         ...
//!     {% else %}
//!         ...
//!     {% endif %}
//!
//! === Logic:
//!     You can combine multiple expressions with the following operators:
//!      - and: Returns true if the left and the right operands are both true.
//!      - or: Returns true if the left or the right operand is true.
//!      - not: Negates a statement.
//!      - &&: equivalent of method 'and'
//!      - ||: equivalent of method 'or'
//!
//! === Brackets support:
//!     Supported grouping expressions with brackets '(...expr...)':
//!     {% set a = 10.5 %}
//!     {% set b = 2 %}
//!     {% set aa = { a:1, b:2, c:3 } %}
//!     {% if (a + aa.b) > 5 && (aa.b < 2 || aa.c >= 3) %}
//!     --- THIS is IF BLOCK BODY ---
//!     {% endif %}
//!
//! === Example 1
//!     {% set test_my_key = 1 %}
//!     {% if test_my_key >= 1 %}
//!     --- THIS is IF BLOCK BODY ---
//!     {% endif %}
//!
//! === Example 2
//!     {% set test_my_key = 1 %}
//!     {% set test_my_key_2 = 0.11 %}
//!     {% if test_my_key > test_my_key_2 %}
//!      --- THIS is IF BLOCK BODY 1 ---
//!     {% elseif test_my_key < test_my_key_2 %}
//!      --- THIS is IF BLOCK BODY 2 ---
//!     {% elseif test_my_key == test_my_key_2 %}
//!      --- THIS is IF BLOCK BODY 3 ---
//!     {% else %}
//!      --- THIS is IF BLOCK BODY 4 ---
//!     {% endif %}
//!
//! === Example 3
//!     {% set tt2 = my_test_array () %}
//!     {% set multi_array = { a: {aa: 1, bb:2, cc:3}, b: { aaa: 11, bbb: 22, ccc: 33} } %}
//!     {% if not false || multi_array && size(tt2) == 3 %}
//!      --- is OK ---
//!     {% else %}
//!      --- is NOT OK ---
//!     {% endif %}
//!
class QtTIControlBlockIf : public QtTIAbstractControlBlock
{
public:
    enum class BodyPosition {
        If = 0,
        ElseIf,
        Else
    };

    QtTIControlBlockIf(QtTIParser *parser);
    QtTIControlBlockIf(QtTIParser *parser, const QString &blockCond, const int lineNum);
    virtual ~QtTIControlBlockIf();

    QtTIAbstractControlBlock *makeBlock(const QString &blockCond, const int lineNum) final;
    QString blockCondition() const final;
    bool isBlockCondStart(const QString &blockCond) final;
    bool isBlockCondIntermediate(const QString &blockCond) final;
    void appendBlockCondIntermediate(const QString &blockCond) final;
    bool isBlockCondEnd(const QString &blockCond) final;
    std::tuple<bool/*isOk*/,QString/*res*/,QString/*err*/> evalBlock() final;

    void appendBlockBody(const QString &blockBody, const int lineNum) final;

private:
    QString _ifCond;
    QString _elseCond;
    QStringList _elseIfConds;

    BodyPosition _bodyPos {BodyPosition::If};

    QMap<int,QString> _ifBody;
    QMap<int,QString> _elseBody;
    QList<QMap<int,QString>> _elseIfBodys;

    static std::tuple<bool/*isOk*/,QVariant/*res*/,QString/*err*/> evalCond(const QString &str,
                                                                            QtTIParserArgs *parserArgs,
                                                                            QtTIParserFunc *parserFunc);

    static std::tuple<bool/*isOk*/,QVariant/*res*/,QString/*err*/> parseParamValue(const QString &str,
                                                                                   QtTIParserArgs *parserArgs,
                                                                                   QtTIParserFunc *parserFunc);
};

#endif // QTTICONTROLBLOCKIF_H
