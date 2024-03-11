# QtTemplateInterpreter

[![License](https://img.shields.io/github/license/AnthonySnow887/QtTemplateInterpreter)](https://github.com/AnthonySnow887/QtTemplateInterpreter/blob/master/LICENSE)
[![Latest Release](https://img.shields.io/github/v/release/AnthonySnow887/QtTemplateInterpreter?label=release)](https://github.com/AnthonySnow887/QtTemplateInterpreter/releases)
![Last Commit](https://img.shields.io/github/last-commit/AnthonySnow887/QtTemplateInterpreter/develop)

Text template interpreter library based on Qt 5.
This library is similar in its work and syntax for writing templates to the PHP template engine Twig (https://github.com/twigphp/Twig).

## Usage

### If you use qmake

To use the library, just copy it to the root of your project and include the "QtTemplateInterpreter/QtTemplateInterpreter.pri" file:

```cpp
include(QtTemplateInterpreter/QtTemplateInterpreter.pri)
```

> 
> Example for qmake:
> ```cpp
> include(QtTemplateInterpreter/QtTemplateInterpreter.pri)
> 
> QT += core
> QT -= gui
> 
> CONFIG += c++14
> QMAKE_CXXFLAGS += -std=c++14
> 
> TARGET = testTemplateInterpreter
> CONFIG += console
> CONFIG += warn_on
> CONFIG -= app_bundle
> 
> TEMPLATE = app
> 
> SOURCES += main.cpp
> 
> # The following define makes your compiler emit warnings if you use
> # any feature of Qt which as been marked deprecated (the exact warnings
> # depend on your compiler). Please consult the documentation of the
> # deprecated API in order to know how to port your code away from it.
> DEFINES += QT_DEPRECATED_WARNINGS
> 
> ```
> 

### If you use CMake

To use the library, just copy it to the root of your project and include the "QtTemplateInterpreter/CMakeLists.txt" file:

```cmake
add_subdirectory(QtTemplateInterpreter)

...

target_link_libraries(<target>
    QtTemplateInterpreter
    ...
    <item>
    ...)
```

> 
> Example for CMake:
> ```cmake
> cmake_minimum_required(VERSION 3.14)
> 
> project(testTemplateInterpreter LANGUAGES CXX)
> 
> set(CMAKE_INCLUDE_CURRENT_DIR ON)
> set(CMAKE_AUTOUIC ON)
> set(CMAKE_AUTOMOC ON)
> set(CMAKE_AUTORCC ON)
> set(CMAKE_CXX_STANDARD 14)
> set(CMAKE_CXX_STANDARD_REQUIRED ON)
> 
> find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
> find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)
> 
> add_subdirectory(QtTemplateInterpreter)
> 
> add_executable(testTemplateInterpreter
>   main.cpp)
> 
> target_link_libraries(testTemplateInterpreter
>     QtTemplateInterpreter
>     Qt${QT_VERSION_MAJOR}::Core)
> ```
> 

## Template writing syntax

A template is a regular text file. It can generate any text-based format (TXT, HTML, XML, CSV, LaTeX, etc.). It doesn't have a specific extension.

A template contains variables or expressions, which get replaced with values when the template is evaluated, and tags, which control the template's logic.

Below is a minimal template that illustrates a few basics. We will cover further details later on:

```twig
![This is Lu and Bryu!]( {{ image_path ('lu_and_bryu.png') }} \"Lu and Bryu\")

Lu images: 
{% for item in lu_images() %}
    "Lu - {{ item.title }}": {{ item.path }}
{% endfor %}

Bryu images: 
{% for item in bryu_images() %}
    "Bryu - {{ item.title }}": {{ item.path }}
{% endfor %}
```

There are two types of delimiters: ```{% ... %}``` and ```{{ ... }}```.
The first is used to perform various block operations, such as a for loop. The second one prints the result of the expression.

## Variables

All parameters inside the interpreter are stored in the QVariant class.

The interpreter passes variables to templates for further manipulation in the template. Variables can also have attributes or methods that you can access. To access attributes or methods of a variable, use a dot ``` . ```:

```twig
{{ foo.bar }}
```

>
> NOTE:
>
> If you need to call a variable method, its name must be followed by parentheses ``` () ```. Arguments may be included in parentheses if the method requires them.
> To be able to call methods on a variable, it must be located in the public section of the class, and the macro ```Q_INVOKABLE``` must be specified before the method.
>
> ```cpp
> #include <QMetaType>
> 
> class MyClass
> {
>   Q_GADGET
> 
> public:
>     MyClass() {}
>     ~MyClass() {}
> 
>     Q_INVOKABLE int func() const { return 123; }
>     Q_INVOKABLE int func_summ(const int a, const int b) const { return a + b; }
> 
> };
> Q_DECLARE_METATYPE(MyClass)
> ```
>
> ```twig
> {{ foo.func() }}
> {{ foo.func_summ(1, 2) }}
> ```
>

>
> NOTE:
>
> It's important to know that the curly braces are not part of the variable but the print statement. 
> When accessing variables inside tags, don't put the braces around them.
>

If the variable or attribute does not exist, NULL will be returned, which will be converted to the empty string when printed in the template.

>
> NOTE:
>
> If a pointer was used as a parameter, then after finishing work and clearing the registered parameters, the interpreter does not delete it!
>

>
> NOTE:
>
> Global variables must have unique names. Registering variables with the same name is prohibited and will result in an error.
>

### Registering variables in the interpreter

Variables are registered in the interpreter using the method ```appendHelpParam(const QString &paramName, const QVariant &value)```:

```cpp
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
void QtTemplateInterpreter::appendHelpParam(const QString &paramName, const QVariant &value);
```

Example:
```cpp
QtTemplateInterpreter ti;
ti.appendHelpParam("test_key", "This is test key");
ti.appendHelpParam("test_key_2", 123);
ti.appendHelpParam("test_key_3", 123.22);
...
```

### Registering c++ classes as a variable

To register as a variable of your own C++ class, follow these steps:
- each class must have its own separate file (otherwise compilation will fail);
- each class must register its type in the Qt meta type system;
- the registered class does not have to be a descendant of QObject.

>
> NOTE:
>
> To be able to call methods on a variable, it must be located in the public section of the class, and the macro ```Q_INVOKABLE``` must be specified before the method.
>
> ```cpp
> #include <QMetaType>
> 
> class MyClass
> {
>   Q_GADGET
> 
> public:
>     MyClass() {}
>     ~MyClass() {}
> 
>     Q_INVOKABLE int func() const { return 123; }
>     Q_INVOKABLE int func_summ(const int a, const int b) const { return a + b; }
> 
> };
> Q_DECLARE_METATYPE(MyClass)
> ```
>

>
> NOTE:
>
> If a pointer was used as a parameter, then after finishing work and clearing the registered parameters, the interpreter does not delete it!
>

#### Registering a class that does not inherit from QObject

To register a class that does not inherit from QObject, use the macros ```Q_GADGET```, ```Q_PROPERTY``` and ```Q_DECLARE_METATYPE```. 
These macros allow you to add to a class that does not inherit from QObject all the necessary information to obtain its properties and methods.

Example:

MyClass.h:
```cpp
#ifndef MYCLASS_H
#define MYCLASS_H

#include <QMetaType>

class MyClass
{
    // append metadata in class
    Q_GADGET

    // listing class properties
    Q_PROPERTY(QString name MEMBER _name) // or method access: Q_PROPERTY(QString name READ name)

public:
    MyClass() {}
    explicit MyClass(const QString &name)
        : _name(name)
    {}
    ~MyClass() {}

    MyClass(const TestD& obj) {
        _name = obj.name();
    }

    MyClass& operator=(const TestD& obj) {
        if (this == &obj)
            return *this;
        _name = obj.name();
        return *this;
    }

    const QString name() const { return _name; }
    
    // append public methods for call
    Q_INVOKABLE int func() const { return 123; }
    Q_INVOKABLE int func_summ(const int a, const int b) const { return a + b; }

protected:
    QString _name;
};

// registration in the metatype system
Q_DECLARE_METATYPE(MyClass)

#endif // MYCLASS_H
```

main.cpp:
```cpp
QtTemplateInterpreter ti;

// Example register my classes (ver.1)
MyClass mc("this is my test name");
QVariant mc_variant = QVariant::fromValue(mc);
tb.appendHelpParam("my_test", mc_variant);

// Example register my classes (ver.2)
tb.appendHelpParam("my_test_2", QVariant::fromValue(new MyClass("this is my test name (ptr)")));

// Example register my classes (ver.3)
tb.appendHelpParam("my_test_3", QVariant(QVariant::UserType, new MyClass("this is my test name (ptr 2)")));
...
```

#### Registering a class that inherits from QObject

To register a class that inherit from QObject, use the macros ```Q_OBJECT```, ```Q_PROPERTY``` and ```Q_DECLARE_METATYPE```. 
These macros allow you to add to a class that inherit from QObject all the necessary information to obtain its properties and methods.

>
> NOTE:
>
> Since the QObject class does not allow copying, we register a pointer to our class in the metatype system:
>
> ```cpp
> Q_DECLARE_METATYPE(MyClass*)
> ```
>

Example:

MyClass.h:
```cpp
#ifndef MYCLASS_H
#define MYCLASS_H

#include <QObject>
#include <QMetaType>

class MyClass : public QObject
{
    // append metadata in class
    Q_OBJECT

    // listing class properties
    Q_PROPERTY(QString name MEMBER _name) // or method access: Q_PROPERTY(QString name READ name)

public:
    explicit MyClass(QObject *parent = nullptr) : QObject(parent) {}
    explicit MyClass(const QString &name, QObject *parent = nullptr)
        : QObject(parent)
        , _name(name)
    {}
    ~MyClass() {}

    const QString name() const { return _name; }

    // append public methods for call
    Q_INVOKABLE int func() const { return 123; }
    Q_INVOKABLE int func_summ(const int a, const int b) const { return a + b; }

protected:
    QString _name;
};

// registration in the metatype system (only ponter for QObject child!)
Q_DECLARE_METATYPE(MyClass*)

#endif // MYCLASS_H
```

main.cpp:
```cpp
QtTemplateInterpreter ti;

// Example register my classes (ver.1)
tb.appendHelpParam("my_test", QVariant::fromValue(new MyClass("this is my test name (ptr)")));
...
```

## Functions

Functions can be called to generate content. Functions are called by their name followed by parentheses ``` () ``` and may have arguments.

List of basic functions that are implemented in the interpreter:
- ```[bool] is_null (object)``` - Check object is Null. Supported object: All.
- ```[bool] is_empty (object)``` - Check object is Empty. Supported object: QByteArray, QString, QStringList, QVariantList, QVariantMap, QVariantHash.
- ```[int] size (object)``` - Get object size. Supported object: QByteArray, QString, QStringList, QVariantList, QVariantMap, QVariantHash.
- ```[bool] contains (object, key)``` - Check if object contains a key. Supported object: QByteArray, QString, QStringList, QVariantList, QVariantMap, QVariantHash.
- ```[bool] reg_exp_match (object, reg_exp)``` - Check if object contains a regular expression. Supported object: QByteArray, QString.
- ```[bool] starts_with (object, str)``` - Check if object starts with a substring. Supported object: QByteArray, QString.
- ```[bool] ends_with (object, str)``` - Check if object ends with a substring. Supported object: QByteArray, QString.
- ```[int] index_of (object, str)``` - Get index position of the first occurrence of the string 'str' in object. Supported object: QByteArray, QString.
- ```[int] last_index_of (object, str)``` - Get index position of the last occurrence of the string 'str' in object. Supported object: QByteArray, QString.
- ```[object] left (object, size)``` - Get substring that contains the 'size' leftmost characters of the object. Supported object: QByteArray, QString.
- ```[object] right (object, size)``` - Get substring that contains the 'size' rightmost characters of the object. Supported object: QByteArray, QString.
- ```[QList<QString>] keys (object)``` - Get list containing all the keys in the object. Supported object: QVariantMap, QVariantHash.
- ```[QList<QVariant>] values (object)``` - Get list containing all the values in the object. Supported object: QVariantMap, QVariantHash.
- ```[QVariant] value (object, key)``` - Get value associated with the key. Supported object: QVariantMap, QVariantHash.
- ```[QVariant] value_at (object, pos)``` - Get item at index position 'pos' in the object. Supported object: QByteArray, QString, QStringList, QVariantList.
- ```[QString] to_str (object)``` - Get string representation of object. Supported object: QByteArray, QString, QStringList, QVariantList, QVariantMap, QVariantHash.
- ```[QVariant] first (object)``` - Get reference to the first item in the object. Supported object: QStringList, QVariantList, QVariantMap, QVariantHash.
- ```[QVariant] last (object)``` - Get reference to the last item in the object. Supported object: QStringList, QVariantList, QVariantMap, QVariantHash.
- ```[QString] first_key (object)``` - Get reference to the first key item in the object. Supported object: QVariantMap, QVariantHash.
- ```[QString] last_key (object)``` - Get reference to the last key item in the object. Supported object: QVariantMap, QVariantHash.
- ```[object] trim (object)``` - Get object that has whitespace removed from the start and the end. Supported object: QByteArray, QString.
- ```[QList<QVariant>] split (object, sep)``` - Splits object into sub-objects wherever 'sep' occurs, and returns the list of those objects. Supported object: QByteArray, QString.
- ```[object] to_lower (object)``` - Get lowercase copy of the object. Supported object: QByteArray, QString.
- ```[object] to_upper (object)``` - Get uppercase copy of the object. Supported object: QByteArray, QString.
- ```[QString] join (object, sep)``` - Joins all the objects list's items into a single object with each element separated by the given separator. Supported object: QByteArrayList, QStringList.
- ```[QString] from_utc_time (UTC time, str_format)``` - Get string representation of UTC datetime format. Supported UTC time: int, uit.
- ```[object] replace (object, before, after)``` - Replaces every occurrence of the object before with the object after and returns a reference to this object. Supported object: QByteArray, QString, QStringList, QVariantList, QVariantMap, QVariantHash.
- ```[object] remove (object, str/value)``` - Remove every occurrence of the 'str/value' and returns a reference to new object. Supported object: QByteArray, QString, QStringList, QVariantList, QVariantMap, QVariantHash.
- ```[object] remove_key (object, key)``` - Removes all the items that have the key 'key' from the object and return new object. Supported object: QVariantMap, QVariantHash.
- ```[object] append (object, str/value)``` - Inserts 'str/value' at the end of the object and return new object. Supported object: QByteArray, QString, QStringList, QVariantList.
- ```[object] append (object, key, value)``` - Inserts a new item with the key 'key' and a value of 'value' and return new object. Supported object: QVariantMap, QVariantHash.
- ```[QString] str_concat(str_1, str_2)``` - combining two strings into one (str_1 and str_2 - QString type).
- ```[bool] str_compare(str_1, str_2)``` - comparison of two strings (str_1 and str_2 - QString type).
- ```[object] round(object)``` - mathematically correct rounding to the nearest integer (object - double / float).
- ```[object] floor(object)``` - rounding down (object - double / float).
- ```[object] ceil(object)``` - rounding up (object - double / float).
- ```[int] to_int(value)``` - convert to Int (value - QVariant).
- ```[uint] to_uint(value)``` - convert to UInt (value - QVariant).
- ```[double] to_double(value)``` - convert to Double (value - QVariant).
- ```[float] to_float(value)``` - convert to Float (value - QVariant).
- ```[longlong] to_long_long(value)``` - convert to LongLong (value - QVariant).
- ```[ulonglong] to_ulong_long(value)``` - convert to ULongLong (value - QVariant).

Example of calling a function and getting its result:
```twig
{{ is_null(my_param) }}
```

### RegExp (regular expressions)

To create an object for working with regular expressions, use the functions:
- ```[RegExp] make_reg_exp(pattern)``` - creates a RegExp class object (pattern - QString).

The RegExp class supports the following methods:
- ```[bool] is_empty()``` - Returns true if the pattern string is empty; otherwise returns false.
- ```[bool] is_valid()``` - Returns true if the regular expression is valid; otherwise returns false. An invalid regular expression never matches.
- ```[QString] pattern()``` - Returns the pattern string of the regular expression.
- ```[void] set_pattern(QString pattern)``` - Sets the pattern string to pattern.
- ```[bool] is_minimal()``` - Returns true if minimal (non-greedy) matching is enabled; otherwise returns false.
- ```[void] set_minimal(bool minimal)``` - Enables or disables minimal matching. If minimal is false, matching is greedy (maximal) which is the default.
- ```[bool] exact_match(QString str)``` - Returns true if str is matched exactly by this regular expression; otherwise returns false.
- ```[int] index_in(QString str, int offset)``` - Attempts to find a match in str from position offset (0 by default). If offset is -1, the search starts at the last character; if -2, at the next to last character; etc. Returns the position of the first match, or -1 if there was no match.
- ```[int] last_index_in(QString str, int offset)``` - Attempts to find a match backwards in str from position offset. If offset is -1 (the default), the search starts at the last character; if -2, at the next to last character; etc. Returns the position of the first match, or -1 if there was no match.
- ```[int] matched_length()``` - Returns the length of the last matched string, or -1 if there was no match.
- ```[int] capture_count()``` - Returns the number of captures contained in the regular expression.
- ```[QStringList] captured_texts()``` - Returns a list of the captured text strings. The first string in the list is the entire matched string. Each subsequent list element contains a string that matched a (capturing) subexpression of the regexp.
- ```[QString] cap(int nth)``` - Returns the text captured by the nth subexpression. The entire match has index 0 and the parenthesized subexpressions have indexes starting from 1 (excluding non-capturing parentheses).
- ```[int] pos(int nth)``` - Returns the position of the nth captured text in the searched string. If nth is 0 (the default), pos() returns the position of the whole match.
- ```[QString] error_string()``` - Returns a text string that explains why a regexp pattern is invalid the case being; otherwise returns "no error occurred".
- ```[QString] escape(QString str)``` - Returns the string str with every regexp special character escaped with a backslash. The special characters are: ```$, (,), *, +, ., ?, [, ,], ^, {, | и }```.

An example of working with the RegExp class:
```twig
{% set rx = make_reg_exp('my_key_(\d+)') %}
{% if rx.index_in('my_key_123', 0) != -1 %}
{% set rx_cap = rx.cap(1) %}
RegExp cap value: {{ rx_cap }}
{% else %}
RegExp cap value: Not Found
{% endif %}
```

### Registration of user functions

All helper functions are implemented by the template class QtTIHelperFunction with a variable number of arguments.

The interpreter allows you to register two types of functions:
- function to return only the result of execution
    ```cpp
    //!
    //! \brief QtTIHelperFunction
    //! \param name Function name
    //! \param callback Function callback
    //!
    //! === Example:
    //!     new QtTIHelperFunction<>("my_test_array", []() {
    //!         return QStringList({ "1", "11", "111" });
    //!     });
    //!
    QtTIHelperFunction(const QString &name, std::function<QVariant/*result*/(const T&... args)> callback);
    ```

- function to return status and error message if required
    ```cpp
    //!
    //! \brief QtTIHelperFunction
    //! \param name Function name
    //! \param callback Function callback
    //!
    //! === Example:
    //!     new QtTIHelperFunction<TestD*>("TestD_ptr_func", [](const TestD *d) {
    //!         if (!d)
    //!             // if not d -> return error
    //!             return std::make_tuple(false /*state*/, QString("") /*result*/, "NULL_PTR" /*error msg*/); 
    //!
    //!         // return result
    //!         return std::make_tuple(true /*state*/, d->name() /*result*/, "" /*error msg*/);
    //!     });
    //!
    QtTIHelperFunction(const QString &name, std::function<std::tuple<bool /*state*/, QVariant /*result*/, QString /*error msg*/>(const T&... args)> callback);
    ```

When registering your own classes as function arguments, keep in mind:
- The interpreter supports inheritance of these classes, and checking for the possibility of type conversion is carried out using the ```QVariant::canConvert<T>()``` method.
- in the case of registering two identical functions, but differing in the arguments of the base class and the one inherited from it, it will be called in the same way as the first one was registered.
    ```cpp
    //
    // Function 1 and function 2 are completely identical, except that MyClass is inherited from MyBaseClass.
    //

    // function 1: 
    appendHelpFunction(new QtTIHelperFunction<MyClass*>("my_function", [](const MyClass *d) { ... }));
    
    // function 2:
    appendHelpFunction(new QtTIHelperFunction<MyBaseClass*>("my_function", [](const MyBaseClass *d) { ... }));
    ```

To register user functions in the interpreter using the method ```appendHelpFunction(const QtTIAbstractHelperFunction *func)```:

```cpp
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
//!             return std::make_tuple(false /*state*/, QString("") /*result*/, "NULL_PTR" /*error msg*/);
//!
//!         return std::make_tuple(true /*state*/, d->name() /*result*/, "" /*error msg*/);
//!     }));
//!
void QtTemplateInterpreter::appendHelpFunction(const QtTIAbstractHelperFunction *func);
```

Example:
```cpp
QtTemplateInterpreter ti;
ti.appendHelpFunction(new QtTIHelperFunction<>("my_array", []() {
    return QStringList({ "1", "11", "111" });
}));

ti.appendHelpFunction(new QtTIHelperFunction<>("my_map", []() {
    return QVariantMap({{"a", "1"}, {"b", "11"}, {"c", "111"}});
}));

ti.appendHelpFunction(new QtTIHelperFunction<TestD>("my_func", [](const TestD &d) {
    return d.name();
}));

ti.appendHelpFunction(new QtTIHelperFunction<TestD*>("my_func_ptr", [](const TestD *d) {
    if (!d)
        return std::make_tuple(false /*state*/, QString("") /*result*/, "NULL_PTR" /*error msg*/);

    return std::make_tuple(true /*state*/, d->name() /*result*/, "" /*error msg*/);
}));
...
```

Functions do not support arguments that have a default value; more precisely, the interpreter will ignore this default value and search for a function with the full number of arguments. The example code below will cause an error:

```cpp
//
// appendHelpFunction(new QtTIHelperFunction<QString,int>("my_test_func", [](const QString &a, const int &b = 1) { ... }));
//

// This template line will lead to an error, since the interpreter will require two input arguments, 
// although from a programming point of view everything should be correct.
{{ my_test_func("test") }}
```

>
> NOTE:
>
> Functions can have the same names if they differ in the list and types of input arguments. 
> Registering functions with the same names and the same number and types of input arguments is prohibited and will result in an error.
>
> ```cpp
> //
> // success
> //
> QtTemplateInterpreter ti;
> ti.appendHelpFunction(new QtTIHelperFunction<>("my_func", []() { ... }));
> ti.appendHelpFunction(new QtTIHelperFunction<int>("my_func", [](const int &v) { ... }));
> ti.appendHelpFunction(new QtTIHelperFunction<double>("my_func", [](const double &v) { ... }));
>
> //
> // failed:
> //
> QtTemplateInterpreter ti;
> ti.appendHelpFunction(new QtTIHelperFunction<int>("my_func", [](const int &v) { ... }));
> ti.appendHelpFunction(new QtTIHelperFunction<int>("my_func", [](const int &v) { ... }));
> ```
>

## Literals

The simplest form of expressions are literals. Literals are representations for Qt types such as strings, numbers, arrays and etc. 
The following literals exist:
- ```"Hello World"``` - Everything between two double or single quotes is a QString. They are useful whenever you need a string in the template (for example as arguments to function calls, filters or just to extend or include a template). A string can contain a delimiter if it is preceded by a backslash ``` \ ``` like in ```'It\'s good'```. If the string contains a backslash (e.g. ```c:\Program Files```) escape it by doubling it (e.g. ```c:\\Program Files```).
- ```42 / 42.23``` - Integers and double numbers are created by writing the number down. If a dot is present the number is a double, otherwise an integer.
- ```["foo", "bar"]``` - QVariantList are defined by a sequence of expressions separated by a comma ``` , ``` and wrapped with squared brackets ``` [] ```.
- ```{"foo": "bar"}``` - QVariantMap are defined by a list of keys and values separated by a comma ``` , ``` and wrapped with curly braces ``` {} ```:
- ```true / false``` - true represents the true value, false represents the false value.

The interpreter also allows you to more strictly specify the types of variables:
- ```int``` : 123 / +123 / -123 / i123 / +i123 / -i123
- ```uint``` : ui123
- ```qlonglong``` : l12345678 or -l12345678
- ```qulonglong``` : ul12345678
- ```double``` : 123.45 / +123.45 / -123.45 / d123.45 / +d123.45 / -d123.45
- ```float``` : f123.45 / +f123.45 / -f123.45
- ```QString``` : '...' / s'...' / "..." / s"..."
- ```QByteArray``` : b'...' / b"..."
- ```QVariantList``` : [...]
- ```QVariantMap``` : {...} / m{...}
- ```QVariantHash``` : h{...}

>
> NOTE:
>
> When strictly specifying the type of a variable, a space between the type and value is not allowed!
>
> ```twig
> {# success #}
> {% set a = ui123 %}
>
> {# failed #}
> {% set a = ui 123 %}
> ```
>

Examples:
```twig
{# Example QVariantList 

    QVariantList {
        0: 1 (int)
        1: -2 (int)
        2: "abc" (QString)
        3: 12.15 (double)
        4: "def" (QByteArray)
        5: 11.05 (float)
        6: "this is test message, 1, 2, 3" (QString)
    }
#}
{% set list = [ 1, -2, 'abc', 12.15, b"def", f11.05, "this is test message, 1, 2, 3" ] %}

{# Example QVariantMap / QVariantHash 

    QVariantMap {
        a: 1 (int)
        b: -2 (int)
        c: "abc" (QString)
        d: 12.15 (double)
        e: "def" (QByteArray)
        f: 11.05 (float)
        g: "this is test message, 1, 2, 3" (QString)
        j: { "a": 1, "b": 2, "c": 3} (QVariantHash)
    }
#}
{% set map = { a: 1, b: -2, c: 'abc', d: 12.15, e: b"def", f: f11.05, g: "this is test message, 1, 2, 3", j: h{ a: 1, b: 2, c: 3 } } %}
```

## Math operations

The interpreter allows you to perform various mathematical operations, and their result can be inserted either into the body of the template or into the condition of the control block, depending on the location of the call.

>
> NOTE:
>
> Math operations are supported exclusively for numeric values!
>

The following operators are supported:
- ```+``` - Adds two numbers together (the operands are casted to numbers):
           
    ```twig
    {{ 1 + 1 }} is 2 
    ```

- ```-``` - Subtracts the second number from the first one:
    
    ```twig
    {{ 3 - 2 }} is 1
    ```

- ```/``` - Divides two numbers. The returned value will be a floating point number:
    
    ```twig
    {{ 1 / 2 }} is 0.5
    ```

- ```%``` - Calculates the remainder of an integer division:

    ```twig        
    {{ 11 % 7 }} is 4
    ```

- ```//``` - Divides two numbers and returns the floored integer result:
    
    ```twig
    {{ 20 // 7 }} is 2
    {{ -20 // 7 }} is -3
    ```

- ```*``` - Multiplies the left operand with the right one:
    
    ```twig
    {{ 2 * 2 }} would return 4
    ```

- ```**``` - Raises the left operand to the power of the right operand:
    
    ```twig
    {{ 2 ** 3 }} would return 8
    ```


Math operations supported grouping expressions with brackets ``` (...expr...) ```:

```twig
{{ (10.5 + 2 * 2) / 3 + 10 - 1 + 2 ** 2 }} is 17.833333333333332
{{ (10.5 + 2 * (2 + 5)) / (3 + 10 * 2) - (1 + 2) ** 2 }} is -7.934782608695652
```


>
> NOTE:
>
> The use of variables in math operations is also allowed, but with the condition that the variable is of a numeric type:
>
> ```twig
> {# success #}
> {% set my_key = 10 %}
> {{ my_key - 2 }} is 8
>
> {# failed #}
> {% set my_key = '10' %}
> {{ my_key - 2 }} will cause an error since the variable 'my_key' type is string
> ```
>

## Logic operations

The interpreter allows you to perform various logical operations, and their result can be inserted either into the body of the template or into the condition of the control block, depending on the location of the call.

>
> NOTE:
>
> Logical operation condition may include the mathematical operations described above.
>

Logical expression supports the following types of comparisons:
- ```==``` - equally
- ```!=``` - not equally
- ```>```  - larger
- ```<```  - less
- ```>=``` - larger or equal
- ```<=``` - less or equal

You can combine multiple expressions using the following operators:
- ```and``` - returns true if the left and the right operands are both true.
- ```or```  - returns true if the left or the right operand is true.
- ```not``` - negates a statement.
- ```&&```  - equivalent of operation ```and```
- ```||```  - equivalent of operation ```or```

Logical expression supports the grouping expressions with brackets ``` (...expr...) ```:
```twig
{% set a = 10.5 %}
{% set b = 2 %}
{% set aa = { a:1, b:2, c:3 } %}
{{ (a + aa.b) > 5 && (aa.b < 2 || aa.c >= 3) }} is true

{# function 'my_test_array()' return array '[1, 2, 3]' #}
{{ my_test_array () && a <= 5 }} is false
```

>
> NOTE:
>
> If a variable (or object) is passed to the logical expression, then the condition is correct if the variable exists and is not NULL:
>
> ```twig
> {% set a = 10.5 %}
> {{ a && a > 10 }} is true
> {{ b && a > 10 }} is false, because variable 'b' does not exist
> ```
>

## Ternary operator

The interpreter allows the use of the ternary operator.

>
> NOTE:
>
> The condition of the ternary operator supports all the capabilities provided by the logical operations described above.
>

```twig
{{ foo ? 'yes' : 'no' }}
{{ foo ?: 'no' }} is the same as {{ foo ? foo : 'no' }}
{{ foo ? 'yes' }} is the same as {{ foo ? 'yes' : '' }}
```

Ternary operator supports the grouping expressions with brackets ``` (...expr...) ```:
```twig
{% set a = 10.5 %}
{% set b = 2 %}
{% set aa = { a:1, b:2, c:3 } %}

{{ ((a + aa.b) > 5 && (aa.b < 2 || aa.c >= 3)) ? 'yes' : 'no' }} is 'yes'
{{ ((a + aa.b) > 5 && (aa.b < 2 || aa.c >= 3)) ? (a + aa.b) : (a - aa.b) }} is 12.5

{{ ((a + aa.b) > 5 && (aa.b < 2 || aa.c >= 3)) ?: 'no' }} is true
{{ ((a + aa.b) > 5 && (aa.b < 2 || aa.c >= 3)) ?: (a - aa.b) }} is true

{{ ((a + aa.b) > 5 && (aa.b < 2 || aa.c >= 3)) ? 'yes' }} is 'yes'
{{ ((a + aa.b) > 5 && (aa.b < 2 || aa.c >= 3)) ? (a + aa.b) }} is 12.5
```

## NULL-Coalescing operator

The interpreter allows the use of the null-coalescing operator.

>
> NOTE:
>
> The condition of the null-coalescing operator supports all the capabilities provided by the logical operations described above.
>

```twig
{# returns the value of foo if it is defined and not null, 'no' otherwise #}
{{ foo ?? 'no' }}
```

NULL-Coalescing operator supports the grouping expressions with brackets ``` (...expr...) ```:
```twig
{% set a = 10.5 %}
{% set b = 2 %}
{% set aa = { a:1, b:2, c:3 } %}

{{ ((a + aa.b) > 5 && (aa.b < 2 || aa.c >= 3)) ?? 'no' }} is 'true'
{{ ((a + aa.b) > 5 && (aa.b < 2 || aa.c >= 3)) ?? (a - aa.b) }} is 'true'
```

## Comments

To comment-out part of a line in a template, use the comment syntax ``` {# ... #} ```.
When processing a template, all comments are skipped and do not appear in the processing result.
This is useful for debugging or to add information for other template designers or yourself:

```twig
{# this is a one line comment #}

{# 
this is 
a multiline 
comment
... 
#}
```

If there are calls to variables, functions, or control blocks inside the comment block, they will also be skipped and will not be included in the processing result:

```twig
![This is Lu and Bryu!]( {{ image_path ('lu_and_bryu.png') }} \"Lu and Bryu\")

{# this part of the code will be skipped
Lu images: 
{% for item in lu_images() %}
    "Lu - {{ item.title }}": {{ item.path }}
{% endfor %}

Bryu images: 
{% for item in bryu_images() %}
    "Bryu - {{ item.title }}": {{ item.path }}
{% endfor %}
#}
```

## Control structure

A control structure refers to all those things that control the flow of a program - conditionals (i.e. if/elseif/else), for-loops, as well as things like blocks. Control structures appear inside ``` {% ... %} ``` blocks.

This version of the interpreter supports the following control blocks:
- ```set``` - allows you to create a temporary variable
- ```unset``` - allows you to delete a temporary variable created earlier
- ```if/elseif/else``` - block of conditions
- ```for``` - for-loop block

>
> NOTE:
>
> Control blocks may also include nested control blocks if required to describe the logic:
>
> ```twig
> {# Output:
>     --- a: 1 ---
>     --- b: 2 ---
>     --- d: 4 ---
> #}
> {% set set my_map = { a: 1, b: 2, c: 3, d: 4 } %}
> {% for key, val in my_map %}
> {% if key != 'c' %}
> --- {{ key }}: {{ val }} ---
> {% endif %}
> {% endfor %}
> ```
>

### Control structure 'set'

A control structure ```set``` allows you to create a temporary variable.
Control structure ```set``` syntax:

```twig
{# Control block (ver.1): #}
{% set KEY = VALUE %}

{# Control block (ver.2): #}
{% set KEY = FUNCTION() %}

{# Control block (ver.3): #}
{% set KEY = (MATH OPERATIONS) %}

{# Control block (ver.4): #}
{% set KEY = (LOGIC OPERATIONS) %}
```

Examples:
```twig
{# Example 1 #}
{% set my_key = 'configure.svg' %}
![This is Lu and Bryu!]({{ image_path (my_key) }} "Lu and Bryu")

{# Example 2 #}
{% set my_key = my_test_func () %}

{# Example 3 #}
{% set my_key = ((1 + 5) * 2) %}

{# Example 4 #}
{% set my_key = ((a + b) > 5 || a < 10) %}
```

>
> NOTE:
>
> Calling control blocks with the same keys will result in the value being overwritten with a new one.
>
> ```twig
> {% set my_key = 'my string value' %} {# Variable 'my_key' stores a string value #}
> {% set my_key = 123 %} {# Now variable 'my_key' stores a numeric value #}
>
> {# The call block below will output the value 123 #}
> {{ my_key }}
> ```
>

>
> NOTE:
>
> The ```set``` control block expression can be wrapped on a new line. When parsed, it will be combined into one line with all hyphens removed:
>
> ```twig
> {% set multi_line_value = [
>     { a: 1, b: 2, c: 3 },
>     { aa: 11, bb: 22, cc: 33 },
>     { aaa: 111, bbb: 222, ccc: 333 }
>    ]
> %}
> ```
>

### Control structure 'unset'

A control structure ```unset``` allows you to delete a temporary variable created earlier.
Control structure ```unset``` syntax:

```twig
{% unset KEY %}
```

Examples:
```twig
{% set my_key = 'configure.svg' %}
![This is Lu and Bryu!]({{ image_path (my_key) }} "Lu and Bryu")
{% unset my_key %}
```

>
> NOTE:
>
> If the variable whose name is passed to the ```unset``` method has not been created previously or has already been deleted, 
> then the operation will be ignored.
>

### Control structure 'if/elseif/else'

A control structure ```if/elseif/else``` allows you to create a block(s) of conditions, which, when met, will only execute its contents (or the body of the block).
Control structure ```if/elseif/else``` syntax:

```twig
{# Control block (ver. 1): #}
{% if LEFT CONDITION RIGHT %}
    ...
{% endif %}

{# Control block (ver. 2): #}
{% if OBJECT %}
    ...
{% endif %}

{# Control block (ver. 3): #}
{% if LEFT CONDITION RIGHT %}
    ...
{% elseif LEFT CONDITION RIGHT %}
    ...
{% elseif OBJECT %}
    ...
{% else %}
    ...
{% endif %}
```

>
> NOTE:
>
> The control structure ```if/elseif/else``` always starts with ```{% if ... %}``` and ends with ```{% endif %}```.
>

>
> NOTE:
>
> The control structure ```if/elseif/else``` supports all the capabilities provided by the logical operations described above.
>

>
> NOTE:
>
> If a variable (or object) is passed to the control block condition, then the condition is correct if the variable exists and is not NULL:
>
> ```twig
> {% if my_key %}
> --- is OK ---
> {% else %}
> --- is NOT OK ---
> {% endif %}
> ```
>

The control structure ```if/elseif/else``` supports the grouping expressions with brackets ``` (...expr...) ```:
```twig
{% set a = 10.5 %}
{% set b = 2 %}
{% set aa = { a:1, b:2, c:3 } %}
{% if (a + aa.b) > 5 && (aa.b < 2 || aa.c >= 3) %}
--- THIS is IF BLOCK BODY ---
{% endif %}
```

Examples:
```twig
{# Example 1 #}
{% set test_my_key = 1 %}
{% if test_my_key >= 1 %}
--- THIS is IF BLOCK BODY ---
{% endif %}

{# Example 2 #}
{% set test_my_key = 1 %}
{% set test_my_key_2 = 0.11 %}
{% if test_my_key > test_my_key_2 %}
--- THIS is IF BLOCK BODY 1 ---
{% elseif test_my_key < test_my_key_2 %}
--- THIS is IF BLOCK BODY 2 ---
{% elseif test_my_key == test_my_key_2 %}
--- THIS is IF BLOCK BODY 3 ---
{% else %}
--- THIS is IF BLOCK BODY 4 ---
{% endif %}

{# Example 3 #}
{% set test_array = my_test_array () %}
{% set multi_array = { a: {aa: 1, bb:2, cc:3}, b: { aaa: 11, bbb: 22, ccc: 33} } %}
{% if not false || multi_array && size(test_array) == 3 %}
--- is OK ---
{% else %}
--- is NOT OK ---
{% endif %}
```

### Control structure 'for'

A control structure ```for``` allows you to create a for-loop block. Syntactically, it is similar to the for-loop in Python.
Control structure ```for``` syntax:

```twig
{# Control block (ver.1): #}
{% for VALUE in LIST %}
    ...
{% endfor %}

{# Control block (ver.2): #}
{% for KEY, VALUE in MAP %}
    ...
{% endfor %}

{# Control block (ver.3): #}
{% for VALUE in MAP %}
    ...
{% endfor %}
```

>
> NOTE:
>
> The control structure ```for``` always starts with ```{% for ... %}``` and ends with ```{% endfor %}```.
>

Examples:

```twig
{# Example 1
   
   Output:
    --- A: 1 ---
    --- A: 2 ---
    --- A: 3 ---
    --- A: 4 ---
#}
{% set set my_arr = [ 1, 2, 3, 4 ] %}
{% for a in my_arr %}
--- A: {{ a }} ---
{% endfor %}

{# Example 2
   
   Output:
    --- a: 1 ---
    --- b: 2 ---
    --- c: 3 ---
    --- d: 4 ---
#}
{% set set my_map = { a: 1, b: 2, c: 3, d: 4 } %}
{% for key, val in my_map %}
--- {{ key }}: {{ val }} ---
{% endfor %}

{# Example 3
   
   Output:
    --- VALUE: 1 ---
    --- VALUE: 2 ---
    --- VALUE: 3 ---
    --- VALUE: 4 ---
#}
{% set set my_map = { a: 1, b: 2, c: 3, d: 4 } %}
{% for val in my_map %}
--- VALUE: {{ val }} ---
{% endfor %}

{# Example 3
   
   NOTE: function 'my_test_map ()' return map '{ a: 1, b: 11, c: 111 }'.

   Output:
    --- a: 1 ---
    --- b: 11 ---
    --- c: 111 ---
#}
{% for key, val in my_test_map () %}
--- {{ key }}: {{ val }} ---
{% endfor %}
```

## Performing template interpretation

To start template interpretation, the following functions are provided:

```cpp
//!
//! \brief Interpret the template data
//! \param data Template data
//! \return
//!
std::tuple<bool/*isOk*/,QString/*result*/,QString/*error*/> interpret(QString data);

//!
//! \brief Interpret the template data and return result
//! \param data Template data
//! \return
//!
//! NOTE: This method only displays a critical message to the console in case of errors.
//!
QString interpretRes(QString data);


//!
//! \brief Interpret the template data from file
//! \param path File path
//! \return
//!
std::tuple<bool/*isOk*/,QString/*result*/,QString/*error*/> interpretFromFile(const QString &path);

//!
//! \brief Interpret the template data from file and return result
//! \param path File path
//! \return
//!
//! NOTE: This method only displays a critical message to the console in case of errors.
//!
QString interpretResFromFile(const QString &path);
```

Examples:

```cpp
QString test_multi_line("![This is Lu and Bryu!]( test_key {{ image_path_b (b'configure.svg') }} \"Lu and Bryu\") "
                        "--- {{ image_path_2 ([ui1, ul2, l3, 4, [5.2, -6]]) }} "
                        "{{ image_path_3 ({a:f1.2, b:2, c : +f3.5, ff : h{d : 4, e:-5}, f:-6.2}) }}\r\n"
                        "{% set test_my_key = 'abc' %} {{ image_path (test_my_key) }} {% unset test_my_key %}\r\n"
                        "{% set test_my_key = { a: 'hi', b: 'hellow', c: 123 } %}{{ image_path (test_my_key.a) }}{% unset test_my_key %}\r\n"
                        "{% set test_my_key = 1 %}\r\n"
                        "{% if test_my_key >= 1 %}\r\n"
                        " --- THIS is IF BLOCK BODY --- \r\n"
                        "{% endif %}\r\n"
                        "{# start comment\r\n"
                        "{% if test_my_key >= 1 %}\r\n"
                        " --- THIS is IF BLOCK BODY IN COMMENT --- \r\n"
                        "{% endif %}\r\n"
                        "end comment #}\r\n");

QtTemplateInterpreter ti;
ti.appendHelpFunction(new QtTIHelperFunction<QString>("image_path", [](const QString &a) { 
    return QString("hellow there: %1").arg(a);
}));
ti.appendHelpFunction(new QtTIHelperFunction<QByteArray>("image_path_b", [](const QByteArray &a) { 
    return QString("hellow there: %1").arg(a.constData());
}));
ti.appendHelpFunction(new QtTIHelperFunction<QVariantList>("image_path_2", [](const QVariantList &a) {
    QStringList tmp;
    for (const QVariant &i : a)
        tmp << i.toString();
    return QString("hellow there list: %1").arg(tmp.join(", "));
}));
ti.appendHelpFunction(new QtTIHelperFunction<QVariantMap>("image_path_3", [](const QVariantMap &a) {
    QStringList tmp;
    QMapIterator<QString,QVariant> it(a);
    while (it.hasNext()) {
        it.next();
        tmp << QString("%1: %2").arg(it.key()).arg(it.value().toString());
    }
    return QString("hellow there map: %1").arg(tmp.join(", "));
}));
qDebug() << ti.interpretRes(test_multi_line);

// Output: "![This is Lu and Bryu!]( test_key hellow there: configure.svg \"Lu and Bryu\") --- hellow there list: 1, 2, 3, 4,  hellow there map: a: 1.2000000476837158, b: 2, c: 3.5, f: -6.2, ff: \r\n hellow there: abc \r\nhellow there: hi\r\n\r\n --- THIS is IF BLOCK BODY --- \r\n\r\n"
```

## Using separate interpreter library classes

When working, you may need to use not the entire interpreter, but its individual parts to parse the string representation of data. 
The library is built in such a way that you can use the following classes of data parsers:
- ```QtTIParserMath``` - math operations parser
- ```QtTIParserLogic``` - logic operations parser
- ```QtTIParserTernaryOperator``` - ternary operator parser
- ```QtTIParserNullCoalescingOperator``` - null-coalescing operator parser

All these classes are located in the corresponding subdirectories of the ```QtTemplateInterpreter/QtTIParser/``` directory.

Most of these classes require variable and function container classes:
- ```QtTIParserArgs``` - variable container class
- ```QtTIParserFunc``` - function container class

Therefore, in the case of using separate parser classes, you should create container classes yourself, and register user variables and functions in them.

Examples:

```cpp
QtTIParserArgs tiArgs;
QtTIParserFunc tiFuncs;

// registrate variables:
tiArgs.appendHelpParam("a", 10.5);
tiArgs.appendHelpParam("b", 2);
tiArgs.appendHelpParam("aa", QVariantMap({{"a", 1}, {"b", 2}, {"c", 3}}));

// registrate functions:
tiFuncs.appendHelpFunction(new QtTIHelperFunction<>("my_test_array", []() { 
    return QStringList({"1", "11", "111"});
}));

// parse math
QString mathExpr("a + aa.b * 2 / 3 + 10 - 1 + 2 ** 2"); // is 24.5
if (QtTIParserMath::isMathExpr(mathExpr)) {
    bool isOk = false;
    QString error;
    QVariant result = QtTIParserMath::parseMath(mathExpr, &tiArgs, &isOk, error);
    if (!isOk)
        qCritical() << qPrintable(QString("Parse math failed! Error: %1").arg(error));
    else
        qDebug() << qPrintable(QString("Parse math success. Result: %1").arg(result.toString()));
}

// parse logic
QString logicExpr_1("(a + aa.b) > 5 && (aa.b < 2 || aa.c >= 3)"); // is true
if (QtTIParserLogic::isLogicExpr(logicExpr_1)) {
    bool isOk = false;
    QString error;
    QVariant result = QtTIParserLogic::parseLogic(logicExpr_1, &tiArgs, &tiFuncs, &isOk, error);
    if (!isOk)
        qCritical() << qPrintable(QString("Parse logic failed! Error: %1").arg(error));
    else
        qDebug() << qPrintable(QString("Parse logic success. Result: %1").arg(result.toString()));
}

QString logicExpr_2("my_test_array () && a <= 5"); // is false
if (QtTIParserLogic::isLogicExpr(logicExpr_2)) {
    bool isOk = false;
    QString error;
    QVariant result = QtTIParserLogic::parseLogic(logicExpr_2, &tiArgs, &tiFuncs, &isOk, error);
    if (!isOk)
        qCritical() << qPrintable(QString("Parse logic failed! Error: %1").arg(error));
    else
        qDebug() << qPrintable(QString("Parse logic success. Result: %1").arg(result.toString()));
}

// parse ternary
QString ternaryExpr("((a + aa.b) > 5 && (aa.b < 2 || aa.c >= 3)) ? 'yes' : 'no'"); // is 'yes'
if (QtTIParserTernaryOperator::isTernaryOperatorExpr(ternaryExpr)) {
    bool isOk = false;
    QString error;
    QVariant result = QtTIParserTernaryOperator::parseTernaryOperator(ternaryExpr, &tiArgs, &tiFuncs, &isOk, error);
    if (!isOk)
        qCritical() << qPrintable(QString("Parse ternary failed! Error: %1").arg(error));
    else
        qDebug() << qPrintable(QString("Parse ternary success. Result: %1").arg(result.toString()));
}

// parse null-coalescing
QString nullCoalescingExpr("((a + aa.b) > 5 && (aa.b < 2 || aa.c >= 3)) ?? 'no'"); // is true
if (QtTIParserNullCoalescingOperator::isNullCoalescingOperatorExpr(nullCoalescingExpr)) {
    bool isOk = false;
    QString error;
    QVariant result = QtTIParserNullCoalescingOperator::parseNullCoalescingOperator(nullCoalescingExpr, &tiArgs, &tiFuncs, &isOk, error);
    if (!isOk)
        qCritical() << qPrintable(QString("Parse null-coalescing failed! Error: %1").arg(error));
    else
        qDebug() << qPrintable(QString("Parse null-coalescing success. Result: %1").arg(result.toString()));
}
```

Example of a method for parsing a string that may contain different representations of data:

```cpp
//!
//! \brief Parse parameter value from string
//! \param str Parameter value string view
//! \param parserArgs QtTIParserArgs object pointer
//! \param parserFunc QtTIParserFunc object pointer
//! \return
//!
std::tuple<bool/*isOk*/,QVariant/*res*/,QString/*err*/> parseParamValue(const QString &str, QtTIParserArgs *parserArgs, QtTIParserFunc *parserFunc)
{
    if (str.isEmpty())
        return std::make_tuple(false, QVariant(), "Parse value failed (empty string passed)");
    if (!parserArgs)
        return std::make_tuple(false, QVariant(), "Parse value failed (QtTIParserArgs is NULL)");
    if (!parserFunc)
        return std::make_tuple(false, QVariant(), "Parse value failed (QtTIParserFunc is NULL)");

    // ternary
    if (QtTIParserTernaryOperator::isTernaryOperatorExpr(str)) {
        bool isOk = false;
        QString error;
        QVariant result = QtTIParserTernaryOperator::parseTernaryOperator(str, parserArgs, parserFunc, &isOk, error);
        if (!isOk)
            return std::make_tuple(false, QVariant(), error);

        return std::make_tuple(true, result, "");
    }

    // null-coalescing
    if (QtTIParserNullCoalescingOperator::isNullCoalescingOperatorExpr(str)) {
        bool isOk = false;
        QString error;
        QVariant result = QtTIParserNullCoalescingOperator::parseNullCoalescingOperator(str, parserArgs, parserFunc, &isOk, error);
        if (!isOk)
            return std::make_tuple(false, QVariant(), error);

        return std::make_tuple(true, result, "");
    }

    // logic
    if (QtTIParserLogic::isLogicExpr(str)) {
        bool isOk = false;
        QString error;
        QVariant result = QtTIParserLogic::parseLogic(str, parserArgs, parserFunc, &isOk, error);
        if (!isOk)
            return std::make_tuple(false, QVariant(), error);

        return std::make_tuple(true, result, "");
    }

    // math
    if (QtTIParserMath::isMathExpr(str)) {
        bool isOk = false;
        QString error;
        QVariant result = QtTIParserMath::parseMath(str, parserArgs, &isOk, error);
        if (!isOk)
            return std::make_tuple(false, QVariant(), error);

        return std::make_tuple(true, result, "");
    }

    // function
    QRegExp rxFunc("(\\s*([\\w]+)\\s*\\(\\s*([A-Za-z0-9_\\ \\+\\-\\,\\.\\'\\\"\\{\\}\\[\\]\\:\\/]*)\\s*\\)\\s*)");
    if (rxFunc.indexIn(str) != -1) {
        QString funcName = rxFunc.cap(2).trimmed();
        QVariantList funcArgs = parserArgs->parseHelpFunctionArgs(rxFunc.cap(3).trimmed());
        return parserFunc->evalHelpFunction(funcName, funcArgs);
    }

    // arg
    QVariantList tmp = parserArgs->parseHelpFunctionArgs(str);
    if (tmp.isEmpty())
        return std::make_tuple(false, QVariant(), "Parse value failed");
    if (tmp.size() > 1)
        return std::make_tuple(false, QVariant(), "Parse value failed (more than one argument is given)");

    return std::make_tuple(true, tmp[0], "");
}
```
