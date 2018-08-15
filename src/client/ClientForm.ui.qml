import QtQuick 2.4
import QtQuick.Window 2.10
import QtQuick.Controls 2.3
import QtGraphicalEffects 1.0

Page {
    height: Screen.desktopAvailableHeight
    property alias button2: button2
    property alias listView: listView
    property alias addCommunityButton: addCommunityButton
    width: 1920

    ScrollView {
        id: communityView
        width: 200
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 50
        anchors.top: parent.top
        anchors.topMargin: 50
        anchors.left: parent.left
        anchors.leftMargin: 0

        ListView {
            id: listView
            y: 0
            width: 100
            height: parent.height
            anchors.leftMargin: 70
            anchors.left: parent.left
            anchors.bottom: parent.bottom
            anchors.top: parent.top
            anchors.topMargin: 10
            model: ListModel {
            }
            delegate: communityDelegate
            focus: true

            RoundButton {
                id: addCommunityButton
                background: Rectangle {
                    color: "red"
                    radius: 50
                    Image {
                        source: "qrc:/plus.png"
                        sourceSize.width: 65
                        sourceSize.height: 65
                        layer.enabled: true
                        layer.effect: OpacityMask {
                            maskSource: Rectangle {
                                radius: 50
                                width: 65
                                height: 65
                                visible: false
                            }
                        }
                    }
                }
                anchors.top: parent.top
                radius: 50
                anchors.topMargin: (65 * listView.model.count)
                width: 65
                height: 65
            }
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
