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

    signal addItem(var itemid)
    signal clicked(var mouse)

    x: packageItem.position.x
    y: packageItem.position.y
    width: _handle.x + _handle.width //objectItem.width
    height: _handle.y + _handle.height //objectItem.height

    onWidthChanged: objectItem.width = width
    onHeightChanged: objectItem.height = height

    color: _root.style.backgroundColor
    border.color: _root.style.borderColor
    border.width: _root.style.borderWidth

    onXChanged: {
        if(mainDrag.active)
            packageItem.position=Qt.point(x, y)
    }
    onYChanged: {
        if(mainDrag.active)
            packageItem.position=Qt.point(x, y)
    }

    TextEdit {
        id: textEdit

        anchors.top: parent.top
        anchors.topMargin: 5
        anchors.leftMargin: 5
        anchors.left: parent.left
        anchors.right: parent.right

        text: _root.packageItem.title
        color: _root.style.borderColor
        onEditingFinished: {
            _root.packageItem.title = field.text
            textEdit.readOnly = true
        }
    }

    DragHandler {
        id: mainDrag
        onActiveChanged: _root.packageItem.isDragged = mainDrag.active
    }

    Rectangle {
        id: _handle
        color: _root.style.borderColor
        width: _root.style.handleSize
        height: _root.style.handleSize

        x: objectItem.width - width
        y: objectItem.height - height

        DragHandler {
            id: handleDrag
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

    Rectangle {
        border.width: 1
        border.color: "red"
        color: "transparent"
        visible: _root.selected
        anchors.fill: parent
    }
}

