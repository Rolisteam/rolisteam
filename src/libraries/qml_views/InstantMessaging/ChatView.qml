import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtMultimedia
import InstantMessaging
import Customization

Item {
    id: root
    property QtObject styleSheet: Theme.styleSheet("InstantMessaging")
    property QtObject paletteSheet: Theme.styleSheet("Palette")
    property alias localPersonModel: imEditText.model
    property alias chatroomModel: repeater.model
    property ChatRoom chatRoom:  chatroomModel.get(tabHeader.currentIndex)
    property alias tabBarHeight: tabHeader.height
    property int tabBarRightMargin: 0
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
        onClicked: (mouse)=>{
            if (mouse.button === Qt.RightButton)
                contextMenu.popup()
        }
        onPressAndHold:(mouse)=> {
            if (mouse.source === Qt.MouseEventNotSynthesized)
                contextMenu.popup()
        }

        onWheel:  (wheel) => {//(wheel)
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
                            color: tabButton.current ? root.paletteSheet.alternateBase : model.unread ? root.paletteSheet.highlight : root.paletteSheet.mid
                        }

                        contentItem: RowLayout {
                            Label {
                                text: model.unread ? "%1 (\*)".arg(model.title) : model.title
                                Layout.fillWidth: true
                                horizontalAlignment: Qt.AlignHCenter
                                color: tabButton.current ? root.paletteSheet.text : root.paletteSheet.button
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
                        property string writerIdldr: model.writerId
                        property string messageTextldr: model.text
                        property bool localldr: model.local
                        property string timeldr: model.time
                        property string writerNameldr: model.writerName
                        property real windowWidthldr: parent.width
                        property url imageLinkldr: model.imageLink ?? ""

                        property bool isTextMessage: model.type === MessageInterface.Text
                        property bool isDiceMessage: model.type === MessageInterface.Dice
                        property bool isCommandMessage: model.type === MessageInterface.Command
                        property bool isErrorMessage: model.type === MessageInterface.Error
                        property bool mustBeOnTheRight: model.local && (isTextMessage || isCommandMessage)
                        anchors.right: mustBeOnTheRight ? parent.right : undefined
                        width: parent.width-10 //(isDiceMessage || isErrorMessage) ?  parent.width-10 : undefined
                        source: isTextMessage ? "TextMessageDelegate.qml" :
                                isCommandMessage ? "CommandMessageDelegate.qml" :
                                isDiceMessage ? "DiceMessageDelegate.qml" : "ErrorMessageDelegate.qml"
                    }
                }
            }

            InstantMessagingEditText {
                id: imEditText
                SplitView.fillWidth: true
                SplitView.preferredHeight: root.styleSheet.preferredHeight

                onSendClicked: (text, imageLink) => {
                   root.chatRoom.addMessage(text,imageLink, imEditText.currentPersonId, imEditText.currentPersonName)
                }
                function updateUnread() {
                    root.chatRoom.unreadMessage = false
                }
                onFocusGained: updateUnread()
            }

        }
    }
}
