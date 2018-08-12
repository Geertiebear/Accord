import QtQuick 2.4
import QtQuick.Window 2.3
import QtQuick.Controls 2.2

ClientForm {
    background: Rectangle {
        color: "#5d5959"
    }

        Component {
            id: communityDelegate
            RoundButton {
                background: Rectangle {
                    radius: 50
                    color: "red"
                }
                text: name
                radius: 50
                width: 65
                height: 65
            }
        }
    }
