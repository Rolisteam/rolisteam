import QtQuick 2.10
import QtQuick.Controls 2.4
import QtQuick.Controls.Styles 1.4
import Rolisteam 1.0

CheckBox {
    id:root
    text : ""
    property color borderColor: "black"
    property Field field: null
    property int hAlign: 0
    property int vAlign: 0
    property bool clippedText: false
    property bool readOnly: false
    property alias radius: indic.radius
    property alias color: indic.fillerColor
    property string tooltip: ""
    leftPadding: 0
    rightPadding: 0
    topPadding: 0
    spacing: 0
    bottomPadding:0
    enabled: !root.readOnly
    checked: root.text === "1"  ? true :false
    ToolTip.text: root.tooltip
    ToolTip.visible: root.tooltip.length >0 && checkbox.pressed

    indicator: Rectangle {
        id: indic
        property alias fillerColor: filler.color
        border.color: root.borderColor
        implicitWidth: root.width
        implicitHeight: root.height
        Rectangle {
            id: filler
            anchors.fill: parent
            scale: 0.7
            radius: root.radius
            color: "black"
            visible: root.checked
        }
    }

    onToggled: {
        field.value = checked ? "1": "0"
    }
}

