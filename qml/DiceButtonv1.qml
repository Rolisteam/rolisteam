import QtQuick 2.0
import QtQuick 2.4
import QtQuick.Controls 1.3

Rectangle {
    id:root
    property alias text : textInput.text
    property alias textColor: textInput.color
    property alias hAlign: textInput.horizontalAlignment
    property alias font: textInput.font
    property alias wrapMode: textInput.wrapMode

    property alias vAlign: textInput.verticalAlignment
    property bool readOnly: false
    scale: mouseZone.pressed ? 0.8 : 1.0
    signal clicked
    Text {//textInput.textColor
        id: textInput
        anchors.fill: parent
        clip: true
        onWidthChanged: {
            computeSizeFont();
        }
        MouseArea {
            id: mouseZone
            anchors.fill: parent
            onClicked:  root.clicked()
            enabled: !root.readOnly
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
