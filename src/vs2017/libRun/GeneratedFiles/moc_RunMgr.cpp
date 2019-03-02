/****************************************************************************
** Meta object code from reading C++ file 'RunMgr.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../libRun/RunMgr.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'RunMgr.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_autocut__RunMgr_t {
    QByteArrayData data[10];
    char stringdata0[112];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_autocut__RunMgr_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_autocut__RunMgr_t qt_meta_stringdata_autocut__RunMgr = {
    {
QT_MOC_LITERAL(0, 0, 15), // "autocut::RunMgr"
QT_MOC_LITERAL(1, 16, 9), // "sigHasErr"
QT_MOC_LITERAL(2, 26, 0), // ""
QT_MOC_LITERAL(3, 27, 6), // "errMsg"
QT_MOC_LITERAL(4, 34, 11), // "sigProgress"
QT_MOC_LITERAL(5, 46, 3), // "msg"
QT_MOC_LITERAL(6, 50, 11), // "sigAllDexed"
QT_MOC_LITERAL(7, 62, 14), // "sigAllAnalyzed"
QT_MOC_LITERAL(8, 77, 16), // "sigWrtOutputDone"
QT_MOC_LITERAL(9, 94, 17) // "wakeupWorkerToDex"

    },
    "autocut::RunMgr\0sigHasErr\0\0errMsg\0"
    "sigProgress\0msg\0sigAllDexed\0sigAllAnalyzed\0"
    "sigWrtOutputDone\0wakeupWorkerToDex"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_autocut__RunMgr[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       5,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   44,    2, 0x06 /* Public */,
       4,    1,   47,    2, 0x06 /* Public */,
       6,    1,   50,    2, 0x06 /* Public */,
       7,    1,   53,    2, 0x06 /* Public */,
       8,    1,   56,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       9,    0,   59,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::QString,    5,
    QMetaType::Void, QMetaType::QString,    5,
    QMetaType::Void, QMetaType::QString,    5,
    QMetaType::Void, QMetaType::QString,    5,

 // slots: parameters
    QMetaType::Void,

       0        // eod
};

void autocut::RunMgr::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        RunMgr *_t = static_cast<RunMgr *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->sigHasErr((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->sigProgress((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: _t->sigAllDexed((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 3: _t->sigAllAnalyzed((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 4: _t->sigWrtOutputDone((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 5: _t->wakeupWorkerToDex(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (RunMgr::*_t)(const QString & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&RunMgr::sigHasErr)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (RunMgr::*_t)(const QString & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&RunMgr::sigProgress)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (RunMgr::*_t)(const QString & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&RunMgr::sigAllDexed)) {
                *result = 2;
                return;
            }
        }
        {
            typedef void (RunMgr::*_t)(const QString & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&RunMgr::sigAllAnalyzed)) {
                *result = 3;
                return;
            }
        }
        {
            typedef void (RunMgr::*_t)(const QString & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&RunMgr::sigWrtOutputDone)) {
                *result = 4;
                return;
            }
        }
    }
}

const QMetaObject autocut::RunMgr::staticMetaObject = {
    { &RunThread::staticMetaObject, qt_meta_stringdata_autocut__RunMgr.data,
      qt_meta_data_autocut__RunMgr,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *autocut::RunMgr::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *autocut::RunMgr::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_autocut__RunMgr.stringdata0))
        return static_cast<void*>(const_cast< RunMgr*>(this));
    return RunThread::qt_metacast(_clname);
}

int autocut::RunMgr::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = RunThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void autocut::RunMgr::sigHasErr(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void autocut::RunMgr::sigProgress(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void autocut::RunMgr::sigAllDexed(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void autocut::RunMgr::sigAllAnalyzed(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void autocut::RunMgr::sigWrtOutputDone(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
