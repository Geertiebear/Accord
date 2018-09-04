import QtQuick 2.4

AddChannelForm {
    button.onClicked: {
        backend.addChannel(textInput.text, textInput1.text,
                           selectedCommunity);
        addChannelPopup.close();
    }
}
