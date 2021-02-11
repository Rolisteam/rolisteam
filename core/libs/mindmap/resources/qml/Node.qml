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
    property alias text: text.text
    property bool isEditable: false
    property bool selected: false
    property int radius: 8
    property alias open: control.open
    property int expandButtonSize: root.style.expandedButtonSize
    property QtObject object
    property QtObject nodeStyle
    property string ident: object.id
    property bool dropOver: false
    property alias buttonColor: control.foreground

    //Signals
    signal clicked(var mouse)
    signal selectStyle()
    signal reparenting(var id)
    signal addChild()
    signal addCharacter(var name, var source, var color)


    onWidthChanged: object.contentWidth = width
    onHeightChanged: object.contentHeight = height
    onXChanged: {
        if(mouse.drag.active)
            object.position=Qt.point(x, y)
    }
    onYChanged: {
       if(mouse.drag.active)
            object.position=Qt.point(x, y)
   }

    //Drag
    Drag.active: mouse.drag.active
    Drag.keys: [ "rmindmap/reparenting","text/plain" ]
    Drag.supportedActions: Qt.MoveAction
    Drag.mimeData: {
        "text/plain": node.id
    }


    Connections {
        target: object
        function onPositionChanged(position) {
            x=position.x
            y=position.y
        }
    }

    ColumnLayout {
        Image {
            id: img
            visible: source
            fillMode: Image.PreserveAspectFit
            sourceSize.height: 100
            sourceSize.width: 100

        }
        TextInput{
            id: text
            enabled: root.isEditable
            color: root.nodeStyle.textColor
            Layout.alignment: Qt.AlignHCenter
            onEnabledChanged: focus = enabled
            onEditingFinished: root.isEditable = false
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
            drag.onActiveChanged: root.object.isDragged = drag.active
        }

        AbstractButton {
            id: control
            property bool open: !checked
            checkable: true
            visible: object.hasLink
            width: root.expandButtonSize
            height: root.expandButtonSize
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
            width: 10
            opacity: 0.7
            radius: parent.radius
            height: parent.height
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            MouseArea {
                anchors.fill: parent
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
