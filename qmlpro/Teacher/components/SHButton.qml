import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Controls.Styles 1.4
import QtQuick.Controls 1.4

Rectangle {
    id : root
    property alias text: label.text
    signal clicked
    property int state: 0    //1:press  0:release
    width: 100
    height: 32
    border.width: root.activeFocus ? 2 : 1
    border.color: "#FF9A14"
    radius: 4
    color: "#FF9A14"
    //渐变
//    gradient: Gradient {
//      GradientStop { position: 0 ; color: root.clicked ? "#EDA848" : "#FF9A14" }
//      GradientStop { position: 1 ; color: root.clicked ? "#FF9A14" : "#EDA848" }
//    }
    Text {
        id:label
        anchors.centerIn: parent
        font.family: "Microsoft YaHei"
        font.pointSize: 14
        font.bold: false
        color: "#FFFFFF"
    }
    MouseArea {
        anchors.fill: parent
        onClicked: {
            root.clicked()
        }
        onPressed: {
           root.state = 1
        }
        onReleased: {
           root.state = 0
        }
    }
    onStateChanged: {
        root.color = root.state == 1 ? "#EDA848":"#FF9A14"
    }
}
