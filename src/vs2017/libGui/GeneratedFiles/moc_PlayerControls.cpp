/****************************************************************************
** Meta object code from reading C++ file 'PlayerControls.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../libGui/PlayerControls.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'PlayerControls.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_PlayerControls_t {
    QByteArrayData data[23];
    char stringdata0[213];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_PlayerControls_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_PlayerControls_t qt_meta_stringdata_PlayerControls = {
    {
QT_MOC_LITERAL(0, 0, 14), // "PlayerControls"
QT_MOC_LITERAL(1, 15, 4), // "play"
QT_MOC_LITERAL(2, 20, 0), // ""
QT_MOC_LITERAL(3, 21, 5), // "pause"
QT_MOC_LITERAL(4, 27, 4), // "stop"
QT_MOC_LITERAL(5, 32, 4), // "next"
QT_MOC_LITERAL(6, 37, 8), // "previous"
QT_MOC_LITERAL(7, 46, 12), // "changeVolume"
QT_MOC_LITERAL(8, 59, 6), // "volume"
QT_MOC_LITERAL(9, 66, 12), // "changeMuting"
QT_MOC_LITERAL(10, 79, 6), // "muting"
QT_MOC_LITERAL(11, 86, 10), // "changeRate"
QT_MOC_LITERAL(12, 97, 4), // "rate"
QT_MOC_LITERAL(13, 102, 8), // "setState"
QT_MOC_LITERAL(14, 111, 19), // "QMediaPlayer::State"
QT_MOC_LITERAL(15, 131, 5), // "state"
QT_MOC_LITERAL(16, 137, 9), // "setVolume"
QT_MOC_LITERAL(17, 147, 8), // "setMuted"
QT_MOC_LITERAL(18, 156, 5), // "muted"
QT_MOC_LITERAL(19, 162, 15), // "setPlaybackRate"
QT_MOC_LITERAL(20, 178, 11), // "playClicked"
QT_MOC_LITERAL(21, 190, 11), // "muteClicked"
QT_MOC_LITERAL(22, 202, 10) // "updateRate"

    },
    "PlayerControls\0play\0\0pause\0stop\0next\0"
    "previous\0changeVolume\0volume\0changeMuting\0"
    "muting\0changeRate\0rate\0setState\0"
    "QMediaPlayer::State\0state\0setVolume\0"
    "setMuted\0muted\0setPlaybackRate\0"
    "playClicked\0muteClicked\0updateRate"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_PlayerControls[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      15,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       8,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   89,    2, 0x06 /* Public */,
       3,    0,   90,    2, 0x06 /* Public */,
       4,    0,   91,    2, 0x06 /* Public */,
       5,    0,   92,    2, 0x06 /* Public */,
       6,    0,   93,    2, 0x06 /* Public */,
       7,    1,   94,    2, 0x06 /* Public */,
       9,    1,   97,    2, 0x06 /* Public */,
      11,    1,  100,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      13,    1,  103,    2, 0x0a /* Public */,
      16,    1,  106,    2, 0x0a /* Public */,
      17,    1,  109,    2, 0x0a /* Public */,
      19,    1,  112,    2, 0x0a /* Public */,
      20,    0,  115,    2, 0x08 /* Private */,
      21,    0,  116,    2, 0x08 /* Private */,
      22,    0,  117,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    8,
    QMetaType::Void, QMetaType::Bool,   10,
    QMetaType::Void, QMetaType::QReal,   12,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 14,   15,
    QMetaType::Void, QMetaType::Int,    8,
    QMetaType::Void, QMetaType::Bool,   18,
    QMetaType::Void, QMetaType::Float,   12,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void PlayerControls::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        PlayerControls *_t = static_cast<PlayerControls *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->play(); break;
        case 1: _t->pause(); break;
        case 2: _t->stop(); break;
        case 3: _t->next(); break;
        case 4: _t->previous(); break;
        case 5: _t->changeVolume((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 6: _t->changeMuting((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 7: _t->changeRate((*reinterpret_cast< qreal(*)>(_a[1]))); break;
        case 8: _t->setState((*reinterpret_cast< QMediaPlayer::State(*)>(_a[1]))); break;
        case 9: _t->setVolume((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 10: _t->setMuted((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 11: _t->setPlaybackRate((*reinterpret_cast< float(*)>(_a[1]))); break;
        case 12: _t->playClicked(); break;
        case 13: _t->muteClicked(); break;
        case 14: _t->updateRate(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (PlayerControls::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&PlayerControls::play)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (PlayerControls::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&PlayerControls::pause)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (PlayerControls::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&PlayerControls::stop)) {
                *result = 2;
                return;
            }
        }
        {
            typedef void (PlayerControls::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&PlayerControls::next)) {
                *result = 3;
                return;
            }
        }
        {
            typedef void (PlayerControls::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&PlayerControls::previous)) {
                *result = 4;
                return;
            }
        }
        {
            typedef void (PlayerControls::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&PlayerControls::changeVolume)) {
                *result = 5;
                return;
            }
        }
        {
            typedef void (PlayerControls::*_t)(bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&PlayerControls::changeMuting)) {
                *result = 6;
                return;
            }
        }
        {
            typedef void (PlayerControls::*_t)(qreal );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&PlayerControls::changeRate)) {
                *result = 7;
                return;
            }
        }
    }
}

const QMetaObject PlayerControls::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_PlayerControls.data,
      qt_meta_data_PlayerControls,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *PlayerControls::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *PlayerControls::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_PlayerControls.stringdata0))
        return static_cast<void*>(const_cast< PlayerControls*>(this));
    return QWidget::qt_metacast(_clname);
}

int PlayerControls::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 15)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 15;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 15)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 15;
    }
    return _id;
}

// SIGNAL 0
void PlayerControls::play()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void PlayerControls::pause()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void PlayerControls::stop()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void PlayerControls::next()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void PlayerControls::previous()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void PlayerControls::changeVolume(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void PlayerControls::changeMuting(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void PlayerControls::changeRate(qreal _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
