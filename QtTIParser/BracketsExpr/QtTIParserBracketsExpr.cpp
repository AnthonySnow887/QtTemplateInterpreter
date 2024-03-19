#include "QtTIParserBracketsExpr.h"

//!
//! \brief Parse brackets expression
//! \param string Input string
//! \param validStringsBeforeBracket Allowed lines before bracket
//! \param parserArgs QtTIParserArgs object pointer
//! \param callback Callback function for method 'QtTIBracketsNode::evalNodeBody(...)'
//! \return
//!
QtTIBracketsNode QtTIParserBracketsExpr::parseBracketsExpression(const QString &string,
                                                                 const QStringList &validStringsBeforeBracket,
                                                                 QtTIParserArgs *parserArgs,
                                                                 std::function<std::tuple<bool, QVariant, QString> (const QString &)> callback)
{
    if (string.isEmpty())
        return QtTIBracketsNode();

    QList<QtTIBracketsNode> nodes({ QtTIBracketsNode(parserArgs, callback) });
    QString tmpValue;
    bool isEescaping = false;
    bool isSkipped = false;
    int openBrackets = 0;
    for (int i = 0; i < string.size(); i++) {
        const QChar ch = string[i];
        QChar chPrev;
        if (i > 0)
            chPrev = string[i - 1];

        if (ch == "'" || ch == "\"") {
            if (chPrev == "\\") {
                tmpValue += ch;
                continue;
            }
            tmpValue += ch;
            isEescaping = !isEescaping;
            continue;
        }
        if (ch == "(" && !isEescaping) {
            // check last string
            const QStringList lastStrings = tmpValue.trimmed().split(" ");
            const QString lastString = lastStrings.last();
            if (!tmpValue.trimmed().isEmpty() && !validStringsBeforeBracket.contains(lastString)) {
                tmpValue += ch;
                isSkipped = true;
                continue;
            }
            isSkipped = false;
            nodes[openBrackets].appendBodyData(tmpValue);
            nodes.append(QtTIBracketsNode(parserArgs, callback));
            openBrackets++;
            tmpValue.clear();
            continue;
        }
        if (ch == ")" && !isEescaping) {
            if (isSkipped) {
                tmpValue += ch;
                isSkipped = false;
                continue;
            }
            QtTIBracketsNode closedNode = nodes[openBrackets];
            closedNode.appendBodyData(tmpValue);
            nodes.removeAt(openBrackets);
            openBrackets--;
            nodes[openBrackets].appendNestedExpr(closedNode);
            tmpValue.clear();
            continue;
        }
        tmpValue += ch;
    }
    QtTIBracketsNode closedNode = nodes[openBrackets];
    closedNode.appendBodyData(tmpValue);
    nodes.removeAt(openBrackets);

    if (nodes.size() != 0)
        return QtTIBracketsNode();

    return closedNode.selectCorrectNode();
}
