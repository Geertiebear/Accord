import QtQuick 2.4
import QtQuick.Window 2.3

Window {
    visible: true
    visibility: "Maximized"
    title: qsTr("Hello World")

    ClientForm {
        anchors.fill: parent
        button.onClicked: {
            listView.model.append({});
        }
    }

    Component {
        id: communityDelegate
        Rectangle {
            radius: 50
            width: 75
            height: 75
            color: "black"
        }
    }
}
