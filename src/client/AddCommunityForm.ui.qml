import QtQuick 2.4
import QtQuick.Controls 2.3

Item {
    id: item1
    property alias name: name
    width: 640
    height: 480
    property alias fileChoose: fileChoose
    property alias submit: submit
    property alias filepath: filepath

    Text {
        id: filepath
        x: 121
        width: 43
        height: 15
        text: qsTr("filepath")
        anchors.top: parent.top
        anchors.topMargin: 233
        anchors.right: fileChoose.left
        anchors.rightMargin: 120
        wrapMode: Text.NoWrap
        font.pixelSize: 12
    }

    Button {
        id: submit
        x: 270
        y: 399
        text: qsTr("Submit")
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 45
        anchors.horizontalCenter: parent.horizontalCenter
    }

    Button {
        id: fileChoose
        x: 308
        width: 75
        height: 15
        text: qsTr("Choose File")
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: 233
    }

    TextInput {
        id: name
        x: 280
        width: 80
        height: 20
        text: qsTr("Name")
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: 110
        horizontalAlignment: Text.AlignHCenter
        font.pixelSize: 12
    }

    Text {
        id: text1
        x: 245
        text: qsTr("Create Community")
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: 34
        font.pixelSize: 18
    }
}
