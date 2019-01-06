import QtQuick 2.10
import QtQuick.Controls 2.3

Rectangle {
    id:root
    property int selected
    property var selectedValue
    property int valueRole: 0
    property bool defined: false
    property alias currentIndex : selectvalues.currentIndex
    property alias currentText: selectvalues.currentText
    property alias count: selectvalues.count
    property alias itemDelegate: selectvalues.delegate
    property alias combo: selectvalues
    property alias indicator: selectvalues.indicator
    property string tooltip: ""
    property color textColor:"black"
    property int hAlign: 0
    property int vAlign: 0
    property bool readOnly: false
    property alias textRole: selectvalues.textRole

    property alias availableValues: selectvalues.model
    property bool clippedText: false
    Component.onCompleted:{
        currentIndex = selected
        defined = true
    }
    onSelectedChanged:{
        if(currentIndex !== selected)
        {
            currentIndex = selected
        }
    }
    ComboBox {
        id: selectvalues
        anchors.fill: parent
        padding: 0
        spacing: 0
        enabled: !root.readOnly
        ToolTip.text: root.tooltip
        ToolTip.visible: root.tooltip.length >0 && pressed
        onCountChanged: {
            currentIndex=selected
        }

        contentItem: Text {
                  leftPadding: 0
                  rightPadding: 0

                  text: selectvalues.displayText
                  font: selectvalues.font
                  verticalAlignment: Text.AlignVCenter
                  horizontalAlignment: Text.AlignHCenter
                  elide: Text.ElideRight
              }

        indicator: Canvas {
                  id: canvas
                  x: selectvalues.width - width
                  y: (selectvalues.availableHeight - height) / 2
                  width: 12
                  height: 8
                  contextType: "2d"

                  Connections {
                      target: selectvalues
                      onPressedChanged: canvas.requestPaint()
                  }

                  onPaint: {
                      context.reset();
                      context.moveTo(0, 0);
                      context.lineTo(width, 0);
                      context.lineTo(width / 2, height);
                      context.closePath();
                      context.fillStyle = "black";
                      context.fill();
                  }
              }
    }
    /*onTextChanged: {
        if(selectvalues.count>0)
        {
            currentIndex=selectvalues.find(text)
        }
    }*/
}

