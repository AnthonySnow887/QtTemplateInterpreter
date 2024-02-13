#ifndef QTTEMPLATEINTERPRETER_H
#define QTTEMPLATEINTERPRETER_H

#include <QVariant>
#include <QString>
#include <QHash>
#include <QMultiHash>

#include "QtTemplateInterpreterVersion.h"
#include "QtTIParser/QtTIParser.h"
#include "QtTIControlBlock/QtTIControlBlockFabric.h"

class QtTemplateInterpreter
{
public:
    QtTemplateInterpreter();
    ~QtTemplateInterpreter();

    void appendHelpFunction(const QtTIAbstractHelperFunction *func);
    void removeHelpFunction(const QString &funcName, const QVariantList &args);

    void appendHelpParam(const QString &paramName, const QVariant &value);
    void removeHelpParam(const QString &paramName);

    void clearFunctions();
    void clearParams();

    std::tuple<bool/*isOk*/,QString/*result*/,QString/*error*/> interpret(QString data);
    QString interpretRes(QString data);

    std::tuple<bool/*isOk*/,QString/*result*/,QString/*error*/> interpretFromFile(const QString &path);
    QString interpretResFromFile(const QString &path);

private:
    QtTIParser *_parser {nullptr};                      //!< data parser
    QtTIControlBlockFabric *_blockFabric {nullptr};     //!< control blocks fabric

    void clear(QtTIAbstractControlBlock *block);
};

#endif // QTTEMPLATEINTERPRETER_H
