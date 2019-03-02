/****************************************************************************
** Meta object code from reading C++ file 'RunBrowseVideo.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../libRun/RunBrowseVideo.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'RunBrowseVideo.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_autocut__RunBrowseVideo_t {
    QByteArrayData data[6];
    char stringdata0[64];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_autocut__RunBrowseVideo_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_autocut__RunBrowseVideo_t qt_meta_stringdata_autocut__RunBrowseVideo = {
    {
QT_MOC_LITERAL(0, 0, 23), // "autocut::RunBrowseVideo"
QT_MOC_LITERAL(1, 24, 9), // "sigHasErr"
QT_MOC_LITERAL(2, 34, 0), // ""
QT_MOC_LITERAL(3, 35, 6), // "errMsg"
QT_MOC_LITERAL(4, 42, 10), // "sigDispImg"
QT_MOC_LITERAL(5, 53, 10) // "sigDexFrms"

    },
    "autocut::RunBrowseVideo\0sigHasErr\0\0"
    "errMsg\0sigDispImg\0sigDexFrms"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_autocut__RunBrowseVideo[] = {

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
       4,    0,   32,    2, 0x06 /* Public */,
       5,    0,   33,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void autocut::RunBrowseVideo::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        RunBrowseVideo *_t = static_cast<RunBrowseVideo *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->sigHasErr((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->sigDispImg(); break;
        case 2: _t->sigDexFrms(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (RunBrowseVideo::*_t)(const QString & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&RunBrowseVideo::sigHasErr)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (RunBrowseVideo::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&RunBrowseVideo::sigDispImg)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (RunBrowseVideo::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&RunBrowseVideo::sigDexFrms)) {
                *result = 2;
                return;
            }
        }
    }
}

const QMetaObject autocut::RunBrowseVideo::staticMetaObject = {
    { &RunThread::staticMetaObject, qt_meta_stringdata_autocut__RunBrowseVideo.data,
      qt_meta_data_autocut__RunBrowseVideo,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *autocut::RunBrowseVideo::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *autocut::RunBrowseVideo::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_autocut__RunBrowseVideo.stringdata0))
        return static_cast<void*>(const_cast< RunBrowseVideo*>(this));
    return RunThread::qt_metacast(_clname);
}

int autocut::RunBrowseVideo::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
void autocut::RunBrowseVideo::sigHasErr(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void autocut::RunBrowseVideo::sigDispImg()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void autocut::RunBrowseVideo::sigDexFrms()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
