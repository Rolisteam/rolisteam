import QtQuick
import QtQuick.Window
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs
import mindmap
import Customization
import utils

Pane {
    id: root
    visible: true
    width: 800
    height: 400
    property QtObject styleSheet: Theme.styleSheet("mindmap")
    property real viewScale: 1
    property int idx: 0
    property bool darkMode: false
    property alias addPackage: _buttonGrid.addPackage
    property MindMapController ctrl: MindmapManager.ctrl

    onDarkModeChanged: Theme.nightMode = root.darkMode

    Component.onCompleted: {
        MindmapManager.ctrl.url = "file:///home/renaud/application/mine/renaudg/rolisteam/src/tests/manual/mindmap/campaign/media/test.rmap"
        //MindmapManager.ctrl.loadFile();
    }

    MindMap {
        id: mindMap
        anchors.fill: parent
        ctrl: root.ctrl
        zoomLevel: root.ctrl.zoomLevel
        addSubLink: _buttonGrid.addArrow
        marginW: _minimized.width
        onOpenImage: (id)=>{
          imgSelector.uuid = id
          imgSelector.open()
        }
        onPressed:  (mouse)=> {
            if(root.addPackage)
                root.ctrl.addPackage(Qt.point(mouse.x, mouse.y))
            else
                mouse.accepted = false
        }
        onPositionChanged: (mouse)=>{
            if(root.addPackage)
                root.ctrl.updatePackage(Qt.point(mouse.x, mouse.y))
            else
                mouse.accepted = false
        }
        onReleased: {
            if(root.addPackage)
                root.addPackage = false
            else
                mouse.accepted = false
        }
    }
    MindMenu {
        id: menu
        ctrl: root.ctrl
    }

    FileDialog {
      id: imgSelector
      property string uuid
      fileMode: FileDialog.OpenFile
      nameFilters: [qsTr("Images (*.jpg *.png *.jpeg *.gif *.bmp)")]
      onAccepted: {
          root.ctrl.openImage(imgSelector.uuid, imgSelector.selectedFile)
      }
    }

    FileDialog {
      id: screenShotSelector
      property string uuid
      defaultSuffix: "png"
      fileMode: FileDialog.SaveFile
      nameFilters: [qsTr("Images (*.png)")]
      onAccepted: {
          mindMap.makeScreenShot(screenShotSelector.selectedFile)
      }
    }


     RowLayout{
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.rightMargin: root.styleSheet.margin
        anchors.topMargin: root.styleSheet.margin

        IconButton {//undo
            source: root.styleSheet.undoIcon
            enabled: root.ctrl.canUndo
            onClicked: root.ctrl.undo()
        }
        IconButton {//redo
            source: root.styleSheet.redoIcon
            enabled: root.ctrl.canRedo
            onClicked: root.ctrl.redo()
        }
        IconButton {
            source: root.styleSheet.listIcon
            onClicked: drawer.open()
        }
    }

    ButtonGrid {
        id: _buttonGrid
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.rightMargin: 14
        anchors.topMargin: 14
        onOpenDrawer: drawer.open()
        automaticSpacing: root.ctrl.spacing
        onAutomaticSpacingChanged: root.ctrl.spacing = _buttonGrid.automaticSpacing
        onExportScene: {
            screenShotSelector.open()
        }
    }

    SideMenu {
        id: drawer
        edge: Qt.RightEdge
        width: 0.4 * root.width
        height: root.height
        mediaCtrl: root.ctrl
        onDarkModeChanged: root.darkMode = darkMode
        onLinkVisibilityChanged: root.ctrl.linkLabelVisibility = linkVisibility
        onDefaultStyleChanged: root.ctrl.defaultStyleIndex = defaultStyle
    }

    Rectangle {
        anchors.fill: _minimized
        anchors.margins: 2
        border.width: 2
        border.color: root.styleSheet.borderColor
        color: root.styleSheet.colorMiniMap
        visible: _buttonGrid.showLittleMap
    }

    ShaderEffectSource {
        id: _minimized
        sourceItem: mindMap.innerItem
        anchors.bottom: _bottomControl.top
        anchors.right: parent.right
        live: true
        visible: _buttonGrid.showLittleMap
        MappingHelper {
            id: helper
        }
        property size computedSize: helper.mapSizeTo(
                                        Qt.size(_bottomControl.width,_bottomControl.width),
                                        Qt.size(mindMap.innerItem.width, mindMap.innerItem.height))
        height: _minimized.computedSize.height
        width: _minimized.computedSize.width

        Rectangle {
            border.width: 2
            border.color: "blue"
            color: "transparent"
            x: mindMap.visibleArea.xPosition * _minimized.width
            y: mindMap.visibleArea.yPosition * _minimized.height
            width: mindMap.visibleArea.widthRatio * _minimized.width
            height: mindMap.visibleArea.heightRatio * _minimized.height

        }
    }
    RowLayout {
        id: _bottomControl
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        Slider {
            id: slider
            from: 0.1
            to: 3
            value: root.ctrl.zoomLevel
            onValueChanged: root.ctrl.zoomLevel = value

        }
        Button {
            text: "reset"
            onClicked: slider.value = 1.0
        }
    }

    MouseArea {
        anchors.fill:parent
        acceptedButtons:Qt.MiddleButton | Qt.RightButton
        propagateComposedEvents: true
        onClicked:(mouse)=>{
            menu.x = mouse.x
            menu.y = mouse.y
            menu.open()
        }
        onWheel: (wheel)=>{
            var step = (wheel.modifiers & Qt.ControlModifier) ? 0.1 : 0.01
            if(wheel.angleDelta.y>0)
            {
                root.viewScale = Math.min(root.viewScale+step,2.0)
            }
            else
                root.viewScale = Math.max(root.viewScale-step,0.2)
        }
    }
}
