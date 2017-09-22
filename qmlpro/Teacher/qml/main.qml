import QtQuick 2.7
import QtQuick.Window 2.2
import QtGraphicalEffects 1.0
import QtQuick.Controls 2.0
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.3
import "../components"

Rectangle {
    property double dpi: Screen.devicePixelRatio
    id: mainRect
    width: 650 * dpi
    height: 740 * dpi
    visible: true
    //灰色0.9透明度
    color:Qt.rgba(0.5,0.5,0.5,0.9)
    MouseArea{
           id: dragRegion
           width: parent.width - 4*dpi
           height: parent.height - 4*dpi
           anchors.fill: parent
           property point clickPos:"0,0"
           onPressed:{
               clickPos = Qt.point(mouse.x,mouse.y)
           }
           onPositionChanged:{
               //鼠标偏移量
               var delta = Qt.point(mouse.x-clickPos.x, mouse.y-clickPos.y)
               //如果mainwindow继承自QWidget,用setPos
               mainwindow.setX(mainwindow.x+delta.x)
               mainwindow.setY(mainwindow.y+delta.y)
           }
    }
    Rectangle {
        id: title
        width: parent.width
        height: 48 * dpi
        color: "#FF9A14"
        anchors.top: parent.top
        Text {
             id: tltext
             text: "学生终端配置"
             font.family : "Microsoft YaHei"
             font.pointSize: 15
             x: 20 * dpi
             y: 10 * dpi
             color : "#FDFBF9"
        }
        Button {
            x:parent.width*dpi -width*dpi - 20*dpi
            y: 15 *dpi
            id : close
            style: ButtonStyle{
                background: Image {
                    id : close_tl
                    source : "../image/close.png"
                    fillMode: Image.PreserveAspectFit
                    smooth: true
                }
            }
            onClicked:{
                mainwindow.close();
            }
        }
    }
    Rectangle {
        id: left
        width: 30
        height: parent.height
        color: "#FFFFFF"
        anchors.top: title.bottom
        anchors.left: parent.left
    }
    Rectangle {
        id: right
        width: 30
        height: parent.height
        color: "#FFFFFF"
        anchors.top: title.bottom
        anchors.right: parent.right
    }

    Rectangle {
        id: tool
        width: parent.width
        height: 40
        color: "#FFFFFF"
        anchors.top: title.bottom
        anchors.left: left.right
        anchors.right: right.left
        RowLayout {
              id: toolbtn
              x: parent.width/2 *dpi - width/2 *dpi
              y: parent.height/2 *dpi - height/2 *dpi
              spacing: 20
              Button {
                  //text: "系统设置"
                  style: ButtonStyle {
                    background : Text {
                            id: txt_sysset
                            anchors.fill: parent
                            font.family: "Microsoft YaHei";
                            font.pointSize: 14;
                            font.bold: false
                            text: "系统设置"
                        }
                  }
                  onClicked:{
                      pageLoader.source = "sysset.qml"
                   }
              }
              Button {
                  //text: "网络检测"
                  style: ButtonStyle {
                    background : Text {
                            id: txt_netcheck
                            anchors.fill: parent
                            font.family: "Microsoft YaHei";
                            font.pointSize: 14;
                            font.bold: false
                            text: "网络检测"
                        }
                  }
                  onClicked: {
                        pageLoader.source = "netset.qml"
                  }
              }
        }
    }
    Rectangle {
        id: bottom
        width: parent.width
        height: 60
        color: "#FFFFFF"
        anchors.bottom: parent.bottom
        SHButton {
            id: ok
            text: "确定"
            x: bottom.width - width*2 - 47 - 47
            y: bottom.height/2 - height/2
        }
        SHButton {
            id: cancel
            text: "取消"
            x: bottom.width -width - 47
            y: bottom.height/2 - height/2
        }
    }
    Rectangle {
        id: context
        color: "#FFFFFF"
        anchors.top: tool.bottom
        anchors.left: left.right
        anchors.right: right.left
        anchors.bottom: bottom.top
        Loader {
            id: pageLoader
        }
    }

    AnimatedImage {
        id: load_gif
        source: "file:///usr/local/shencloud/image1/Loaging.gif"
        anchors {
            horizontalCenter: parent.horizontalCenter
            verticalCenter: parent.verticalCenter
        }
        z: 1
        opacity: 0
    }

    Component.onCompleted :{
        pageLoader.source = "sysset.qml"
    }

    Connections {
        target : ok
        onClicked : {
            load_gif.opacity = 1
            pageLoader.item.initdata()
        }
    }

    Connections {
        target: pageLoader.item
        onSetMsg : {
            switch(result)
            {
                case 0:
                    load_gif.opacity = 0
                    break
                case 1:
                    load_gif.opacity = 0
                    console.log("save data success .......")
                    break
            }
        }
    }
}
