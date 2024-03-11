#ifndef REGEXP_H
#define REGEXP_H

#include <QString>
#include <QStringList>
#include <QRegExp>
#include <QMetaType>

class RegExp
{
    Q_GADGET

public:
    RegExp() {}
    RegExp(const QString &pattern)
        : _rx(pattern)
    {}
    ~RegExp() = default;

    RegExp(const RegExp& obj) {
        _rx = obj._rx;
    }

    RegExp& operator=(const RegExp& obj) {
        if (this == &obj)
            return *this;
        _rx = obj._rx;
        return *this;
    }

    Q_INVOKABLE bool is_empty() const { return _rx.isEmpty(); }
    Q_INVOKABLE bool is_valid() const { return _rx.isValid(); }
    Q_INVOKABLE QString pattern() const { return _rx.pattern(); }
    Q_INVOKABLE void set_pattern(const QString &pattern) { _rx.setPattern(pattern); }

    Q_INVOKABLE bool is_minimal() const { return _rx.isMinimal(); }
    Q_INVOKABLE void set_minimal(bool minimal) { _rx.setMinimal(minimal); }

    Q_INVOKABLE bool exact_match(const QString &str) const { return _rx.exactMatch(str); }

    Q_INVOKABLE int index_in(const QString &str, int offset = 0) const { return _rx.indexIn(str, offset); }
    Q_INVOKABLE int last_index_in(const QString &str, int offset = -1) const { return _rx.lastIndexIn(str, offset); }
    Q_INVOKABLE int matched_length() const { return _rx.matchedLength(); }

#ifndef QT_NO_REGEXP_CAPTURE
    Q_INVOKABLE int capture_count() const { return _rx.captureCount(); }
    Q_INVOKABLE QStringList captured_texts() const { return _rx.capturedTexts(); }
    Q_INVOKABLE QString cap(int nth = 0) const { return _rx.cap(nth); }
    Q_INVOKABLE int pos(int nth = 0) const { return _rx.pos(nth); }
    Q_INVOKABLE QString error_string() const { return _rx.errorString(); }
#endif

    Q_INVOKABLE static QString escape(const QString &str) { return QRegExp::escape(str); }

private:
    QRegExp _rx;
};
Q_DECLARE_METATYPE(RegExp)

#endif // REGEXP_H
