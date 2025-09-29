#ifndef QTTIPARSER_H
#define QTTIPARSER_H

#include <QString>
#include <tuple>

#include "ControlBlockFabric/QtTIControlBlockFabric.h"
#include "Abstract/QtTIAbstractParser.h"
#include "QtTIParserArgs.h"
#include "QtTIParserFunc.h"

class QtTIParser : public QtTIAbstractParser
{
public:
    QtTIParser();
    virtual ~QtTIParser();

    QtTIAbstractParserArgs *parserArgs() final;
    QtTIAbstractParserFunc *parserFunc() final;

    std::tuple<bool/*isOk*/,QString/*res*/,QString/*err*/> parseLine(const QString &line,
                                                                     const int lineNum,
                                                                     QtTIAbstractParserBlock *&block) final;

    std::tuple<bool/*isOk*/,QVariant/*res*/,QString/*err*/> parseAndExecBlockData(QtTIAbstractParserBlock *block) final;
    std::tuple<bool/*isOk*/,QVariant/*res*/,QString/*err*/> parseAndExecBlockData(const QString &data, const QPair<int, int> &startPos) final;

private:
    QString evalHelpParam(const QString& paramName);
    QString evalHelpTmpParam(const QString& paramName);

    std::tuple<bool/*isOk*/,QVariant/*res*/,QString/*err*/> evalHelpFunction(const QString& funcName, const QVariantList &args) const;

private:
    QtTIParserArgs _parserArgs; //!< parser args object
    QtTIParserFunc _parserFunc; //!< parser func object
    QtTIControlBlockFabric _blockFabric {this}; //!< control blocks fabric
};

#endif // QTTIPARSER_H
