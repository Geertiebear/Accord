import QtQuick 2.4
import QtQuick.Window 2.3
import QtQuick.Controls 2.2

Item {
    id: client
    ClientForm {
        anchors.fill: parent
        button2.onClicked: {
            listView.model.append({});
        }
    }

    Component {
        id: communityDelegate
        RoundButton {
            radius: 50
            width: 65
            height: 65
        }
    }
}
