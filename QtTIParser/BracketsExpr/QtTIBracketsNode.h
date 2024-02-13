#ifndef QTTIBRACKETSNODE_H
#define QTTIBRACKETSNODE_H

#include <QString>
#include <QVariant>
#include <QList>
#include <QUuid>
#include <QCryptographicHash>
#include <functional>
#include <tuple>

#include "../QtTIParserArgs.h"

//!
//! \brief The QtTIBracketsNode class
//!
class QtTIBracketsNode
{
public:
    QtTIBracketsNode() = default;

    //!
    //! \brief QtTIBracketsNode
    //! \param parserArgs QtTIParserArgs object pointer
    //! \param callback Callback function for method 'QtTIBracketsNode::evalNodeBody(...)'
    //!
    explicit QtTIBracketsNode(QtTIParserArgs *parserArgs,
                              std::function<std::tuple<bool/*isOk*/,QVariant/*res*/,QString/*err*/>(const QString &nodeBody)> callback)
        : _uuid(QUuid::createUuid().toString().mid(1,36))
        , _uuidHash(QCryptographicHash::hash(_uuid.toUtf8(), QCryptographicHash::Sha1).toHex().constData())
        , _parserArgs(parserArgs)
        , _callback(callback)
    {}

    ~QtTIBracketsNode() {
        if (!isValid())
            return;
        for (const QtTIBracketsNode &n : qAsConst(_nestedExpr))
            _parserArgs->removeTmpParam(QString("bn_%1").arg(n.uuidHash()));
        _nestedExpr.clear();
        _parserArgs = nullptr;
    }

    QtTIBracketsNode(const QtTIBracketsNode& obj) {
        _uuid = obj.uuid();
        _uuidHash = obj.uuidHash();
        _parserArgs = obj.parserArgs();
        _callback = obj.callback();
        _body = obj.body();
        _nestedExpr = obj.nestedExpr();
    }

    QtTIBracketsNode& operator=(const QtTIBracketsNode& obj) {
        if (this == &obj)
            return *this;
        _uuid = obj.uuid();
        _uuidHash = obj.uuidHash();
        _parserArgs = obj.parserArgs();
        _callback = obj.callback();
        _body = obj.body();
        _nestedExpr = obj.nestedExpr();
        return *this;
    }

    //!
    //! \brief Get node uuid
    //! \return
    //!
    QString uuid() const {
        return _uuid;
    }

    //!
    //! \brief Get node uuid SHA1 hash
    //! \return
    //!
    QString uuidHash() const {
        return _uuidHash;
    }

    //!
    //! \brief Check is valid node object
    //! \return
    //!
    bool isValid() const {
        return (_parserArgs != nullptr
                && _callback != nullptr
                && !_uuid.isEmpty()
                && !_uuidHash.isEmpty());
    }

    //!
    //! \brief Append node body data
    //! \param body
    //!
    void appendBodyData(const QString &body) {
        _body += body;
    }

    //!
    //! \brief Append nested node with expression
    //! \param node
    //!
    void appendNestedExpr(const QtTIBracketsNode &node) {
        if (!node.isValid())
            return;
        _nestedExpr.append(node);
        appendBodyData(QString("bn_%1").arg(node.uuidHash()));
    }

    //!
    //! \brief Execute node body
    //! \return
    //!
    std::tuple<bool/*isOk*/,QVariant/*res*/,QString/*err*/> evalNodeBody() const {
        for (const QtTIBracketsNode &n : qAsConst(_nestedExpr)) {
            bool isOk = false;
            QVariant result;
            QString error;
            std::tie(isOk, result, error) = n.evalNodeBody();
            if (!isOk)
                return std::make_tuple(false, QVariant(), error);
            _parserArgs->appendTmpParam(QString("bn_%1").arg(n.uuidHash()), result);
        }
        return _callback(_body);
    }

    //!
    //! \brief Select correct node
    //! \return
    //!
    //! If the expression is of the form '(...expr...)', then the brackets can be discarded
    //! and the node containing '...expr...' can be returned.
    //!
    //! === Example
    //!     '(( a + aa.b * 2 ) / 3 + 10 - 1 + 2 ** 2)' is equal '( a + aa.b * 2 ) / 3 + 10 - 1 + 2 ** 2'
    //!
    QtTIBracketsNode &selectCorrectNode() {
        if (_nestedExpr.size() == 1
            && _body.trimmed() == QString("bn_%1").arg(_nestedExpr[0].uuidHash()))
            return _nestedExpr[0].selectCorrectNode();

        return *this;
    }

private:
    //!
    //! \brief Get QtTIParserArgs object pointer
    //! \return
    //!
    QtTIParserArgs *parserArgs() const { return _parserArgs; }

    //!
    //! \brief Get callback function for method 'QtTIBracketsNode::evalNodeBody(...)'
    //! \return
    //!
    std::function<std::tuple<bool/*isOk*/,QVariant/*res*/,QString/*err*/>(const QString &nodeBody)> callback() const { return _callback; }

    //!
    //! \brief Get node body
    //! \return
    //!
    QString body() const { return _body; }

    //!
    //! \brief Get nested nodes with expressions
    //! \return
    //!
    QList<QtTIBracketsNode> nestedExpr() const { return _nestedExpr; }

private:
    QString _uuid;                          //!< block uuid
    QString _uuidHash;                      //!< block uuid SHA1 hash
    QtTIParserArgs *_parserArgs {nullptr};  //!< QtTIParserArgs object pointer

    std::function<std::tuple<bool/*isOk*/,QVariant/*res*/,QString/*err*/>(const QString &nodeBody)> _callback {nullptr}; //!< callback function for method 'QtTIBracketsNode::evalNodeBody(...)'

    QString _body;                          //!< node body
    QList<QtTIBracketsNode> _nestedExpr;    //!< nested nodes with expressions
};

#endif // QTTIBRACKETSNODE_H
