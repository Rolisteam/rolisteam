import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import Customization 1.0

Pane
{
    id: root
    //Properties
    property QtObject style: Theme.styleSheet("Controls")
    property alias source: img.source
    property alias text: _textlbl.text
    property bool readWrite: false
    property bool isEditable: false
    property bool selected: currentNode.selected
    property int radius: root.style.radius
    property alias open: control.open
    property int expandButtonSize: root.style.expandedButtonSize
    property QtObject currentNode
    property QtObject nodeStyle
    property alias foldingBtnVisible: control.visible

    property string ident: currentNode.id
    property bool isDragged: currentNode.isDragged
    property bool dropOver: false
    property alias buttonColor: control.foreground

    x: currentNode.position.x
    y: currentNode.position.y

    visible: currentNode.visible
    onWidthChanged: currentNode.contentWidth = width
    onHeightChanged: currentNode.contentHeight = height

    onXChanged: {
        if(mouse.drag.active)
            currentNode.position=Qt.point(x, y)
    }
    onYChanged: {
       if(mouse.drag.active)
            currentNode.position=Qt.point(x, y)
   }
    Connections {
        target: currentNode
        function onPositionChanged(position) {
            x=position.x
            y=position.y
        }
    }

    focusPolicy: Qt.MouseFocusReason | Qt.ShortcutFocusReason | Qt.OtherFocusReason

    //Signals
    signal clicked(var mouse)
    signal selectStyle()
    signal reparenting(var id)
    signal addChild()
    signal addCharacter(var name, var source, var color)
    signal textEdited(var text)

    //Drag
    Drag.active: mouse.drag.active
    Drag.keys: [ "rmindmap/reparenting","text/plain" ]
    Drag.supportedActions: Qt.MoveAction
    Drag.mimeData: {
        "text/plain": root.ident
    }

    ColumnLayout {
        Image {
            id: img
            visible: source
            fillMode: Image.PreserveAspectFit
            sourceSize.height: root.style.imageSize
            sourceSize.width: root.style.imageSize
            Layout.alignment: Qt.AlignHCenter
        }
        TextInput{
            id: _textlbl
            text: root.currentNode.text
            enabled: root.readWrite && root.isEditable
            color: root.nodeStyle.textColor
            Layout.alignment: Qt.AlignHCenter
            onEnabledChanged: focus = enabled
            onEditingFinished: {
              root.isEditable = false
              root.textEdited(text)
            }
        }
    }



    background: Rectangle {
        radius: root.radius
        border.width: (root.dropOver || root.selected) ? 4 : 1
        //border.color: root.dropOver ? "red" : root.selected ? "blue": "black"
        border.color: root.dropOver ? root.style.overColor : root.selected ? root.style.highlightColor : root.style.textColor
        gradient: Gradient {
            GradientStop { position: 0.0; color: root.nodeStyle.colorOne }
            GradientStop { position: 1.0; color: root.nodeStyle.colorTwo }
        }
        MouseArea {
            id: mouse
            anchors.fill: parent
            drag.target: root
            drag.axis: Drag.XAndYAxis
            onPressed:{
                root.clicked(mouse)
                root.grabToImage(function(result) {
                                if(mouse.modifiers & Qt.ControlModifier)
                                {
                                    root.Drag.dragType = Drag.Automatic
                                    root.Drag.keys = [ "rmindmap/reparenting","text/plain" ]
                                }
                                else
                                {
                                    root.Drag.dragType = Drag.Internal
                                    root.Drag.keys = []
                                }
                                root.Drag.imageSource = result.url
                            })
            }

            onDoubleClicked: root.isEditable = true
            drag.onActiveChanged: root.isDragged = drag.active
        }

        AbstractButton {
            id: control
            property bool open: !checked
            checkable: true
            width: root.style.childrenButtonSize
            height: root.style.childrenButtonSize
            anchors.verticalCenter: parent.bottom
            anchors.horizontalCenter: parent.horizontalCenter
            topPadding: 0
            padding: 0
            rotation: control.checked ? 180 : 0
            property color foreground: root.style.textColor
            onForegroundChanged: canvas.requestPaint()
            contentItem: Canvas {
                id: canvas
                onPaint: {
                    var color = control.foreground
                    var ctx = getContext("2d")
                    ctx.fillStyle = Qt.rgba(color.r,color.g,color.b, 1)
                    ctx.beginPath();
                    ctx.moveTo(width/2,0)
                    ctx.lineTo(0,height)
                    ctx.lineTo(width,height)
                    ctx.lineTo(width/2,0)
                    ctx.closePath()
                    ctx.fill()
                }
            }
            background: Item {
            }
        }

        Rectangle {
            color: "blue"
            visible: root.readWrite
            width: 10
            opacity: 0.7
            radius: parent.radius
            height: parent.height
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            MouseArea {
                anchors.fill: parent
                enabled: root.readWrite
                onClicked: root.addChild()
            }
        }

        Rectangle {
            id: style
            visible: root.selected
            width: root.expandButtonSize
            height: root.expandButtonSize
            radius: root.expandButtonSize/2
            color: "blue"
            anchors.verticalCenter: parent.top
            anchors.horizontalCenter: parent.right
            Text {
                topPadding: 0
                padding: 0
                width: 2
                height: 2
                anchors.centerIn: parent
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                fontSizeMode: Text.Fit
                minimumPixelSize: 2
                color: "white"
                text: "▼"
            }
            MouseArea {
                anchors.fill: parent
                onClicked: selectStyle()
            }
        }

        DropArea {
            anchors.fill: parent
            keys: [ "rmindmap/reparenting","text/plain", "rolisteam/userlist-item" ]
            onDropped: {               
                var reparenting = false
                for(var i=0; i< drop.keys.length; ++i)
                {
                    if(drop.keys[i] === "rmindmap/reparenting")
                        reparenting = true
                }
                if(reparenting)
                {
                    console.log("reparenting")
                    root.reparenting(drop.text)
                }
                else
                {
                    console.log("add character")
                    root.addCharacter(drop.text, drop.urls[0], drop.colorData)
                }
                root.dropOver = false
            }
            onEntered: {
                if(drag.source === root)
                    drag.accepted = false

                if(drag.source !== root)
                    root.dropOver = true
            }
            onExited:root.dropOver = false
        }
    }
}
