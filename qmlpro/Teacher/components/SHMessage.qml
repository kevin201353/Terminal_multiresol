import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Controls.Styles 1.4
import QtQuick.Controls 1.4

Item {
    property alias text: msg.text
    width: 100
    height: 80
    Rectangle {
        id : control
        anchors.fill: parent
        radius: 2
        opacity: 0
        Button {
            text : "OK"
            width: 30
            height: 20
            anchors {
                centerIn: parent
            }
            onClicked: {
                control.opacity = 0
            }
        }

        Text {
            id: msg
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            font.pointSize:  10
        }
    }
}
