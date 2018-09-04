import QtQuick 2.4
import QtQuick.Window 2.3
import QtQuick.Controls 2.2
import QtGraphicalEffects 1.0

ClientForm {
    background: Rectangle {
        color: "#5d5959"
    }

    addCommunityButton.onClicked: {
        addCommunityPopup.open();
    }

    addChannelButton.onClicked: {
        addChannelPopup.open();
    }

    listView.onCountChanged: {
        addCommunityButton.anchors.topMargin = (65 * listView.count)
    }

    channelList.onCountChanged: {
        addChannelButton.anchors.topMargin = (20 * channelList.count)
    }

    messageInput.onAccepted: {
        backend.sendMessage(messageInput.text, selectedChannel);
        messageInput.clear();
    }
        Component {
            id: communityDelegate
            RoundButton {
                background: Image {
                    source: "image://communityImageProvider/" + modelData.id
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
                onClicked: {
                    var id = modelData.id;
                    selectedCommunity = id;
                    backend.loadChannels(id);
                }

                text: modelData.name
                radius: 50
                width: 65
                height: 65
            }
        }
        Component {
            id: channelDelegate
            Rectangle {
                Button {
                    text: modelData.name
                    onClicked: {
                        var id = modelData.id;
                        selectedChannel = id;
                        backend.loadMessages(id);
                    }

                    width: 50
                    height: 20
                }
                color: "red"
                width: 50
                height: 20
            }
        }

        Popup {
            id: addCommunityPopup
            x: Math.round((parent.width - width) / 2)
            y: Math.round((parent.height - height) / 2)
            width: 640
            height: 480
            modal: true
            focus: true
            closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
            AddCommunity {}
        }

        Popup {
            id: addChannelPopup
            x: Math.round((parent.width - width) / 2)
            y: Math.round((parent.height - height) / 2)
            width: 640
            height: 480
            modal: true
            focus: true
            closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
            AddChannel {}
        }

        Component {
            id: messageDelegate
            Text {
                text: modelData.contents
            }
        }
    }
