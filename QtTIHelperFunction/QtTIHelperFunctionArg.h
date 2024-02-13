#ifndef QTTIHELPERFUNCTIONARG_H
#define QTTIHELPERFUNCTIONARG_H

#include <type_traits>
#include <QString>
#include <QBitArray>
#include <QVariant>
#include <QRegExp>
#include <QMetaType>
#include <QStringList>

//!
//! \brief The QtTIHelperFunctionArg class
//!
template<typename T>
class QtTIHelperFunctionArg
{
public:
    QtTIHelperFunctionArg()
        : _typeId(qMetaTypeId<T>())
        , _typeName(QMetaType(qMetaTypeId<T>()).name())
    {}

    QtTIHelperFunctionArg(const QVariant &data)
        : _data(toT(data))
        , _typeId(qMetaTypeId<T>())
        , _typeName(QMetaType(qMetaTypeId<T>()).name())
    {}

    ~QtTIHelperFunctionArg()
    {}

    //!
    //! \brief Get argument data
    //! \return
    //!
    T data() const { return _data; }

    //!
    //! \brief Get argument type id
    //! \return
    //!
    int typeId() const { return _typeId; }

    //!
    //! \brief Get argument string type name
    //! \return
    //!
    QString typeName() const { return _typeName; }

    //!
    //! \brief Check can convert data value to type T
    //! \param data
    //! \return
    //!
    bool canConvert(const QVariant &data) const {
        if (std::is_same<T, QVariant>::value)
            return true;
        return data.canConvert<T>();
    }

private:
    T _data;                            //!< argument data
    int _typeId;                        //!< argument type id
    QString _typeName;                  //!< argument string type name

    //!
    //! \brief Convert QVariant to data type T
    //! \param data Data in QVariant
    //! \return
    //!
    T toT(const QVariant &data) const {
        return data.value<T>();
    }
};

#endif // QTTIHELPERFUNCTIONARG_H
