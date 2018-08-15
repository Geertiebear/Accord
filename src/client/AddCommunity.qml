import QtQuick 2.4
import QtQuick.Dialogs 1.2

AddCommunityForm {
    property url url
    fileChoose.onClicked: {
        fileDialog.open();
    }

    submit.onClicked: {
        backend.addCommunity(name.text, url)
        addCommunityPopup.close()
    }

    FileDialog {
       id: fileDialog
       title: "Choose a profile picture"
       folder: shortcuts.home
       onAccepted: {
           filepath.text = fileDialog.fileUrl
           url = fileDialog.fileUrl
       }
    }
}
