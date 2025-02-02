import QtQuick
import QtQuick.Controls
import QtQuick.Templates as T

T.AbstractButton {
    id:root
    property color color: "#ff0000"
    property color pressedColor: "#990000"
    property color backgroundColor
    property color secondaryColor: "#00000000"// transparent
    property color textColor: text.color
    property alias hAlign: text.horizontalAlignment
    property alias wrapMode: text.wrapMode
    property string command: ""
    property alias vAlign: text.verticalAlignment
    property bool readOnly: false
    property bool hasAlias: true
    property string tooltip: ""
    property alias radius: background.radius
    down: mouseZone.pressed

    background: Rectangle {
        id: background
        opacity: enabled ? 1 : 0.3
        border.color: root.down ? root.pressedColor : root.textColor
        border.width: 1
        //color: root.down ? root.color : root.backgroundColor
        radius: 15
        gradient: Gradient {
            GradientStop { position: 0.0; color: root.down ? root.secondaryColor : root.backgroundColor }
            GradientStop { position: 1.0; color: root.down ? root.backgroundColor : root.secondaryColor }
        }
    }

    contentItem: Text {
        id: text
        font: root.font
        text: root.text
        opacity: enabled ? 1.0 : 0.3
        color: root.down ? root.pressedColor : root.textColor
        wrapMode: Text.WrapAnywhere
    }

    Drag.active: mouseZone.drag.active
    Drag.mimeData: {"text/label":root.text,
                    "text/command": root.command,
                    "text/hasAlias": root.hasAlias}
    Drag.dragType: Drag.Automatic
    Drag.supportedActions: Qt.CopyAction

    function computeSizeFont()
    {
        while((contentWidth>root.width)&&(font.pointSize>1)&&(root.width>0))
        {
            font.pointSize-=1
        }
        while((contentWidth+2<width)&&(contentHeight+2<height))
        {
            font.pointSize+=1
        }
    }

    MouseArea {
        id: mouseZone
        anchors.fill: parent
        ToolTip.visible: root.tooltip.length >0 && mouseZone.pressed
        ToolTip.text: root.tooltip
        onClicked: {
            root.clicked()
            if(root.checkable)
            {
                console.log("checked")
                root.checked = !root.checked
            }
        }
        enabled: !root.readOnly
        drag.target: parent
        onPressed: {
            parent.grabToImage(function(result) {
            parent.Drag.imageSource = result.url
        })
        }
    }
}
