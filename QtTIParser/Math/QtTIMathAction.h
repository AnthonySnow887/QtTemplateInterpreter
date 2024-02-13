#ifndef QTTIMATHACTION_H
#define QTTIMATHACTION_H

#include <QString>
#include <QVariant>

#define MAX_OP_PRIORITY 2

//!
//! \brief The QtTIMathAction class
//!
class QtTIMathAction
{
public:
    QtTIMathAction() = default;

    //!
    //! \brief QtTIMathAction
    //! \param left Left value
    //! \param right Right value
    //! \param op Operation string view
    //!
    //! === Math operators (only for numbers):
    //!     The following operators are supported:
    //!      + : Adds two numbers together (the operands are casted to numbers):
    //!      - : Subtracts the second number from the first one:
    //!      / : Divides two numbers. The returned value will be a floating point number:
    //!      % : Calculates the remainder of an integer division:
    //!     // : Divides two numbers and returns the floored integer result:
    //!      * : Multiplies the left operand with the right one:
    //!     ** : Raises the left operand to the power of the right operand:
    //!
    explicit QtTIMathAction(const QVariant &left, const QVariant &right, const QString &op)
        : _left(left)
        , _right(right)
        , _op(op)
    {}
    ~QtTIMathAction() = default;

    QtTIMathAction(const QtTIMathAction& obj) {
        _left = obj.left();
        _op = obj.op();
        _right = obj.right();
    }

    QtTIMathAction& operator=(const QtTIMathAction& obj) {
        if (this == &obj)
            return *this;
        _left = obj.left();
        _op = obj.op();
        _right = obj.right();
        return *this;
    }

    //!
    //! \brief Check is valid math action object
    //! \return
    //!
    bool isValid() const { return !_op.isEmpty(); }

    //!
    //! \brief Get left value
    //! \return
    //!
    QVariant left() const { return _left; }

    //!
    //! \brief Get right value
    //! \return
    //!
    QVariant right() const { return _right; }

    //!
    //! \brief Get operation string view
    //! \return
    //!
    QString op() const { return _op; }

    //!
    //! \brief Set left value
    //! \param value
    //!
    void setLeft(const QVariant &value) { _left = value; }

    //!
    //! \brief Set right value
    //! \param value
    //!
    void setRight(const QVariant &value) { _right = value; }

    //!
    //! \brief Get operation priority number
    //! \return
    //!
    //! 0 is the highest priority
    //!
    //! === Operation priorities
    //!     0 : **
    //!     1 : *, /, %, //
    //!     2 : +, -
    //!
    int opPriority() {
        if (_op == "**")
            return 0;
        else if (_op == "*"
                 || _op == "/"
                 || _op == "%"
                 || _op == "//")
            return 1;

        return MAX_OP_PRIORITY; // +, -
    }

private:
    QVariant _left;     //!< left value
    QVariant _right;    //!< right value
    QString _op;        //!< operation string view
};

#endif // QTTIMATHACTION_H
