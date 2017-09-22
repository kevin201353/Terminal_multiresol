import QtQuick 2.7
import QtQuick.Controls 2.0

TextField {
    id: control
	font.pixelSize: 14;
	wrapMode: TextEdit.Wrap
	horizontalAlignment:TextInput.AlignLeft
	verticalAlignment:TextInput.AlignTop
	background: Rectangle {
			  implicitWidth: 200
			  implicitHeight: 30
              border.color: control.enabled ? "#21be2b" : "#21be2b"
	}
	property int maxLength: 14//最大输入长度
    onLengthChanged:
    {
        if(control.length > maxLength)
        {
            var prePosition = cursorPosition;
            control.text = control.text.substring(0, maxLength);
            cursorPosition = Math.min(prePosition, maxLength);
        }
    }
}
