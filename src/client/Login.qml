import QtQuick 2.4
import QtQuick.Controls 2.2

Item {
    property bool failed
    property bool registering
    id: login
    LoginForm {
        id: loginForm
        anchors.fill: parent
        login.onClicked: {
            if(registering) {
                backend.regist(name.text, email.text, password.text);
                return;
            }
            backend.authenticate(name.text, password.text);
        }

        register.onClicked: {
            loginForm.login.text = "Register";
            register.visible = false;
            email.visible = true;
            registering = true
        }

        text1.visible: failed
    }    
}
