#ifndef QTTIABSTRACTHELPERFUNCTION_H
#define QTTIABSTRACTHELPERFUNCTION_H

#include <QString>
#include <QStringList>
#include <QVariantList>

//!
//! \brief The QtTIAbstractHelperFunction class
//!
class QtTIAbstractHelperFunction
{
public:
    QtTIAbstractHelperFunction(const QString &name)
        : _name(name.trimmed())
    {}

    virtual ~QtTIAbstractHelperFunction() = default;

    //!
    //! \brief Get function name
    //! \return
    //!
    QString name() const {
        return _name;
    }

    //!
    //! \brief Get function full name (with input args)
    //! \return
    //!
    virtual QString fullName() const = 0;

    //!
    //! \brief Needed arguments types for function
    //! \return
    //!
    virtual QStringList neededArgsTypes() const = 0;

    //!
    //! \brief Check is can convert args types
    //! \param args Input arguments
    //! \return
    //!
    virtual bool canConvertArgsTypes(const QVariantList &args) const = 0;

    //!
    //! \brief Execute function
    //! \param args Input arguments
    //! \return
    //!
    virtual std::tuple<bool/*isOk*/,QVariant/*res*/,QString/*err*/> evalFunction(const QVariantList &args) const = 0;

    //!
    //! \brief QVarianList to arguments types list
    //! \param args Input arguments
    //! \return
    //!
    static QStringList vListArgsTypes(const QVariantList &args) {
        QStringList lst;
        for (const QVariant &a : args) {
            if (a.isNull())
                lst << QString("Null");
            else
                lst << QString(a.typeName());
        }
        return lst;
    }

    //!
    //! \brief Arguments types list to string
    //! \param types Arguments types list
    //! \return
    //!
    static QString typesToStr(const QStringList &types) {
        return types.join(", ");
    }

private:
    QString _name;  //!< function name
};

#endif // QTTIABSTRACTHELPERFUNCTION_H
