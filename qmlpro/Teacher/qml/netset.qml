import QtQuick 2.7
import QtQuick.Window 2.2
import QtGraphicalEffects 1.0
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.0
import "../components"

Item {
    id :item
    property double dpi: Screen.devicePixelRatio
    width: 650 * dpi  - 60 *dpi
    height: 590 * dpi
    visible: true
    Rectangle {
        anchors.fill: parent
        color: "#FFFFFF"
        GroupBox {
            font.family: "微软雅黑"
            font.pointSize: 14
            title: qsTr("网络检测")
            anchors.fill: parent
            SHTextArea {
                id: ip_net
                x : 90
                y : 29
            }
            SHButton {
                id: netcheck
                text: "检测"
                x: ip_net.x + ip_net.width + 113
                y: ip_net.y - 2
            }

            SHListView1 {
                id: ping
                x: 81
                y: netcheck.y + netcheck.height + 27
                width: item.width - x * 2
                height: item.height - y *2
            }
        }
    }
}
