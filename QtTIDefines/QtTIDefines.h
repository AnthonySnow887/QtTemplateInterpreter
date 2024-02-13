#ifndef QTTIDEFINES_H
#define QTTIDEFINES_H

#define Q_ASSERT_WITH_MSG(cond, message) ((!(cond)) ? qt_assert_x(#cond,qPrintable(message),__FILE__,__LINE__) : qt_noop())

#endif // QTTIDEFINES_H
