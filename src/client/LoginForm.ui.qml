import QtQuick 2.4
import QtQuick.Window 2.3
import QtQuick.Controls 2.2

Item {
    id: item1
    property alias name: name
    width: 1280
    height: 720
    property alias email: email
    property alias register: register
    property alias text1: text1
    property alias password: password
    property alias login: login

    Rectangle {
        id: rectangle
        x: 451
        y: 199
        width: parent.width * 0.3
        height: parent.height * 0.45
        color: "#ffffff"
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter

        TextInput {
            id: name
            x: parent.width / 2 - width / 2
            width: 80
            height: 20
            text: qsTr("Name/email")
            anchors.top: parent.top
            anchors.topMargin: 63
            anchors.horizontalCenter: parent.horizontalCenter
            font.pixelSize: 12
        }

        TextInput {
            id: password
            echoMode: TextInput.Normal
            x: 149
            width: 80
            height: 20
            text: "password"
            anchors.top: parent.top
            anchors.topMargin: 157
            anchors.horizontalCenter: parent.horizontalCenter
            horizontalAlignment: Text.AlignHCenter
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
            text: qsTr("Login")
            anchors.top: parent.top
            anchors.topMargin: 255
            anchors.horizontalCenter: parent.horizontalCenter
        }

        Button {
            id: register
            width: 77
            height: 18
            text: qsTr("Register")
            anchors.left: parent.left
            anchors.leftMargin: 8
            anchors.top: parent.top
            anchors.topMargin: 20
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
