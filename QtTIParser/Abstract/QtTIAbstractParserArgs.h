#ifndef QTTIABSTRACTPARSERARGS_H
#define QTTIABSTRACTPARSERARGS_H

#include <QString>
#include <QVariant>

class QtTIAbstractParserArgs
{
public:
    QtTIAbstractParserArgs() = default;
    virtual ~QtTIAbstractParserArgs() = default;

    //!
    //! \brief Append global help param
    //! \param paramName Global parameter name
    //! \param value Global parameter value
    //!
    virtual void appendParam(const QString &paramName, const QVariant &value) = 0;

    //!
    //! \brief Remove global help parameter
    //! \param paramName Global parameter name
    //!
    //! NOTE: if a pointer was used as a parameter, then this method does not delete it!
    //!
    virtual void removeParam(const QString &paramName) = 0;

    //!
    //! \brief Check has global help parameter found
    //! \param paramName Parameter name
    //! \return
    //!
    virtual bool hasParam(const QString& paramName) = 0;

    //!
    //! \brief Get global help parameter value
    //! \param paramName Parameter name
    //! \return
    //!
    virtual QVariant param(const QString& paramName) = 0;

    //!
    //! \brief Delete all added global parameters
    //!
    //! NOTE: if a pointer was used as a parameter, then this method does not delete it!
    //!
    virtual void clearParams() = 0;

    //!
    //! \brief Append help tmp parameter
    //! \param paramName Tmp parameter name
    //! \param paramValue Tmp parameter value
    //!
    virtual void appendTmpParam(const QString& paramName, const QVariant &paramValue) = 0;

    //!
    //! \brief Remove help tmp parameter
    //! \param paramName Tmp parameter name
    //!
    virtual void removeTmpParam(const QString& paramName) = 0;

    //!
    //! \brief Check has help tmp parameter found
    //! \param paramName Tmp parameter name
    //! \return
    //!
    virtual bool hasTmpParam(const QString& paramName) = 0;

    //!
    //! \brief Get help tmp parameter value
    //! \param paramName Tmp parameter name
    //! \return
    //!
    virtual QVariant tmpParam(const QString& paramName) = 0;

    //!
    //! \brief Delete all added tmp parameters
    //!
    //! NOTE: if a pointer was used as a parameter, then this method does not delete it!
    //!
    virtual void clearTmpParams() = 0;

    //!
    //! \brief Parse help function arguments
    //! \param args String representation of arguments
    //! \param delimiter Arguments delimiter
    //! \return
    //!
    virtual QVariantList parseHelpFunctionArgs(const QString &args, const QChar &delimiter = QChar(',')) = 0;

    //!
    //! \brief Parse argument
    //! \param arg String representation of argument
    //! \return
    //!
    //! Convert data types:
    //!            int : 123 / +123 / -123 / i123 / +i123 / -i123
    //!           uint : ui123
    //!      qlonglong : l12345678 or -l12345678
    //!     qulonglong : ul12345678
    //!         double : 123.45 / +123.45 / -123.45 / d123.45 / +d123.45 / -d123.45
    //!          float : f123.45 / +f123.45 / -f123.45
    //!        QString : '...' / s'...' / "..." / s"..."
    //!     QByteArray : b'...' / b"..."
    //!   QVariantList : [...]
    //!    QVariantMap : {...} / m{...}
    //!   QVariantHash : h{...}
    //!
    virtual QVariant prepareHelpFunctionArg(const QString &arg) = 0;
};

#endif // QTTIABSTRACTPARSERARGS_H
