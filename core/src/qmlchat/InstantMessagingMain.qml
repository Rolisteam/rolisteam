import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtMultimedia 5.15
import InstantMessaging 1.0

Item {
    id: root
    anchors.fill: parent
    property ChatRoom chatRoom: _ctrl.globalChatroom

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

    ColumnLayout {
        anchors.fill: parent
        RowLayout {
            Layout.fillWidth: true
            TabBar {
                id: tabHeader
                Layout.fillWidth: true
                Repeater {
                    id: repeater
                    model : _ctrl.mainModel
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
                                text: "^"
                                ToolTip.text: qsTr("detach")
                                ToolTip.visible: down
                            }
                            ToolButton {
                                visible: model.closable
                                text: "X"
                                ToolTip.text: qsTr("close")
                                ToolTip.visible: down
                            }
                        }
                        onClicked: root.chatRoom = model.chatroom
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
                text: "+"
                onClicked: newChatRoom.open()
                AddChatRoomDialog {
                    id: newChatRoom
                    model: _ctrl.playerModel
                    x: -width+parent.width
                    onChatRoomAdded: {
                        _ctrl.addExtraChatroom(newChatRoom.title, newChatRoom.all, newChatRoom.recipiants)
                    }
                }
            }
            ToolButton {
                icon.source: "qrc:/resources/icons/menu-rounded-solid.svg"
                onClicked: sideMenu.open()
            }
        }
        SplitView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.leftMargin: 0
            Layout.rightMargin: 0
            orientation: Qt.Vertical

            ListView {
                id: listView
                SplitView.fillWidth: true
                SplitView.fillHeight: true
                model: chatRoom.messageModel
                clip: true
                verticalLayoutDirection: ListView.BottomToTop
                delegate: Column {
                    anchors.right: model.local ? parent.right : undefined
                    spacing: 0
                    Row {
                        id: messageRow
                        spacing: 0
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
                            width: Math.min(messageId.contentWidth + 24,
                                            listView.width - (!model.local ? avatar.width + messageRow.spacing : 0))
                            height: messageId.implicitHeight + 24
                            color: model.local ? "lightgrey" : "steelblue"
                            radius: 6
                            Label {
                                id: messageId
                                text: model.text
                                anchors.fill: parent
                                anchors.margins: 12
                                wrapMode: Label.Wrap
                                onLinkActivated: _ctrl.openLink(link)
                            }
                        }
                    }
                    Label {
                        id: timestamp
                        text: model.time
                        anchors.right: model.local ? parent.right : undefined
                        font.pixelSize: 10
                        opacity: 0.6
                    }
                }
            }
            InstantMessagingEditText {
                id: imEditText
                SplitView.fillWidth: true
                SplitView.preferredHeight: 100
                model: _ctrl.localPersonModel
                onSendClicked: root.chatRoom.addMessage(text, imEditText.currentPersonId)
            }
        }
    }
}

