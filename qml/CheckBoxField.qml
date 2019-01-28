import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Controls.Styles 1.4
import Rolisteam 1.0

Item {
    id:root

    property string text : ""
    property color textColor: "black"
    property Field field: null
    property int hAlign: 0
    property int vAlign: 0
    property bool clippedText: false
    property bool readOnly: false
    property alias checked: checkbox.checked
    property alias indicator: checkbox.indicator
    property alias contentItem: checkbox.contentItem
    property alias background: checkbox.background
    property int radius: 3
    property alias color: checkbox.color
    property string tooltip: ""

    CheckBox {
        id: checkbox
        anchors.fill: parent
        checked: root.text === "1"  ? true :false
        enabled: !root.readOnly
        leftPadding: 0
        rightPadding: 0
        topPadding: 0
        bottomPadding:0
        ToolTip.text: root.tooltip
        ToolTip.visible: root.tooltip.length >0 && checkbox.pressed
        property alias color: indic.color
        indicator: Rectangle {
            id: indic
            radius: root.radius
            border.color: "black"
            implicitWidth: root.width
            implicitHeight: root.height
            Rectangle {
                anchors.fill: parent
                scale: 0.7
                radius: root.radius
                color: "black"
                visible: checkbox.checked
            }
        }

        onToggled: {
            field.value = checked ? "1": "0"
        }
    }
}

