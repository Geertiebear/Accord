import QtQuick 2.4
import QtQuick.Controls 2.3

Item {
    width: 400
    height: 400
    property alias button: button
    property alias textInput1: textInput1
    property alias textInput: textInput

    TextInput {
        id: textInput
        x: 160
        y: 83
        width: 80
        height: 20
        text: qsTr("Name")
        font.pixelSize: 12
    }

    TextInput {
        id: textInput1
        x: 160
        y: 169
        width: 80
        height: 20
        text: qsTr("Description")
        font.pixelSize: 12
    }

    Button {
        id: button
        x: 150
        y: 307
        text: qsTr("Add")
    }
}
