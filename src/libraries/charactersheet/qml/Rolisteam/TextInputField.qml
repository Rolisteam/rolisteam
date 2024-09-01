import QtQuick
import QtQuick.Controls

TextField {
    id:root

    property alias backgroundColor: back.color

    property bool clippedText: false
    property alias hAlign: root.horizontalAlignment
    property alias vAlign: root.verticalAlignment
    property string tooltip: ""
    bottomPadding: 0
    topPadding: 0
    padding: 0
    clip: root.clippedText

    function computeSizeFont()
    {
        if(!root.clippedText)
            return;

        while((contentWidth>root.width)&&(font.pointSize>1)&&(root.width>0))
        {
            font.pointSize-=1
        }
        while((contentWidth+2<width)&&(contentHeight+2<height))
        {
            font.pointSize+=1
        }

    }

    selectByMouse: true

    ToolTip.text: root.tooltip
    ToolTip.visible:root.tooltip.length >0 &&textInput.activeFocus

    background: Rectangle {
        id: back
    }
    onWidthChanged: {
        computeSizeFont();
    }
}
