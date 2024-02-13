#ifndef QTTIPARSERFUNC_H
#define QTTIPARSERFUNC_H

#include <QMultiHash>
#include <QString>
#include "../QtTIHelperFunction/QtTIHelperFunction.h"

class QtTIParserFunc
{
public:
    QtTIParserFunc();
    ~QtTIParserFunc();

    void appendHelpFunction(const QtTIAbstractHelperFunction *func);
    void removeHelpFunction(const QString &funcName, const QVariantList &args);

    bool hasHelpFunction(const QString& funcName, const QVariantList &args, const bool strictArgs = false) const;
    const QtTIAbstractHelperFunction *findHelpFunction(const QString& funcName, const QVariantList &args, const bool strictArgs = false) const;
    std::tuple<bool/*isOk*/,QVariant/*res*/,QString/*err*/> evalHelpFunction(const QString& funcName, const QVariantList &args, const bool strictArgs = false) const;

    void clearFunctions();

private:
    QMultiHash<QString, const QtTIAbstractHelperFunction *> _functions;
};

#endif // QTTIPARSERFUNC_H
