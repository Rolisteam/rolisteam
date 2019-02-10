import QtQuick 2.10
import QtQuick.Controls 2.4
import QtQuick.Templates 2.12 as T

T.AbstractButton {
    id:root
    property color color: "#ff0000"
    property color pressedColor: "#990000"
    property alias backgroundColor: background.color
    property alias textColor: text.color
    property alias hAlign: text.horizontalAlignment
    property alias wrapMode: text.wrapMode
    property string command: ""
    property alias vAlign: text.verticalAlignment
    property bool readOnly: false
    property bool hasAlias: true
    property string tooltip: ""
    down: mouseZone.pressed

    background: Rectangle {
        id: background
        opacity: enabled ? 1 : 0.3
        border.color: root.down ? root.pressedColor : root.color
        border.width: 1
    }

    contentItem: Text {
        id: text
        font: root.font
        text: root.text
        opacity: enabled ? 1.0 : 0.3
        color: root.down ? root.pressedColor : root.color
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
        onClicked:  root.clicked()
        enabled: !root.readOnly
        drag.target: parent
        onPressed: {
            parent.grabToImage(function(result) {
            parent.Drag.imageSource = result.url
        })
        }
    }
}
