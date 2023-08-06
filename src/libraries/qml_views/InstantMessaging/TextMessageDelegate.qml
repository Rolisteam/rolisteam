import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Customization
import org.rolisteam.InstantMessaging


Column {
    id: root
    property string writerId : writerIdldr
    property string text: messageTextldr
    property bool local: localldr
    property string time: timeldr
    property string writerName: writerNameldr
    property real parentWidth: windowWidthldr
    property real factor : fontFactor
    property url imageLink: imageLinkldr

    property QtObject styleSheet: Theme.styleSheet("TextMessage")
    spacing: root.styleSheet.spacing
    signal linkClicked(string link)
    Row {
        id: messageRow
        spacing: root.styleSheet.textAvatarSpacing
        anchors.right: local ? parent.right : undefined

        Image {
            id: avatar
            source: "image://avatar/%1".arg(root.writerId)
            visible: !root.local
            fillMode: Image.PreserveAspectFit
            sourceSize.width:  root.styleSheet.imageSize
            sourceSize.height: root.styleSheet.imageSize
        }

        Rectangle {
            id: rect
            width: messageLyt.implicitWidth + root.styleSheet.textTopMargin
            height: messageLyt.implicitHeight + root.styleSheet.textTopMargin
            color: root.local ? root.styleSheet.localMsgColor : root.styleSheet.RemoteMsgColor
            radius: root.styleSheet.radiusSize
            ColumnLayout {
                id: messageLyt
                x: root.styleSheet.textTopMargin/2
                y: root.styleSheet.textTopMargin/2
                Label {
                    id: messageId
                    text: root.text
                    visible: root.text
                    font.pixelSize: root.styleSheet.fontSize * root.factor
                    Layout.preferredWidth: Math.min(root.parentWidth, contentWidth)

                    wrapMode: Text.WordWrap
                    onLinkActivated: InstantMessagerManager.ctrl.openLink(link)
                }
                Image {
                    id: imagePanel
                    fillMode: Image.PreserveAspectFit
                    source: root.imageLink
                    visible: root.imageLink
                    sourceSize.width: root.parentWidth*0.8

                    MouseArea {
                        anchors.fill: parent
                        onClicked: InstantMessagerManager.ctrl.openLink(root.imageLink)
                    }
                }
            }
        }
    }
    Label {
        id: timestamp
        text: root.local ? root.time : "%1 - %2".arg(root.time).arg(root.writerName)
        anchors.right: root.local ? parent.right : undefined
        font.pixelSize: root.styleSheet.fontSizeTime * root.factor
        opacity: root.styleSheet.opacityTime
    }
}
