import QtQuick 2.7
import QtQuick.Controls 2.13
import QtQuick.Layouts 1.12
import InstantMessaging 1.0

Item {
    id: root
    anchors.fill: parent
    property ChatRoom chatRoom: _ctrl.globalChatroom
   /* Timer {
        running: true
        repeat: true
        interval: 30000
        onTriggered: console.log("Chatroom:"+root.chatRoom)
    }*/

    // TO BE REMOVE - Test data
    ListModel {
        id: contact
        ListElement {
            name: "Grivious"
            avatar: "https://encrypted-tbn0.gstatic.com/images?q=tbn%3AANd9GcR0lAVH9IW0PW0B-gj3ki1tV21WHWbP8qNd4XlEEHICw_Oit6CZ&usqp=CAU"
        }
        ListElement {
            name: "Chewbacca"
            avatar: "https://a.wattpad.com/useravatar/Chewbacca187.256.113313.jpg"
        }
        ListElement {
            name: "Vador"
            avatar: "https://media.anakinworld.com/uploads/entries/square_large/personnage-darth-vader.jpg"
        }
        ListElement {
            name: "Wedge"
            avatar: "https://66.media.tumblr.com/avatar_0986a83b5d58_128.pnj"
        }
    }

    ListModel {
        id: local
        ListElement {
            name: "Obi"
            role: "player"
            avatar: "https://d26oc3sg82pgk3.cloudfront.net/files/media/edit/image/33482/square_thumb%402x.jpg"
        }
        ListElement {
            name: "Batman"
            role: "character"
            avatar: "https://i1.wp.com/www.inforumatik.com/wp-content/uploads/2013/06/BAO_KA_20_05.jpg?fit=256%2C256&ssl=1"
        }
    }
    // TO BE REMOVE - Test data

    Drawer {
        id: add_chat
        width: 0.33 * parent.width
        height: parent.height
        edge: Qt.RightEdge

        Frame {
            anchors.fill: parent

            ColumnLayout {

                RowLayout {
                    Layout.fillWidth: true
                    Label{
                        text: qsTr("Title")
                    }
                    TextField {
                        id: title
                    }
                }
                RowLayout {
                    Layout.fillWidth: true
                    Label{
                        text: qsTr("Everybody")
                    }
                    Switch {
                        id: all
                        checked: true
                    }
                }
                Frame {
                    id: recipiants
                    enabled: !all.checked
                    property int checkedItem: 0
                    ColumnLayout {
                        Repeater {
                            model:  contact
                            RowLayout {
                                Layout.fillWidth: true
                                Image {
                                    source: model.avatar
                                    fillMode: Image.PreserveAspectFit
                                    sourceSize.width: 50
                                    sourceSize.height: 50
                                    opacity: all.checked ? 0.5 : 1.0
                                }
                                Label {
                                    text: model.name
                                    Layout.fillWidth: true
                                }
                                Switch {
                                    enabled: !all.checked
                                    onCheckedChanged: recipiants.checkedItem += checked ? 1 : -1
                                }
                            }
                        }
                    }
                }

                Button {
                    text: "add"
                    enabled: title.text.length > 0 && (all.checked || recipiants.checkedItem > 0 )
                }
            }
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
                        onClicked: listView.chatRoom = model.chatroom
                    }
                }
            }
            ToolButton {
                text: "+"
                onClicked: add_chat.open()
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
                            source: "image://avatar/%1".arg(model.ownerId)
                            visible: !model.local
                            fillMode: Image.PreserveAspectFit
                            property int side: 50
                            sourceSize.height: side
                            sourceSize.width:  side
                        }

                        Rectangle {
                            id: rect
                            width: Math.min(messageId.implicitWidth + 24,
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
                SplitView.fillWidth: true
                SplitView.minimumHeight: 70
                model: _ctrl.localPersonModel
                onSendClicked: root.chatRoom.addMessage(text)
            }
        }
    }
}

