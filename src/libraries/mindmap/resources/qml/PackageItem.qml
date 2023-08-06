import QtQuick
import QtQuick.Controls
import Customization

GroupBox {
    id: _root
    property QtObject style: Theme.styleSheet("Package")
    property QtObject packageItem
    property bool selected: false
    property bool dropOver: false

    signal addItem(var itemid)
    signal clicked(var mouse)

    padding: 0

    label : TextEdit {
        id: textEdit
        x:  _root.style.padding
        text: _root.packageItem.title
        color: _root.style.borderColor
        onEditingFinished: {
            _root.packageItem.title = field.text
            textEdit.readOnly = true
        }

    }

    Dialog {
        id: editTitle
        standardButtons: Dialog.Ok | Dialog.Cancel
        TextField {
            id: field
            placeholderText: qsTr("Title")
            text: _root.title
        }
        onAccepted: {
            _root.packageItem.title = field.text
            editTitle.close()
        }
    }

    x: packageItem.position.x
    y: packageItem.position.y

    onXChanged: {
        if(dragMouse.drag.active)
            packageItem.position=Qt.point(x, y)
    }
    onYChanged: {
        if(dragMouse.drag.active)
            packageItem.position=Qt.point(x, y)
    }


    background: Rectangle {
        color: _root.style.backgroundColor
        border.color: _root.style.borderColor
        border.width: _root.style.borderWidth
        Rectangle {
            border.width: 1
            border.color: "red"
            color: "transparent"
            visible: _root.selected
            anchors.fill: parent
        }
    }

    contentItem: MouseArea {
        id: dragMouse
        drag.target: _root
        drag.axis: Drag.XAndYAxis
        drag.minimumX: 0
        drag.minimumY: 0
        preventStealing: true
        drag.onActiveChanged: packageItem.isDragged = drag.active
        onDoubleClicked: {
            //editTitle.open()
            textEdit.readOnly = false
            textEdit.focus = true
        }
        onPressed: (mouse) => {_root.clicked(mouse)}

        Rectangle {
            id: _handle
            color: _root.style.borderColor
            width: _root.style.handleSize
            height: _root.style.handleSize
            x: dragMouse.width - width
            y: dragMouse.height - height
            onXChanged: {
                if(_handleMouse.drag.active)
                    packageItem.width= _handle.x + _handle.width
            }
            onYChanged: {
                if(_handleMouse.drag.active)
                    packageItem.height= _handle.y + _handle.height
            }
            MouseArea {
                    id: _handleMouse
                    anchors.fill: parent
                    drag.target: _handle
                    drag.axis: Drag.XAndYAxis
                    drag.minimumX: 0
                    drag.minimumY: 0
                    preventStealing: true
            }
        }
    }


    DropArea {
        anchors.fill: parent
        keys: [ "rmindmap/reparenting","text/plain" ]
        onDropped: (drop)=>{
            addItem(drop.text)
        }
        onEntered: (drag)=>{
            if(drag.source === root)
                drag.accepted = false

            if(drag.source !== root)
                _root.dropOver = true
        }
        onExited:_root.dropOver = false
    }
}

