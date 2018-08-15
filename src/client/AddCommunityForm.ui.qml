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
        y: 233
        width: 43
        height: 15
        text: qsTr("filepath")
        anchors.right: fileChoose.left
        anchors.rightMargin: 145
        wrapMode: Text.NoWrap
        font.pixelSize: 12
    }

    Button {
        id: submit
        x: 270
        y: 399
        text: qsTr("Submit")
    }

    Button {
        id: fileChoose
        x: 308
        y: 233
        width: 75
        height: 15
        text: qsTr("Choose File")
    }

    TextInput {
        id: name
        x: 280
        y: 110
        width: 80
        height: 20
        text: qsTr("Name")
        horizontalAlignment: Text.AlignHCenter
        font.pixelSize: 12
    }

    Text {
        id: text1
        x: 245
        y: 34
        text: qsTr("Create Community")
        font.pixelSize: 18
    }
}
