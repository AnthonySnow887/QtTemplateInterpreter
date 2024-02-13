#include "QtTIParserFunc.h"

#include <QJsonDocument>
#include <QDateTime>
#include <QRegExp>

QtTIParserFunc::QtTIParserFunc()
{
    // add default functions

    //
    // Check object is Null.
    //
    // [bool] is_null (object)
    //
    // === Supported object: All
    //
    appendHelpFunction(new QtTIHelperFunction<QVariant>("is_null", [](const QVariant &d) {
        return d.isNull();
    }));

    //
    // Check object is Empty.
    //
    // [bool] is_empty (object)
    //
    // === Supported object:
    //      - QByteArray
    //      - QString
    //      - QStringList
    //      - QVariantList
    //      - QVariantMap
    //      - QVariantHash
    //
    appendHelpFunction(new QtTIHelperFunction<QByteArray>("is_empty", [](const QByteArray &d) {
        return d.isEmpty();
    }));
    appendHelpFunction(new QtTIHelperFunction<QString>("is_empty", [](const QString &d) {
        return d.isEmpty();
    }));
    appendHelpFunction(new QtTIHelperFunction<QStringList>("is_empty", [](const QStringList &d) {
        return d.isEmpty();
    }));
    appendHelpFunction(new QtTIHelperFunction<QVariantList>("is_empty", [](const QVariantList &d) {
        return d.isEmpty();
    }));
    appendHelpFunction(new QtTIHelperFunction<QVariantMap>("is_empty", [](const QVariantMap &d) {
        return d.isEmpty();
    }));
    appendHelpFunction(new QtTIHelperFunction<QVariantHash>("is_empty", [](const QVariantHash &d) {
        return d.isEmpty();
    }));

    //
    // Get object size.
    //
    // [int] size (object)
    //
    // === Supported object:
    //      - QByteArray
    //      - QString
    //      - QStringList
    //      - QVariantList
    //      - QVariantMap
    //      - QVariantHash
    //
    appendHelpFunction(new QtTIHelperFunction<QByteArray>("size", [](const QByteArray &d) {
        return d.size();
    }));
    appendHelpFunction(new QtTIHelperFunction<QString>("size", [](const QString &d) {
        return d.size();
    }));
    appendHelpFunction(new QtTIHelperFunction<QStringList>("size", [](const QStringList &d) {
        return d.size();
    }));
    appendHelpFunction(new QtTIHelperFunction<QVariantList>("size", [](const QVariantList &d) {
        return d.size();
    }));
    appendHelpFunction(new QtTIHelperFunction<QVariantMap>("size", [](const QVariantMap &d) {
        return d.size();
    }));
    appendHelpFunction(new QtTIHelperFunction<QVariantHash>("size", [](const QVariantHash &d) {
        return d.size();
    }));

    //
    // Check if object contains a key.
    //
    // [bool] contains (object, key)
    //
    // === Supported object:
    //      - QByteArray
    //      - QString
    //      - QStringList
    //      - QVariantList
    //      - QVariantMap
    //      - QVariantHash
    //
    appendHelpFunction(new QtTIHelperFunction<QByteArray,QByteArray>("contains", [](const QByteArray &d, const QByteArray &key) {
        return d.contains(key);
    }));
    appendHelpFunction(new QtTIHelperFunction<QString,QString>("contains", [](const QString &d, const QString &key) {
        return d.contains(key);
    }));
    appendHelpFunction(new QtTIHelperFunction<QStringList,QString>("contains", [](const QStringList &d, const QString &key) {
        return d.contains(key);
    }));
    appendHelpFunction(new QtTIHelperFunction<QVariantList,QString>("contains", [](const QVariantList &d, const QString &key) {
        return d.contains(key);
    }));
    appendHelpFunction(new QtTIHelperFunction<QVariantMap,QString>("contains", [](const QVariantMap &d, const QString &key) {
        return d.contains(key);
    }));
    appendHelpFunction(new QtTIHelperFunction<QVariantHash,QString>("contains", [](const QVariantHash &d, const QString &key) {
        return d.contains(key);
    }));

    //
    // Check if object contains a regular expression.
    //
    // [bool] reg_exp_match (object, reg_exp)
    //
    // === Supported object:
    //      - QByteArray
    //      - QString
    //
    appendHelpFunction(new QtTIHelperFunction<QByteArray,QString>("reg_exp_match", [](const QByteArray &d, const QString &reg_exp) {
        QRegExp rx(reg_exp);
        return rx.exactMatch(QString::fromUtf8(d));
    }));
    appendHelpFunction(new QtTIHelperFunction<QString,QString>("reg_exp_match", [](const QString &d, const QString &reg_exp) {
        QRegExp rx(reg_exp);
        return rx.exactMatch(d);
    }));

    //
    // Check if object starts with a substring.
    //
    // [bool] starts_with (object, str)
    //
    // === Supported object:
    //      - QByteArray
    //      - QString
    //
    appendHelpFunction(new QtTIHelperFunction<QByteArray,QByteArray>("starts_with", [](const QByteArray &d, const QByteArray &str) {
        return d.startsWith(str);
    }));
    appendHelpFunction(new QtTIHelperFunction<QString,QString>("starts_with", [](const QString &d, const QString &str) {
        return d.startsWith(str);
    }));

    //
    // Check if object ends with a substring.
    //
    // [bool] ends_with (object, str)
    //
    // === Supported object:
    //      - QByteArray
    //      - QString
    //
    appendHelpFunction(new QtTIHelperFunction<QByteArray,QByteArray>("ends_with", [](const QByteArray &d, const QByteArray &str) {
        return d.endsWith(str);
    }));
    appendHelpFunction(new QtTIHelperFunction<QString,QString>("ends_with", [](const QString &d, const QString &str) {
        return d.endsWith(str);
    }));

    //
    // Get index position of the first occurrence of the string 'str' in object.
    //
    // [int] index_of (object, str)
    //
    // === Supported object:
    //      - QByteArray
    //      - QString
    //
    appendHelpFunction(new QtTIHelperFunction<QByteArray,QByteArray>("index_of", [](const QByteArray &d, const QByteArray &str) {
        return d.indexOf(str);
    }));
    appendHelpFunction(new QtTIHelperFunction<QString,QString>("index_of", [](const QString &d, const QString &str) {
        return d.indexOf(str);
    }));

    //
    // Get index position of the last occurrence of the string 'str' in object.
    //
    // [int] last_index_of (object, str)
    //
    // === Supported object:
    //      - QByteArray
    //      - QString
    //
    appendHelpFunction(new QtTIHelperFunction<QByteArray,QByteArray>("last_index_of", [](const QByteArray &d, const QByteArray &str) {
        return d.lastIndexOf(str);
    }));
    appendHelpFunction(new QtTIHelperFunction<QString,QString>("last_index_of", [](const QString &d, const QString &str) {
        return d.lastIndexOf(str);
    }));

    //
    // Get substring that contains the 'size' leftmost characters of the object.
    //
    // [object] left (object, size)
    //
    // === Supported object:
    //      - QByteArray
    //      - QString
    //
    appendHelpFunction(new QtTIHelperFunction<QByteArray,int>("left", [](const QByteArray &d, const int &size) {
        return d.left(size);
    }));
    appendHelpFunction(new QtTIHelperFunction<QString,int>("left", [](const QString &d, const int &size) {
        return d.left(size);
    }));

    //
    // Get substring that contains the 'size' rightmost characters of the object.
    //
    // [object] right (object, size)
    //
    // === Supported object:
    //      - QByteArray
    //      - QString
    //
    appendHelpFunction(new QtTIHelperFunction<QByteArray,int>("right", [](const QByteArray &d, const int &size) {
        return d.right(size);
    }));
    appendHelpFunction(new QtTIHelperFunction<QString,int>("right", [](const QString &d, const int &size) {
        return d.right(size);
    }));

    //
    // Get list containing all the keys in the object.
    //
    // [QList<QString>] keys (object)
    //
    // === Supported object:
    //      - QVariantMap
    //      - QVariantHash
    //
    appendHelpFunction(new QtTIHelperFunction<QVariantMap>("keys", [](const QVariantMap &d) {
        return QVariant(d.keys());
    }));
    appendHelpFunction(new QtTIHelperFunction<QVariantHash>("keys", [](const QVariantHash &d) {
        return QVariant(d.keys());
    }));

    //
    // Get list containing all the values in the object.
    //
    // [QList<QVariant>] values (object)
    //
    // === Supported object:
    //      - QVariantMap
    //      - QVariantHash
    //
    appendHelpFunction(new QtTIHelperFunction<QVariantMap>("values", [](const QVariantMap &d) {
        return QVariant(d.values());
    }));
    appendHelpFunction(new QtTIHelperFunction<QVariantHash>("values", [](const QVariantHash &d) {
        return QVariant(d.values());
    }));

    //
    // Get value associated with the key.
    //
    // [QVariant] value (object, key)
    //
    // === Supported object:
    //      - QVariantMap
    //      - QVariantHash
    //
    appendHelpFunction(new QtTIHelperFunction<QVariantMap,QString>("value", [](const QVariantMap &d, const QString &key) {
        return d.value(key, QVariant());
    }));
    appendHelpFunction(new QtTIHelperFunction<QVariantHash,QString>("value", [](const QVariantHash &d, const QString &key) {
        return d.value(key, QVariant());
    }));

    //
    // Get item at index position 'pos' in the object.
    //
    // [QVariant] value_at (object, pos)
    //
    // === Supported object:
    //      - QByteArray
    //      - QString
    //      - QStringList
    //      - QVariantList
    //
    appendHelpFunction(new QtTIHelperFunction<QByteArray,int>("value_at", [](const QByteArray &d, const int &pos) {
        if (pos < 0 || pos >= d.size())
            return std::make_tuple(false, QChar(), "Invalid index!");
        return std::make_tuple(true, QChar::fromLatin1(d[pos]), "");
    }));
    appendHelpFunction(new QtTIHelperFunction<QString,int>("value_at", [](const QString &d, const int &pos) {
        if (pos < 0 || pos >= d.size())
            return std::make_tuple(false, QChar(), "Invalid index!");
        return std::make_tuple(true, d[pos], "");
    }));
    appendHelpFunction(new QtTIHelperFunction<QStringList,int>("value_at", [](const QStringList &d, const int &pos) {
        if (pos < 0 || pos >= d.size())
            return std::make_tuple(false, QString(), "Invalid index!");
        return std::make_tuple(true, d[pos], "");
    }));
    appendHelpFunction(new QtTIHelperFunction<QVariantList,int>("value_at", [](const QVariantList &d, const int &pos) {
        if (pos < 0 || pos >= d.size())
            return std::make_tuple(false, QVariant(), "Invalid index!");
        return std::make_tuple(true, d[pos], "");
    }));

    //
    // Get string representation of object.
    //
    // [QString] to_str (object)
    //
    // === Supported object:
    //      - QByteArray
    //      - QString
    //      - QStringList
    //      - QVariantList
    //      - QVariantMap
    //      - QVariantHash
    //
    appendHelpFunction(new QtTIHelperFunction<QByteArray>("to_str", [](const QByteArray &d) {
        return QString::fromUtf8(d);
    }));
    appendHelpFunction(new QtTIHelperFunction<QString>("to_str", [](const QString &d) {
        return d;
    }));
    appendHelpFunction(new QtTIHelperFunction<QVariant>("to_str", [](const QVariant &d) {
        QJsonDocument jDoc = QJsonDocument::fromVariant(d);
        return QString::fromUtf8(jDoc.toJson(QJsonDocument::Compact));
    }));

    //
    // Get reference to the first item in the object.
    //
    // [QVariant] first (object)
    //
    // === Supported object:
    //      - QStringList
    //      - QVariantList
    //      - QVariantMap
    //      - QVariantHash
    //
    appendHelpFunction(new QtTIHelperFunction<QStringList>("first", [](const QStringList &d) {
        if (d.isEmpty())
            return QString();
        return d.first();
    }));
    appendHelpFunction(new QtTIHelperFunction<QVariantList>("first", [](const QVariantList &d) {
        if (d.isEmpty())
            return QVariant();
        return d.first();
    }));
    appendHelpFunction(new QtTIHelperFunction<QVariantMap>("first", [](const QVariantMap &d) {
        if (d.isEmpty())
            return QVariant();
        return d.first();
    }));
    appendHelpFunction(new QtTIHelperFunction<QVariantHash>("first", [](const QVariantHash &d) {
        if (d.isEmpty())
            return QVariant();
        return d.values().first();
    }));

    //
    // Get reference to the last item in the object.
    //
    // [QVariant] last (object)
    //
    // === Supported object:
    //      - QStringList
    //      - QVariantList
    //      - QVariantMap
    //      - QVariantHash
    //
    appendHelpFunction(new QtTIHelperFunction<QStringList>("last", [](const QStringList &d) {
        if (d.isEmpty())
            return QString();
        return d.last();
    }));
    appendHelpFunction(new QtTIHelperFunction<QVariantList>("last", [](const QVariantList &d) {
        if (d.isEmpty())
            return QVariant();
        return d.last();
    }));
    appendHelpFunction(new QtTIHelperFunction<QVariantMap>("last", [](const QVariantMap &d) {
        if (d.isEmpty())
            return QVariant();
        return d.last();
    }));
    appendHelpFunction(new QtTIHelperFunction<QVariantHash>("last", [](const QVariantHash &d) {
        if (d.isEmpty())
            return QVariant();
        return d.values().last();
    }));

    //
    // Get reference to the first key item in the object.
    //
    // [QString] first_key (object)
    //
    // === Supported object:
    //      - QVariantMap
    //      - QVariantHash
    //
    appendHelpFunction(new QtTIHelperFunction<QVariantMap>("first_key", [](const QVariantMap &d) {
        if (d.isEmpty())
            return QString();
        return d.keys().first();
    }));
    appendHelpFunction(new QtTIHelperFunction<QVariantHash>("first_key", [](const QVariantHash &d) {
        if (d.isEmpty())
            return QString();
        return d.keys().first();
    }));

    //
    // Get reference to the last key item in the object.
    //
    // [QString] last_key (object)
    //
    // === Supported object:
    //      - QVariantMap
    //      - QVariantHash
    //
    appendHelpFunction(new QtTIHelperFunction<QVariantMap>("last_key", [](const QVariantMap &d) {
        if (d.isEmpty())
            return QString();
        return d.keys().last();
    }));
    appendHelpFunction(new QtTIHelperFunction<QVariantHash>("last_key", [](const QVariantHash &d) {
        if (d.isEmpty())
            return QString();
        return d.keys().last();
    }));

    //
    // Get object that has whitespace removed from the start and the end.
    //
    // [object] trim (object)
    //
    // === Supported object:
    //      - QByteArray
    //      - QString
    //
    appendHelpFunction(new QtTIHelperFunction<QByteArray>("trim", [](const QByteArray &d) {
        return d.trimmed();
    }));
    appendHelpFunction(new QtTIHelperFunction<QString>("trim", [](const QString &d) {
        return d.trimmed();
    }));

    //
    // Splits object into sub-objects wherever 'sep' occurs, and returns the list of those objects.
    //
    // [QList<QVariant>] split (object, sep)
    //
    // === Supported object:
    //      - QByteArray
    //      - QString
    //
    appendHelpFunction(new QtTIHelperFunction<QByteArray,QString>("split", [](const QByteArray &d, const QString &sep) {
        if (sep.isEmpty() || sep.size() > 1)
            return std::make_tuple(false, QVariant(), "Invalid separator!");
        return std::make_tuple(true, QVariant::fromValue(d.split(sep[0].toLatin1())), "");
    }));
    appendHelpFunction(new QtTIHelperFunction<QString,QString>("split", [](const QString &d, const QString &sep) {
        if (sep.isEmpty())
            return std::make_tuple(false, QVariant(), "Invalid separator!");
        return std::make_tuple(true, QVariant::fromValue(d.split(sep)), "");
    }));

    //
    // Get lowercase copy of the object.
    //
    // [object] to_lower (object)
    //
    // === Supported object:
    //      - QByteArray
    //      - QString
    //
    appendHelpFunction(new QtTIHelperFunction<QByteArray>("to_lower", [](const QByteArray &d) {
        return d.toLower();
    }));
    appendHelpFunction(new QtTIHelperFunction<QString>("to_lower", [](const QString &d) {
        return d.toLower();
    }));

    //
    // Get uppercase copy of the object.
    //
    // [object] to_upper (object)
    //
    // === Supported object:
    //      - QByteArray
    //      - QString
    //
    appendHelpFunction(new QtTIHelperFunction<QByteArray>("to_upper", [](const QByteArray &d) {
        return d.toUpper();
    }));
    appendHelpFunction(new QtTIHelperFunction<QString>("to_upper", [](const QString &d) {
        return d.toUpper();
    }));

    //
    // Joins all the objects list's items into a single object with each element separated by the given separator
    //
    // [QString] join (object, sep)
    //
    // === Supported object:
    //      - QByteArrayList
    //      - QStringList
    //
    appendHelpFunction(new QtTIHelperFunction<QByteArrayList,QByteArray>("join", [](const QByteArrayList &d, const QByteArray &sep) {
        if (sep.isEmpty())
            return std::make_tuple(false, QByteArray(), "Invalid separator!");
        return std::make_tuple(true, d.join(sep), "");
    }));
    appendHelpFunction(new QtTIHelperFunction<QStringList,QString>("join", [](const QStringList &d, const QString &sep) {
        if (sep.isEmpty())
            return std::make_tuple(false, QString(), "Invalid separator!");
        return std::make_tuple(true, d.join(sep), "");
    }));

    //
    // Get string representation of UTC datetime format.
    //
    // [QString] from_utc_time (UTC time, str_format)
    //
    // === Supported UTC time:
    //      - int
    //      - uint
    //
    appendHelpFunction(new QtTIHelperFunction<int,QString>("from_utc_time", [](const int &d, const QString &str_format) {
        if (str_format.isEmpty())
            return std::make_tuple(false, QString(), "Invalid string format!");
        return std::make_tuple(true, QDateTime::fromTime_t(d).toString(str_format), "");
    }));
    appendHelpFunction(new QtTIHelperFunction<uint,QString>("from_utc_time", [](const uint &d, const QString &str_format) {
        if (str_format.isEmpty())
            return std::make_tuple(false, QString(), "Invalid string format!");
        return std::make_tuple(true, QDateTime::fromTime_t(d).toString(str_format), "");
    }));

    //
    // Replaces every occurrence of the object before with the object after and returns a reference to this object.
    //
    // [object] replace (object, before, after)
    //
    // === Supported object:
    //      - QByteArray
    //      - QString
    //      - QStringList
    //      - QVariantList
    //      - QVariantMap
    //      - QVariantHash
    //
    appendHelpFunction(new QtTIHelperFunction<QByteArray,QByteArray,QByteArray>("replace", [](const QByteArray &d, const QByteArray &before, const QByteArray &after) {
        if (before == after)
            return d;
        return QByteArray(d).replace(before, after);
    }));
    appendHelpFunction(new QtTIHelperFunction<QString,QString,QString>("replace", [](const QString &d, const QString &before, const QString &after) {
        return QString(d).replace(before, after);
    }));
    appendHelpFunction(new QtTIHelperFunction<QStringList,QString,QString>("replace", [](const QStringList &d, const QString &before, const QString &after) {
        if (before == after)
            return d;
        QStringList tmpLst(d);
        int pos = 0;
        while (true) {
            pos = tmpLst.indexOf(before, pos);
            if (pos < 0)
                return tmpLst;
            tmpLst.replace(pos, after);
        }
    }));
    appendHelpFunction(new QtTIHelperFunction<QVariantList,QVariant,QVariant>("replace", [](const QVariantList &d, const QVariant &before, const QVariant &after) {
        if (before == after)
            return d;
        QVariantList tmpLst(d);
        int pos = 0;
        while (true) {
            pos = tmpLst.indexOf(before, pos);
            if (pos < 0)
                return tmpLst;
            tmpLst.replace(pos, after);
        }
    }));
    appendHelpFunction(new QtTIHelperFunction<QVariantMap,QVariant,QVariant>("replace", [](const QVariantMap &d, const QVariant &before, const QVariant &after) {
        if (before == after)
            return d;
        QVariantMap tmpMap(d);
        while (true) {
            const QString key = tmpMap.key(before, QString());
            if (key.isNull())
                return tmpMap;
            tmpMap.remove(key);
            tmpMap.insert(key, after);
        }
    }));
    appendHelpFunction(new QtTIHelperFunction<QVariantHash,QVariant,QVariant>("replace", [](const QVariantHash &d, const QVariant &before, const QVariant &after) {
        if (before == after)
            return d;
        QVariantHash tmpMap(d);
        while (true) {
            const QString key = tmpMap.key(before, QString());
            if (key.isNull())
                return tmpMap;
            tmpMap.remove(key);
            tmpMap.insert(key, after);
        }
    }));

    //
    // Remove every occurrence of the 'str/value' and returns a reference to new object.
    //
    // [object] remove (object, str/value)
    //
    // === Supported object:
    //      - QByteArray
    //      - QString
    //      - QStringList
    //      - QVariantList
    //      - QVariantMap
    //      - QVariantHash
    //
    appendHelpFunction(new QtTIHelperFunction<QByteArray,QByteArray>("remove", [](const QByteArray &d, const QByteArray &str) {
        QByteArray tmpBa(d);
        int pos = 0;
        while ((pos = tmpBa.indexOf(str)) != -1) {
            tmpBa = tmpBa.remove(pos, str.size());
        }
        return tmpBa;
    }));
    appendHelpFunction(new QtTIHelperFunction<QString,QString>("remove", [](const QString &d, const QString &str) {
        return QString(d).remove(str);
    }));
    appendHelpFunction(new QtTIHelperFunction<QStringList,QString>("remove", [](const QStringList &d, const QString &value) {
        QStringList tmpLst(d);
        int pos = 0;
        while (true) {
            pos = tmpLst.indexOf(value, pos);
            if (pos < 0)
                return tmpLst;
            tmpLst.removeAt(pos);
        }
    }));
    appendHelpFunction(new QtTIHelperFunction<QVariantList,QVariant>("remove", [](const QVariantList &d, const QVariant &value) {
        QVariantList tmpLst(d);
        int pos = 0;
        while (true) {
            pos = tmpLst.indexOf(value, pos);
            if (pos < 0)
                return tmpLst;
            tmpLst.removeAt(pos);
        }
    }));
    appendHelpFunction(new QtTIHelperFunction<QVariantMap,QString>("remove", [](const QVariantMap &d, const QVariant &value) {
        QVariantMap tmpMap(d);
        while (true) {
            const QString key = tmpMap.key(value, QString());
            if (key.isNull())
                return tmpMap;
            tmpMap.remove(key);
        }
    }));
    appendHelpFunction(new QtTIHelperFunction<QVariantHash,QString>("remove", [](const QVariantHash &d, const QVariant &value) {
        QVariantHash tmpMap(d);
        while (true) {
            const QString key = tmpMap.key(value, QString());
            if (key.isNull())
                return tmpMap;
            tmpMap.remove(key);
        }
    }));

    //
    // Removes all the items that have the key 'key' from the object and return new object.
    //
    // [object] remove_key (object, key)
    //
    // === Supported object:
    //      - QVariantMap
    //      - QVariantHash
    //
    appendHelpFunction(new QtTIHelperFunction<QVariantMap,QString>("remove_key", [](const QVariantMap &d, const QString &key) {
        if (!d.contains(key))
            return d;
        QVariantMap tmpMap(d);
        tmpMap.remove(key);
        return tmpMap;
    }));
    appendHelpFunction(new QtTIHelperFunction<QVariantHash,QString>("remove_key", [](const QVariantHash &d, const QString &key) {
        if (!d.contains(key))
            return d;
        QVariantHash tmpMap(d);
        tmpMap.remove(key);
        return tmpMap;
    }));

    //
    // Inserts 'str/value' at the end of the object and return new object.
    //
    // [object] append (object, str/value)
    //
    // === Supported object:
    //      - QByteArray
    //      - QString
    //      - QStringList
    //      - QVariantList
    //
    appendHelpFunction(new QtTIHelperFunction<QByteArray,QByteArray>("append", [](const QByteArray &d, const QByteArray &str) {
        return QByteArray(d).append(str);
    }));
    appendHelpFunction(new QtTIHelperFunction<QString,QString>("append", [](const QString &d, const QString &str) {
        return QString(d).append(str);
    }));
    appendHelpFunction(new QtTIHelperFunction<QStringList,QString>("append", [](const QStringList &d, const QString &value) {
        QStringList tmpLst(d);
        tmpLst.append(value);
        return tmpLst;
    }));
    appendHelpFunction(new QtTIHelperFunction<QVariantList,QVariant>("append", [](const QVariantList &d, const QVariant &value) {
        QVariantList tmpLst(d);
        tmpLst.append(value);
        return tmpLst;
    }));

    //
    // Inserts a new item with the key 'key' and a value of 'value' and return new object.
    //
    // [object] append (object, key, value)
    //
    // === Supported object:
    //      - QVariantMap
    //      - QVariantHash
    //
    appendHelpFunction(new QtTIHelperFunction<QVariantMap,QString,QVariant>("append", [](const QVariantMap &d, const QString &key, const QVariant &value) {
        QVariantMap tmpMap(d);
        tmpMap.insert(key, value);
        return tmpMap;
    }));
    appendHelpFunction(new QtTIHelperFunction<QVariantHash,QString,QVariant>("append", [](const QVariantHash &d, const QString &key, const QVariant &value) {
        QVariantHash tmpHash(d);
        tmpHash.insert(key, value);
        return tmpHash;
    }));
}

QtTIParserFunc::~QtTIParserFunc()
{
    clearFunctions();
}

//!
//! \brief Append help function
//! \param func Function pointer
//!
//! === Example 1:
//!     QtTIParserFunc.appendHelpFunction(new QtTIHelperFunction<>("my_test_array", []() {
//!         return QStringList({ "1", "11", "111" });
//!     }));
//!
//! === Example 2:
//!     QtTIParserFunc.appendHelpFunction(new QtTIHelperFunction<QStringList>("size", [](const QStringList &lst) {
//!         return lst.size();
//!     }));
//!
//! === Example 3:
//!     QtTIParserFunc.appendHelpFunction(new QtTIHelperFunction<TestD*>("TestD_ptr_func", [](const TestD *d) {
//!         if (!d)
//!             return std::make_tuple(false/*isOk*/, QString("")/*res*/, "NULL_PTR"/*err*/);
//!
//!         return std::make_tuple(true/*isOk*/, d->name()/*res*/, ""/*err*/);
//!     }));
//!
void QtTIParserFunc::appendHelpFunction(const QtTIAbstractHelperFunction *func)
{
    Q_ASSERT_WITH_MSG(func != nullptr, "Helper function is Null!");
    Q_ASSERT_WITH_MSG(func->name().isEmpty() != true, "Helper function name is Empty!");
    if (_functions.contains(func->name())) {
        const QList<const QtTIAbstractHelperFunction *> funcLst = _functions.values(func->name());
        for (const QtTIAbstractHelperFunction *f : funcLst) {
            if (f->neededArgsTypes() == func->neededArgsTypes()) {
                qWarning() << qPrintable(QString("[QtTIParserFunc][appendHelpFunction] Help function already added (func: \"%1\")!")
                                         .arg(func->fullName()));
                delete func;
                return;
            }
        }
    }
    _functions.insert(func->name(), func);
}

//!
//! \brief Remove help function
//! \param funcName Function name
//! \param args Function arguments array
//!
//! === Example 1:
//!     QtTIParserFunc.removeHelpFunction("my_test_array", QVariantList());
//!
//! === Example 2:
//!     QtTIParserFunc.removeHelpFunction("size", QVariantList({ QStringList() }));
//!
//! === Example 3:
//!     QtTIParserFunc.removeHelpFunction("TestD_ptr_func", QVariantList({ QVariant::fromValue(new TestD()) }));
//!
void QtTIParserFunc::removeHelpFunction(const QString &funcName, const QVariantList &args)
{
    Q_ASSERT_WITH_MSG(funcName.isEmpty() != true, "Helper function name is Empty!");
    const QtTIAbstractHelperFunction *f = findHelpFunction(funcName, args, true);
    if (!f)
        return;
    _functions.remove(funcName, f);
    delete f;
}

//!
//! \brief Check has help function found
//! \param funcName Function name
//! \param args Function arguments
//! \return
//!
bool QtTIParserFunc::hasHelpFunction(const QString &funcName, const QVariantList &args, const bool strictArgs) const
{
    return (findHelpFunction(funcName, args, strictArgs) != nullptr);
}

//!
//! \brief Find help function by name and arguments
//! \param funcName Function name
//! \param args Function argumnets
//! \return
//!
const QtTIAbstractHelperFunction *QtTIParserFunc::findHelpFunction(const QString &funcName, const QVariantList &args, const bool strictArgs) const
{
    if (!_functions.contains(funcName))
        return nullptr;
    const QStringList argsTypes = QtTIAbstractHelperFunction::vListArgsTypes(args);
    const QList<const QtTIAbstractHelperFunction *> funcLst = _functions.values(funcName);
    for (const QtTIAbstractHelperFunction *f : funcLst) {
        if (f->neededArgsTypes() == argsTypes)
            return f;
        if (!strictArgs && f->canConvertArgsTypes(args))
            return f;
    }
    return nullptr;
}

//!
//! \brief Execute help function
//! \param funcName Function name
//! \param args Function arguments
//! \return
//!
std::tuple<bool, QVariant, QString> QtTIParserFunc::evalHelpFunction(const QString &funcName, const QVariantList &args, const bool strictArgs) const
{
    const QtTIAbstractHelperFunction *f = findHelpFunction(funcName, args, strictArgs);
    if (!f)
        return std::make_tuple(false,
                               QVariant(),
                               QString("Not found help function '%1 (%2)'")
                               .arg(funcName)
                               .arg(QtTIAbstractHelperFunction::typesToStr(QtTIAbstractHelperFunction::vListArgsTypes(args))));

    return f->evalFunction(args);
}

//!
//! \brief Delete all added functions
//!
void QtTIParserFunc::clearFunctions()
{
    qDeleteAll(_functions);
    _functions.clear();
}
