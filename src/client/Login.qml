import QtQuick 2.4
import QtQuick.Controls 2.2

Item {
    id: login
    LoginForm {
        anchors.fill: parent
        login.onClicked: {
            backend.authenticate(email.text, password.text);
        }
    }
}
