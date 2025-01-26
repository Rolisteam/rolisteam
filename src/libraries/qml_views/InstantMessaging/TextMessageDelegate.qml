import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Customization
import org.rolisteam.InstantMessaging


ColumnLayout {
    id: root
    property string writerId : writerIdldr
    property string text: messageTextldr
    property bool local: localldr
    property string time: timeldr
    property string writerName: writerNameldr
    property real parentWidth: windowWidthldr
    property url imageLink: imageLinkldr

    Component.onCompleted: {
        root.local = !root.local
        root.local = !root.local
    }

    property QtObject styleSheet: Theme.styleSheet("TextMessage")
    spacing: root.styleSheet.spacing
    signal linkClicked(string link)

    implicitHeight: messageRow.implicitHeight

    RowLayout {
        id: messageRow
        spacing: root.styleSheet.textAvatarSpacing
        Layout.fillWidth: true
        Layout.alignment: root.local ? Qt.AlignRight : Qt.AlignLeft
        readonly property real textPadding: root.styleSheet.textTopMargin * 2
        readonly property real widthUsed: (root.local ? 0 : avatar.width) + messageRow.textPadding
        readonly property real availableSpace: root.width - messageRow.widthUsed

        Image {
            id: avatar
            source: "image://avatar/%1".arg(root.writerId)
            visible: !root.local
            fillMode: Image.PreserveAspectFit
            sourceSize.width:  root.styleSheet.imageSize
            sourceSize.height: root.styleSheet.imageSize
        }

        ColumnLayout {
            id: messageLyt
            Layout.fillWidth: true

            Label {
                id: messageId
                text: root.text

                Layout.maximumWidth: messageRow.availableSpace
                visible: root.text
                padding: root.styleSheet.textTopMargin
                font: Theme.imFont
                textFormat: Text.StyledText
                wrapMode: Text.WordWrap
                onLinkActivated:(link) => InstantMessagerManager.ctrl.openLink(link)
                background: Rectangle {
                    id: bubble
                    color: root.local ? root.styleSheet.localMsgColor : root.styleSheet.RemoteMsgColor
                    opacity: 0.3
                    radius: root.styleSheet.radiusSize
                }
            }
            Image {
                id: imagePanel
                fillMode: Image.PreserveAspectFit
                source: root.imageLink
                visible: root.imageLink
                sourceSize.width: messageRow.availableSpace*0.8
                Layout.alignment: Qt.AlignCenter

                MouseArea {
                    anchors.fill: parent
                    onClicked: InstantMessagerManager.ctrl.openLink(root.imageLink)
                }
            }
        }

    }
    Label {
        id: timestamp
        text: root.local ? root.time : "%1 - %2".arg(root.time).arg(root.writerName)
        //anchors.right: root.local ? parent.right : undefined
        Layout.alignment: root.local ? Qt.AlignRight : Qt.AlignLeft
        font.pixelSize: root.styleSheet.fontSizeTime * root.factor
        opacity: root.styleSheet.opacityTime
    }
}


//implicitWidth: messageLyt.implicitWidth + root.styleSheet.textTopMargin
//                implicitHeight: messageLyt.implicitHeight + root.styleSheet.textTopMargin
//implicitHeight: messageId.implicitHeight + (root.imageLink? imagePanel.paintedHeight : 0)
//implicitWidth: messageId.implicitWidth + (root.imageLink? imagePanel.paintedWidth : 0)
//anchors.fill: parent
//anchors.margins: root.styleSheet.textTopMargin
//implicitWidth: Math.min(messageId.implicitWidth, messageRow.availableSpace)
