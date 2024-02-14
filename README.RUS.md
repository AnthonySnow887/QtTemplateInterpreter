# QtTemplateInterpreter

[![License](https://img.shields.io/github/license/AnthonySnow887/QtTemplateInterpreter)](https://github.com/AnthonySnow887/QtTemplateInterpreter/blob/master/LICENSE)
[![Latest Release](https://img.shields.io/github/v/release/AnthonySnow887/QtTemplateInterpreter?label=release)](https://github.com/AnthonySnow887/QtTemplateInterpreter/releases)
![Last Commit](https://img.shields.io/github/last-commit/AnthonySnow887/QtTemplateInterpreter/develop)

Библиотека интерпретатора текстовых шаблонов на основе Qt 5.
Эта библиотека по своей работе и синтаксису написания шаблонов аналогична движку шаблонов PHP Twig (https://github.com/twigphp/Twig).

## Использование

### Если вы используете qmake

Чтобы использовать библиотеку, просто скопируйте ее в корень вашего проекта и включите файл "QtTemplateInterpreter/QtTemplateInterpreter.pri":

```cpp
include(QtTemplateInterpreter/QtTemplateInterpreter.pri)
```

> 
> Пример для qmake:
>
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

### Если вы используете CMake

Чтобы использовать библиотеку, просто скопируйте ее в корень вашего проекта и включите файл "QtTemplateInterpreter/CMakeLists.txt":

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
> Пример для CMake:
>
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

## Синтаксис написания шаблонов

Шаблон представляет собой обычный текстовый файл. Он может генерировать любой текстовый формат (TXT, HTML, XML, CSV, LaTeX и т.д.). Он не имеет конкретного расширения.

Шаблон содержит переменные или выражения, которые заменяются значениями при оценке шаблона, а также теги, управляющие логикой шаблона.

Ниже приведен минимальный шаблон, иллюстрирующий некоторые основы. Более подробную информацию мы расскажем позже:

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

Существует два типа разделителей: ```{% ... %}``` и ```{{ ... }}```.
Первый используется для выполнения различных блочных операций, например цикла for. Второй печатает результат выражения.

## Переменные

Все параметры внутри интерпретатора хранятся в классе QVariant.

Интерпретатор передает переменные в шаблоны для дальнейших манипуляций в шаблоне. Переменные также могут иметь атрибуты или методы, к которым у вас есть доступ. Для доступа к атрибутам или методам переменной используйте точку ``` . ```:

```twig
{{ foo.bar }}
```

>
> ПРИМЕЧАНИЕ:
>
> Если вам требуется вызвать метод переменной, то после его названия должны следовать фигурные скобки. В фигурных скобках могут быть указаны аргументы, если этого требует метод.
> Для возможности вызова методов переменной, он должен располагаться в public секции класса, а перед методом должен быть указан макрос ```Q_INVOKABLE```.
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
> ПРИМЕЧАНИЕ:
>
> Важно знать, что фигурные скобки являются частью не переменной, а оператора печати.
> При доступе к переменным внутри тегов не заключайте их в фигурные скобки.
>

Если переменная или атрибут не существует, будет возвращен NULL, который будет преобразован в пустую строку при печати в шаблоне.

>
> ПРИМЕЧАНИЕ:
>
> Если в качестве параметра использовался указатель, то после окончания работы и очистки зарегистрированных параметров интерпретатор его не удаляет!
>

>
> ПРИМЕЧАНИЕ:
>
> Глобальные переменные должны иметь уникальные имена. Регистрация переменных с одинаковым именем запрещена и приведет к ошибке.
>

### Регистрация переменных в интерпретаторе

Переменные регистрируются в интерпретаторе с помощью метода ```appendHelpParam(const QString &paramName, const QVariant &value)```:

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

Пример:

```cpp
QtTemplateInterpreter ti;
ti.appendHelpParam("test_key", "This is test key");
ti.appendHelpParam("test_key_2", 123);
ti.appendHelpParam("test_key_3", 123.22);
...
```

### Регистрация классов C++ в качестве переменной

Чтобы зарегистрировать переменную вашего собственного класса C++, запомните следующие шаги:
- каждый класс должен иметь свой отдельный файл (иначе компиляция не удастся);
- каждый класс должен зарегистрировать свой тип в системе метатипов Qt;
- зарегистрированный класс не обязательно должен быть потомком QObject.

>
> ПРИМЕЧАНИЕ:
>
> Для возможности вызова методов переменной, он должен располагаться в public секции класса, а перед методом должен быть указан макрос ```Q_INVOKABLE```.
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
> ПРИМЕЧАНИЕ:
>
> Если в качестве параметра использовался указатель, то после окончания работы и очистки зарегистрированных параметров интерпретатор его не удаляет!
>

#### Регистрация класса, который не наследуется от QObject

Чтобы зарегистрировать класс, который не наследуется от QObject, используйте макросы ```Q_GADGET```, ```Q_PROPERTY``` и ```Q_DECLARE_METATYPE```.
Эти макросы позволяют добавить в класс, не наследуемый от QObject, всю необходимую информацию для получения его свойств и методов.

Пример:

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

#### Регистрация класса, наследуемого от QObject

Чтобы зарегистрировать класс, который наследуется от QObject, используйте макросы ```Q_OBJECT```, ```Q_PROPERTY``` и ```Q_DECLARE_METATYPE```.
Эти макросы позволяют добавлять в класс, наследуемый от QObject, всю необходимую информацию для получения его свойств и методов.

>
> ПРИМЕЧАНИЕ:
>
> Поскольку у класса QObject запрещено копирование, мы можем регистрировать в системе метатипов Qt указатель на наш класс:
>
> ```cpp
> Q_DECLARE_METATYPE(MyClass*)
> ```
>

Пример:

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

// Example register my classes
tb.appendHelpParam("my_test", QVariant::fromValue(new MyClass("this is my test name (ptr)")));
...
```

## Функции

Функции можно вызывать для генерации контента. Функции вызываются по имени, за которым следуют круглые скобки ``` () ``` и могут иметь аргументы.

Список основных функций, которые реализованы в интерпретаторе:
- ```[bool] is_null (object)``` - проверка, является ли объект Null. Поддерживаемые типы: Все.
- ```[bool] is_empty (object)``` - проверка, является ли объект пустым. Поддерживаемые типы: QByteArray, QString, QStringList, QVariantList, QVariantMap, QVariantHash.
- ```[int] size (object)``` - получить размер объекта. Поддерживаемые типы: QByteArray, QString, QStringList, QVariantList, QVariantMap, QVariantHash.
- ```[bool] contains (object, key)``` - проверка, содержит ли объект ключ key. Поддерживаемые типы: QByteArray, QString, QStringList, QVariantList, QVariantMap, QVariantHash.
- ```[bool] reg_exp_match (object, reg_exp)``` - проверка, содержит ли объект данные, отвечающие регулярному выражению. Поддерживаемые типы: QByteArray, QString.
- ```[bool] starts_with (object, str)``` - проверка, начинается ли объект с подстроки. Поддерживаемые типы: QByteArray, QString.
- ```[bool] ends_with (object, str)``` - проверка, заканчивается ли объект подстрокой. Поддерживаемые типы: QByteArray, QString.
- ```[int] index_of (object, str)``` - получить индекс позиции первого вхождения подстроки 'str' в объекте. Поддерживаемые типы: QByteArray, QString.
- ```[int] last_index_of (object, str)``` - получить индекс позиции последнего вхождения подстроки 'str' в объекте. Поддерживаемые типы: QByteArray, QString.
- ```[object] left (object, size)``` - получить подстроку, содержащую size крайних левых символов объекта. Поддерживаемые типы: QByteArray, QString.
- ```[object] right (object, size)``` - получить подстроку, содержащую size крайних правых символов объекта. Поддерживаемые типы: QByteArray, QString.
- ```[QList<QString>] keys (object)``` - получить список всех ключей объекта. Поддерживаемые типы: QVariantMap, QVariantHash.
- ```[QList<QVariant>] values (object)``` - получить список всех значений объекта. Поддерживаемые типы: QVariantMap, QVariantHash.
- ```[QVariant] value (object, key)``` - получить значение по имени его ключа. Поддерживаемые типы: QVariantMap, QVariantHash.
- ```[QVariant] value_at (object, pos)``` - получить значение по индексу его позиции 'pos' в объекте. Поддерживаемые типы: QByteArray, QString, QVariantMap, QVariantHash.
- ```[QString] to_str (object)``` - получить строковое представление объекта. Поддерживаемые типы: QByteArray, QString, QStringList, QVariantList, QVariantMap, QVariantHash.
- ```[QVariant] first (object)``` - получить первое идущее значение в объекте. Поддерживаемые типы: QStringList, QVariantList, QVariantMap, QVariantHash.
- ```[QVariant] last (object)``` - получить последнее идущее значение в объекте. Поддерживаемые типы: QStringList, QVariantList, QVariantMap, QVariantHash.
- ```[QString] first_key (object)``` - получить первый идущий ключ в объекте. Поддерживаемые типы: QVariantMap, QVariantHash.
- ```[QString] last_key (object)``` - получить последний идущий ключ в объекте. Поддерживаемые типы: QVariantMap, QVariantHash.
- ```[object] trim (object)``` - удалить пробелы и специальные символы в начале и в конце объекта. Поддерживаемые типы: QByteArray, QString.
- ```[QList<QVariant>] split (object, sep)``` - разделить объект на массив объектов, используя разделитель 'sep'. Поддерживаемые типы: QByteArray, QString.
- ```[object] to_lower (object)``` - получить копию объекта, преобразованную к нижнему регистру. Поддерживаемые типы: QByteArray, QString.
- ```[object] to_upper (object)``` - получить копию объекта, преобразованную к верхнему регистру. Поддерживаемые типы: QByteArray, QString.
- ```[QString] join (object, sep)``` - объединить список объектов в один, используя разделитель 'sep'. Поддерживаемые типы: QByteArrayList, QStringList.
- ```[QString] from_utc_time (UTC time, str_format)``` - получить строковое предстваление даты-времени формата UTC. Поддерживаемые типы UTC: int, uit.
- ```[object] replace (object, before, after)``` - заменить все вхождения before на after в объекте и вернуть его копию. Поддерживаемые типы: QByteArray, QString, QStringList, QVariantList, QVariantMap, QVariantHash.
- ```[object] remove (object, str/value)``` - удалить все вхождения 'str/value' в объекте и вернуть его копию. Поддерживаемые типы: QByteArray, QString, QStringList, QVariantList, QVariantMap, QVariantHash.
- ```[object] remove_key (object, key)``` - удалить все значения, имеющие ключ 'key' из объекта и вернуть новый объект. Поддерживаемые типы: QVariantMap, QVariantHash.
- ```[object] append (object, str/value)``` - добавить 'str/value' в конец объекта и вернуть новый объект. Поддерживаемые типы: QByteArray, QString, QStringList, QVariantList.
- ```[object] append (object, key, value)``` - добавить новое значение 'value' с ключом 'key' в объект и вернуть новый объект. Поддерживаемые типы: QVariantMap, QVariantHash.

Пример вызова функции и получения ее результата:
```twig
{{ is_null(my_param) }}
```

### Регистрация функций пользователя

Все вспомогательные функции реализованы шаблонным классом QtTIHelperFunction с переменным количеством аргументов.

Интерпретатор позволяет регистрировать два типа функций:
- функция, возвращающая только результат выполнения
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

- функция, возвращающая так же статус выполнения и сообщение об ошибке, если требуется
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

Регистрируя собственные классы в качестве аргументов функции, имейте в виду:
- интерпретатор поддерживает наследование этих классов, а проверка возможности преобразования типов осуществляется с помощью метода ```QVariant::canConvert<T>()```.
- в случае регистрации двух одинаковых функций, но отличающихся аргументами базового класса и унаследованного от него, будет вызывана первая зарегистрированая.
    ```cpp
    //
    // Function 1 and function 2 are completely identical, except that MyClass is inherited from MyBaseClass.
    //

    // function 1: 
    appendHelpFunction(new QtTIHelperFunction<MyClass*>("my_function", [](const MyClass *d) { ... }));
    
    // function 2:
    appendHelpFunction(new QtTIHelperFunction<MyBaseClass*>("my_function", [](const MyBaseClass *d) { ... }));
    ```

Регистрация пользовательских функций в интерпретаторе возможна методом ```appendHelpFunction(const QtTIAbstractHelperFunction *func)```:

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

Пример:
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

Функции не поддерживают аргументы, имеющие значение по умолчанию; точнее, интерпретатор проигнорирует это значение по умолчанию и будет искать функцию с полным количеством аргументов. Пример кода ниже приведет к ошибке:

```cpp
//
// appendHelpFunction(new QtTIHelperFunction<QString,int>("my_test_func", [](const QString &a, const int &b = 1) { ... }));
//

// This template line will lead to an error, since the interpreter will require two input arguments, 
// although from a programming point of view everything should be correct.
{{ my_test_func("test") }}
```

>
> ПРИМЕЧАНИЕ:
>
> Функции могут иметь одинаковые имена, если они отличаются количеством и типами входных аргументов.
> Регистрация функций с одинаковыми именами и одинаковым количеством и типами входных аргументов запрещена и приведет к ошибке.
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

## Литералы

Простейшей формой выражений являются литералы. Литералы — это представления типов Qt, таких как строки, числа, массивы и т.д.
Существуют следующие литералы:
- ```"Hello World"``` - Все, что находится между двумя двойными или одинарными кавычками, является QString. Они полезны всякий раз, когда вам нужна строка в шаблоне (например, в качестве аргументов для вызовов функций, фильтров или просто для расширения или включения шаблона). Строка может содержать разделитель, если ей предшествует обратная косая черта ``` \ ```, как в ```'It\'s good'```. Если строка содержит обратную косую черту (например, ```c:\Program Files```), экранируйте ее, удвоив ее (например, ```c:\\Program Files```).
- ```42 / 42.23``` - integer и double числа создаются путем записи числа. Если присутствует точка, число является double, в противном случае — integer.
- ```["foo", "bar"]``` - QVariantList определяются последовательностью выражений, разделенных запятыми ``` , ``` и заключенных в квадратные скобки ``` [] ```.
- ```{"foo": "bar"}``` - QVariantMap определяется списком ключей и значений, разделенных запятыми ``` , ``` и заключенных в фигурные скобки ``` {} `` `:
- ```true/false``` - true представляет истинное значение, false представляет ложное значение.

Интерпретатор также позволяет более строго указать типы переменных:
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
> ПРИМЕЧАНИЕ:
>
> При строгом указании типа переменной пробел между типом и значением не допускается!
>
> ```twig
> {# success #}
> {% set a = ui123 %}
>
> {# failed #}
> {% set a = ui 123 %}
> ```
>

Пример:
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

## Математические операции

Интерпретатор позволяет выполнять различные математические операции, а их результат можно вставлять либо в тело шаблона, либо в условие управляющего блока, в зависимости от места вызова.

>
> ПРИМЕЧАНИЕ:
>
> Математические операции поддерживаются исключительно для числовых значений!
>

Поддерживаются следующие операторы:
- ```+``` - складывает два числа (операнды приводятся к числам):
           
    ```twig
    {{ 1 + 1 }} is 2 
    ```

- ```-``` - Вычитает второе число из первого:
    
    ```twig
    {{ 3 - 2 }} is 1
    ```

- ```/``` - Делит два числа. Возвращаемое значение будет числом с плавающей запятой:
    
    ```twig
    {{ 1 / 2 }} is 0.5
    ```

- ```%``` - Вычисляет остаток от целочисленного деления:

    ```twig        
    {{ 11 % 7 }} is 4
    ```

- ```//``` - Делит два числа и возвращает целое от деления число:
    
    ```twig
    {{ 20 // 7 }} is 2
    {{ -20 // 7 }} is -3
    ```

- ```*``` - Умножает левый операнд на правый:
    
    ```twig
    {{ 2 * 2 }} would return 4
    ```

- ```**``` - Возводит левый операнд в степень правого операнда:
    
    ```twig
    {{ 2 ** 3 }} would return 8
    ```


Математические операции поддерживают группировку выражений с помощью скобок ``` (...expr...) ```:

```twig
{{ (10.5 + 2 * 2) / 3 + 10 - 1 + 2 ** 2 }} is 17.833333333333332
{{ (10.5 + 2 * (2 + 5)) / (3 + 10 * 2) - (1 + 2) ** 2 }} is -7.934782608695652
```


>
> ПРИМЕЧАНИЕ:
>
> Использование переменных в математических операциях также допускается, но с условием, что переменная имеет числовой тип:
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

## Логические операции

Интерпретатор позволяет выполнять различные логические операции, а их результат можно вставлять либо в тело шаблона, либо в условие управляющего блока, в зависимости от места вызова.

>
> ПРИМЕЧАНИЕ:
>
> Условие логической операции может включать описанные выше математические операции.
>

Логическое выражение поддерживает следующие типы сравнений:
- ```==``` - равно
- ```!=``` - не равно
- ```>```  - больше
- ```<```  - меньше
- ```>=``` - больше или равно
- ```<=``` - меньше или равно

Вы можете объединить несколько выражений, используя следующие операторы:
- ```and``` - возвращает true, если левый и правый операнды оба верны.
- ```or```  - возвращает true, если левый или правый операнд имеет значение true.
- ```not``` - отрицает утверждение.
- ```&&```  - аналогичен оператору ```and```
- ```||```  - аналогичен оператору ```or```

Логическое выражение поддерживает группировку выражений с помощью скобок ``` (...expr...) ```:
```twig
{% set a = 10.5 %}
{% set b = 2 %}
{% set aa = { a:1, b:2, c:3 } %}
{{ (a + aa.b) > 5 && (aa.b < 2 || aa.c >= 3) }} is true

{# function 'my_test_array()' return array '[1, 2, 3]' #}
{{ my_test_array () && a <= 5 }} is false
```

>
> ПРИМЕЧАНИЕ:
>
> Если в логическое выражение передается переменная (или объект), то условие верно, если переменная существует и не равна NULL:
>
> ```twig
> {% set a = 10.5 %}
> {{ a && a > 10 }} is true
> {{ b && a > 10 }} is false, because variable 'b' does not exist
> ```
>

## Тернарный оператор

Интерпретатор позволяет использовать тернарный оператор.

>
> ПРИМЕЧАНИЕ:
>
> Условие тернарного оператора поддерживает все возможности, предоставляемые описанными выше логическими операциями.
>

```twig
{{ foo ? 'yes' : 'no' }}
{{ foo ?: 'no' }} is the same as {{ foo ? foo : 'no' }}
{{ foo ? 'yes' }} is the same as {{ foo ? 'yes' : '' }}
```

Тернарный оператор поддерживает группировку выражений с помощью скобок ``` (...expr...) ```:
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

## Оператор NULL-Coalescing

Интерпретатор позволяет использовать оператор NULL-Coalescing.

>
> ПРИМЕЧАНИЕ:
>
> Условие оператора NULL-Coalescing поддерживает все возможности, предоставляемые логическими операциями, описанными выше.
>

```twig
{# returns the value of foo if it is defined and not null, 'no' otherwise #}
{{ foo ?? 'no' }}
```

NULL-Coalescing оператор поддерживает выражения группировки с помощью скобок ``` (...expr...) ```:
```twig
{% set a = 10.5 %}
{% set b = 2 %}
{% set aa = { a:1, b:2, c:3 } %}

{{ ((a + aa.b) > 5 && (aa.b < 2 || aa.c >= 3)) ?? 'no' }} is 'true'
{{ ((a + aa.b) > 5 && (aa.b < 2 || aa.c >= 3)) ?? (a - aa.b) }} is 'true'
```

## Комментарии

Чтобы закомментировать часть строки в шаблоне, используйте синтаксис комментария ``` {# ... #} ```.
При обработке шаблона все комментарии пропускаются и не добавляются в результат обработки.
Это полезно для отладки или добавления информации для других дизайнеров шаблонов или для себя:

```twig
{# this is a one line comment #}

{# 
this is 
a multiline 
comment
... 
#}
```

Если внутри блока комментариев есть вызовы переменных, функций или блоков управления, они также будут пропущены и не будут включены в результат обработки:

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

## Блоки управления

Блоки управления относятся ко всем тем вещам, которые управляют ходом программы — условному оператору (т.е. if/elseif/else), циклу for и прочие. Блоки управления описываются внутри тегов ``` {% ... %} ```.

Данная версия интерпретатора поддерживает следующие блоки управления:
- ```set``` - позволяет создать временную переменную
- ```unset``` - позволяет удалить временную переменную, созданную ранее
- ```if/elseif/else``` - блок условий
- ```for``` - блок цикла for

>
> ПРИМЕЧАНИЕ:
>
> Блоки управления могут также включать вложенные блоки управления, если это необходимо для описания логики:
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

### Блок управления 'set'

Блок управления ```set`` позволяет создавать временную переменную.
Синтаксис блока управления ```set``:

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

Пример:
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
> ПРИМЕЧАНИЕ:
>
> Вызов блоков управления с теми же ключами приведет к перезаписи значения новым.
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
> ПРИМЕЧАНИЕ:
>
> Выражение управляющего блока ```set``` можно перенести на новую строку. При разборе он будет объединен в одну строку с удалением всех переносов:
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

### Блок управления 'unset'

Блок управления ```unset``` позволяет удалить созданную ранее временную переменную.
Синтаксис блока управления ```unset```:

```twig
{% unset KEY %}
```

Пример:
```twig
{% set my_key = 'configure.svg' %}
![This is Lu and Bryu!]({{ image_path (my_key) }} "Lu and Bryu")
{% unset my_key %}
```

>
> ПРИМЕЧАНИЕ:
>
> Если переменная, имя которой передается в метод ```unset```, ранее не была создана или уже удалена,
> то операция будет проигнорирована.
>

### Блок управления 'if/elseif/else'

Блок управления ```if/elseif/else``` позволяет создавать блок(и) условий, при выполнении которых будет выполняться только его содержимое (или тело блока).
Синтаксис блока управления ```if/elseif/else```:

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
> ПРИМЕЧАНИЕ:
>
> Блок управления ```if/elseif/else``` всегда начинается с ```{% if ... %}``` и заканчивается ```{% endif %}```.
>

>
> ПРИМЕЧАНИЕ:
>
> Блок управления ```if/elseif/else``` поддерживает все возможности, предоставляемые логическими операциями, описанными выше.
>

>
> ПРИМЕЧАНИЕ:
>
> Если переменная (или объект) передается в условие блока управления, то условие является правильным, если переменная существует и не равна NULL:
>
> ```twig
> {% if my_key %}
> --- is OK ---
> {% else %}
> --- is NOT OK ---
> {% endif %}
> ```
>

Блок управления ```if/elseif/else``` поддерживает группировку выражений скобками ``` (...expr...) ```:
```twig
{% set a = 10.5 %}
{% set b = 2 %}
{% set aa = { a:1, b:2, c:3 } %}
{% if (a + aa.b) > 5 && (aa.b < 2 || aa.c >= 3) %}
--- THIS is IF BLOCK BODY ---
{% endif %}
```

Пример:
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

### Блок управления 'for'

Блок управления ```for``` позволяет создавать блок цикла for. Синтаксически он похож на цикл for в Python.
Синтаксис блока управления ```for```:

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
> ПРИМЕЧАНИЕ:
>
> Блок управления ```for``` всегда начинается с ```{% for ... %}``` и заканчивается ```{% endfor %}```.
>

Пример:

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

## Выполнение интерпретации шаблона

Для запуска интерпретации шаблона предусмотрены следующие функции:

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

Пример:

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

## Использование отдельных классов библиотеки интерпретатора

При работе может потребоваться использовать не весь интерпретатор целиком, а отдельные его части для разбора строкового представления данных.
Библиотека построена таким образом, что вы можете использовать следующие классы парсеров данных:
- ```QtTIParserMath``` - парсер математических операций
- ```QtTIParserLogic``` - парсер логических операций
- ```QtTIParserTernaryOperator``` - парсер троичных операторов
- ```QtTIParserNullCoalescingOperator``` - синтаксический анализатор операторов объединения по нулю.

Все эти классы расположены в соответствующих подкаталогах каталога ```QtTemplateInterpreter/QtTIParser/```.

Большинству этих классов требуются классы-контейнеры переменных и функций:
- ```QtTIParserArgs``` - класс контейнера переменных
- ```QtTIParserFunc``` - класс контейнера функций

Поэтому в случае использования отдельных классов парсера следует создавать классы-контейнеры самостоятельно и регистрировать именно в них пользовательские переменные и функции.

Пример:

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

Пример метода анализа строки, которая может содержать разные представления данных:

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
