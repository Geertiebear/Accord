import QtQuick 2.4
import QtQuick.Controls 2.2

Item {
    property bool failed
    id: login
    LoginForm {
        id: loginForm
        anchors.fill: parent
        login.onClicked: {
            backend.authenticate(email.text, password.text);
        }
        text1.visible: failed
    }    
}
