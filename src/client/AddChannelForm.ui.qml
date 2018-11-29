import QtQuick 2.4
import QtQuick.Controls 2.3

Item {
    id: item1
    width: 400
    height: 400
    property alias button: button
    property alias textInput1: textInput1
    property alias textInput: textInput

    TextInput {
        id: textInput
        x: 160
        width: 80
        height: 20
        text: qsTr("Name")
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: 83
        font.pixelSize: 12
    }

    TextInput {
        id: textInput1
        x: 160
        width: 80
        height: 20
        text: qsTr("Description")
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: 169
        font.pixelSize: 12
    }

    Button {
        id: button
        x: 150
        y: 307
        text: qsTr("Add")
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 57
        anchors.horizontalCenter: parent.horizontalCenter
    }
}
