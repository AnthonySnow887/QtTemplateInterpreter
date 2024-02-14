#include "QtTIParser.h"
#include "Math/QtTIParserMath.h"
#include "Logic/QtTIParserLogic.h"
#include "TernaryOperator/QtTIParserTernaryOperator.h"
#include "NullCoalescingOperator/QtTIParserNullCoalescingOperator.h"

QtTIParser::QtTIParser()
{
}

QtTIParser::~QtTIParser()
{
    _parserFunc.clearFunctions();
    _parserArgs.clearParams();
    _parserArgs.clearTmpParams();
}

//!
//! \brief Select parser args object pointer
//! \return
//!
QtTIParserArgs *QtTIParser::parserArgs()
{
    return &_parserArgs;
}

//!
//! \brief Select parser func object pointer
//! \return
//!
QtTIParserFunc *QtTIParser::parserFunc()
{
    return &_parserFunc;
}

//!
//! \brief Remove all comments in line
//! \param line Line data
//! \param isMultiline Check is multi-line comment
//! \return
//!
QString QtTIParser::removeComments(const QString &line, bool *isMultiline)
{
    bool isComment = false;
    if (isMultiline)
        isComment = *isMultiline;
    QString tmpLine;
    for (int i = 0; i < line.size(); i++) {
        const QChar ch = line[i];
        QChar chPrev;
        if (i > 0)
            chPrev = line[i - 1];
        QChar chNext;
        if (i < line.size() - 1)
            chNext = line[i + 1];

        // check start comment
        if (ch == '{' && chNext == '#') {
            isComment = true;
            continue;
        }
        // check end comment
        if (ch == '}' && chPrev == '#') {
            isComment = false;
            continue;
        }
        // check
        if (isComment)
            continue;
        // append char
        tmpLine += ch;
    }
    if (isMultiline)
        *isMultiline = isComment;
    return tmpLine;
}

//!
//! \brief Parse line data (parse help functions and help parameters)
//! \param line Line data
//! \param lineNum Line number
//! \return
//!
std::tuple<bool, QString, QString> QtTIParser::parseLine(const QString &line, const int lineNum)
{
    if (line.isEmpty())
        return std::make_tuple(true, line, "");
    bool isOk = false;
    QString newLine = line;
    QString error;

    // check functions
    std::tie(isOk, newLine, error) = parseHelpFunctions(newLine, lineNum);
    if (!isOk)
        return std::make_tuple(false, "", error);

    // check params
    std::tie(isOk, newLine, error) = parseHelpParams(newLine, lineNum);
    if (!isOk)
        return std::make_tuple(false, "", error);

    return std::make_tuple(true, newLine, "");
}


//!
//! \brief Parse help functions in line
//! \param line Line data
//! \param lineNum Line number
//! \return
//!
//! === Example
//!     ![This is Lu and Bryu!]( {{ image_path ('configure.svg') }} "Lu and Bryu")
//!
//! === Example 2:
//!     ![This is Lu and Bryu!]( {{ image_path_b (b'configure.svg') }} \"Lu and Bryu\") --- {{ image_path_2 ([ui1, ul2, l3, 4, [5.2, -6]]) }}  {{ image_path_3 ({a:f1.2, b:2, c : +f3.5, ff : h{d : 4, e:-5}, f:-6.2}) }}
//!
std::tuple<bool, QString, QString> QtTIParser::parseHelpFunctions(const QString &line, const int lineNum)
{
    QString tmpLine = line;
    bool isOk = false;
    QString error;
    QMap<QString, QVariant> funcRes = parseAndExecHelpFunctions(line, lineNum, &isOk, error);
    if (!isOk)
        return std::make_tuple(false, "", error);

    QMapIterator<QString, QVariant> it(funcRes);
    while (it.hasNext()) {
        it.next();
        tmpLine = tmpLine.replace(it.key(), it.value().toString());
    }
    return std::make_tuple(true, tmpLine, "");
}

//!
//! \brief Parse and exec help functions in line
//! \param line Line data
//! \param lineNum Line number
//! \param[in,out] isOk Check is success
//! \param[in,out] error Error string
//! \return
//!
QMap<QString, QVariant> QtTIParser::parseAndExecHelpFunctions(const QString &line, const int lineNum, bool *isOk, QString &error)
{
    error.clear();
    QMap<QString, QVariant> tmpRes;
    QString tmpLine = line;
    QRegExp rxFunc("(\\{([\\{\\#])\\s*([\\w]+)\\s*\\(\\s*([A-Za-z0-9_\\ \\+\\-\\,\\.\\'\\\"\\{\\}\\[\\]\\:\\/]*)\\s*\\)\\s*([\\}\\#])\\})");
    int pos = 0;
    while ((pos = rxFunc.indexIn(line, pos)) != -1) {
        pos += rxFunc.matchedLength();
        if (rxFunc.captureCount() < 5)
            continue;
        QString replaceStr = rxFunc.cap(1);
        QString tagOpen = rxFunc.cap(2);
        QString helpFunc = rxFunc.cap(3);
        QVariantList helpFuncArgs = _parserArgs.parseHelpFunctionArgs(rxFunc.cap(4));
        QString tagClose = rxFunc.cap(5);
        // check
        if (tagOpen == "{" && tagClose != "}") {
            if (isOk)
                *isOk = false;
            error = QString("Invalid closed symbol (not '}') in line %1").arg(lineNum);
            return QMap<QString, QVariant>();
        }
        if (tagOpen == "#" && tagClose != "#") {
            if (isOk)
                *isOk = false;
            error = QString("Invalid closed symbol (not '#') in line %1").arg(lineNum);
            return QMap<QString, QVariant>();
        }
        if (!_parserFunc.hasHelpFunction(helpFunc, helpFuncArgs)) {
            if (isOk)
                *isOk = false;
            error = QString("Unsupported help function '%1 (%2)' in line %3")
                    .arg(helpFunc)
                    .arg(QtTIAbstractHelperFunction::typesToStr(QtTIAbstractHelperFunction::vListArgsTypes(helpFuncArgs)))
                    .arg(lineNum);
            return QMap<QString, QVariant>();
        }
        QVariant funcRes;
        if (tagOpen != "#") {
            bool isOkEval = false;
            QString err;
            std::tie(isOkEval, funcRes, err) = evalHelpFunction(helpFunc, helpFuncArgs);

            // check
            if (!isOkEval) {
                if (isOk)
                    *isOk = false;
                error = QString("Eval help function '%1 (%2)' in line %3 failed! Error: '%4'")
                        .arg(helpFunc)
                        .arg(QtTIAbstractHelperFunction::typesToStr(QtTIAbstractHelperFunction::vListArgsTypes(helpFuncArgs)))
                        .arg(lineNum)
                        .arg(err);
                return QMap<QString, QVariant>();
            }
        }
        tmpRes.insert(replaceStr, funcRes);
    }
    if (isOk)
        *isOk = true;
    return tmpRes;
}

//!
//! \brief Parse help parameters in line
//! \param line Line data
//! \param lineNum Line number
//! \return
//!
//! === Example
//!       Key: {{ my_key }}
//!     Key 2: {{ my_key_2 }}
//!     Key 3: {{ my_key3 }}
//!
//! === Variables:
//!     The application passes variables to the templates for manipulation in the template.
//!     Variables may have attributes or elements you can access, too.
//!     The visual representation of a variable depends heavily on the application providing it.
//!     Use a dot (.) to access attributes of a variable (properties of a object, or items of a array):
//!             {{ foo.bar }}
//!
//! === Math operators (only for numbers):
//!     The following operators are supported:
//!      + : Adds two numbers together (the operands are casted to numbers):
//!             {{ 1 + 1 }} is 2
//!
//!      - : Subtracts the second number from the first one:
//!             {{ 3 - 2 }} is 1
//!
//!      / : Divides two numbers. The returned value will be a floating point number:
//!             {{ 1 / 2 }} is {{ 0.5 }}
//!
//!      % : Calculates the remainder of an integer division:
//!             {{ 11 % 7 }} is 4
//!
//!     // : Divides two numbers and returns the floored integer result:
//!             {{ 20 // 7 }} is 2, {{ -20  // 7 }} is -3 (this is just syntactic sugar for the round filter)
//!
//!      * : Multiplies the left operand with the right one:
//!             {{ 2 * 2 }} would return 4
//!
//!     ** : Raises the left operand to the power of the right operand:
//!             {{ 2 ** 3 }} would return 8
//!
//! === Other operators:
//!     ?: : The ternary operator:
//!             {{ foo ? 'yes' : 'no' }}
//!             {{ foo ?: 'no' }} is the same as {{ foo ? foo : 'no' }}
//!             {{ foo ? 'yes' }} is the same as {{ foo ? 'yes' : '' }}
//!
//!     ?? : The null-coalescing operator:
//!             {# returns the value of foo if it is defined and not null, 'no' otherwise #}
//!             {{ foo ?? 'no' }}
//!
std::tuple<bool, QString, QString> QtTIParser::parseHelpParams(const QString &line, const int lineNum)
{
    QString tmpLine = line;
    bool isOk = false;
    QString error;
    QMap<QString, QVariant> funcRes = parseAndExecHelpParams(line, lineNum, &isOk, error);
    if (!isOk)
        return std::make_tuple(false, "", error);

    QMapIterator<QString, QVariant> it(funcRes);
    while (it.hasNext()) {
        it.next();
        tmpLine = tmpLine.replace(it.key(), it.value().toString());
    }
    return std::make_tuple(true, tmpLine, "");
}

//!
//! \brief Parse and exec help parameters in line
//! \param line Line data
//! \param lineNum Line number
//! \param[in,out] isOk Check is success
//! \param[in,out] error Error string
//! \return
//!
QMap<QString, QVariant> QtTIParser::parseAndExecHelpParams(const QString &line, const int lineNum, bool *isOk, QString &error)
{
    error.clear();
    QMap<QString, QVariant> tmpRes;
    QRegExp rx("(\\{([\\{\\#])\\s{0,}([\\w\\.\\,\\ \\+\\-\\/\\*\\%\\?\\:\\'\\\"\\(\\)\\[\\]\\{\\}\\<\\>\\=\\!\\&\\|]+)\\s{0,}([\\}\\#])\\})");
    QRegExp rxParam("\\s{0,}([\\w\\.]+)\\s{0,}");
    int pos = 0;
    while ((pos = rx.indexIn(line, pos)) != -1) {
        pos += rx.matchedLength();
        if (rx.captureCount() < 4)
            continue;
        QString replaceStr = rx.cap(1);
        QString tagOpen = rx.cap(2);

        // get valif expr
        replaceStr = validParamExpr(replaceStr, QString("%1}").arg((tagOpen == "{") ? "}" : "#" ));
        // re-calculate pos
        pos -= (rx.matchedLength() - replaceStr.size());
        // re-select regex data
        if (rx.indexIn(replaceStr, 0) == -1)
            continue;
        QString helpCondition = rx.cap(3).trimmed();
        QString tagClose = rx.cap(4);

        // check
        if (tagOpen == "{" && tagClose != "}") {
            if (isOk)
                *isOk = false;
            error = QString("Invalid closed symbol (not '}') in line %1").arg(lineNum);
            return QMap<QString, QVariant>();
        }
        if (tagOpen == "#" && tagClose != "#") {
            if (isOk)
                *isOk = false;
            error = QString("Invalid closed symbol (not '#') in line %1").arg(lineNum);
            return QMap<QString, QVariant>();
        }

        // check condition
        QVariant resultValue;
        if (tagOpen != "#" && QtTIParserTernaryOperator::isTernaryOperatorExpr(helpCondition)) {
            bool calcIsOk = false;
            QString calcErr;
            QVariant calcRes = QtTIParserTernaryOperator::parseTernaryOperator(helpCondition, &_parserArgs, &_parserFunc, &calcIsOk, calcErr);
            if (!calcIsOk) {
                if (isOk)
                    *isOk = false;
                error = QString("%1 in line %2").arg(calcErr).arg(lineNum);
                return QMap<QString, QVariant>();
            }
            resultValue = calcRes;

        } else if (tagOpen != "#" && QtTIParserNullCoalescingOperator::isNullCoalescingOperatorExpr(helpCondition)) {
            bool calcIsOk = false;
            QString calcErr;
            QVariant calcRes = QtTIParserNullCoalescingOperator::parseNullCoalescingOperator(helpCondition, &_parserArgs, &_parserFunc, &calcIsOk, calcErr);
            if (!calcIsOk) {
                if (isOk)
                    *isOk = false;
                error = QString("%1 in line %2").arg(calcErr).arg(lineNum);
                return QMap<QString, QVariant>();
            }
            resultValue = calcRes;

        } else if (tagOpen != "#" && QtTIParserLogic::isLogicExpr(helpCondition)) {
            bool calcIsOk = false;
            QString calcErr;
            QVariant calcRes = QtTIParserLogic::parseLogic(helpCondition, &_parserArgs, &_parserFunc, &calcIsOk, calcErr);
            if (!calcIsOk) {
                if (isOk)
                    *isOk = false;
                error = QString("%1 in line %2").arg(calcErr).arg(lineNum);
                return QMap<QString, QVariant>();
            }
            resultValue = calcRes;

        } else if (tagOpen != "#" && QtTIParserMath::isMathExpr(helpCondition)) {
            bool calcIsOk = false;
            QString calcErr;
            QVariant calcRes = QtTIParserMath::parseMath(helpCondition, &_parserArgs, &calcIsOk, calcErr);
            if (!calcIsOk) {
                if (isOk)
                    *isOk = false;
                error = QString("%1 in line %2").arg(calcErr).arg(lineNum);
                return QMap<QString, QVariant>();
            }
            resultValue = calcRes;

        } else if (tagOpen != "#" && rxParam.indexIn(helpCondition) != -1) {
            if (_parserArgs.hasParam(helpCondition)) {
                resultValue = evalHelpParam(helpCondition);
            } else if (_parserArgs.hasTmpParam(helpCondition)) {
                resultValue = evalHelpTmpParam(helpCondition);
            } else {
                if (isOk)
                    *isOk = false;
                error = QString("Unsupported help parameter '%1' in line %2").arg(helpCondition).arg(lineNum);
                return QMap<QString, QVariant>();
            }
        }
        tmpRes.insert(replaceStr, resultValue);
    }
    if (isOk)
        *isOk = true;
    return tmpRes;
}

//!
//! \brief Convert help parameter value to string
//! \param paramName Parameter name
//! \return
//!
QString QtTIParser::evalHelpParam(const QString &paramName)
{
    return _parserArgs.param(paramName).toString();
}

//!
//! \brief Convert help tmp parameter value to string
//! \param paramName Tmp parameter name
//! \return
//!
QString QtTIParser::evalHelpTmpParam(const QString &paramName)
{
    return _parserArgs.tmpParam(paramName).toString();
}

//!
//! \brief Execute help function
//! \param funcName Function name
//! \param args Function arguments
//! \return
//!
std::tuple<bool, QVariant, QString> QtTIParser::evalHelpFunction(const QString &funcName, const QVariantList &args) const
{
    return _parserFunc.evalHelpFunction(funcName, args);
}

//!
//! \brief Get valid parameter expression
//! \param str String for select
//! \param ending Substring that should end the original string
//! \return
//!
//! === Example
//!           Str: "{{ foo ?: 'no' }} is the same as {{ foo ? foo : 'no' }}"
//!        Ending: "}}"
//!        Result: "{{ foo ?: 'no' }}"
//!
QString QtTIParser::validParamExpr(const QString &str, const QString &ending) const
{
    if (str.isEmpty() || ending.isEmpty())
        return str;
    QString tmpValue;
    bool isEescaping = false;
    for (int i = 0; i < str.size(); i++) {
        if (tmpValue.endsWith(ending) && !isEescaping)
            return tmpValue;

        const QChar ch = str[i];
        QChar chPrev;
        if (i > 0)
            chPrev = str[i - 1];

        if (ch == "'" || ch == "\"") {
            if (chPrev == "\\") {
                tmpValue += ch;
                continue;
            }
            tmpValue += ch;
            isEescaping = !isEescaping;
            continue;
        }
        tmpValue += ch;
    }
    return str;
}
