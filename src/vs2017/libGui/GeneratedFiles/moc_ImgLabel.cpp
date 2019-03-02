/****************************************************************************
** Meta object code from reading C++ file 'ImgLabel.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../libGui/ImgLabel.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ImgLabel.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_autocut__ImgLabel_t {
    QByteArrayData data[6];
    char stringdata0[63];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_autocut__ImgLabel_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_autocut__ImgLabel_t qt_meta_stringdata_autocut__ImgLabel = {
    {
QT_MOC_LITERAL(0, 0, 17), // "autocut::ImgLabel"
QT_MOC_LITERAL(1, 18, 15), // "sigRectSelected"
QT_MOC_LITERAL(2, 34, 0), // ""
QT_MOC_LITERAL(3, 35, 3), // "roi"
QT_MOC_LITERAL(4, 39, 20), // "sigLeftButtonPressed"
QT_MOC_LITERAL(5, 60, 2) // "pt"

    },
    "autocut::ImgLabel\0sigRectSelected\0\0"
    "roi\0sigLeftButtonPressed\0pt"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_autocut__ImgLabel[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   24,    2, 0x06 /* Public */,
       4,    1,   27,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QRect,    3,
    QMetaType::Void, QMetaType::QPoint,    5,

       0        // eod
};

void autocut::ImgLabel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        ImgLabel *_t = static_cast<ImgLabel *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->sigRectSelected((*reinterpret_cast< const QRect(*)>(_a[1]))); break;
        case 1: _t->sigLeftButtonPressed((*reinterpret_cast< const QPoint(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (ImgLabel::*_t)(const QRect & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ImgLabel::sigRectSelected)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (ImgLabel::*_t)(const QPoint & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ImgLabel::sigLeftButtonPressed)) {
                *result = 1;
                return;
            }
        }
    }
}

const QMetaObject autocut::ImgLabel::staticMetaObject = {
    { &QLabel::staticMetaObject, qt_meta_stringdata_autocut__ImgLabel.data,
      qt_meta_data_autocut__ImgLabel,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *autocut::ImgLabel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *autocut::ImgLabel::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_autocut__ImgLabel.stringdata0))
        return static_cast<void*>(const_cast< ImgLabel*>(this));
    return QLabel::qt_metacast(_clname);
}

int autocut::ImgLabel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QLabel::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void autocut::ImgLabel::sigRectSelected(const QRect & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void autocut::ImgLabel::sigLeftButtonPressed(const QPoint & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
