import QtQuick 2.4
import QtQuick.Window 2.3

Window {
    id: loginWindow
    visible: true
    visibility: "Maximized"
    LoginForm {
        anchors.fill: parent
        login.onClicked: {
            backend.authenticate(email.text, password.text);
            mainLoader.source = "Client.qml";
            loginWindow.visible = false;
        }
    }
}
