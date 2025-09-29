#ifndef QTTIABSTRACTPARSERFUNC_H
#define QTTIABSTRACTPARSERFUNC_H

#include <QString>
#include "../../QtTIHelperFunction/QtTIAbstractHelperFunction.h"

class QtTIAbstractParserFunc
{
public:
    QtTIAbstractParserFunc() = default;
    virtual ~QtTIAbstractParserFunc() = default;

    //!
    //! \brief Append help function
    //! \param func Function pointer
    //!
    virtual void appendHelpFunction(const QtTIAbstractHelperFunction *func) = 0;

    //!
    //! \brief Remove help function
    //! \param funcName Function name
    //! \param args Function arguments array
    //!
    virtual void removeHelpFunction(const QString &funcName, const QVariantList &args) = 0;

    //!
    //! \brief Check has help function found
    //! \param funcName Function name
    //! \param args Function arguments
    //! \param strictArgs Use strong typing of function arguments without converting them
    //! \return
    //!
    virtual bool hasHelpFunction(const QString& funcName,
                                 const QVariantList &args,
                                 const bool strictArgs = false) const = 0;

    //!
    //! \brief Find help function by name and arguments
    //! \param funcName Function name
    //! \param args Function argumnets
    //! \param strictArgs Use strong typing of function arguments without converting them
    //! \return
    //!
    virtual const QtTIAbstractHelperFunction *findHelpFunction(const QString& funcName,
                                                               const QVariantList &args,
                                                               const bool strictArgs = false) const = 0;

    //!
    //! \brief Execute help function
    //! \param funcName Function name
    //! \param args Function arguments
    //! \param strictArgs Use strong typing of function arguments without converting them
    //! \return
    //!
    virtual std::tuple<bool/*isOk*/,QVariant/*res*/,QString/*err*/> evalHelpFunction(const QString& funcName,
                                                                                     const QVariantList &args,
                                                                                     const bool strictArgs = false) const = 0;

    //!
    //! \brief Delete all added functions
    //!
    virtual void clearFunctions() = 0;
};

#endif // QTTIABSTRACTPARSERFUNC_H
