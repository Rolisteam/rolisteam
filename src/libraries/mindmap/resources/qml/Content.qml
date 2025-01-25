import QtQuick
import QtQuick.Window
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs
import mindmap
import mindmapcpp
import Customization
import Utils

Pane {
    id: root

    property alias actions: _buttonGrid.actions
    required property MindMapController ctrl
    property bool darkMode: false
    property int idx: 0
    property QtObject styleSheet: Theme.styleSheet("mindmap")

    signal saveMap

    onDarkModeChanged: Theme.nightMode = root.darkMode

    MindMap {
        id: mindMap

        addSubLink: _buttonGrid.addArrow
        anchors.fill: parent
        ctrl: root.ctrl
        marginW: _minimized.width
        zoomLevel: root.ctrl.zoomLevel

        onOpenImage: id => {
            imgSelector.uuid = id;
            imgSelector.open();
        }
        /*onPositionChanged: mouse => {
            console.log("Mouse postion changed:",mouse.x, mouse.y)
            if (root.ctrl.isPackage)
                root.ctrl.updatePackage(Qt.point(mouse.x, mouse.y));
            else
                mouse.accepted = false;
        }
        onPressed: mouse => {
            console.log("Mouse on pressed:",mouse.x, mouse.y)
            if (root.ctrl.isPackage)
                root.ctrl.addPackage(Qt.point(mouse.x, mouse.y));
            else
                mouse.accepted = false;
        }
        onReleased: {
            if (root.ctrl.isPackage)
                root.ctrl.tool = MindMapController.Handler;
            else
                mouse.accepted = false;
        }*/
    }
    MouseArea {
        anchors.fill: parent
        acceptedButtons:Qt.LeftButton
        //preventStealing: true

        onClicked: {
            console.log("is clicked!")
        }

        onPressed: (mouse)=>{
                       console.log("content: On pressed ",root.ctrl.isPackage)
                       ctrl.selectionCtrl.clearSelection()
                       if (root.ctrl.isPackage)
                           root.ctrl.addPackage(Qt.point(mouse.x, mouse.y));
                       else
                           mouse.accepted = false;
                   }
        onPositionChanged: (mouse)=> {
                               console.log("On position changed")
                               //_flick.positionChanged(mouse)
                               if (root.ctrl.isPackage)
                                   root.ctrl.updatePackage(Qt.point(mouse.x, mouse.y));
                               else
                                   mouse.accepted = false;
                           }
        onReleased: (mouse)=>{
                        console.log("On released")
                        //_flick.released(mouse)
                        if (root.ctrl.isPackage)
                            root.ctrl.tool = MindMapController.Handler;
                        else
                            mouse.accepted = false;
                    }

        /*Rectangle {
            color: "red"
            opacity: 0.2
            anchors.fill: parent
        }*/
    }
    Keys.onDeletePressed: {
        root.ctrl.removeSelection();
    }
    MindMenu {
        id: menu

        ctrl: root.ctrl

        onSaveMap: {
            console.log("Content save map");
            root.saveMap();
        }
    }
    FileDialog {
        id: imgSelector

        property string uuid

        fileMode: FileDialog.OpenFile
        nameFilters: [qsTr("Images (*.jpg *.png *.jpeg *.gif *.bmp)")]

        onAccepted: {
            root.ctrl.openImage(imgSelector.uuid, imgSelector.selectedFile);
        }
    }
    FileDialog {
        id: screenShotSelector

        property string uuid

        defaultSuffix: "png"
        fileMode: FileDialog.SaveFile
        nameFilters: [qsTr("Images (*.png)")]

        onAccepted: {
            mindMap.makeScreenShot(screenShotSelector.selectedFile);
        }
    }
    RowLayout {
        anchors.right: parent.right
        anchors.rightMargin: root.styleSheet.margin
        anchors.top: parent.top
        anchors.topMargin: root.styleSheet.margin

        IconButton {
            enabled: root.ctrl.canUndo
            //undo
            source: root.styleSheet.undoIcon

            onClicked: root.ctrl.undo()
        }
        IconButton {
            enabled: root.ctrl.canRedo
            //redo
            source: root.styleSheet.redoIcon

            onClicked: root.ctrl.redo()
        }
        IconButton {
            source: root.styleSheet.listIcon

            onClicked: drawer.open()
        }
    }
    ButtonGrid {
        id: _buttonGrid

        anchors.right: parent.right
        anchors.rightMargin: 14
        anchors.top: parent.top
        anchors.topMargin: 14
        automaticSpacing: root.ctrl.spacing
        ctrl: root.ctrl

        onAutomaticSpacingChanged: root.ctrl.spacing = _buttonGrid.automaticSpacing
        onExportScene: {
            screenShotSelector.open();
        }
        onOpenDrawer: drawer.open()
        onSaveMap: {
            root.saveMap();
        }
    }
    SideMenu {
        id: drawer

        darkMode: root.darkMode
        edge: Qt.RightEdge
        height: root.height
        linkVisibility: root.ctrl.linkLabelVisibility
        mediaCtrl: root.ctrl
        width: 0.4 * root.width

        onDarkModeChanged: root.darkMode = darkMode
        onDefaultStyleChanged: root.ctrl.defaultStyleIndex = defaultStyle
    }
    Rectangle {
        anchors.fill: _minimized
        anchors.margins: 2
        border.color: root.styleSheet.borderColor
        border.width: 2
        color: root.styleSheet.colorMiniMap
        visible: _buttonGrid.showLittleMap
    }
    ShaderEffectSource {
        id: _minimized

        anchors.bottom: _bottomControl.top
        anchors.right: parent.right
        height: width / 2
        live: true
        sourceItem: mindMap.innerItem
        sourceRect: helper.maxRect(Qt.rect(0, 0, root.width, root.height), Qt.rect(0, 0, mindMap.innerItem.width, mindMap.innerItem.height))
        visible: _buttonGrid.showLittleMap
        width: _bottomControl.width

        MappingHelper {
            id: helper

        }
        Rectangle {
            border.color: "blue"
            border.width: 2
            color: "transparent"
            height: mindMap.visibleArea.heightRatio * _minimized.height
            width: mindMap.visibleArea.widthRatio * _minimized.width
            x: mindMap.visibleArea.xPosition * _minimized.width
            y: mindMap.visibleArea.yPosition * _minimized.height
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
        acceptedButtons: Qt.MiddleButton | Qt.RightButton
        anchors.fill: parent
        propagateComposedEvents: true

        onClicked: mouse => {
            menu.x = mouse.x;
            menu.y = mouse.y;
            menu.open();
        }
        onWheel: wheel => {
            var step = (wheel.modifiers & Qt.ControlModifier) ? 0.1 : 0.01;
            if (wheel.angleDelta.y > 0) {
                ctrl.zoomLevel = Math.min(ctrl.zoomLevel + step, 3.0);
            } else
                ctrl.zoomLevel = Math.max(ctrl.zoomLevel - step, 0.1);
        }
    }
}
