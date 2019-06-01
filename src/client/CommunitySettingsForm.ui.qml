import QtQuick 2.4
import QtQuick.Controls 2.2

Item {
    property string selectedCommunity
    width: 400
    height: 400

    ScrollView {
        id: rolesView
        width: parent.width
        height: parent.height
        anchors.rightMargin: 0
        anchors.bottomMargin: 0
        anchors.leftMargin: 0
        anchors.topMargin: 0
        anchors.fill: parent

        ListView {
            id: roleList
            y: 0
            width: 150
            height: 379
            boundsBehavior: Flickable.DragOverBounds
            anchors.bottomMargin: 11
            anchors.leftMargin: 0
            anchors.left: parent.left
            anchors.topMargin: 5
            anchors.bottom: parent.bottom
            anchors.top: parent.top
            model: communityRolesMap[selectedCommunity].data
            delegate: rolesDelegate
            focus: true
        }

        Button {
            id: button
            x: 236
            y: 36
            text: qsTr("Button")
        }

        CheckBox {
            id: readChannels
            x: 236
            y: 113
            text: qsTr("Read Channels")
        }

        CheckBox {
            id: sendMsgs
            x: 236
            y: 141
            text: qsTr("Send Messages")
        }

        TextInput {
            id: name
            x: 240
            y: 181
            width: 80
            height: 20
            text: qsTr("Name")
            font.pixelSize: 12
        }
    }
}
