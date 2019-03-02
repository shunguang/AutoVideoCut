/****************************************************************************
** Meta object code from reading C++ file 'Worker.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../libRun/Worker.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'Worker.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_autocut__Worker_t {
    QByteArrayData data[8];
    char stringdata0[77];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_autocut__Worker_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_autocut__Worker_t qt_meta_stringdata_autocut__Worker = {
    {
QT_MOC_LITERAL(0, 0, 15), // "autocut::Worker"
QT_MOC_LITERAL(1, 16, 9), // "sigHasErr"
QT_MOC_LITERAL(2, 26, 0), // ""
QT_MOC_LITERAL(3, 27, 6), // "errMsg"
QT_MOC_LITERAL(4, 34, 11), // "sigProgress"
QT_MOC_LITERAL(5, 46, 3), // "msg"
QT_MOC_LITERAL(6, 50, 15), // "sigLastFrmDexed"
QT_MOC_LITERAL(7, 66, 10) // "sigJobDone"

    },
    "autocut::Worker\0sigHasErr\0\0errMsg\0"
    "sigProgress\0msg\0sigLastFrmDexed\0"
    "sigJobDone"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_autocut__Worker[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   34,    2, 0x06 /* Public */,
       4,    1,   37,    2, 0x06 /* Public */,
       6,    0,   40,    2, 0x06 /* Public */,
       7,    0,   41,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::QString,    5,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void autocut::Worker::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Worker *_t = static_cast<Worker *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->sigHasErr((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->sigProgress((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: _t->sigLastFrmDexed(); break;
        case 3: _t->sigJobDone(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (Worker::*_t)(const QString & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Worker::sigHasErr)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (Worker::*_t)(const QString & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Worker::sigProgress)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (Worker::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Worker::sigLastFrmDexed)) {
                *result = 2;
                return;
            }
        }
        {
            typedef void (Worker::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Worker::sigJobDone)) {
                *result = 3;
                return;
            }
        }
    }
}

const QMetaObject autocut::Worker::staticMetaObject = {
    { &RunThread::staticMetaObject, qt_meta_stringdata_autocut__Worker.data,
      qt_meta_data_autocut__Worker,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *autocut::Worker::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *autocut::Worker::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_autocut__Worker.stringdata0))
        return static_cast<void*>(const_cast< Worker*>(this));
    return RunThread::qt_metacast(_clname);
}

int autocut::Worker::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = RunThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void autocut::Worker::sigHasErr(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void autocut::Worker::sigProgress(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void autocut::Worker::sigLastFrmDexed()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void autocut::Worker::sigJobDone()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
