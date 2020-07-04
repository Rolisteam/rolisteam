import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtMultimedia 5.15
import InstantMessaging 1.0
import Customization 1.0

Item {
    id: root
    anchors.fill: parent
    property QtObject styleSheet: Theme.styleSheet("InstantMessaging")

    SplitView {
        anchors.fill: parent
        Repeater {
            model: _ctrl.mainModel
            onCountChanged: console.log("count:"+count)
            delegate: ChatView {
                chatroomModel: model.filterModel
                styleSheet: root.styleSheet
                SplitView.minimumWidth: 50
                SplitView.fillHeight: true
                SplitView.fillWidth: true
                playerModel: _ctrl.playerModel
                localPersonModel: _ctrl.localPersonModel
                onAddChat: _ctrl.addExtraChatroom(title, all, recipiants)
                onSplit: _ctrl.splitScreen(uuid, index)
                onDetach: _ctrl.detach(uuid, index)
            }
        }
    }
}

