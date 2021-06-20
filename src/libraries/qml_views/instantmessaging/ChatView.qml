import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtMultimedia 5.15
import InstantMessaging 1.0
import Customization 1.0

Item {
    id: root
    property QtObject styleSheet: Theme.styleSheet("InstantMessaging")
    property alias localPersonModel: imEditText.model
    property alias chatroomModel: repeater.model
    property ChatRoom chatRoom:  chatroomModel.get(tabHeader.currentIndex)
    property alias tabBarHeight: tabHeader.height
    property int tabBarRightMargin: 0
    property real fontFactor : 1.0
    signal zoomChanged(var delta)
    signal addChat(var title, var all, var recipiants)
    signal split(var uuid, var index)
    signal detach(var uuid, var index)

    SoundEffect {
        id: effect
        source: "qrc:/resources/sounds/Doorbell.wav"
        muted: !sideMenu.sound
        volume: 1.0
    }


    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        onClicked: {
            if (mouse.button === Qt.RightButton)
                contextMenu.popup()
        }
        onPressAndHold: {
            if (mouse.source === Qt.MouseEventNotSynthesized)
                contextMenu.popup()
        }

        onWheel: {
            if (wheel.modifiers & Qt.ControlModifier) {
                zoomChanged(wheel.angleDelta.y / 240)
                /*fontFactor += wheel.angleDelta.y / 240;
                if(fontFactor<1.0)
                    fontFactor=1.0
                else if(fontFactor>10.0)
                    fontFactor=10.0*/
            }
        }

        Menu {
            id: contextMenu
            Action {
                text: qsTr("Split view")
                onTriggered: {
                    root.split(root.chatRoom.uuid, tabHeader.currentIndex)
                    root.chatRoom =  chatroomModel.get(tabHeader.currentIndex)
                }
            }
            Action {
                text: qsTr("Detach")
                onTriggered: root.detach(root.chatRoom.uuid, tabHeader.currentIndex)
            }
            Action {
                text: qsTr("Reatach")
                //onTriggered: root.detach(root.chatRoom.uuid, tabHeader.currentIndex)
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
                Layout.rightMargin: root.tabBarRightMargin
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
                spacing: 5
                verticalLayoutDirection: ListView.BottomToTop
                delegate: Component {
                    id: delegateComponent
                    Loader {
                        property bool isTextMessage: model.type === MessageInterface.Text
                        property bool isDiceMessage: model.type === MessageInterface.Dice
                        property bool isCommandMessage: model.type === MessageInterface.Command
                        property bool isErrorMessage: model.type === MessageInterface.Error
                        property bool mustBeOnTheRight: model.local && (isTextMessage || isCommandMessage)
                        property real fontFactor: root.fontFactor
                        anchors.right: mustBeOnTheRight ? parent.right : undefined
                        width: (isDiceMessage || isErrorMessage) ?  parent.width-10 : undefined
                        source: isTextMessage ? "TextMessageDelegate.qml" : isCommandMessage ? "CommandMessageDelegate.qml" : isDiceMessage ? "DiceMessageDelegate.qml" : "ErrorMessageDelegate.qml"
                    }
                }
            }

            InstantMessagingEditText {
                id: imEditText
                SplitView.fillWidth: true
                SplitView.preferredHeight: root.styleSheet.preferredHeight

                onSendClicked: root.chatRoom.addMessage(text, imEditText.currentPersonId, imEditText.currentPersonName)
                function updateUnread() {
                    root.chatRoom.unreadMessage = false
                }
                onFocusGained: updateUnread()
            }

        }
    }
}
