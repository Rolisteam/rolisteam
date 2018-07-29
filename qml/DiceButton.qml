import QtQuick 2.4
import QtQuick.Controls 2.0

Rectangle {
    id:root
    property alias text : textInput.text
    property alias textColor: textInput.color
    property alias hAlign: textInput.horizontalAlignment
    property alias font: textInput.font
    property alias wrapMode: textInput.wrapMode
    property string command: ""
    property alias vAlign: textInput.verticalAlignment
    property bool readOnly: false
    property bool hasAlias: true
    property string tooltip: ""


    scale: mouseZone.pressed ? 0.8 : 1.0
    signal clicked
    Drag.active: mouseZone.drag.active
    Drag.mimeData: {"text/label":root.label,
                    "text/command": root.text,
                    "text/hasAlias": root.hasAlias}
    Drag.dragType: Drag.Automatic
    Drag.supportedActions: Qt.CopyAction
    Text {//textInput.textColor
        id: textInput
        anchors.fill: parent
        clip: true
        onWidthChanged: {
            if(parent.clippedText)
                computeSizeFont();
        }
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
    }
    MouseArea {
        id: mouseZone
        anchors.fill: parent
        ToolTip.visible: root.tooltip.length >0 && mouseZone.pressed
        ToolTip.text: root.tooltip
        onClicked:  root.clicked()
        enabled: !root.readOnly
        drag.target: parent
        onPressed: parent.grabToImage(function(result) {
            parent.Drag.imageSource = result.url
        })
    }
}
