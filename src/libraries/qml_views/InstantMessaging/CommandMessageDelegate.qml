import QtQuick 2.15
import QtQuick.Controls 2.15
import Customization 1.0

Column {
    id: root
    property QtObject styleSheet: Theme.styleSheet("TextMessage")
    spacing: root.styleSheet.spacing
    property real factor : fontFactor
    Rectangle {
        anchors.right: model.local ? parent.right : undefined
        id: rect
        width: messageId.contentWidth + root.styleSheet.textTopMargin
        height: messageId.contentHeight + root.styleSheet.textTopMargin
        color: model.local ? "lightgrey" : "steelblue"
        radius: root.styleSheet.radiusSize
        Label {
            id: messageId
            text: model.text
            font.pixelSize: root.styleSheet.fontSize * root.factor
            anchors.centerIn: parent
            wrapMode: Text.WordWrap
            onLinkActivated: _ctrl.openLink(link)
        }
    }
    Label {
        id: timestamp
        text: model.time
        anchors.right: model.local ? parent.right : undefined
        font.pixelSize: root.styleSheet.fontSize * root.factor
        opacity: root.styleSheet.opacityTime
    }
}
