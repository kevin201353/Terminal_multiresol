import QtQuick 2.7
import QtQuick.Window 2.2
import QtGraphicalEffects 1.0
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.0


Rectangle {
      //width: 516; height: 300
      color : "#FFFFFF"
      border.width: 2
      border.color : "#ECECEC"
      Component {
          id: contactDelegate
          Item {
              width: 180; height: 40
          }
      }

      ListView {
          anchors.fill: parent
          //model: ContactModel {}
          delegate: contactDelegate
          //highlight: Rectangle { color: "lightsteelblue"; radius: 5 }
          focus: true
      }
  }
