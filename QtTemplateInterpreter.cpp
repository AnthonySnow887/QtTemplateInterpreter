#include "QtTemplateInterpreter.h"

#include <QTextStream>
#include <QFile>

QtTemplateInterpreter::QtTemplateInterpreter()
{
    _parser = new QtTIParser();
    _blockFabric = new QtTIControlBlockFabric(_parser);
}

QtTemplateInterpreter::~QtTemplateInterpreter()
{
    delete _blockFabric;
    delete _parser;
}

//!
//! \brief Append help function
//! \param func Function pointer
//!
//! === Example 1:
//!     QtTemplateBuilder.appendHelpFunction(new QtTIHelperFunction<>("my_test_array", []() {
//!         return QStringList({ "1", "11", "111" });
//!     }));
//!
//! === Example 2:
//!     QtTemplateBuilder.appendHelpFunction(new QtTIHelperFunction<QStringList>("size", [](const QStringList &lst) {
//!         return lst.size();
//!     }));
//!
//! === Example 3:
//!     QtTemplateBuilder.appendHelpFunction(new QtTIHelperFunction<TestD*>("TestD_ptr_func", [](const TestD *d) {
//!         if (!d)
//!             return std::make_tuple(false/*isOk*/, QString("")/*res*/, "NULL_PTR"/*err*/);
//!
//!         return std::make_tuple(true/*isOk*/, d->name()/*res*/, ""/*err*/);
//!     }));
//!
void QtTemplateInterpreter::appendHelpFunction(const QtTIAbstractHelperFunction *func)
{
    _parser->parserFunc()->appendHelpFunction(func);
}

//!
//! \brief Remove help function
//! \param funcName Function name
//! \param args Function arguments array
//!
//! === Example 1:
//!     QtTemplateBuilder.removeHelpFunction("my_test_array", QVariantList());
//!
//! === Example 2:
//!     QtTemplateBuilder.removeHelpFunction("size", QVariantList({ QStringList() }));
//!
//! === Example 3:
//!     QtTemplateBuilder.removeHelpFunction("TestD_ptr_func", QVariantList({ QVariant::fromValue(new TestD()) }));
//!
void QtTemplateInterpreter::removeHelpFunction(const QString &funcName, const QVariantList &args)
{
    _parser->parserFunc()->removeHelpFunction(funcName, args);
}

//!
//! \brief Append global help param
//! \param paramName Global parameter name
//! \param value Global parameter value
//!
//! === Example 1:
//!     QtTemplateBuilder.appendHelpParam("test_key", "This is test key");
//!
//! === Example 2:
//!     QtTemplateBuilder.appendHelpParam("my_test_d", QVariant::fromValue(TestD("this is my test name")));
//!     QtTemplateBuilder.appendHelpParam("my_test_d_ptr", QVariant::fromValue(new TestD("this is my test name (ptr)")));
//!
void QtTemplateInterpreter::appendHelpParam(const QString &paramName, const QVariant &value)
{
    _parser->parserArgs()->appendParam(paramName, value);
}

//!
//! \brief Remove global help parameter
//! \param paramName Global parameter name
//!
//! NOTE: if a pointer was used as a parameter, then this method does not delete it!
//!
//! === Example:
//!     QtTemplateBuilder.removeHelpParam("my_test_d");
//!
void QtTemplateInterpreter::removeHelpParam(const QString &paramName)
{
    _parser->parserArgs()->removeParam(paramName);
}

//!
//! \brief Delete all added functions
//!
void QtTemplateInterpreter::clearFunctions()
{
    _parser->parserFunc()->clearFunctions();
}

//!
//! \brief Delete all added global parameters
//!
//! NOTE: if a pointer was used as a parameter, then this method does not delete it!
//!
void QtTemplateInterpreter::clearParams()
{
    _parser->parserArgs()->clearParams();
}

//!
//! \brief Interpret the template data
//! \param data Template data
//! \return
//!
std::tuple<bool, QString, QString> QtTemplateInterpreter::interpret(QString data)
{
    QString lineEndAppender;
    if (data.indexOf("\n") != -1)
        lineEndAppender = QString("\r\n");

    QTextStream in(&data);
    int lineNum = 0;
    bool isMultilineComment = false;
    QtTIAbstractControlBlock *block = nullptr;
    QString unfinishedBlockCond;
    QString tmpData;
    while (!in.atEnd()) {
        if (!tmpData.isEmpty()
            && tmpData[tmpData.size() - 1] != '\n')
            tmpData += "\r\n";

        lineNum++;
        QString line = in.readLine() + lineEndAppender;

        // remove comments
        line = _parser->removeComments(line, &isMultilineComment);

        // search block
        bool isOk = false;
        QString error;
        std::tie(isOk, line, block, unfinishedBlockCond, error) = _blockFabric->parseBlock(line, block, unfinishedBlockCond, lineNum);
        if (!isOk) {
            clear(block);
            return std::make_tuple(false, "", error);
        }
        if (!unfinishedBlockCond.isEmpty() || isMultilineComment)
            continue;

        // search params & functions
        std::tie(isOk, line, error) = _parser->parseLine(line, lineNum);
        if (!isOk) {
            clear(block);
            return std::make_tuple(false, "", error);
        }
        tmpData += line;
    }

    // check block
    if (block) {
        QString error = QString("Unfinished block in line %1!").arg(block->lineNum());
        clear(block);
        return std::make_tuple(false, "", error);
    }

    clear(block);
    return std::make_tuple(true, tmpData, "");
}

//!
//! \brief Interpret the template data and return result
//! \param data Template data
//! \return
//!
//! NOTE: This method only displays a critical message to the console in case of errors.
//!
QString QtTemplateInterpreter::interpretRes(QString data)
{
    bool isOk = false;
    QString result, error;
    std::tie(isOk, result, error) = interpret(data);
    if (!isOk)
        qCritical() << qPrintable(QString("[QtTemplateInterpreter][interpretRes] %1").arg(error));
    return result;
}

//!
//! \brief Interpret the template data from file
//! \param path File path
//! \return
//!
std::tuple<bool, QString, QString> QtTemplateInterpreter::interpretFromFile(const QString &path)
{
    if (path.isEmpty())
        return std::make_tuple(false, "", "Empty file path");
    if (!QFile::exists(path))
        return std::make_tuple(false, "", QString("File '%1' is not exist").arg(path));
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
        return std::make_tuple(false, "", QString("Open file '%1' for read failed").arg(path));

    const QByteArray fData = f.readAll();
    f.close();
    return interpret(QString::fromUtf8(fData));
}

//!
//! \brief Interpret the template data from file and return result
//! \param path File path
//! \return
//!
//! NOTE: This method only displays a critical message to the console in case of errors.
//!
QString QtTemplateInterpreter::interpretResFromFile(const QString &path)
{
    bool isOk = false;
    QString result, error;
    std::tie(isOk, result, error) = interpretFromFile(path);
    if (!isOk)
        qCritical() << qPrintable(QString("[QtTemplateInterpreter][interpretResFromFile] %1").arg(error));
    return result;
}

//!
//! \brief Clear all tmp data
//! \param block Control block pointer
//! \private
//!
void QtTemplateInterpreter::clear(QtTIAbstractControlBlock *block)
{
    _parser->parserArgs()->clearTmpParams();
    if (block)
        delete block;
}
