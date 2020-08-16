import QtQuick 2.15
import QtQuick.Controls 2.15
import Customization 1.0


Column {
    id: root
    property QtObject styleSheet: Theme.styleSheet("TextMessage")
    spacing: root.styleSheet.spacing
    property real factor : fontFactor
    Row {
        id: messageRow
        spacing: root.styleSheet.textAvatarSpacing
        anchors.right: model.local ? parent.right : undefined

        Image {
            id: avatar
            source: "image://avatar/%1".arg(model.writerId)
            visible: !model.local
            fillMode: Image.PreserveAspectFit
            property int side: messageId.contentHeight / messageId.lineCount + root.styleSheet.textTopMargin
            sourceSize.height: side
            sourceSize.width:  side
        }

        Rectangle {
            id: rect
            width: Math.min(messageId.contentWidth + root.styleSheet.textTopMargin,
                            listView.width - (!model.local ? avatar.width + messageRow.spacing : 0))
            height: messageId.contentHeight + root.styleSheet.textTopMargin
            color: model.local ? "lightgrey" : "steelblue"
            radius: root.styleSheet.radiusSize
            Label {
                id: messageId
                text: model.text
                font.pixelSize: root.styleSheet.fontSize * root.factor
                anchors.centerIn: parent
                //anchors.margins: root.styleSheet.verticalMargin
                wrapMode: Text.WordWrap
                onLinkActivated: _ctrl.openLink(link)
            }
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
