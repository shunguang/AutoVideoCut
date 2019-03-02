/****************************************************************************
** Meta object code from reading C++ file 'RunMuxEnc.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.8.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../libRun/RunMuxEnc.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'RunMuxEnc.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.8.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_autocut__RunAutoCutCore_t {
    QByteArrayData data[7];
    char stringdata0[69];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_autocut__RunAutoCutCore_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_autocut__RunAutoCutCore_t qt_meta_stringdata_autocut__RunAutoCutCore = {
    {
QT_MOC_LITERAL(0, 0, 23), // "autocut::RunAutoCutCore"
QT_MOC_LITERAL(1, 24, 9), // "sigHasErr"
QT_MOC_LITERAL(2, 34, 0), // ""
QT_MOC_LITERAL(3, 35, 6), // "errMsg"
QT_MOC_LITERAL(4, 42, 11), // "sigProgress"
QT_MOC_LITERAL(5, 54, 3), // "msg"
QT_MOC_LITERAL(6, 58, 10) // "sigJobDone"

    },
    "autocut::RunAutoCutCore\0sigHasErr\0\0"
    "errMsg\0sigProgress\0msg\0sigJobDone"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_autocut__RunAutoCutCore[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   29,    2, 0x06 /* Public */,
       4,    1,   32,    2, 0x06 /* Public */,
       6,    0,   35,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::QString,    5,
    QMetaType::Void,

       0        // eod
};

void autocut::RunAutoCutCore::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        RunAutoCutCore *_t = static_cast<RunAutoCutCore *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->sigHasErr((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->sigProgress((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: _t->sigJobDone(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (RunAutoCutCore::*_t)(const QString & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&RunAutoCutCore::sigHasErr)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (RunAutoCutCore::*_t)(const QString & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&RunAutoCutCore::sigProgress)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (RunAutoCutCore::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&RunAutoCutCore::sigJobDone)) {
                *result = 2;
                return;
            }
        }
    }
}

const QMetaObject autocut::RunAutoCutCore::staticMetaObject = {
    { &RunThread::staticMetaObject, qt_meta_stringdata_autocut__RunAutoCutCore.data,
      qt_meta_data_autocut__RunAutoCutCore,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *autocut::RunAutoCutCore::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *autocut::RunAutoCutCore::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_autocut__RunAutoCutCore.stringdata0))
        return static_cast<void*>(const_cast< RunAutoCutCore*>(this));
    return RunThread::qt_metacast(_clname);
}

int autocut::RunAutoCutCore::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = RunThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void autocut::RunAutoCutCore::sigHasErr(const QString & _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void autocut::RunAutoCutCore::sigProgress(const QString & _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void autocut::RunAutoCutCore::sigJobDone()
{
    QMetaObject::activate(this, &staticMetaObject, 2, Q_NULLPTR);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
