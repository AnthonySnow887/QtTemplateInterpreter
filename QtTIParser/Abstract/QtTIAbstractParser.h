#ifndef QTTIABSTRACTPARSER_H
#define QTTIABSTRACTPARSER_H

#include "QtTIAbstractParserArgs.h"
#include "QtTIAbstractParserFunc.h"
#include "QtTIAbstractParserBlock.h"

#include <QString>
#include <QVariant>

class QtTIAbstractParser
{
public:
    QtTIAbstractParser() = default;
    virtual ~QtTIAbstractParser() = default;

    virtual QtTIAbstractParserArgs *parserArgs() = 0;
    virtual QtTIAbstractParserFunc *parserFunc() = 0;

    // TODO delete!
    virtual std::tuple<bool/*isOk*/,QString/*res*/,QString/*err*/> parseLine(const QString &line, const int lineNum) = 0;

    virtual std::tuple<bool/*isOk*/,QString/*res*/,QString/*err*/> parseLine_v2(const QString &line,
                                                                                const int lineNum,
                                                                                QtTIAbstractParserBlock *&block) = 0;

    virtual std::tuple<bool/*isOk*/,QVariant/*res*/,QString/*err*/> parseAndExecBlockData(QtTIAbstractParserBlock *block) = 0;

    virtual std::tuple<bool/*isOk*/,QVariant/*res*/,QString/*err*/> parseAndExecBlockData(const QString &data,
                                                                                          const QPair<int, int> &startPos) = 0;

    static QString lstrip(const QString &str) {
        int n = 0;
        for (; n < str.size(); ++n) {
            if (!str.at(n).isSpace())
                return str.right(str.size() - n);
        }
        return QString();
    }

    static QString rstrip(const QString &str) {
        int n = str.size() - 1;
        for (; n >= 0; --n) {
            if (!str.at(n).isSpace())
                return str.left(n + 1);
        }
        return QString();
    }
};

#endif // QTTIABSTRACTPARSER_H
