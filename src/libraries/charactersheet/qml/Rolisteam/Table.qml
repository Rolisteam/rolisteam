import QtQuick
import QtQuick.Controls
import Rolisteam

TableView {
    id: root
    //property Field field: null
    property int maxRow: 0
    property bool readOnly: false
    property alias actions: contextMenu.contentData

    signal addLine()
    signal removeLine(int line)

    interactive: rows>maxRow?true:false
    clip: true
    MouseArea {
        id: listmouseid_1
        anchors.fill: parent
        acceptedButtons:Qt.LeftButton | Qt.RightButton
        preventStealing: true
        onClicked: (mouse)=>{
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
                onTriggered: root.addLine()//field.addLine()
            }
            MenuItem {
                text: qsTr("Remove current Line")
                onTriggered: {
                    let cell = root.cellAtPosition(Qt.point(contextMenu.x,contextMenu.y+root.contentY), true)
                    root.removeLine(root.modelIndex(cell).row)
                    //field.removeLine(_id_1list.indexAt(contextMenu.x, contextMenu.y+root.contentY))
                }
            }
            MenuItem {
                text: qsTr("Remove Last line")
                onTriggered: root.removeLine(_id_1list.count())//field.removeLastLine()
            }
        }
    }

}
