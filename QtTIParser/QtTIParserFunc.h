#ifndef QTTIPARSERFUNC_H
#define QTTIPARSERFUNC_H

#include <QMultiHash>
#include <QString>
#include "Abstract/QtTIAbstractParserFunc.h"
#include "../QtTIHelperFunction/QtTIHelperFunction.h"

class QtTIParserFunc : public QtTIAbstractParserFunc
{
public:
    QtTIParserFunc();
    virtual ~QtTIParserFunc();

    void appendHelpFunction(const QtTIAbstractHelperFunction *func) final;
    void removeHelpFunction(const QString &funcName, const QVariantList &args) final;

    bool hasHelpFunction(const QString& funcName, const QVariantList &args, const bool strictArgs = false) const final;
    const QtTIAbstractHelperFunction *findHelpFunction(const QString& funcName, const QVariantList &args, const bool strictArgs = false) const final;
    std::tuple<bool/*isOk*/,QVariant/*res*/,QString/*err*/> evalHelpFunction(const QString& funcName, const QVariantList &args, const bool strictArgs = false) const final;

    void clearFunctions() final;

private:
    QMultiHash<QString, const QtTIAbstractHelperFunction *> _functions; //!< functions array
};

#endif // QTTIPARSERFUNC_H
