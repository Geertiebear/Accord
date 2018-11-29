import QtQuick 2.4
import QtQuick.Controls 2.2

Item {
    property bool failed
    property bool registering
    id: login
    LoginForm {
        id: loginForm
        anchors.fill: parent

        password.onTextChanged: {
            password.echoMode = TextInput.Password
        }

        login.onClicked: {
            if(registering) {
                backend.regist(name.text, email.text, password.text);
                return;
            }
            backend.authenticate(name.text, password.text);
        }

        register.onClicked: {
            failed = false
            if (!registering) {
                registering = true
                return;
            }
            registering = false;
        }

        text1.visible: failed
        text1.text: registering ? "Registration Error!" : "Authentication Error!"
        login.text: registering ? "Register" : "Login"
        email.visible: registering
        register.text: registering ? "Back" : "Register"
    }    
}
