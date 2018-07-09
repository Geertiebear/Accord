import QtQuick 2.4
import QtQuick.Window 2.3
import QtQuick.Controls 2.3

Item {
    property alias mainLoader: mainLoader
    width: 1920
    height: Screen.desktopAvailableHeight
    property alias email: email
    property alias password: password
    property alias login: login

    Loader {
        id: mainLoader
    }

    Rectangle {
        id: rectangle
        x: 771
        y: 379
        width: 378
        height: 322
        color: "#ffffff"

        TextInput {
            id: email
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

        Button {
            id: login
            x: 139
            y: 255
            text: qsTr("Login")
        }
    }
}
