import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import org.rolisteam.InstantMessaging
import InstantMessaging
import Customization
import IMModels

Pane {
    id: root

    implicitHeight: 300
    implicitWidth: 300
    property ChatRoom chatRoom
    onChatRoomChanged: {
        if(root.chatRoom) {
            rest.recipiants = Qt.binding(function(){ return root.chatRoom.recipiants.recipiants;})
            rest.sourceModel = root.chatRoom.players

        }
    }

    property bool isGlobal: root.chatRoom ? root.chatRoom.type === ChatRoom.GLOBAL : false
    property bool isExtra: root.chatRoom ? root.chatRoom.type === ChatRoom.EXTRA : false

    FilteredPlayerModel {
        id: rest
        allow: false
    }

    ColumnLayout {
        id: mainLyt
        anchors.fill: parent
        Label {
            text: qsTr("Players:")
        }
        Label {
            text: qsTr("All")
            visible: root.isGlobal
        }

        ListView {
            id: recipiantsList
            Layout.fillHeight: true
            Layout.fillWidth: true
            visible: !root.isGlobal
            model: root.chatRoom ? root.chatRoom.recipiants : 0
            delegate: RowLayout {
                width: root.width - (root.padding * 2)
                height: 30
                required property string uuid
                required property string name

                Image {
                    source: "image://avatar/%1".arg(uuid)
                    sourceSize.width: 24
                    sourceSize.height: 24
                }
                Label {
                    text: name
                    Layout.fillWidth: true
                }
                ToolButton {
                    icon.name: "delete"
                    icon.color: "transparent"
                    visible: uuid !== root.chatRoom.localId && root.chatRoom.type === ChatRoom.EXTRA
                    onClicked: root.chatRoom.recipiants.removeRecipiants(uuid)
                }
            }
        }
        ToolButton {
            icon.name: "add"
            icon.color: "transparent"
            visible: root.isExtra
            text: qsTr("Add User")
            enabled: addPlayerInst.count > 0
            opacity: enabled ? 1.0 : 0.4
            onClicked: {
                addPlayer.open()
            }
            Menu {
                id: addPlayer


                Instantiator {
                    id: addPlayerInst
                    model: rest
                    delegate: MenuItem {
                        required property string uuid
                        required property string name
                        text: name
                        icon.source: "image://avatar/%1".arg(uuid)
                        icon.color: "transparent"
                        onTriggered: root.chatRoom.recipiants.addRecipiants(uuid)
                    }

                    onObjectAdded: (index, object) => addPlayer.insertItem(index, object)
                    onObjectRemoved: (index, object) => addPlayer.removeItem(object)
                }
            }
        }
    }

}
