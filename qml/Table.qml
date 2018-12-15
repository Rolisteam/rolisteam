import QtQuick 2.0
import QtQuick.Controls 2.0
import Rolisteam 1.0

ListView {
    id: root
    property Field field: null
    property int maxRow: 0
    property bool readOnly: false
    property alias actions: contextMenu.contentData
    interactive: count>maxRow?true:false
    clip: true
    MouseArea {
        id: listmouseid_1
        anchors.fill: parent
        acceptedButtons: contextMenu.visible ? Qt.LeftButton : Qt.RightButton
        onClicked: {
            mouse.accepted = false
            if(mouse.button === Qt.RightButton)
                contextMenu.popup()
            if(mouse.button === Qt.LeftButton)
                contextMenu.dismiss()
        }
        Menu {
            id: contextMenu
            MenuItem {
                text: qsTr("Add line")
                onTriggered: field.addLine()
            }
            MenuItem {
                text: qsTr("Remove current Line")
                onTriggered: {
                    field.removeLine(_id_1list.indexAt(contextMenu.x, contextMenu.y+root.contentY))
                }
            }
            MenuItem {
                text: qsTr("Remove Last line")
                onTriggered: field.removeLastLine()
            }
        }
    }

}
