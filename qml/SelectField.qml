import QtQuick 2.10
import QtQuick.Controls 2.4

ComboBox {
    id:root
    property int selected
    property var selectedValue
    property int valueRole: 0
    property bool defined: false
    property string tooltip: ""
    property color textColor:"black"
    property int hAlign: 0
    property int vAlign: 0
    property bool readOnly: false
    property alias color: back.color

    bottomPadding: 0
    leftPadding: 0
    rightPadding: 0
    topPadding: 0
    padding: 0
    spacing: 0

    ToolTip.text: root.tooltip
    ToolTip.visible: root.tooltip.length >0 && pressed

    property alias availableValues: root.model
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

    background: Rectangle {
        id: back
    }

    contentItem: Text {
      leftPadding: 0
      rightPadding: 0

      text: root.displayText
      font: root.font
      verticalAlignment: Text.AlignVCenter
      horizontalAlignment: Text.AlignHCenter
      elide: Text.ElideRight
    }
    indicator: Canvas {
              id: canvas
              x: root.width - width
              y: (root.availableHeight - height) / 2
              width: 12
              height: 8
              contextType: "2d"

              Connections {
                  target: root
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
    onCountChanged: {
        currentIndex=selected
    }
}

