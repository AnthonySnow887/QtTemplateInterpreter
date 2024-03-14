#include "QtTIControlBlockFor.h"

#include <QList>
#include <QMap>
#include <QHash>

QtTIControlBlockFor::QtTIControlBlockFor(QtTIParser *parser)
    : QtTIAbstractControlBlock(parser, -1)
{}

QtTIControlBlockFor::QtTIControlBlockFor(QtTIParser *parser, const QString &blockCond, const int lineNum)
    : QtTIAbstractControlBlock(parser, lineNum)
    , _blockCond(blockCond)
{}

QtTIControlBlockFor::~QtTIControlBlockFor()
{}

//!
//! \brief Make control block
//! \param blockCond Control block condition
//! \param lineNum Line number
//! \return
//!
QtTIAbstractControlBlock *QtTIControlBlockFor::makeBlock(const QString &blockCond, const int lineNum)
{
    return new QtTIControlBlockFor(parser(), blockCond, lineNum);
}

//!
//! \brief Get control block condition
//! \return
//!
QString QtTIControlBlockFor::blockCondition() const
{
    return _blockCond;
}

//!
//! \brief Check is control block start
//! \param blockCond Control block condition
//! \return
//!
bool QtTIControlBlockFor::isBlockCondStart(const QString &blockCond)
{
    QRegExp rx("^(for\\s+([A-Za-z0-9_\\ \\,]+)\\s+in\\s+([A-Za-z0-9_\\ \\+\\-\\,\\.\\'\\\"\\{\\}\\[\\]\\:\\/\\(\\)]+))");
    return (rx.indexIn(blockCond) != -1);
}

//!
//! \brief Check is control block end
//! \param blockCond Control block condition
//! \return
//!
bool QtTIControlBlockFor::isBlockCondEnd(const QString &blockCond)
{
    QRegExp rx("^(endfor)$");
    return (rx.indexIn(blockCond) != -1);
}

//!
//! \brief Append control block body
//! \param blockBody Control block body
//!
void QtTIControlBlockFor::appendBlockBody(const QString &blockBody, const int lineNum)
{
    _blockBody[lineNum].append(blockBody);
}

//!
//! \brief Execute control block
//! \return
//!
std::tuple<bool, QString, QString> QtTIControlBlockFor::evalBlock()
{
    QRegExp rx("^(for\\s+([A-Za-z0-9_\\ \\,]+)\\s+in\\s+([A-Za-z0-9_\\ \\+\\-\\,\\.\\'\\\"\\{\\}\\[\\]\\:\\/\\(\\)]+))");
    if (rx.indexIn(_blockCond) != -1) {
        const QStringList paramNames = rx.cap(2).trimmed().split(",");

        bool isOk = false;
        QVariant paramValue;
        QString error;
        std::tie(isOk, paramValue, error) = parseParamValue(rx.cap(3).trimmed());
        if (!isOk)
            return std::make_tuple(false, "", error);
        if (paramNames.isEmpty())
            return std::make_tuple(false, "", QString("Invalid parameter names (empty) in block 'for ...' in line %1").arg(lineNum()));
        if (paramNames.size() > 2)
            return std::make_tuple(false, "", QString("Invalid parameter names (max two values) in block 'for ...' in line %1").arg(lineNum()));
        if (paramValue.isNull())
            return std::make_tuple(false, "", QString("Invalid parameter container (Null) in block 'for ...' in line %1").arg(lineNum()));
        if (!paramValue.canConvert<QVariantList>()
            && !paramValue.canConvert<QVariantMap>()
            && !paramValue.canConvert<QVariantHash>())
            return std::make_tuple(false, "", QString("Invalid container type '%1' (not List/Map/Hash) in block 'for ...' in line %2").arg(paramValue.typeName()).arg(lineNum()));
        if (paramValue.canConvert<QVariantList>()
            && paramNames.size() != 1)
            return std::make_tuple(false, "", QString("Too many parameters for type List in block 'for ...' in line %1").arg(lineNum()));
        for (const QString &arg : paramNames) {
            if (parser()->parserArgs()->hasParam(arg))
                return std::make_tuple(false, "", QString("Parameter with name '%1' in block 'for ...' in line %2 already declared in the global parameter list").arg(arg).arg(lineNum()));
        }
        return evalFor(paramNames, paramValue);
    }
    return std::make_tuple(false, "", QString("Unsupported block condition '%1' in line %2").arg(_blockCond).arg(lineNum()));
}

//!
//! \brief Execute for-loop
//! \param args
//! \param container
//! \return
//!
std::tuple<bool, QString, QString> QtTIControlBlockFor::evalFor(const QStringList &args, const QVariant &container)
{
    if (container.canConvert<QVariantList>())
        return evalList(args[0], container);
    else if (container.canConvert<QVariantMap>())
        return evalMap(args, container.toMap());
    else if (container.canConvert<QVariantHash>())
        return evalMap(args, container.toHash());

    return std::make_tuple(false, "", QString("Unsupoorted container type '%1' for block 'for...' in line %2").arg(container.typeName()).arg(lineNum()));
}

//!
//! \brief Execute for-loop for list
//! \param arg
//! \param container
//! \return
//!
std::tuple<bool, QString, QString> QtTIControlBlockFor::evalList(const QString &arg, const QVariant &container)
{
    bool resetArg = false;
    QVariant resetArgValue;
    if (parser()->parserArgs()->hasTmpParam(arg)) {
        resetArgValue = parser()->parserArgs()->tmpParam(arg);
        resetArg = true;
    }

    QSequentialIterable iterable = container.value<QSequentialIterable>();
    bool isOk = false;
    QString allBody;
    QString error;
    for (const QVariant &v : iterable) {
        if (!allBody.isEmpty()
            && allBody[allBody.size() - 1] != '\n')
            allBody += "\r\n";

        parser()->parserArgs()->appendTmpParam(arg, v);
        QString tmpBody;
        std::tie(isOk, tmpBody, error) = buildBlockBody(_blockBody);
        parser()->parserArgs()->removeTmpParam(arg);
        if (!isOk)
            return std::make_tuple(false, "", error);

        allBody += tmpBody;
    }
    if (resetArg)
        parser()->parserArgs()->appendTmpParam(arg, resetArgValue);

    return std::make_tuple(true, allBody, "");
}

//!
//! \brief Execute for-loop for map or hash
//! \param args
//! \param container
//! \return
//!
std::tuple<bool, QString, QString> QtTIControlBlockFor::evalMap(const QStringList &args, const QVariant &container)
{
    bool resetArg = false;
    QVariantMap resetArgValues;
    for (const QString &arg : args) {
        if (parser()->parserArgs()->hasTmpParam(arg.trimmed())) {
            resetArgValues.insert(arg.trimmed(), parser()->parserArgs()->tmpParam(arg));
            resetArg = true;
        }
    }

    bool isOk = false;
    QString allBody;
    QString error;
    QAssociativeIterable iterable = container.value<QAssociativeIterable>();
    QAssociativeIterable::const_iterator it = iterable.begin();
    const QAssociativeIterable::const_iterator end = iterable.end();
    for ( ; it != end; ++it) {
        if (!allBody.isEmpty()
            && allBody[allBody.size() - 1] != '\n')
            allBody += "\r\n";

        if (args.size() == 1) {
            parser()->parserArgs()->appendTmpParam(args[0].trimmed(), it.value());
        } else {
            parser()->parserArgs()->appendTmpParam(args[0].trimmed(), it.key());
            parser()->parserArgs()->appendTmpParam(args[1].trimmed(), it.value());
        }

        QString tmpBody;
        std::tie(isOk, tmpBody, error) = buildBlockBody(_blockBody);

        if (args.size() == 1) {
            parser()->parserArgs()->removeTmpParam(args[0].trimmed());
        } else {
            parser()->parserArgs()->removeTmpParam(args[0].trimmed());
            parser()->parserArgs()->removeTmpParam(args[1].trimmed());
        }

        if (!isOk)
            return std::make_tuple(false, "", error);

        allBody += tmpBody;
    }

    if (resetArg) {
        QMapIterator<QString, QVariant> it (resetArgValues);
        while (it.hasNext()) {
            it.next();
            parser()->parserArgs()->appendTmpParam(it.key(), it.value());
        }
    }
    return std::make_tuple(true, allBody, "");
}
