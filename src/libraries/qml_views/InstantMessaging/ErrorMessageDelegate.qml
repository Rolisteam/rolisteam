import QtQuick 2.15
import QtQuick.Controls 2.15
import Customization 1.0

Frame {
    id: frame
    property QtObject message: model.message
    property QtObject styleSheet: Theme.styleSheet("InstantMessaging")

    Label {
        anchors.fill: parent
        text: frame.message.text
        color: "red"
        textFormat: Label.RichText
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        wrapMode: Text.WrapAnywhere
        onLinkActivated: _ctrl.openLink(link)
    }

    background: Rectangle {
        border.width: 1
        border.color: "black"
        color: "lightgrey"
        radius: 10
        opacity: 0.5
    }
}
