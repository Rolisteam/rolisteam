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
            x: objectItem.topLeftCorner.x
            y: objectItem.topLeftCorner.y
            width: objectItem.normalizedWidth
            height: objectItem.normalizedHeight
            color: _flick.styleSheet.linkColor
            opacity: objectItem.constraint ? 1.0 : 0.4
            controller: objectItem
            visible: objectItem.visible
            text: objectItem.text ?  objectItem.text : qsTr("is linked")
            visibleLabel: _flick.ctrl.linkLabelVisibility

            onSelected: {
                _flick.ctrl.selectionCtrl.clearSelection()
                _flick.ctrl.selectionCtrl.addToSelection(objectItem)
            }

            Rectangle {
                border.width: 1
                border.color: "red"
                color: "transparent"
                visible: objectItem.selected
                anchors.fill: parent
            }

        }
    }

    Component {
        id: packComp
        PackageItem {
            packageItem: objectItem
            width: objectItem.width
            height: objectItem.height
            visible: objectItem.visible
            selected: objectItem.selected
            title: objectItem.title
            onAddItem: (itemid)=>{
                           _flick.ctrl.addItemIntoPackage(itemid, objectItem.id)
                       }
            onClicked: (mouse) => {
               if(_flick.addSubLink)
               {
                   if(_flick.selectedNodeId.length > 0)
                   {
                       _flick.ctrl.addLink(_flick.selectedNodeId, objectItem.id)
                       _flick.selectedNodeId = ""
                   }
                   else
                   _flick.selectedNodeId = objectItem.id

               }
               else if(mouse.modifiers & Qt.ControlModifier) {
                   if(selected)
                   _flick.ctrl.selectionCtrl.removeFromSelection(objectItem)
                   else
                   _flick.ctrl.selectionCtrl.addToSelection(objectItem)
               }
               else if(!selected){
                   _flick.ctrl.selectionCtrl.clearSelection()
                   _flick.ctrl.selectionCtrl.addToSelection(objectItem)
               }

           }

        }
    }


    Component {
        id: nodeComp
        Node {
            id: nodeItem
            currentNode: objectItem
            nodeStyle: _flick.ctrl.style(objectItem.styleIndex)
            readWrite: _flick.ctrl.hasNetwork ? _flick.ctrl.readWrite : true
            focus: true
            text : objectItem.text ? objectItem.text : "new node"
            source: hasAvatar ? "image://nodeImages/%1".arg(objectItem.id) : ""
            visible: objectItem.visible
            selected: objectItem.selected
            //color: _flick.styleSheet.linkColor
            buttonColor: _flick.styleSheet.textColor
            onAddChild: {
                _flick.ctrl.addNode(objectItem.id)
                updateZoom()
            }
            onOpenChanged: _flick.ctrl.itemModel.openItem(objectItem.id, open)
            onReparenting: _flick.ctrl.reparenting(objectItem,id)
            onAddImage: (img, data)=>{ _flick.ctrl.addImageFor(objectItem.id, img, data)}
            onSelectStyle: {
                _stylePopup.parent = nodeItem
                _stylePopup.node = objectItem
                _stylePopup.open()
            }

            onClicked: (mouse) => {
                           if(_flick.addSubLink)
                           {
                               if(_flick.selectedNodeId.length > 0)
                               {
                                   _flick.ctrl.addLink(_flick.selectedNodeId, objectItem.id)
                                   _flick.selectedNodeId = ""
                               }
                               else
                               _flick.selectedNodeId = objectItem.id

                           }
                           else if(mouse.modifiers & Qt.ControlModifier) {
                               if(selected)
                               _flick.ctrl.selectionCtrl.removeFromSelection(objectItem)
                               else
                               _flick.ctrl.selectionCtrl.addToSelection(objectItem)
                           }
                           else if(!selected){
                               _flick.ctrl.selectionCtrl.clearSelection()
                               _flick.ctrl.selectionCtrl.addToSelection(objectItem)
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
                property QtObject objectItem:  object
                property bool isSelected: selected
                property bool isVisible: visible
                property bool hasAvatar: hasPicture


                sourceComponent: type == MindItem.PackageType ? packComp : type == MindItem.LinkType ? linkComp : nodeComp
            }
        }
    }
}
