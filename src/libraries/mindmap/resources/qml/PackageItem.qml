import QtQuick
import QtQuick.Controls
import Customization

Rectangle {
    id: _root

    property QtObject style: Theme.styleSheet("Package")
    property QtObject packageItem
    property alias title: textEdit.text

    property bool selected: false
    property bool dropOver: false

    property alias editable: textEdit.enabled

    signal addItem(var itemid)
    signal clicked(var mouse)

    x: packageItem.position.x
    y: packageItem.position.y

    width: _handle.x + _handle.width //objectItem.width
    height: _handle.y + _handle.height //objectItem.height

    radius: 10

    onWidthChanged: packageItem.width = width
    onHeightChanged: packageItem.height = height

    color: _root.style.backgroundColor
    border.color: _root.style.borderColor
    border.width: _root.style.borderWidth

    onXChanged: {
        if(_root.packageItem.isDragged)
            packageItem.position=Qt.point(x, y)
    }
    onYChanged: {
        if(_root.packageItem.isDragged)
            packageItem.position=Qt.point(x, y)
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        drag.target: _root
        drag.onActiveChanged: _root.packageItem.isDragged = mouseArea.drag.active
        preventStealing: true
        onPressed: (mouse) => {
           console.log("click on me")
           _root.clicked(mouse)
        }
    }

    DropArea {
        anchors.fill: parent
        keys: [ "rmindmap/reparenting","text/plain","text/x-reparenting" ]
        onDropped: (drop)=>{
            const id = drop.getDataAsString("text/x-reparenting")
            console.log("Dropped:", drop.keys," :",drop.formats," text:",id)
            addItem(id)
        }
        onEntered: (drag)=>{
            //drag.accepted = true
            if(drag.source === root)
               drag.accepted = false

            if(drag.source !== root)
                _root.dropOver = true
        }
        onExited:_root.dropOver = false
    }

    TextEdit {
        id: textEdit

        anchors.top: parent.top
        anchors.topMargin: 5+_root.radius
        anchors.leftMargin: 5+_root.radius
        anchors.left: parent.left
        anchors.right: parent.right

        text: _root.packageItem.title
        color: _root.style.borderColor
        onEditingFinished: {
            _root.packageItem.title = textEdit.text
            textEdit.readOnly = true
        }
    }

    Rectangle {
        id: _handle
        color: _root.style.borderColor
        width: _root.style.handleSize
        height: _root.style.handleSize

        x: _root.packageItem.width - width
        y: _root.packageItem.height - height

        DragHandler {
            id: handleDrag
        }
    }



    Rectangle {
        border.width: 1
        border.color: "red"
        color: "transparent"
        visible: _root.selected
        anchors.fill: parent
    }
}

