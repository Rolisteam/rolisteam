import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Customization
import mindmap

Flickable {
    id: _flick
    property real zoomLevel: 1
    required property MindMapController ctrl
    property QtObject styleSheet: Theme.styleSheet("mindmapinteral")
    property string selectedNodeId:""
    property bool addSubLink: false
    property alias innerItem: inner
    property real marginW: 100


    onZoomLevelChanged: {
        updateZoom()
    }

    function updateZoom(){
        var zoomPoint = Qt.point(_flick.width/2 + _flick.contentX,
                             _flick.height/2 + _flick.contentY);
        _flick.resizeContent((inner.width * _flick.zoomLevel), (inner.height * _flick.zoomLevel), zoomPoint);
        _flick.returnToBounds();
    }

    signal openImage(var id)
    signal pressed(var mouse)
    signal positionChanged(var mouse)
    signal released(var mouse)

    function makeScreenShot(path){
        inner.grabToImage(function(result){
            result.saveToFile(path)
        });
    }

    contentHeight: inner.height
    contentWidth: inner.width
    interactive: true
    boundsBehavior: Flickable.StopAtBounds

    Shortcut {
        sequences: [StandardKey.Undo]
        onActivated:  ctrl.undo();
        onActivatedAmbiguously: ctrl.undo()
    }
    Shortcut {
        sequences: [StandardKey.Redo]
        onActivated: ctrl.redo();
        onActivatedAmbiguously: ctrl.redo()
    }
    Component {
        id: linkComp
        Link {
            id: linkItem
            x: item.topLeftCorner.x
            y: item.topLeftCorner.y
            width: item.normalizedWidth
            height: item.normalizedHeight
            color: _flick.styleSheet.linkColor
            opacity: item.constraint ? 1.0 : 0.4
            controller: item
            visible: item.visible
            text: item.text ?  item.text : qsTr("is linked")
            visibleLabel: _flick.ctrl.linkLabelVisibility

            onSelected: {
                _flick.ctrl.selectionCtrl.clearSelection()
                _flick.ctrl.selectionCtrl.addToSelection(item)
            }

            Rectangle {
                border.width: 1
                border.color: "red"
                color: "transparent"
                visible: item.selected
                anchors.fill: parent
            }

        }
    }

    Component {
        id: packComp
        PackageItem {
            packageItem: item
            width: item.width
            height: item.height
            visible: item.visible
            selected: item.selected
            title: item.title
            onAddItem: (itemid)=>{
                           _flick.ctrl.addItemIntoPackage(itemid, item.id)
                       }
            onClicked: (mouse) => {
               if(_flick.addSubLink)
               {
                   if(_flick.selectedNodeId.length > 0)
                   {
                       _flick.ctrl.addLink(_flick.selectedNodeId, item.id)
                       _flick.selectedNodeId = ""
                   }
                   else
                   _flick.selectedNodeId = item.id

               }
               else if(mouse.modifiers & Qt.ControlModifier) {
                   if(selected)
                   _flick.ctrl.selectionCtrl.removeFromSelection(item)
                   else
                   _flick.ctrl.selectionCtrl.addToSelection(item)
               }
               else if(!selected){
                   _flick.ctrl.selectionCtrl.clearSelection()
                   _flick.ctrl.selectionCtrl.addToSelection(item)
               }

           }

        }
    }


    Component {
        id: nodeComp
        Node {
            id: nodeItem
            currentNode: item
            nodeStyle: _flick.ctrl.style(item.styleIndex)
            readWrite: _flick.ctrl.hasNetwork ? _flick.ctrl.readWrite : true
            focus: true
            text : item.text ? item.text : "new node"
            source: hasAvatar ? "image://nodeImages/%1".arg(item.id) : ""
            Timer {
                running: true
                repeat: true
                onTriggered: console.log("hasAvatar",hasAvatar)
            }

            visible: item.visible
            selected: item.selected
            //color: _flick.styleSheet.linkColor
            buttonColor: _flick.styleSheet.textColor
            onAddChild: {
                _flick.ctrl.addNode(item.id)
                updateZoom()
            }
            onOpenChanged: _flick.ctrl.itemModel.openItem(item.id, open)
            onReparenting: _flick.ctrl.reparenting(item,id)
            onAddImage: (img, data)=>{ _flick.ctrl.addImageFor(item.id, img, data)}
            onSelectStyle: {
                _stylePopup.parent = nodeItem
                _stylePopup.node = item
                _stylePopup.open()
            }

            onClicked: (mouse) => {
                           if(_flick.addSubLink)
                           {
                               if(_flick.selectedNodeId.length > 0)
                               {
                                   _flick.ctrl.addLink(_flick.selectedNodeId, item.id)
                                   _flick.selectedNodeId = ""
                               }
                               else
                               _flick.selectedNodeId = item.id

                           }
                           else if(mouse.modifiers & Qt.ControlModifier) {
                               if(selected)
                               _flick.ctrl.selectionCtrl.removeFromSelection(item)
                               else
                               _flick.ctrl.selectionCtrl.addToSelection(item)
                           }
                           else if(!selected){
                               _flick.ctrl.selectionCtrl.clearSelection()
                               _flick.ctrl.selectionCtrl.addToSelection(item)
                           }
                       }

        }
    }

    NodeSettingPopup {
        id: _stylePopup
        ctrl: _flick.ctrl
    }

    Item {
        id: inner
        width: _flick.ctrl.contentRect.width+_flick.marginW
        height: _flick.ctrl.contentRect.height
        anchors.centerIn: parent
        scale: _flick.zoomLevel

        MouseArea {
            anchors.fill:parent
            acceptedButtons:Qt.LeftButton
            preventStealing: true

            onPressed: (mouse)=>{
                _flick.pressed(mouse)
               ctrl.selectionCtrl.clearSelection()
               _stylePopup.node = null
            }
            onPositionChanged: (mouse)=> {
                _flick.positionChanged(mouse)
            }
            onReleased: (mouse)=>{
                _flick.released(mouse)
            }
        }

        Repeater {
            id: itemLoop
            anchors.fill: parent

            model: _flick.ctrl.itemModel


            delegate: Loader {
                property string text: label
                property QtObject item:  object
                property bool isSelected: selected
                property bool isVisible: visible
                property bool hasAvatar: hasPicture


                sourceComponent: type == MindItem.PackageType ? packComp : type == MindItem.LinkType ? linkComp : nodeComp
            }
        }
    }
}
