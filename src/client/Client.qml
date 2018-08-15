import QtQuick 2.4
import QtQuick.Window 2.3
import QtQuick.Controls 2.2

ClientForm {
    background: Rectangle {
        color: "#5d5959"
    }
    addCommunityButton.onClicked: {
        addCommunityPopup.open();
    }
        Component {
            id: communityDelegate
            RoundButton {
                background: Rectangle {
                    radius: 50
                    color: "red"
                }
                onClicked: print("LOL");
                text: name
                radius: 50
                width: 65
                height: 65
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
    }
