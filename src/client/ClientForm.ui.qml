import QtQuick 2.4
import QtQuick.Window 2.10
import QtQuick.Controls 2.3

Item {
    id: main
    width: Screen.desktopAvailableWidth
    height: Screen.desktopAvailableHeight
    property alias listView: listView
    property alias button: button

    ScrollView {
        id: communityView
        y: 0
        width: 144
        height: main.height
        anchors.left: parent.left
        anchors.leftMargin: 0

        ListView {
            id: listView
            y: 0
            width: communityView.width
            height: communityView.height
            anchors.left: parent.left
            anchors.leftMargin: 0
            model: ListModel {
            }
            delegate: communityDelegate
            focus: true
        }
    }

    Button {
        id: button
        y: 92
        text: qsTr("Button")
        anchors.left: parent.left
        anchors.leftMargin: 558
    }
}
