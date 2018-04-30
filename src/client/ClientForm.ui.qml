import QtQuick 2.4
import QtQuick.Window 2.10
import QtQuick.Controls 2.3

Item {
    id: item1
    width: 400
    height: 400
    property alias label: label
    property alias button: button

    Button {
        id: button
        text: qsTr("Push me")
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
    }

    Label {
        id: label
        y: 155
        anchors.horizontalCenter: parent.horizontalCenter
    }
}
