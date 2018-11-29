import QtQuick 2.4
import QtQuick.Window 2.10
import QtQuick.Controls 2.3
import QtGraphicalEffects 1.0

Page {
    id: page
    height: 720
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
            model: communitiesList.data
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

    ScrollView {
        id: channelView
        x: 1
        y: -4
        width: 200
        anchors.left: communityView.right
        anchors.leftMargin: 10
        anchors.topMargin: 50
        anchors.bottom: parent.bottom
        ListView {
            id: channelList
            y: 0
            width: 100
            height: parent.height
            anchors.left: parent.left
            anchors.leftMargin: 70
            anchors.topMargin: 10
            anchors.bottom: parent.bottom
            anchors.top: parent.top
            model: channelsMap[selectedCommunity].data
            delegate: channelDelegate
            focus: true
        }
        anchors.top: parent.top
        anchors.bottomMargin: 50

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
            width: 50
            height: 20
        }
    }

    ScrollView {
        id: messagesView
        x: 2
        y: 2
        width: parent.width * 0.4
        height: screen.height * 0.43
        anchors.right: parent.right
        anchors.rightMargin: 57
        anchors.bottomMargin: 204
        anchors.topMargin: 50
        anchors.left: channelView.right
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
            anchors.leftMargin: 70
            delegate: messageDelegate
        }
        anchors.bottom: parent.bottom
        anchors.leftMargin: 20
    }

    TextInput {
        id: messageInput
        height: 86
        text: qsTr("Text Input")
        anchors.right: messagesView.right
        anchors.rightMargin: 167
        anchors.left: messagesView.left
        anchors.leftMargin: 208
        horizontalAlignment: Text.AlignHCenter
        anchors.top: messagesView.bottom
        anchors.topMargin: 30
        font.pixelSize: 12
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
}
