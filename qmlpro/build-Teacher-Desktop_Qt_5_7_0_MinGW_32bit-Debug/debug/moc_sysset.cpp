/****************************************************************************
** Meta object code from reading C++ file 'sysset.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.7.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../Teacher/sysset.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'sysset.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.7.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Sysset_t {
    QByteArrayData data[21];
    char stringdata0[207];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Sysset_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Sysset_t qt_meta_stringdata_Sysset = {
    {
QT_MOC_LITERAL(0, 0, 6), // "Sysset"
QT_MOC_LITERAL(1, 7, 16), // "teacheripChanged"
QT_MOC_LITERAL(2, 24, 0), // ""
QT_MOC_LITERAL(3, 25, 13), // "seatnoChanged"
QT_MOC_LITERAL(4, 39, 13), // "isdhcpChanged"
QT_MOC_LITERAL(5, 53, 14), // "isdhcp2Changed"
QT_MOC_LITERAL(6, 68, 12), // "netipChanged"
QT_MOC_LITERAL(7, 81, 11), // "maskChanged"
QT_MOC_LITERAL(8, 93, 14), // "gatewayChanged"
QT_MOC_LITERAL(9, 108, 11), // "dns1Changed"
QT_MOC_LITERAL(10, 120, 11), // "dns2Changed"
QT_MOC_LITERAL(11, 132, 13), // "slot_save_acq"
QT_MOC_LITERAL(12, 146, 9), // "teacherip"
QT_MOC_LITERAL(13, 156, 6), // "seatno"
QT_MOC_LITERAL(14, 163, 6), // "isdhcp"
QT_MOC_LITERAL(15, 170, 7), // "isdhcp2"
QT_MOC_LITERAL(16, 178, 5), // "netip"
QT_MOC_LITERAL(17, 184, 4), // "mask"
QT_MOC_LITERAL(18, 189, 7), // "gateway"
QT_MOC_LITERAL(19, 197, 4), // "dns1"
QT_MOC_LITERAL(20, 202, 4) // "dns2"

    },
    "Sysset\0teacheripChanged\0\0seatnoChanged\0"
    "isdhcpChanged\0isdhcp2Changed\0netipChanged\0"
    "maskChanged\0gatewayChanged\0dns1Changed\0"
    "dns2Changed\0slot_save_acq\0teacherip\0"
    "seatno\0isdhcp\0isdhcp2\0netip\0mask\0"
    "gateway\0dns1\0dns2"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Sysset[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       9,   74, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       9,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   64,    2, 0x06 /* Public */,
       3,    0,   65,    2, 0x06 /* Public */,
       4,    0,   66,    2, 0x06 /* Public */,
       5,    0,   67,    2, 0x06 /* Public */,
       6,    0,   68,    2, 0x06 /* Public */,
       7,    0,   69,    2, 0x06 /* Public */,
       8,    0,   70,    2, 0x06 /* Public */,
       9,    0,   71,    2, 0x06 /* Public */,
      10,    0,   72,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      11,    0,   73,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,

 // properties: name, type, flags
      12, QMetaType::QString, 0x00495103,
      13, QMetaType::QString, 0x00495103,
      14, QMetaType::Int, 0x00495103,
      15, QMetaType::Int, 0x00495103,
      16, QMetaType::QString, 0x00495103,
      17, QMetaType::QString, 0x00495103,
      18, QMetaType::QString, 0x00495103,
      19, QMetaType::QString, 0x00495103,
      20, QMetaType::QString, 0x00495103,

 // properties: notify_signal_id
       0,
       1,
       2,
       3,
       4,
       5,
       6,
       7,
       8,

       0        // eod
};

void Sysset::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Sysset *_t = static_cast<Sysset *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->teacheripChanged(); break;
        case 1: _t->seatnoChanged(); break;
        case 2: _t->isdhcpChanged(); break;
        case 3: _t->isdhcp2Changed(); break;
        case 4: _t->netipChanged(); break;
        case 5: _t->maskChanged(); break;
        case 6: _t->gatewayChanged(); break;
        case 7: _t->dns1Changed(); break;
        case 8: _t->dns2Changed(); break;
        case 9: _t->slot_save_acq(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (Sysset::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Sysset::teacheripChanged)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (Sysset::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Sysset::seatnoChanged)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (Sysset::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Sysset::isdhcpChanged)) {
                *result = 2;
                return;
            }
        }
        {
            typedef void (Sysset::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Sysset::isdhcp2Changed)) {
                *result = 3;
                return;
            }
        }
        {
            typedef void (Sysset::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Sysset::netipChanged)) {
                *result = 4;
                return;
            }
        }
        {
            typedef void (Sysset::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Sysset::maskChanged)) {
                *result = 5;
                return;
            }
        }
        {
            typedef void (Sysset::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Sysset::gatewayChanged)) {
                *result = 6;
                return;
            }
        }
        {
            typedef void (Sysset::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Sysset::dns1Changed)) {
                *result = 7;
                return;
            }
        }
        {
            typedef void (Sysset::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Sysset::dns2Changed)) {
                *result = 8;
                return;
            }
        }
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        Sysset *_t = static_cast<Sysset *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QString*>(_v) = _t->teacherip(); break;
        case 1: *reinterpret_cast< QString*>(_v) = _t->seatno(); break;
        case 2: *reinterpret_cast< int*>(_v) = _t->isdhcp(); break;
        case 3: *reinterpret_cast< int*>(_v) = _t->isdhcp2(); break;
        case 4: *reinterpret_cast< QString*>(_v) = _t->netip(); break;
        case 5: *reinterpret_cast< QString*>(_v) = _t->mask(); break;
        case 6: *reinterpret_cast< QString*>(_v) = _t->gateway(); break;
        case 7: *reinterpret_cast< QString*>(_v) = _t->dns1(); break;
        case 8: *reinterpret_cast< QString*>(_v) = _t->dns2(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        Sysset *_t = static_cast<Sysset *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setTeacherip(*reinterpret_cast< QString*>(_v)); break;
        case 1: _t->setSeatno(*reinterpret_cast< QString*>(_v)); break;
        case 2: _t->setIsdhcp(*reinterpret_cast< int*>(_v)); break;
        case 3: _t->setIsdhcp2(*reinterpret_cast< int*>(_v)); break;
        case 4: _t->setNetip(*reinterpret_cast< QString*>(_v)); break;
        case 5: _t->setMask(*reinterpret_cast< QString*>(_v)); break;
        case 6: _t->setGateway(*reinterpret_cast< QString*>(_v)); break;
        case 7: _t->setDns1(*reinterpret_cast< QString*>(_v)); break;
        case 8: _t->setDns2(*reinterpret_cast< QString*>(_v)); break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
    Q_UNUSED(_a);
}

const QMetaObject Sysset::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Sysset.data,
      qt_meta_data_Sysset,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Sysset::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Sysset::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Sysset.stringdata0))
        return static_cast<void*>(const_cast< Sysset*>(this));
    return QObject::qt_metacast(_clname);
}

int Sysset::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 10)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 10;
    }
#ifndef QT_NO_PROPERTIES
   else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 9;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 9;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 9;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 9;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 9;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void Sysset::teacheripChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, Q_NULLPTR);
}

// SIGNAL 1
void Sysset::seatnoChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, Q_NULLPTR);
}

// SIGNAL 2
void Sysset::isdhcpChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 2, Q_NULLPTR);
}

// SIGNAL 3
void Sysset::isdhcp2Changed()
{
    QMetaObject::activate(this, &staticMetaObject, 3, Q_NULLPTR);
}

// SIGNAL 4
void Sysset::netipChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 4, Q_NULLPTR);
}

// SIGNAL 5
void Sysset::maskChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 5, Q_NULLPTR);
}

// SIGNAL 6
void Sysset::gatewayChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 6, Q_NULLPTR);
}

// SIGNAL 7
void Sysset::dns1Changed()
{
    QMetaObject::activate(this, &staticMetaObject, 7, Q_NULLPTR);
}

// SIGNAL 8
void Sysset::dns2Changed()
{
    QMetaObject::activate(this, &staticMetaObject, 8, Q_NULLPTR);
}
QT_END_MOC_NAMESPACE
