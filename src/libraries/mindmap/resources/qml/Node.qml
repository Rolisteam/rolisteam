import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Customization

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
    onWidthChanged: currentNode.width = width
    onHeightChanged: {
        currentNode.height = height
    }

    onXChanged: {
        if(dragMouse.drag.active)
            currentNode.position=Qt.point(x, y)
    }
    onYChanged: {
        if(dragMouse.drag.active)
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
    signal addImage(var img, var data)
    signal addCharacter(var name, var source, var color)
    signal textEdited(var text)

    Binding on Drag.active {
      value: dragMouse.drag.active
      delayed: true
    }
    Drag.keys: [ "rmindmap/reparenting","text/plain" ]
    Drag.supportedActions: Qt.MoveAction
    Drag.mimeData: {
        "text/plain": root.ident
    }
    Item {
        id: centralItem
        implicitWidth: lyt.implicitWidth
        implicitHeight: lyt.implicitHeight
        MouseArea {
            id: dragMouse
            anchors.fill: parent
            anchors.margins: -root.padding
            drag.target: root
            drag.axis: Drag.XAndYAxis
            drag.minimumX: 0
            drag.minimumY: 0
            preventStealing: true
            onPressed:(mouse)=>{
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

            onDoubleClicked: {
                root.isEditable = true
            }
            drag.onActiveChanged: root.isDragged = drag.active
        }
            ColumnLayout {
                id: lyt
                anchors.fill: parent
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
                    focus: true
                    onEditingFinished: {
                        root.isEditable = false
                        root.textEdited(text)
                    }

                }
            }





        AbstractButton {
            id: control
            property bool open: !checked
            property color foreground: root.style.textColor
            checkable: true
            visible: currentNode.hasLink
            width: implicitWidth //root.style.childrenButtonSize
            height: implicitHeight //root.style.childrenButtonSize
            anchors.verticalCenter: dragMouse.bottom
            anchors.horizontalCenter: dragMouse.horizontalCenter 
            topPadding: 0
            padding: 0
            contentItem: Text {
                text: control.checked ?  "▲" : "▼"
                color: control.foreground
            }

            background: Item {

            }
        }
        AbstractButton {
            visible: root.readWrite
            width: 10
            opacity: 0.7
            height: dragMouse.height
            anchors.right: dragMouse.right
            anchors.verticalCenter: dragMouse.verticalCenter
            onClicked: {
                root.addChild()
            }
            contentItem: Item{}
            background: Rectangle {
                radius: root.radius
                color: "blue"
            }
        }

        AbstractButton {
            id: style
            width: root.expandButtonSize
            height: root.expandButtonSize
            onClicked: selectStyle()
            anchors.verticalCenter: dragMouse.top
            anchors.horizontalCenter: dragMouse.right
            text: "▼"
            contentItem: Text {
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

            background: Rectangle {
                radius: root.expandButtonSize/2
                color: "blue"
            }
        }

        DropArea {
            anchors.fill: dragMouse
            keys: [ "rmindmap/reparenting","text/plain","text/uri-list", "rolisteam/userlist-item", "image/png","image/jpg","image/jpeg","image/gif" ]
            onDropped: (drop)=>{
                           console.log("keys:"+drop.keys)
                           var reparenting = false
                           var hasUrl = false
                           var character = false
                           var hasPng = false
                           var hasJpg = false
                           var hasJpeg = false
                           var hasGif = false
                           for(var i=0; i< drop.keys.length; ++i)
                           {

                                console.log("keys:"+drop.keys[i])
                                if(drop.keys[i] === "rmindmap/reparenting")
                                    reparenting = true
                                else if(drop.keys[i] === "text/uri-list")
                                    hasUrl = true
                                else if(drop.keys[i] === "rolisteam/userlist-item")
                                    character= true
                               else if(drop.keys[i] === "image/png")
                                   hasPng= true
                               else if(drop.keys[i] === "image/jpg")
                                   hasJpg= true
                               else if(drop.keys[i] === "image/jpeg")
                                   hasJpeg= true
                               else if(drop.keys[i] === "image/gif")
                                   hasGif= true

                           }
                           if(reparenting)
                           {
                               console.log("reparenting")
                               root.reparenting(drop.text)
                           }
                           else if(hasUrl && !character)
                           {
                               console.log("Has url")
                               var url = drop.urls[0]
                               var urlstr = url.toString()
                               console.log(urlstr)
                               if(urlstr.endsWith(".png") || urlstr.endsWith(".jpg") || urlstr.endsWith(".gif")|| urlstr.endsWith(".jpeg^") || urlstr.startsWith("http"))
                               {
                                    var text = hasPng ? "image/png" : hasJpg ? "image/jpg" : hasJpeg ? "image/jpeg" : "image/gif"
                                    root.addImage(url, drop.getDataAsArrayBuffer(text))
                               }
                           }
                           else if(character)
                           {
                               console.log("add character")
                               root.addCharacter(drop.text, drop.urls[0], drop.colorData)
                           }
                           root.dropOver = false
                       }
            onEntered: (drag)=>{
                           if(drag.source === root)
                           drag.accepted = false

                           if(drag.source !== root)
                           root.dropOver = true
                       }
            onExited:root.dropOver = false
        }

    }



    background: Rectangle {
        radius: root.radius
        border.width: (root.dropOver || root.selected) ? 4 : 1
        border.color: root.dropOver ? root.style.overColor : root.selected ? root.style.highlightColor : root.style.textColor
        gradient: Gradient {
            GradientStop { position: 0.0; color: root.nodeStyle.colorOne }
            GradientStop { position: 1.0; color: root.nodeStyle.colorTwo }
        }
    }
}
