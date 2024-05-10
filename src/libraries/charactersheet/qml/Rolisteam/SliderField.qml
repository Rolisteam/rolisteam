import QtQuick
import QtQuick.Controls
import Rolisteam

Slider {
    id: _root
    //required property QtObject field
    property bool readOnly: false
    property color backgroundColor
    property color valueColor
    property color handleColor: _root.pressed ? "#f0f0f0" : "#f6f6f6"
    property color handleBorderColor: "#bdbebf"
    enabled: !_root.readOnly
    property string tooltip: ""
    ToolTip.text: _root.tooltip
    ToolTip.visible: _root.tooltip.length >0 && checkbox.pressed

    background: Rectangle {
             x: _root.leftPadding
             y: _root.topPadding + _root.availableHeight / 2 - height / 2
             width: _root.availableWidth
             height: _root.height
             radius: 2
             color: _root.backgroundColor

             Rectangle {
                 width: _root.visualPosition * parent.width
                 height: parent.height
                 color: _root.valueColor
                 radius: 2
             }
         }

    handle: Rectangle {
        x: -width/2 + _root.leftPadding + _root.visualPosition * _root.width
        y: _root.topPadding + _root.availableHeight / 2 - height / 2
        implicitWidth: _root.height
        height: _root.height
        radius: _root.height/4
        border.color: _root.handleBorderColor
        color: _root.handleColor
    }

    /*onMoved: {
        if(_root.field)
            _root.field.value = _root.value
    }*/
}
