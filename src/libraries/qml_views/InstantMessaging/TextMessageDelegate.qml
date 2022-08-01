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
            property string writerid
            source: "image://avatar/%1".arg(avatar.writerid)
            visible: !root.local
            fillMode: Image.PreserveAspectFit
            sourceSize.width:  root.styleSheet.imageSize
            sourceSize.height: root.styleSheet.imageSize
        }

        Rectangle {
            id: rect
            width: messageLyt.implicitWidth + root.styleSheet.textTopMargin//Math.min(messageId.contentWidth + root.styleSheet.textTopMargin,
                            //listView.width - (!root.local ? avatar.width + messageRow.spacing : 0))
            height: messageLyt.implicitHeight + root.styleSheet.textTopMargin //messageId.contentHeight + root.styleSheet.textTopMargin
            color: root.local ? root.styleSheet.localMsgColor : root.styleSheet.RemoteMsgColor
            radius: root.styleSheet.radiusSize
            ColumnLayout {
                id: messageLyt
                anchors.centerIn: parent
                Label {
                    id: messageId
                    text: root.text
                    visible: root.text
                    font.pixelSize: root.styleSheet.fontSize * root.factor
                    width: Math.min(root.parentWidth, contentWidth)

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
