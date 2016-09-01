import QtQuick 2.0
import QtQuick 2.4
import QtQuick.Controls 1.3

Rectangle {
    id:root
    property alias text : textInput.text
    property alias textColor: textInput.color
    property alias hAlign: textInput.horizontalAlignment
    property alias vAlign: textInput.verticalAlignment
    property bool clippedText: false
    property bool readOnly: false
    TextInput {//textInput.textColor
        id: textInput
        anchors.fill: parent
        selectByMouse: true
        readOnly: root.readOnly
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
