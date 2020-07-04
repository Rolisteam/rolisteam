import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtMultimedia 5.15
import InstantMessaging 1.0
import Customization 1.0

Item {
    id: root
    property QtObject styleSheet: Theme.styleSheet("InstantMessaging")
    property alias playerModel: newChatRoom.model
    property alias localPersonModel: imEditText.model
    property alias chatroomModel: repeater.model
    property ChatRoom chatRoom:  chatroomModel.get(tabHeader.currentIndex)
    signal addChat(var title, var all, var recipiants)
    signal split(var uuid, var index)
    signal detach(var uuid, var index)

    SoundEffect {
        id: effect
        source: "qrc:/resources/sounds/Doorbell.wav"
        muted: !sideMenu.sound
        volume: 1.0
    }

    SideMenu {
        id: sideMenu
        height: parent.height
        edge: Qt.RightEdge
    }

    AddChatRoomDialog {
        id: newChatRoom
        x: -width+parent.width
        onChatRoomAdded: {
            root.addChat(newChatRoom.title, newChatRoom.all, newChatRoom.recipiants)
        }
    }
    Menu {
        id: menu
        x: -width+parent.width
        Action {
            text: qsTr("Add chatroom")
            onTriggered: newChatRoom.open()
        }
        Action {
            text: qsTr("Split view")
            onTriggered: {
                root.split(root.chatRoom.uuid, tabHeader.currentIndex)
                root.chatRoom =  chatroomModel.get(tabHeader.currentIndex)
            }
        }
        Action {
            text: qsTr("Detach current")
            onTriggered: root.detach(root.chatRoom.uuid, tabHeader.currentIndex)
        }
        Action {
            text: qsTr("Settings")
            onTriggered: sideMenu.open()
        }
    }


    ColumnLayout {
        anchors.fill: parent
        RowLayout {
            Layout.fillWidth: true
            TabBar {
                id: tabHeader
                Layout.fillWidth: true
                Repeater {
                    id: repeater
                    TabButton {
                        id: tabButton
                        property bool current: tabHeader.currentIndex === index
                        background: Rectangle {
                            color: tabButton.current ? "white" : model.unread ? "red" : "#242424"
                        }

                        contentItem: RowLayout {
                            Label {
                                text: model.unread ? "%1 (\*)".arg(model.title) : model.title
                                Layout.fillWidth: true
                                horizontalAlignment: Qt.AlignHCenter
                                color: tabButton.current ? "black" : "white"
                            }
                            ToolButton {
                                visible: model.closable
                                text: "X"
                                ToolTip.text: qsTr("close")
                                ToolTip.visible: down
                            }
                        }
                        Connections {
                            target: model.chatroom
                            function onUnreadMessageChanged(unread) {
                                if(unread && !tabButton.current)
                                    effect.play()
                            }
                        }
                    }
                }
            }
            ToolButton {
                icon.source: "qrc:/resources/images/menu-rounded-solid.svg"
                onClicked: menu.open()
            }
        }
        SplitView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.leftMargin: root.styleSheet.sideMargin
            Layout.rightMargin: root.styleSheet.sideMargin
            orientation: Qt.Vertical

            ListView {
                id: listView
                SplitView.fillWidth: true
                SplitView.fillHeight: true
                model: root.chatRoom.messageModel
                clip: true
                verticalLayoutDirection: ListView.BottomToTop
                delegate: Column {
                    anchors.right: model.local ? parent.right : undefined
                    spacing: root.styleSheet.spacing
                    Row {
                        id: messageRow
                        spacing: root.styleSheet.spacing
                        anchors.right: model.local ? parent.right : undefined

                        Image {
                            id: avatar
                            source: "image://avatar/%1".arg(model.writerId)
                            visible: !model.local
                            fillMode: Image.PreserveAspectFit
                            property int side: 50
                            sourceSize.height: side
                            sourceSize.width:  side
                        }

                        Rectangle {
                            id: rect
                            width: Math.min(messageId.contentWidth + root.styleSheet.textTopMargin,
                                            listView.width - (!model.local ? avatar.width + messageRow.spacing : 0))
                            height: messageId.contentHeight + root.styleSheet.textTopMargin
                            color: model.local ? "lightgrey" : "steelblue"
                            radius: 6
                            Label {
                                id: messageId
                                text: model.text
                                anchors.fill: parent
                                anchors.margins: root.styleSheet.verticalMargin
                                wrapMode: Text.WordWrap
                                onLinkActivated: _ctrl.openLink(link)
                            }
                        }
                    }
                    Label {
                        id: timestamp
                        text: model.time
                        anchors.right: model.local ? parent.right : undefined
                        font.pixelSize: root.styleSheet.fontSize
                        opacity: root.styleSheet.opacityTime
                    }
                }
            }
            InstantMessagingEditText {
                id: imEditText
                SplitView.fillWidth: true
                SplitView.preferredHeight: root.styleSheet.preferredHeight

                onSendClicked: root.chatRoom.addMessage(text, imEditText.currentPersonId)
                function updateUnread() {
                    root.chatRoom.unreadMessage = false
                }
                onFocusGained: updateUnread()
            }
        }
    }
}
