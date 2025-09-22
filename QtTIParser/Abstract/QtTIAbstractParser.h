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

    //!
    //! \brief Select parser args object pointer
    //! \return
    //!
    virtual QtTIAbstractParserArgs *parserArgs() = 0;

    //!
    //! \brief Select parser func object pointer
    //! \return
    //!
    virtual QtTIAbstractParserFunc *parserFunc() = 0;

    //!
    //! \brief Parse line data (parse help functions and help parameters)
    //! \param line Line data
    //! \param lineNum Line number
    //! \param block Abstract parser block object
    //! \return
    //!
    virtual std::tuple<bool/*isOk*/,QString/*res*/,QString/*err*/> parseLine(const QString &line,
                                                                             const int lineNum,
                                                                             QtTIAbstractParserBlock *&block) = 0;

    //!
    //! \brief Parse and execute abstract block data
    //! \param block Abstract parser block object
    //! \return
    //!
    virtual std::tuple<bool/*isOk*/,QVariant/*res*/,QString/*err*/> parseAndExecBlockData(QtTIAbstractParserBlock *block) = 0;

    //!
    //! \brief Parse and execute abstract block data
    //! \param data Parser block data
    //! \param startPos Parser block start position in line
    //! \return
    //!
    virtual std::tuple<bool/*isOk*/,QVariant/*res*/,QString/*err*/> parseAndExecBlockData(const QString &data,
                                                                                          const QPair<int, int> &startPos) = 0;

    //!
    //! \brief Trim the line on the left
    //! \param str String
    //! \return
    //!
    static QString lstrip(const QString &str) {
        int n = 0;
        for (; n < str.size(); ++n) {
            if (!str.at(n).isSpace())
                return str.right(str.size() - n);
        }
        return QString();
    }

    //!
    //! \brief Trim the line on the right
    //! \param str String
    //! \return
    //!
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
