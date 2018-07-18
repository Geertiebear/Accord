import QtQuick 2.4
import QtQuick.Window 2.3
import QtQuick.Controls 2.3

Item {
    property alias name: name
    width: 1920
    height: Screen.desktopAvailableHeight
    property alias email: email
    property alias register: register
    property alias text1: text1
    property alias password: password
    property alias login: login

    Rectangle {
        id: rectangle
        x: 771
        y: 379
        width: 378
        height: 322
        color: "#ffffff"

        TextInput {
            id: name
            x: parent.width / 2 - width / 2
            y: 63
            width: 80
            height: 20
            text: qsTr("Text Input")
            font.pixelSize: 12
        }

        TextInput {
            id: password
            x: 149
            y: 157
            width: 80
            height: 20
            text: qsTr("Text Input")
            font.pixelSize: 12
        }

        Text {
            id: text1
            x: 134
            y: 32
            color: "#f70808"
            text: qsTr("Authentication Error!")
            visible: false
            horizontalAlignment: Text.AlignHCenter
            font.pixelSize: 12
        }

        Button {
            id: login
            x: 139
            y: 255
            text: qsTr("Login")
        }

        Button {
            id: register
            x: 8
            y: 20
            width: 77
            height: 18
            text: qsTr("Register")
            wheelEnabled: false
        }

        TextInput {
            id: email
            x: 149
            y: 112
            width: 80
            height: 20
            visible: false
            text: qsTr("Text Input")
            font.pixelSize: 12
        }
    }
}
