import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Customization
import mindmap
import mindmapcpp
import rolistyle
import QtQml.Models

Flickable {
    id: _flick
    property real zoomLevel: 1
    required property MindMapController ctrl
    property QtObject styleSheet: Theme.styleSheet("mindmapinteral")
    property string selectedNodeId:""
    onSelectedNodeIdChanged: console.log("selectedIdNode:",selectedNodeId)
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
        console.log("MakeScreenShot")
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
            property bool hasValidText: _flick.ctrl.hideEmptyLabel ? objectItem.text.length > 0 : true
            writable: root.ctrl.readWrite
            visibleLabel: hasValidText ? _flick.ctrl.linkLabelVisibility : false
            onTextEdited: {
                objectItem.text = linkItem.text
            }

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
            id: item
            packageItem: objectItem
            visible: objectItem.visible
            selected: objectItem.selected
            title: objectItem.title
            editable: root.ctrl.readWrite
            onAddItem: (itemid)=>{
                           _flick.ctrl.addItemIntoPackage(itemid, objectItem.id)
                       }

            onMenu: {
                contextMenu.packageItem = packageItem
                contextMenu.subnodes = packageItem.model
                contextMenu.parent =item
                contextMenu.x = item.width - contextMenu.width
                contextMenu.open()
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
            readWrite: _flick.ctrl.readWrite
            focus: true
            text : objectItem.text
            source: hasPicture ? "image://nodeImages/%1".arg(objectItem.id) : ""
            visible: objectItem.visible
            selected: objectItem.selected
            onTextEdited: {
                objectItem.text = nodeItem.text
            }

            //color: _flick.styleSheet.linkColor
            buttonColor: _flick.styleSheet.textColor
            onAddChild: {
                _flick.ctrl.addNode(objectItem.id)
                updateZoom()
            }
            onOpenChanged: _flick.ctrl.itemModel.openItem(objectItem.id, open)
            onReparenting: (id) => {_flick.ctrl.reparenting(objectItem,id)}
            onAddImage: (img, data)=>{ _flick.ctrl.addImageFor(objectItem.id, img, data)}
            onSelectStyle: {
                _stylePopup.parent = nodeItem

                const h = Math.min(_flick.height - nodeItem.y - _flick.contentY, _stylePopup.implicitHeight)
                _stylePopup.y = h < 300 ? -Math.abs(300 - h)  : 0
                _stylePopup.x = nodeItem.width
                _stylePopup.height = Math.max(h, 300)
                _stylePopup.hasAvatar = hasPicture

                _stylePopup.node = objectItem
                _stylePopup.open()
            }

            onAddCharacter: (text, imgUrl, color) => {
                console.log("imgUrl",text," ",imgUrl)
                objectItem.imageUri = imgUrl.toString()
                objectItem.text = text
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

    Menu {
        id: contextMenu
        title: qsTr("Detach")
        property QtObject packageItem
        property alias subnodes: repeater.model

        onClosed: {
            contextMenu.subnodes = null
        }

        Instantiator {
            id: repeater

             MenuItem {
                 text: model.text
                 onTriggered: {
                     contextMenu.packageItem.removeChild(model.childId)
                 }
             }

            onObjectAdded: (index, object)=>contextMenu.insertItem(index, object)
            onObjectRemoved: (index, object)=>contextMenu.removeItem(object)
        }
    }



    Item {
        id: inner
        width: Math.max(_flick.ctrl.contentRect.width+_flick.marginW, _flick.width-_flick.marginW)
        height: Math.max(_flick.ctrl.contentRect.height, _flick.height-_flick.marginW)
        anchors.centerIn: parent
        scale: _flick.zoomLevel

        /*Timer {
            running: true
            repeat: true
            onTriggered: console.log("Mindmap - w:",_flick.width," h:",_flick.height," i:",_flick.ctrl.contentRect.width)
        }*/


        // MouseAreau
        /*MouseArea {
            anchors.fill: parent
            acceptedButtons:Qt.LeftButton
            preventStealing: true

            onClicked: {
                console.log("is clicked!")
            }

            onPressed: (mouse)=>{
                           console.log("On pressed")
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

            Rectangle {
                color: "red"
                opacity: 0.2
                anchors.fill: parent
            }
        }*/

        Repeater {
            id: itemLoop
            anchors.fill: parent
            model: _flick.ctrl.itemModel
            delegate: Loader {
                required property string label
                required property QtObject objectItem
                required property bool isSelected
                required property bool isVisible
                required property bool hasPicture
                required property int type
                sourceComponent: type == MindItem.PackageType ? packComp : type == MindItem.LinkType ? linkComp : nodeComp
            }
        }
    }
}
