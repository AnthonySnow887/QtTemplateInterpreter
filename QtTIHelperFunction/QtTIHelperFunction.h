#ifndef QTTIHELPERFUNCTION_H
#define QTTIHELPERFUNCTION_H

#include "../QtTIDefines/QtTIDefines.h"
#include "QtTIAbstractHelperFunction.h"
#include "QtTIHelperFunctionArg.h"

#include <typeinfo>
#include <functional>
#include <QDebug>

//!
//! \brief The QtTIHelperFunction class
//!
template<typename... T>
class QtTIHelperFunction : public QtTIAbstractHelperFunction
{
public:
    //!
    //! \brief QtTIHelperFunction
    //! \param name Function name
    //! \param callback Function callback
    //!
    //! === Example:
    //!     new QtTIHelperFunction<>("my_test_array", []() {
    //!         return QStringList({ "1", "11", "111" });
    //!     });
    //!
    QtTIHelperFunction(const QString &name,
                       std::function<QVariant/*res*/(const T&... args)> callback)
        : QtTIAbstractHelperFunction(name)
        , _callback(callback) {
        Q_ASSERT_WITH_MSG (_callback != nullptr,
                           QString("[QtTIHelperFunction - %1] Invalid callback function!")
                           .arg(fullName()));
    }

    //!
    //! \brief QtTIHelperFunction
    //! \param name Function name
    //! \param callback Function callback
    //!
    //! === Example:
    //!     new QtTIHelperFunction<TestD*>("TestD_ptr_func", [](const TestD *d) {
    //!         if (!d)
    //!             return std::make_tuple(false/*isOk*/, QString("")/*res*/, "NULL_PTR"/*err*/);
    //!
    //!         return std::make_tuple(true/*isOk*/, d->name()/*res*/, ""/*err*/);
    //!     });
    //!
    QtTIHelperFunction(const QString &name,
                       std::function<std::tuple<bool/*isOk*/,QVariant/*res*/,QString/*err*/>(const T&... args)> callback)
        : QtTIAbstractHelperFunction(name)
        , _callback_tuple(callback) {
        Q_ASSERT_WITH_MSG (_callback_tuple != nullptr,
                           QString("[QtTIHelperFunction - %1] Invalid callback function!")
                           .arg(fullName()));
    }

    virtual ~QtTIHelperFunction() = default;

    //!
    //! \brief Get function full name (with input args)
    //! \return
    //!
    QString fullName() const final {
        return QString("%1 (%2)").arg(name()).arg(typesToStr(neededArgsTypes()));
    }

    //!
    //! \brief Needed arguments types for function
    //! \return
    //!
    QStringList neededArgsTypes() const final {
        return argsTypesHelper(std::index_sequence_for<T...>{});
    }

    //!
    //! \brief Check is can convert args types
    //! \param args Input arguments
    //! \return
    //!
    bool canConvertArgsTypes(const QVariantList &args) const final {
        if (neededArgsTypes().size() != args.size())
            return false;
        const QList<bool> tmpList = canConvertArgsTypesHelper(args, std::index_sequence_for<T...>{});
        return !tmpList.contains(false);
    }

    //!
    //! \brief Execute function
    //! \param args Input arguments
    //! \return
    //!
    std::tuple<bool/*isOk*/,QVariant/*res*/,QString/*err*/> evalFunction(const QVariantList &args) const final {
        const QStringList neededTypes = neededArgsTypes();
        const QStringList inputTypes = vListArgsTypes(args);
        if (neededTypes != inputTypes && !canConvertArgsTypes(args)) {
            QString err = QString("Invalid function args!"
                                  "\r\n  - needed: %1"
                                  "\r\n  -  input: %2 (%3)")
                          .arg(fullName())
                          .arg(name())
                          .arg(inputTypes.join(", "));

            return std::make_tuple(false, QVariant(), err);
        }
        if (_callback)
            return std::make_tuple(true,
                                   this->evalFunctionHelper(args, std::index_sequence_for<T...>{}),
                                   "");

        return this->evalFunctionHelper_tuple(args, std::index_sequence_for<T...>{});
    }

private:
    //!
    //! \brief Helper method to call a function 'std::function<QVariant(const T&... args)> _callback'
    //! \param args
    //! \return
    //!
    template<std::size_t... I>
    QVariant evalFunctionHelper(const QVariantList &args, std::index_sequence<I...>) const {
        return _callback(QtTIHelperFunctionArg<T>(args.at(I)).data()...);
    }

    //!
    //! \brief Helper method to call a function 'std::function<std::tuple<bool,QVariant,QString>(const T&... args)> _callback_tuple'
    //! \param args
    //! \return
    //!
    template<std::size_t... I>
    std::tuple<bool/*isOk*/,QVariant/*res*/,QString/*err*/> evalFunctionHelper_tuple(const QVariantList &args, std::index_sequence<I...>) const {
        return _callback_tuple(QtTIHelperFunctionArg<T>(args.at(I)).data()...);
    }

    //!
    //! \brief Helper method for generating a list of argument types
    //! \return
    //!
    template<std::size_t... I>
    QStringList argsTypesHelper(std::index_sequence<I...>) const {
        return QStringList({QtTIHelperFunctionArg<T>().typeName()...});
    }

    //!
    //! \brief Helper method for generating a list of can convert args types results
    //! \param args
    //! \return
    //!
    template<std::size_t... I>
    QList<bool> canConvertArgsTypesHelper(const QVariantList &args, std::index_sequence<I...>) const {
        return QList<bool>({QtTIHelperFunctionArg<T>().canConvert(args.at(I))...});
    }

private:
    std::function<QVariant/*res*/(const T&... args)> _callback {nullptr};                                               //!< callback for function returning QVariant
    std::function<std::tuple<bool/*isOk*/,QVariant/*res*/,QString/*err*/>(const T&... args)> _callback_tuple {nullptr}; //!< callback for function returning std::tuple<bool/*isOk*/,QVariant/*res*/,QString/*err*/>
};

#endif // QTTIHELPERFUNCTION_H
