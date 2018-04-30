import QtQuick 2.4
import QtQuick.Window 2.10
import QtQuick.Controls 2.3

Item {
    id: main
    height: Screen.desktopAvailableHeight
    property alias button2: button2
    property alias listView: listView
    width: 1920

    ScrollView {
        id: communityView
        width: 200
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 10
        anchors.top: parent.top
        anchors.topMargin: 50
        anchors.left: parent.left
        anchors.leftMargin: 0

        ListView {
            spacing: 10
            id: listView
            y: 0
            width: 100
            height: communityView.height
            anchors.leftMargin: 70
            anchors.left: parent.left
            anchors.bottom: parent.bottom
            anchors.top: parent.top
            anchors.topMargin: 10
            model: ListModel {
            }
            delegate: communityDelegate
            focus: true
        }
    }

    Row {
        id: row
        anchors.rightMargin: -communityView.width
        anchors.right: communityView.left
        anchors.bottom: communityView.top
        anchors.bottomMargin: 0
        anchors.top: parent.top
        anchors.topMargin: 0
        anchors.left: parent.left
        anchors.leftMargin: 0

        Button {
            id: button
            width: row.width / 2
            height: row.height
            text: qsTr("Communities")
            font.pointSize: 9
            anchors.top: parent.top
            anchors.topMargin: 0
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 0
        }

        Button {
            id: button1
            width: row.width / 2
            height: row.height
            text: qsTr("Friends")
            anchors.top: parent.top
            anchors.topMargin: 0
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 0
        }
    }

    Button {
        id: button2
        x: 688
        y: 137
        text: qsTr("Button")
    }
}
