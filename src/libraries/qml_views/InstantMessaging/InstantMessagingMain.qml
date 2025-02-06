import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtMultimedia
import InstantMessaging
import Customization
import org.rolisteam.InstantMessaging

Pane {
    id: root
    anchors.fill: parent
    property QtObject styleSheet: Theme.styleSheet("InstantMessaging")

    padding: 0

    Connections {
        target: InstantMessagerManager.ctrl
        function onNightModeChanged() {
            Theme.nightMode= InstantMessagerManager.ctrl.nightMode
        }
    }

    Component.onCompleted: Theme.nightMode= InstantMessagerManager.ctrl.nightMode

    SideMenu {
        id: sideMenu
        height: parent.height
        edge: Qt.RightEdge
    }

    AddChatRoomDialog {
        id: newChatRoom
        x: -width+parent.width
        model: InstantMessagerManager.ctrl.playerModel
        onChatRoomAdded: {
            InstantMessagerManager.ctrl.addExtraChatroom(newChatRoom.title, newChatRoom.all, newChatRoom.recipiants)
        }
    }
    Menu {
        id: menu
        x: -width+parent.width
        Action {
            text: qsTr("Settings")
            onTriggered: sideMenu.open()
        }
        Action {
            text: qsTr("Add chatroom")
            onTriggered: newChatRoom.open()
        }
    }

    SplitView {
        anchors.fill: parent
        Repeater {
            id: reaper
            model: InstantMessagerManager.ctrl.mainModel
            property real headerHeight: 0
            delegate: ChatView {
                chatroomModel: model.filterModel
                styleSheet: root.styleSheet
                onZoomChanged: sideMenu.fontFactor += delta
                SplitView.minimumWidth: root.styleSheet.minimumWidth
                SplitView.preferredWidth: root.width/reaper.count
                SplitView.fillHeight: true
                SplitView.fillWidth: true
                tabBarRightMargin: index == reaper.count-1 ? listButton.width : 0
                Component.onCompleted: reaper.headerHeight = tabBarHeight
                localPersonModel: InstantMessagerManager.ctrl.localPersonModel
                onAddChat: InstantMessagerManager.ctrl.addExtraChatroom(title, all, recipiants)
                onSplit: InstantMessagerManager.ctrl.splitScreen(uuid, model.index)
                onDetach: InstantMessagerManager.ctrl.detach(uuid, index, model.index)
            }
        }
    }

    ToolButton {
        id: listButton
        anchors.top: parent.top
        anchors.right: parent.right
        height: reaper.headerHeight
        icon.name: "menu-rounded-solid"
        onClicked: menu.open()
    }
}

