#include "QtTIParserArgs.h"
#include "../QtTIDefines/QtTIDefines.h"
#include "../QtTIHelperFunction/QtTIAbstractHelperFunction.h"

#include <QDebug>
#include <QMetaType>
#include <QMetaObject>
#include <QMetaProperty>
#include <QMetaMethod>


QtTIParserArgs::~QtTIParserArgs()
{
    clearParams();
    clearTmpParams();
}

//!
//! \brief Append global help param
//! \param paramName Global parameter name
//! \param value Global parameter value
//!
//! === Example 1:
//!     QtTIParserArgs.appendParam("test_key", "This is test key");
//!
//! === Example 2:
//!     QtTIParserArgs.appendParam("my_test_d", QVariant::fromValue(TestD("this is my test name")));
//!     QtTIParserArgs.appendParam("my_test_d_ptr", QVariant::fromValue(new TestD("this is my test name (ptr)")));
//!
void QtTIParserArgs::appendParam(const QString &paramName, const QVariant &value)
{
    Q_ASSERT_WITH_MSG(paramName.isEmpty() != true, "Param key is Empty!");
    QRegExp rxStr("^[\\w]+$");
    if (rxStr.indexIn(paramName) == -1) {
        qWarning() << qPrintable(QString("[QtTIParserArgs][appendParam] Invalid help param key (key: \"%1\")!")
                                 .arg(paramName));
        return;
    }
    if (_params.contains(paramName)) {
        qWarning() << qPrintable(QString("[QtTIParserArgs][appendParam] Help param already added (key: \"%1\")!")
                                 .arg(paramName));
        return;
    }
    if (value.isNull() && value.type() != QVariant::Type::String)
        qWarning() << qPrintable(QString("[QtTIParserArgs][appendParam] Param value is Null (key: \"%1\")!")
                                 .arg(paramName));

    _params.insert(paramName, value);
}

//!
//! \brief Remove global help parameter
//! \param paramName Global parameter name
//!
//! NOTE: if a pointer was used as a parameter, then this method does not delete it!
//!
//! === Example:
//!     QtTIParserArgs.removeParam("my_test_d");
//!
void QtTIParserArgs::removeParam(const QString &paramName)
{
    Q_ASSERT_WITH_MSG(paramName.isEmpty() != true, "Param key is Empty!");
    QRegExp rxStr("^[\\w]+$");
    if (rxStr.indexIn(paramName) == -1) {
        qWarning() << qPrintable(QString("[QtTIParserArgs][removeParam] Invalid help param key (key: \"%1\")!")
                                 .arg(paramName));
        return;
    }
    _params.remove(paramName);
}

//!
//! \brief Check has global help parameter found
//! \param paramName Parameter name
//! \return
//!
bool QtTIParserArgs::hasParam(const QString &paramName)
{
    const QVariant p = param(paramName);    
    if (p.type() == QVariant::Type::String)
        return true;
    return !p.isNull();
}

//!
//! \brief Get global help parameter value
//! \param paramName Parameter name
//! \return
//!
QVariant QtTIParserArgs::param(const QString &paramName)
{
    QString tmpArgName = paramName.split(".")[0];
    if (_params.contains(tmpArgName))
        return paramValueRecursive(paramName, _params);

    return QVariant();
}

//!
//! \brief Delete all added global parameters
//!
//! NOTE: if a pointer was used as a parameter, then this method does not delete it!
//!
void QtTIParserArgs::clearParams()
{
    _params.clear();
}

//!
//! \brief Append help tmp parameter
//! \param paramName Tmp parameter name
//! \param paramValue Tmp parameter value
//!
void QtTIParserArgs::appendTmpParam(const QString &paramName, const QVariant &paramValue)
{
    _tmpParams.insert(paramName, paramValue);
}

//!
//! \brief Remove help tmp parameter
//! \param paramName Tmp parameter name
//!
void QtTIParserArgs::removeTmpParam(const QString &paramName)
{
    _tmpParams.remove(paramName);
}

//!
//! \brief Check has help tmp parameter found
//! \param paramName Tmp parameter name
//! \return
//!
bool QtTIParserArgs::hasTmpParam(const QString &paramName)
{
    const QVariant p = tmpParam(paramName);
    if (p.type() == QVariant::Type::String)
        return true;
    return !p.isNull();
}

//!
//! \brief Get help tmp parameter value
//! \param paramName Tmp parameter name
//! \return
//!
QVariant QtTIParserArgs::tmpParam(const QString &paramName)
{
    QString tmpArgName = paramName.split(".")[0];
    if (_tmpParams.contains(tmpArgName))
        return paramValueRecursive(paramName, _tmpParams);

    return QVariant();
}

//!
//! \brief Delete all added tmp parameters
//!
//! NOTE: if a pointer was used as a parameter, then this method does not delete it!
//!
void QtTIParserArgs::clearTmpParams()
{
    _tmpParams.clear();
}

//!
//! \brief Parse help function arguments
//! \param args String representation of arguments
//! \param delimiter Arguments delimiter
//! \return
//!
QVariantList QtTIParserArgs::parseHelpFunctionArgs(const QString &args, const QChar &delimiter)
{
    if (args.trimmed().isEmpty())
        return QVariantList();
    QVariantList tmpArgs;
    QString currentArgStr;
    bool quote = false;
    bool dbl_quote = false;
    int openSquareBrackets = 0;
    int openBraces = 0;
    int roundBrackets = 0;
    for (int i = 0; i < args.size(); i++) {
        const QChar ch = args[i];
        QChar chPrev;
        if (i > 0)
            chPrev = args[i - 1];

        // check quotes
        if (ch == '\'' && !dbl_quote && (chPrev != '\\'))
            quote = !quote;
        if (ch == '"' && !quote && (chPrev != '\\'))
            dbl_quote = !dbl_quote;
        // check array
        if (ch == '[' && (chPrev != '\\') && !quote && !dbl_quote) {
            openSquareBrackets++;
        }
        if (ch == ']' && (chPrev != '\\') && !quote && !dbl_quote) {
            openSquareBrackets--;
        }
        // check hash
        if (ch == '{' && (chPrev != '\\') && !quote && !dbl_quote) {
            openBraces++;
        }
        if (ch == '}' && (chPrev != '\\') && !quote && !dbl_quote) {
            openBraces--;
        }
        // check func
        if (ch == '(' && (chPrev != '\\') && !quote && !dbl_quote) {
            roundBrackets++;
        }
        if (ch == ')' && (chPrev != '\\') && !quote && !dbl_quote) {
            roundBrackets--;
        }
        // check delimiter
        if (ch == delimiter
            && !quote && !dbl_quote
            && openSquareBrackets == 0
            && openBraces == 0
            && roundBrackets == 0) {
            tmpArgs.append(prepareHelpFunctionArg(currentArgStr));
            currentArgStr.clear();
            continue;
        }
        currentArgStr += ch;
    }
    tmpArgs.append(prepareHelpFunctionArg(currentArgStr));
    return tmpArgs;
}

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
//! === Example QVariantList
//!     [ 1, -2, 'abc', 12.15, b"def", f11.05, "this is test message, 1, 2, 3" ]
//!
//!     QVariantList {
//!         0: 1 (int)
//!         1: -2 (int)
//!         2: "abc" (QString)
//!         3: 12.15 (double)
//!         4: "def" (QByteArray)
//!         5: 11.05 (float)
//!         6: "this is test message, 1, 2, 3" (QString)
//!     }
//!
//! === Example QVariantMap / QVariantHash
//!     { a: 1, b: -2, c: 'abc', d: 12.15, e: b"def", f: f11.05, g: "this is test message, 1, 2, 3" }
//!
//!     QVariantMap {
//!         a: 1 (int)
//!         b: -2 (int)
//!         c: "abc" (QString)
//!         d: 12.15 (double)
//!         e: "def" (QByteArray)
//!         f: 11.05 (float)
//!         g: "this is test message, 1, 2, 3" (QString)
//!     }
//!
QVariant QtTIParserArgs::prepareHelpFunctionArg(const QString &arg)
{
    QString tmpArg = arg.trimmed();
    if (tmpArg.isEmpty())
        return QVariant();
    QRegExp rxStr("^(s|b)?[\'\"]{1,1}(.*)[\'\"]{1,1}$");
    QRegExp rxInt("^(\\+|\\-)?(i|ui|l|ul)?(\\d+)$");
    QRegExp rxDouble("^(\\+|\\-)?(d|f)?(\\d+\\.\\d+)$");
    QRegExp rxBool("^(true|false|True|False|TRUE|FALSE)$");
    QRegExp rxArray("^\\[(.*)\\]$");
    QRegExp rxHash("^(m|h)?\\{(((.*)\\s*\\:\\s*(.*,?))*)\\}$");
    if (rxStr.indexIn(tmpArg) == 0) {
        QString type = rxStr.cap(1);
        QString value = rxStr.cap(2);
        if (type == "b")
            return QVariant(this->unescapeStr(value).toUtf8());
        return QVariant(this->unescapeStr(value));
    } else if (rxInt.indexIn(tmpArg) == 0) {
        QString type = rxInt.cap(2);
        QString value = QString("%1%2").arg(rxInt.cap(1)).arg(rxInt.cap(3)).trimmed();
        if (type == "l")
            return QVariant(value.toLongLong());
        else if (type == "ul")
            return QVariant(value.toULongLong());
        else if (type == "ui")
            return QVariant(value.toUInt());
        return QVariant(value.toInt());
    } else if (rxDouble.indexIn(tmpArg) == 0) {
        QString type = rxDouble.cap(2);
        QString value = QString("%1%2").arg(rxDouble.cap(1)).arg(rxDouble.cap(3)).trimmed();
        if (type == "f")
            return QVariant(value.toFloat());
        return QVariant(value.toDouble());
    } else if (rxBool.indexIn(tmpArg) == 0) {
        bool tmpV = false;
        if (arg.toLower() == "true")
            tmpV = true;
        return QVariant(tmpV);
    } else if (rxArray.indexIn(tmpArg) == 0) {
        QVariantList vList;
        const QStringList arrayValues = prepareArrayValues(rxArray.cap(1).trimmed(), ',', true);
        for (const QString &v : arrayValues)
            vList.append(prepareHelpFunctionArg(v.trimmed()));
        return vList;
    } else if (rxHash.indexIn(tmpArg) == 0) {
        QString type = rxHash.cap(1);
        QVariantMap vMap;
        QVariantHash vHash;
        const QMap<QString, QString> map = prepareMapKeysValues(rxHash.cap(2).trimmed());
        QMapIterator<QString, QString> mIt (map);
        while (mIt.hasNext()) {
            mIt.next();
            QString hItKey = this->unescapeStr(mIt.key());
            if (type == "h")
                vHash.insert(hItKey, prepareHelpFunctionArg(mIt.value()));
            else
                vMap.insert(hItKey, prepareHelpFunctionArg(mIt.value()));
        }
        if (type == "h")
            return vHash;
        return vMap;
    }
    // check is param
    if (hasParam(tmpArg))
        return param(tmpArg);
    else if (hasTmpParam(tmpArg))
        return tmpParam(tmpArg);

    // not found -> invalid
    return QVariant();
}


// --- Private methods ---

//!
//! \brief Parse array
//! \param arrayStr String representation of array
//! \param delimiter Array values delimiter
//! \param addQuotes Append quotes in string if found
//! \return
//!
QStringList QtTIParserArgs::prepareArrayValues(const QString &arrayStr, const QChar &delimiter, const bool addQuotes)
{
    if (arrayStr.isEmpty())
        return QStringList();
    QStringList tmpArray;
    QString tmpValue;
    bool isEescaping = false;
    int openBrackets = 0;
    for (int i = 0; i < arrayStr.size(); i++) {
        const QChar ch = arrayStr[i];
        QChar chPrev;
        if (i > 0)
            chPrev = arrayStr[i - 1];

        if (ch == delimiter && !isEescaping && openBrackets == 0) {
            tmpArray << tmpValue.trimmed();
            tmpValue.clear();
            continue;
        }
        if (ch == "'" || ch == "\"") {
            if (chPrev == "\\") {
                tmpValue += ch;
                continue;
            }
            if (addQuotes)
                tmpValue += ch;
            isEescaping = !isEescaping;
            continue;
        }
        if ((ch == "[" && !isEescaping)
            || (ch == "{" && !isEescaping)) {
            openBrackets++;
            tmpValue += ch;
            continue;
        }
        if ((ch == "]" && !isEescaping)
            || (ch == "}" && !isEescaping)) {
            openBrackets--;
            tmpValue += ch;
            continue;
        }
        tmpValue += ch;
    }
    tmpArray << tmpValue.trimmed();
    return tmpArray;
}

//!
//! \brief Parse map
//! \param mapStr String representation of map
//! \param delimiter Map values delimiter
//! \return
//!
QMap<QString, QString> QtTIParserArgs::prepareMapKeysValues(const QString &mapStr, const QChar &delimiter)
{
    const QStringList tmpList = prepareArrayValues(mapStr, delimiter, true);
    QMap<QString, QString> tmpMap;
    for (const QString &v : tmpList) {
        QPair<QString, QString> keyValue = prepareMapKeyValue(v.trimmed());
        tmpMap.insert(keyValue.first, keyValue.second);
    }
    return tmpMap;
}

//!
//! \brief Parse map key-value pair
//! \param mapKeyValueStr String representation of map key-value pair
//! \param delimiter Map key-value pair delimiter
//! \return
//!
QPair<QString, QString> QtTIParserArgs::prepareMapKeyValue(const QString &mapKeyValueStr, const QString &delimiter)
{
    if (mapKeyValueStr.isEmpty())
        return QPair<QString, QString>();
    QString tmpKey;
    QString tmpValue;
    int index = -1;
    while (true) {
        index = mapKeyValueStr.indexOf(delimiter, index + 1);
        if (index == -1)
            break;
        // check is not escaped
        bool isEescaping = false;
        const QString tmpStrPath = mapKeyValueStr.left(index);
        for (int i = 0; i < tmpStrPath.size(); i++) {
            const QChar ch = tmpStrPath[i];
            QChar chPrev;
            if (i > 0)
                chPrev = tmpStrPath[i - 1];

            if (ch == "'" || ch == "\"") {
                if (chPrev == "\\")
                    continue;
                isEescaping = !isEescaping;
                continue;
            }
        }
        if (!isEescaping) {
            tmpKey = tmpStrPath.trimmed();
            tmpValue = mapKeyValueStr.right(mapKeyValueStr.size() - index - delimiter.size()).trimmed();
            break;
        }
    }
    return QPair<QString, QString>(tmpKey, tmpValue);
}

//!
//! \brief Unescape string
//! \param value String value
//! \return
//!
QString QtTIParserArgs::unescapeStr(const QString &value)
{
    // string
    // check is escaped
    QRegExp rxEscape1("^\\'(.*)\\'$");
    QRegExp rxEscape2("^\\\"(.*)\\\"$");
    if (rxEscape1.indexIn(value) == 0)
        return rxEscape1.cap(1);
    else if (rxEscape2.indexIn(value) == 0)
        return rxEscape2.cap(1);

    return value;
}

//!
//! \brief Select parameter value (recursive)
//! \param key Parameter name
//! \param parent Parameter container
//! \return
//!
//! === Example
//!     {% set test_my_key = { a => 'hi', b => 'hellow', c => 123 } %}{{ image_path (test_my_key.a) }}{% unset test_my_key %}
//!
//!     Result: "hellow there: hi"
//!
//! === Example 2:
//!     a - class Test with public methods:
//!         Q_INVOKABLE int index() const { return 123; }
//!         Q_INVOKABLE int index(const int i) const { return 123 + i; }
//!
//!     {{ a.index() }}
//!     {{ a.index(1) }}
//!
//!     Result:
//!         a.index() = 123
//!         a.index(1) = 124
//!
QVariant QtTIParserArgs::paramValueRecursive(const QString &key, const QVariant &parent)
{
    if (key.isEmpty() || parent.isNull())
        return QVariant();
    QStringList tmpKeys = key.split(".");
    const QString tmpKey = tmpKeys[0];
    tmpKeys.removeAt(0);
    QVariant tmpValue;
    if (parent.type() == QVariant::Map) {
        tmpValue = parent.toMap().value(tmpKey, QVariant());
    } else if (parent.type() == QVariant::Hash) {
        tmpValue = parent.toHash().value(tmpKey, QVariant());
    } else if (parent.type() == QVariant::UserType) {
        QObject *objPtr = qvariant_cast<QObject*>(parent);
        if (objPtr) {
            const QMetaObject *mObj = objPtr->metaObject();
            if (!mObj)
                return QVariant();
            QRegExp rxFunc("^(\\s*([\\w]+)\\s*\\(\\s*([A-Za-z0-9_\\ \\+\\-\\*\\,\\.\\'\\\"\\{\\}\\[\\]\\(\\)\\:\\/\\^\\$\\\\\\@\\#\\!\\<\\>\\=\\&\\%\\|\\;\\~]*)\\s*\\)\\s*)");
            if (rxFunc.indexIn(key) != -1) {
                // eval needed method
                QString funcName = rxFunc.cap(2).trimmed();
                QVariantList funcArgs = parseHelpFunctionArgs(rxFunc.cap(3).trimmed());
                tmpValue = evalParamMethod(objPtr, mObj, funcName, funcArgs);
            } else {
                // search needed property
                for (int i = mObj->propertyOffset(); i < mObj->propertyCount(); ++i) {
                    const QMetaProperty mProp = mObj->property(i);
                    if (mProp.name() == key) {
                        if (!mProp.isReadable()) {
                            qWarning() << qPrintable(QString("[QtTIParserArgs][paramValueRecursive] Class property '%1::%2' is not readable!")
                                                     .arg(mObj->className())
                                                     .arg(key));
                            return QVariant();
                        }
                        tmpValue = mProp.read(objPtr);
                        break;
                    }
                }
            }
        } else {
            const QString t = parent.typeName(); // user class name
            const int tId = QMetaType::type(t.toStdString().c_str()); // id of the derived class
            const QMetaObject *mObj = QMetaType::metaObjectForType(tId);
            if (!mObj)
                return QVariant();

            QRegExp rxFunc("^(\\s*([\\w]+)\\s*\\(\\s*([A-Za-z0-9_\\ \\+\\-\\*\\,\\.\\'\\\"\\{\\}\\[\\]\\(\\)\\:\\/\\^\\$\\\\\\@\\#\\!\\<\\>\\=\\&\\%\\|\\;\\~]*)\\s*\\)\\s*)");
            if (rxFunc.indexIn(key) != -1) {
                // eval needed method
                QString funcName = rxFunc.cap(2).trimmed();
                QVariantList funcArgs = parseHelpFunctionArgs(rxFunc.cap(3).trimmed());
                tmpValue = evalParamMethod(const_cast<void*>(parent.data()), mObj, funcName, funcArgs);
            } else {
                // search needed property
                for (int i = mObj->propertyOffset(); i < mObj->propertyCount(); ++i) {
                    const QMetaProperty mProp = mObj->property(i);
                    if (mProp.name() == key) {
                        if (!mProp.isReadable()) {
                            qWarning() << qPrintable(QString("[QtTIParserArgs][paramValueRecursive] Class property '%1::%2' is not readable!")
                                                     .arg(mObj->className())
                                                     .arg(key));
                            return QVariant();
                        }
                        tmpValue = mProp.readOnGadget(parent.data());
                        break;
                    }
                }
            }
        }
    }

    if (tmpKeys.isEmpty())
        return tmpValue;
    return paramValueRecursive(tmpKeys.join("."), tmpValue);
}

QVariant QtTIParserArgs::evalParamMethod(QObject *object, const QMetaObject *mObj, const QString &funcName, const QVariantList &funcArgs)
{
    QString funcArgsTypes = QtTIAbstractHelperFunction::typesToStr(QtTIAbstractHelperFunction::vListArgsTypes(funcArgs));
    QByteArray mNorSignature = QMetaObject::normalizedSignature(QString("%1(%2)").arg(funcName).arg(funcArgsTypes).toStdString().c_str());
    int mIndex = mObj->indexOfMethod(mNorSignature);
    if (mIndex == -1) {
        qWarning() << qPrintable(QString("[QtTIParserArgs][evalParamMethod] Class method '%1::%2(%3)' not found!")
                                 .arg(mObj->className())
                                 .arg(funcName)
                                 .arg(funcArgsTypes));
        return QVariant();
    }
    QMetaMethod mMethod = mObj->method(mIndex);
    if (mMethod.access() != QMetaMethod::Public) {
        qWarning() << qPrintable(QString("[QtTIParserArgs][evalParamMethod] Class method '%1::%2(%3)' is not public!")
                                 .arg(mObj->className())
                                 .arg(funcName)
                                 .arg(funcArgsTypes));
        return QVariant();
    }

    QList<QByteArray> mTypes = mMethod.parameterTypes();
    if (mTypes.size() != funcArgs.size()) {
        qWarning() << qPrintable(QString("[QtTIParserArgs][evalParamMethod] Invalid arguments count for class method '%1::%2(%3)'!")
                                 .arg(mObj->className())
                                 .arg(funcName)
                                 .arg(funcArgsTypes));
        return QVariant();
    }

    QVariantList funcArgsConverted;
    for (int i = 0; i < mTypes.size(); i++) {
        const QVariant& arg = funcArgs.at(i);

        QByteArray methodTypeName = mTypes.at(i);
        QByteArray argTypeName = arg.typeName();

        QVariant::Type methodType = QVariant::nameToType(methodTypeName);
        QVariant copy = QVariant(arg);

        if (copy.type() != methodType) {
            if (!copy.canConvert(methodType)
                || !copy.convert(methodType)) {
                qWarning() << qPrintable(QString("[QtTIParserArgs][evalParamMethod] Convert argument at index '%1' for class method '%2::%3(%4)' failed!")
                                         .arg(i)
                                         .arg(mObj->className())
                                         .arg(funcName)
                                         .arg(funcArgsTypes));
                return QVariant();
            }
        }
        funcArgsConverted << copy;
    }

    QList<QGenericArgument> funcArgsGeneric;
    for (int i = 0; i < funcArgsConverted.size(); i++) {
        QVariant& argument = funcArgsConverted[i];

        // A const_cast is needed because calling data() would detach
        // the QVariant.
        QGenericArgument genericArgument(QMetaType::typeName(argument.userType()),
                                         const_cast<void*>(argument.constData()));

        funcArgsGeneric << genericArgument;
    }

    QVariant returnValue(QMetaType::type(mMethod.typeName()), static_cast<void*>(NULL));
    QGenericReturnArgument returnArgument(mMethod.typeName(), const_cast<void*>(returnValue.constData()));
    bool ok = mMethod.invoke(object,
                             Qt::DirectConnection,
                             returnArgument,
                             funcArgsGeneric.value(0),
                             funcArgsGeneric.value(1),
                             funcArgsGeneric.value(2),
                             funcArgsGeneric.value(3),
                             funcArgsGeneric.value(4),
                             funcArgsGeneric.value(5),
                             funcArgsGeneric.value(6),
                             funcArgsGeneric.value(7),
                             funcArgsGeneric.value(8),
                             funcArgsGeneric.value(9));

    if (!ok) {
        qWarning() << qPrintable(QString("[QtTIParserArgs][paramValueRecursive] Call class method '%2::%3(%4)' failed!")
                                 .arg(mObj->className())
                                 .arg(funcName)
                                 .arg(funcArgsTypes));
        return QVariant();
    }
    return returnValue;
}

QVariant QtTIParserArgs::evalParamMethod(void *object, const QMetaObject *mObj, const QString &funcName, const QVariantList &funcArgs)
{
    QString funcArgsTypes = QtTIAbstractHelperFunction::typesToStr(QtTIAbstractHelperFunction::vListArgsTypes(funcArgs));
    QByteArray mNorSignature = QMetaObject::normalizedSignature(QString("%1(%2)").arg(funcName).arg(funcArgsTypes).toStdString().c_str());
    int mIndex = mObj->indexOfMethod(mNorSignature);
    if (mIndex == -1) {
        qWarning() << qPrintable(QString("[QtTIParserArgs][evalParamMethod] Class method '%1::%2(%3)' not found!")
                                 .arg(mObj->className())
                                 .arg(funcName)
                                 .arg(funcArgsTypes));
        return QVariant();
    }
    QMetaMethod mMethod = mObj->method(mIndex);
    if (mMethod.access() != QMetaMethod::Public) {
        qWarning() << qPrintable(QString("[QtTIParserArgs][evalParamMethod] Class method '%1::%2(%3)' is not public!")
                                 .arg(mObj->className())
                                 .arg(funcName)
                                 .arg(funcArgsTypes));
        return QVariant();
    }

    QList<QByteArray> mTypes = mMethod.parameterTypes();
    if (mTypes.size() != funcArgs.size()) {
        qWarning() << qPrintable(QString("[QtTIParserArgs][evalParamMethod] Invalid arguments count for class method '%1::%2(%3)'!")
                                 .arg(mObj->className())
                                 .arg(funcName)
                                 .arg(funcArgsTypes));
        return QVariant();
    }

    QVariantList funcArgsConverted;
    for (int i = 0; i < mTypes.size(); i++) {
        const QVariant& arg = funcArgs.at(i);

        QByteArray methodTypeName = mTypes.at(i);
        QByteArray argTypeName = arg.typeName();

        QVariant::Type methodType = QVariant::nameToType(methodTypeName);
        QVariant copy = QVariant(arg);

        if (copy.type() != methodType) {
            if (!copy.canConvert(methodType)
                || !copy.convert(methodType)) {
                qWarning() << qPrintable(QString("[QtTIParserArgs][evalParamMethod] Convert argument at index '%1' for class method '%2::%3(%4)' failed!")
                                         .arg(i)
                                         .arg(mObj->className())
                                         .arg(funcName)
                                         .arg(funcArgsTypes));
                return QVariant();
            }
        }
        funcArgsConverted << copy;
    }

    QList<QGenericArgument> funcArgsGeneric;
    for (int i = 0; i < funcArgsConverted.size(); i++) {
        QVariant& argument = funcArgsConverted[i];

        // A const_cast is needed because calling data() would detach
        // the QVariant.
        QGenericArgument genericArgument(QMetaType::typeName(argument.userType()),
                                         const_cast<void*>(argument.constData()));

        funcArgsGeneric << genericArgument;
    }

    QVariant returnValue(QMetaType::type(mMethod.typeName()), static_cast<void*>(NULL));
    QGenericReturnArgument returnArgument(mMethod.typeName(), const_cast<void*>(returnValue.constData()));
    bool ok = mMethod.invokeOnGadget(object,
                                     returnArgument,
                                     funcArgsGeneric.value(0),
                                     funcArgsGeneric.value(1),
                                     funcArgsGeneric.value(2),
                                     funcArgsGeneric.value(3),
                                     funcArgsGeneric.value(4),
                                     funcArgsGeneric.value(5),
                                     funcArgsGeneric.value(6),
                                     funcArgsGeneric.value(7),
                                     funcArgsGeneric.value(8),
                                     funcArgsGeneric.value(9));

    if (!ok) {
        qWarning() << qPrintable(QString("[QtTIParserArgs][paramValueRecursive] Call class method '%2::%3(%4)' failed!")
                                 .arg(mObj->className())
                                 .arg(funcName)
                                 .arg(funcArgsTypes));
        return QVariant();
    }
    return returnValue;
}
