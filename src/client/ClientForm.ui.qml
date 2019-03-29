import QtQuick 2.4
import QtQuick.Window 2.10
import QtQuick.Controls 2.3
import QtGraphicalEffects 1.0

Page {
    id: page
    height: 720
    property alias communityRectangle: communityRectangle
    property alias onlineView: onlineView
    property alias onlineList: onlineList
    width: 1280
    property alias inviteInput: inviteInput
    property alias addChannelButton: addChannelButton
    property alias messageInput: messageInput
    property alias messagesList: messagesList
    property alias messagesView: messagesView
    property alias channelList: channelList
    property alias channelView: channelView
    property alias listView: listView
    property alias addCommunityButton: addCommunityButton
    property string selectedCommunity
    property string selectedChannel

    Rectangle {
        id: communityRectangle
        visible: true
        color: "#5d5858"
        width: 200
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 50
        anchors.top: parent.top
        anchors.topMargin: 50
        anchors.left: parent.left
        anchors.leftMargin: 0

        ScrollView {
            id: communityView
            width: parent.width
            height: parent.height
            anchors.fill: parent

            ListView {
                id: listView
                y: 0
                width: parent.width
                height: parent.height
                anchors.bottom: parent.bottom
                anchors.top: parent.top
                anchors.topMargin: 10
                model: communitiesList.data
                delegate: communityDelegate
                focus: true

                RoundButton {
                    anchors.horizontalCenter: parent.horizontalCenter
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
                    width: 65
                    height: 65
                }
            }
        }
    }

    Row {
        id: row
        anchors.rightMargin: -200
        anchors.right: communityRectangle.left
        anchors.bottom: communityRectangle.top
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

    Rectangle {
        id: channelRectangle
        color: "#5d5858"
        width: 200
        anchors.bottomMargin: 0
        anchors.top: parent.top
        anchors.left: communityRectangle.right
        anchors.leftMargin: 10
        anchors.topMargin: 50
        anchors.bottom: parent.bottom

        ScrollView {
            id: channelView
            width: parent.width
            height: parent.height
            anchors.fill: parent
            anchors.margins: 0

            ListView {
                id: channelList
                y: 0
                width: parent.width
                height: parent.height
                anchors.leftMargin: 0.025 * parent.width
                anchors.left: parent.left
                anchors.topMargin: 10
                anchors.bottom: parent.bottom
                anchors.top: parent.top
                model: channelsMap[selectedCommunity].data
                delegate: channelDelegate
                focus: true

                Button {
                    id: addChannelButton
                    background: Rectangle {
                        Image {
                            source: "qrc:/plus.png"
                            sourceSize.width: 50
                            sourceSize.height: 20
                        }
                    }
                    anchors.top: parent.top
                    height: 20
                    anchors.left: parent.left
                }
            }
            anchors.top: parent.top
            anchors.bottomMargin: 50
        }
    }

    ScrollView {
        id: messagesView
        y: 2
        width: parent.width * 0.55
        height: screen.height * 0.43
        anchors.bottomMargin: 204
        anchors.topMargin: 50
        anchors.left: channelRectangle.right
        anchors.top: parent.top
        ListView {
            id: messagesList
            y: 0
            width: 100
            height: parent.height
            focus: true
            model: messagesMap[selectedChannel].data
            anchors.topMargin: 10
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            delegate: messageDelegate
        }
        anchors.bottom: parent.bottom
        anchors.leftMargin: 20
    }

    Rectangle {
        height: 86
        anchors.top: messagesView.bottom
        anchors.topMargin: 30
        anchors.right: messagesView.right
        anchors.rightMargin: 167
        anchors.left: messagesView.left
        anchors.leftMargin: 208
        color: "#5d5858"

        TextInput {
            id: messageInput
            anchors.fill: parent
            text: qsTr("Text Input")
            color: "white"
            horizontalAlignment: Text.AlignLeft
            font.pixelSize: 14
            wrapMode: Text.Wrap
        }
    }

    TextInput {
        id: inviteInput
        x: 60
        width: 80
        height: 20
        text: qsTr("Invite")
        anchors.top: parent.bottom
        anchors.topMargin: -100
        font.pixelSize: 12
    }

    ScrollView {
        id: onlineView
        anchors.top: parent.top
        anchors.topMargin: 50
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 0
        anchors.left: messagesView.right
        anchors.leftMargin: 10
        anchors.right: parent.right
        anchors.rightMargin: 11

        Rectangle {
            visible: true
            anchors.fill: parent
            color: "#5d5858"
            width: parent.width
            height: parent.height

            ListView {
                id: onlineList
                anchors.topMargin: 0.01 * parent.height
                anchors.leftMargin: 0.03 * parent.width
                anchors.fill: parent
                model: onlineMap[selectedChannel].data
                delegate: onlineDelegate
            }
        }
    }

    Text {
        id: text1
        x: 210
        text: qsTr("Channels")
        color: "white"
        anchors.bottom: channelRectangle.top
        anchors.top: parent.top
        font.pixelSize: 24
    }

    Text {
        id: text2
        y: 17
        color: "#ffffff"
        text: qsTr("Members")
        anchors.left: onlineView.left
        anchors.leftMargin: 0
        anchors.bottomMargin: 0.01 * parent.height
        anchors.bottom: onlineView.top
        font.pixelSize: 24
    }
}
