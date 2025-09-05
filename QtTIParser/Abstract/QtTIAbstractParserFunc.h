#ifndef QTTIABSTRACTPARSERFUNC_H
#define QTTIABSTRACTPARSERFUNC_H

#include <QString>
#include "../../QtTIHelperFunction/QtTIAbstractHelperFunction.h"

class QtTIAbstractParserFunc
{
public:
    QtTIAbstractParserFunc() = default;
    virtual ~QtTIAbstractParserFunc() = default;

    virtual void appendHelpFunction(const QtTIAbstractHelperFunction *func) = 0;
    virtual void removeHelpFunction(const QString &funcName, const QVariantList &args) = 0;

    virtual bool hasHelpFunction(const QString& funcName,
                                 const QVariantList &args,
                                 const bool strictArgs = false) const = 0;

    virtual const QtTIAbstractHelperFunction *findHelpFunction(const QString& funcName,
                                                               const QVariantList &args,
                                                               const bool strictArgs = false) const = 0;

    virtual std::tuple<bool/*isOk*/,QVariant/*res*/,QString/*err*/> evalHelpFunction(const QString& funcName,
                                                                                     const QVariantList &args,
                                                                                     const bool strictArgs = false) const = 0;

    virtual void clearFunctions() = 0;
};

#endif // QTTIABSTRACTPARSERFUNC_H
