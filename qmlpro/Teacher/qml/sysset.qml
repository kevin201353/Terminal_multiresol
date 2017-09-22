import QtQuick 2.7
import QtQuick.Window 2.2
import QtGraphicalEffects 1.0
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.0
import "../components"
import Sysset 2.0


Rectangle {
    id : mainsys
    property double dpi: Screen.devicePixelRatio
    property var reg: /((25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9])[\\.]){3}(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9])/
    width: 650 * dpi  - 60 *dpi
    height: 590 * dpi
    visible: true
    //灰色0.9透明度
    color:Qt.rgba(0.5,0.5,0.5,0.9)
    signal setMsg(int result)
    Sysset {
        id : syssetid
        onSaveResult: {
           mainsys.setMsg(result)
        }
    }
    Rectangle {
        id: sys
        width: parent.width
        height: parent.height/4 - 10
        color: "#FFFFFF"
        anchors.top: parent.top
        GroupBox {
            font.family: "微软雅黑"
            font.pointSize: 14
            title: qsTr("终端设置")
            anchors.fill: parent
            width: parent.width - 4
            Label {
                font.family: "微软雅黑"
                font.pointSize: 12
                id: ip_label
                x: 192
                y: 13
                text: qsTr("教师机IP")
            }
            SHTextArea {
                id: ip_edit
                x : 192 + ip_label.width + 15
                y : 4
                text : syssetid.teacherip
                validator: RegExpValidator{regExp: reg }
            }
            Label {
                font.family: "微软雅黑"
                font.pointSize: 12
                id: seatNo
                x: 192
                y: ip_label.y + ip_label.height + 18
                text: qsTr("座位号")
            }
            SHTextArea {
                id: seat_edit
                x : 192 + ip_label.width + 15
                y : ip_edit.y + ip_edit.height + 15
                text : syssetid.seatno
                validator: RegExpValidator{regExp: reg }
            }
       }//GroupBox 系统设置
    }

    Rectangle {
        id: net
        width: parent.width
        height: parent.height - parent.height/4
        color: "#FFFFFF"
        anchors.top: sys.bottom
        anchors.bottom: parent.bottom
        GroupBox {
            font.family: "微软雅黑"
            font.pointSize: 14
            title: qsTr("网络设置")
            anchors.fill: parent
            width: parent.width - 4

            Label {
                font.family: "微软雅黑"
                font.pointSize: 12
                id: ip_set
                x: 130
                y: parent.top + 38
                text: qsTr("IP设置")
            }

            ColumnLayout {
                id :colra
                x: ip_set.x + ip_set.width + 57
                y: ip_set.y
                spacing:2
                SHRadioButton {
                    id:dhcpip
                    font.family: "微软雅黑"
                    font.pointSize: 12
                    checked: true
                    text: qsTr("DHCP动态获取")
                    onCheckedChanged: disablenetEdit()
                }
                SHRadioButton {
                    id:staticip
                    font.family: "微软雅黑"
                    font.pointSize: 12
                    text: qsTr("静态获取")
                    checked: false
                }
            }

            Label {
                font.family: "微软雅黑"
                font.pointSize: 12
                id: netaddr_label
                x: colra.x
                y: colra.y + colra.height + 18
                text: qsTr("网络地址")
            }
            SHTextArea {
                id: netaddr_edit
                x : netaddr_label.x + netaddr_label.width + 15
                y : netaddr_label.y - 5
                text: syssetid.netip
                validator: RegExpValidator{regExp: reg }
            }

            Label {
                font.family: "微软雅黑"
                font.pointSize: 12
                id: mask_label
                x: netaddr_label.x
                y: netaddr_label.y + netaddr_label.height + 18
                text: qsTr("子网掩码")
            }
            SHTextArea {
                id: mask_edit
                x : mask_label.x + mask_label.width + 15
                y : mask_label.y - 5
                text : syssetid.mask
                validator: RegExpValidator{regExp: reg }
            }

            Label {
                font.family: "微软雅黑"
                font.pointSize: 12
                id: gate_label
                x: mask_label.x
                y: mask_label.y + mask_label.height + 18
                text: qsTr("网关地址")
            }
            SHTextArea {
                id: gate_edit
                x : gate_label.x + gate_label.width + 15
                y : gate_label.y - 5
                text: syssetid.gateway
                validator: RegExpValidator{regExp: reg }
            }

            //分隔
            Rectangle {
                id : split
                x: 10
                y: gate_label.y + gate_label.height + 18
                width: parent.width - 20;
                height: 2
                color : "#404244"
            }

            Label {
                font.family: "微软雅黑"
                font.pointSize: 12
                id: dns_set
                x: 130
                y: split.y + split.height + 18
                text: qsTr("DNS设置")
            }
            ColumnLayout {
                id :colra2
                x: dns_set.x + dns_set.width + 32
                y: dns_set.y
                spacing:2
                SHRadioButton {
                    id:dhcpdns
                    font.family: "微软雅黑"
                    font.pointSize: 12
                    checked: true
                    text: qsTr("DHCP动态获取")
                    onCheckedChanged: disabeldnsEdit()
                }
                SHRadioButton {
                    id:statidns
                    font.family: "微软雅黑"
                    font.pointSize: 12
                    text: qsTr("静态获取")
                    checked: false
                }
            }

            Label {
                font.family: "微软雅黑"
                font.pointSize: 12
                id: dns1_label
                x: colra2.x
                y: colra2.y + colra2.height + 18
                text: qsTr("DNS1")
            }
            SHTextArea {
                id: dns1_edit
                x : dns1_label.x + dns1_label.width + 15
                y : dns1_label.y - 5
                text: syssetid.dns1
                validator: RegExpValidator{regExp: reg }
            }
            Label {
                font.family: "微软雅黑"
                font.pointSize: 12
                id: dns2_label
                x: dns1_label.x
                y: dns1_label.y + dns1_label.height + 18
                text: qsTr("DNS2")
            }
            SHTextArea {
                id: dns2_edit
                x : dns2_label.x + dns2_label.width + 15
                y : dns2_label.y - 5
                text: syssetid.dns2
                validator: RegExpValidator{regExp: reg }
            }
        }//groupbox
    }

    SHMessage {
        id: message
        anchors.verticalCenter: parent.verticalCenter
        anchors.horizontalCenter: parent.horizontalCenter
        opacity: 0
    }

    Component.onCompleted : {
        dhcpip.checked = true
        dhcpdns.checked = true
        disablenetEdit()
        disabeldnsEdit()

    }
    function initdata()
    {
        console.log("sysset initdata .......")
        if (netaddr_edit.text == "")
        {
            message.text = "ip is NULL"
            message.opacity = 1
        }
        if (mask_edit.text == "")
        {
            message.text = "ip is NULL"
            message.opacity = 1
        }
        if (gate_edit.text == "")
        {
            message.text = "gate is NULL"
            message.opacity = 1
        }
        if (dns1_edit.text == "")
        {
            message.text = "dns1 is NULL"
            message.opacity = 1
        }
        syssetid.teacherip = ip_edit.text
        syssetid.seatno = seat_edit.text
        syssetid.netip = netaddr_edit.text
        syssetid.mask = mask_edit.text
        syssetid.gateway = gate_edit.text
        syssetid.dns1 = dns1_edit.text
        syssetid.dns2 = dns2_edit.text
        syssetid.isdhcp = dhcpip.checked == true ? 1 : 0
        syssetid.isdhcp2 = dhcpdns.checked == true ? 1 : 0
        syssetid.slot_save_acq()
    }

    function disablenetEdit()
    {
        if (dhcpip.checked == true)
        {
            netaddr_edit.enabled = false
            mask_edit.enabled = false
            gate_edit.enabled = false
            netaddr_edit.clear()
            mask_edit.clear()
            gate_edit.clear()
        }else
        {
            netaddr_edit.enabled = true
            mask_edit.enabled = true
            gate_edit.enabled = true
        }

    }

    function disabeldnsEdit()
    {
        if (dhcpdns.checked == true)
        {
            dns1_edit.enabled = false
            dns2_edit.enabled = false
            dns1_edit.clear()
            dns2_edit.clear()
        }else
        {
            dns1_edit.enabled = true
            dns2_edit.enabled = true
        }
    }
}
