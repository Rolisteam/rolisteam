import QtQuick 2.0
import QtQuick 2.4
import QtQuick.Controls 2.0

Rectangle {
    id:root
    property alias text : textInput.text
    property alias textColor: textInput.color
    property alias hAlign: textInput.horizontalAlignment
    property alias vAlign: textInput.verticalAlignment
    property alias font : textInput.font
    property alias wrapMode : textInput.wrapMode
    property bool clippedText: false
    property alias readOnly: textInput.readOnly
    property string tooltip: ""
    TextInput {//textInput.textColor
        id: textInput
        anchors.fill: parent
        selectByMouse: true
        ToolTip.text: root.tooltip
        ToolTip.visible:root.tooltip.length >0 &&textInput.activeFocus
        onWidthChanged: {
            computeSizeFont();
        }
        function computeSizeFont()
        {
            if(parent.clippedText)
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
    }
}
