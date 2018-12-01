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

    messagesList.onCountChanged: {
        messagesList.positionViewAtEnd();
    }

    channelList.onCountChanged: {
        addChannelButton.anchors.topMargin = (20 * channelList.count)
    }

    messageInput.onAccepted: {
        backend.sendMessage(messageInput.text, selectedChannel);
        messageInput.clear();
    }

    inviteInput.onAccepted: {
        backend.sendInvite(inviteInput.text);
        inviteInput.clear();
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

                Menu {
                    id: communityMenu
                    MenuItem {
                        text: "Create invite"
                        onClicked: {
                            backend.requestInvite(modelData.id);
                        }
                    }
                }

                MouseArea {
                    anchors.fill: parent
                    acceptedButtons: Qt.LeftButton | Qt.RightButton
                    onClicked: {
                        if (mouse.button === Qt.RightButton) {
                            communityMenu.popup();
                            return;
                        }
                        var id = modelData.id;
                        selectedCommunity = id;
                        backend.loadChannels(id);
                    }
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
            width: parent.width * 0.33
            height: parent.width * 0.33
            modal: true
            focus: true
            closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
            AddCommunity {
                width: parent.width
                height: parent.height
            }
        }

        Popup {
            id: addChannelPopup
            x: Math.round((parent.width - width) / 2)
            y: Math.round((parent.height - height) / 2)
            width: parent.width * 0.33
            height: parent.width * 0.33
            modal: true
            focus: true
            closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
            AddChannel {
                width: parent.width
                height: parent.height
            }
        }

        Component {
            id: messageDelegate
            Text {
                color: {
                    if (modelData.pending)
                        return "#ff8c00";
                    return "#000000";
                }

                text: {
                    if(modelData.pending)
                        return "PENDING: " + modelData.contents;
                    var userData = userMap[modelData.sender];
                    if (typeof userData !== "undefined")
                        return userData.name + ": " + modelData.contents;
                    backend.loadUser(modelData.sender);
                    return "";
                }
            }
        }
}
