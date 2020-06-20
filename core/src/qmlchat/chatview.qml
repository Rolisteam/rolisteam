import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import InstantMessaging 1.0

Item {
    id: root
    anchors.fill: parent
    property ChatRoom chatRoom: _ctrl.globalChatroom

    SidePanel {
        id: sidePanel
        height: parent.height
        edge: Qt.RightEdge
        model: _ctrl.playerModel
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
                        text: model.title
                        ToolButton {
                            anchors.right: parent.right
                            anchors.top: parent.top
                            anchors.bottom: parent.bottom
                            anchors.margins: 5
                            text: "^"
                            ToolTip.text: "detach"
                            ToolTip.visible: down
                        }
                        onClicked: root.chatRoom = model.chatroom
                    }
                }
            }
            ToolButton {
                text: "+"
                onClicked: sidePanel.open()
            }
        }
        SplitView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.leftMargin: 10
            Layout.rightMargin: 10
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
                    spacing: 6
                    Row {
                        id: messageRow
                        spacing: 6
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
                            Label {
                                id: messageId
                                text: model.text
                                anchors.fill: parent
                                anchors.margins: 12
                                wrapMode: Label.Wrap
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

