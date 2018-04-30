import QtQuick 2.4
import QtQuick.Window 2.3

Window {
    visible: true
    width: 180
    height: 480
    title: qsTr("Hello World")

    ClientForm {
        anchors.fill: parent
        button.onClicked: {
            label.text = qsTr("I've been changed!");
        }
    }
}

