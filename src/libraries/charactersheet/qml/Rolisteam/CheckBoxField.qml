import QtQuick
import QtQuick.Controls
import Rolisteam

CheckBox {
    id:root
    text : ""
    property color borderColor: "black"
    //property Field field: null
    property int hAlign: 0
    property int vAlign: 0
    property bool clippedText: false
    property bool readOnly: false
    property alias radius: indic.radius
    property alias color: indic.color
    property string tooltip: ""
    leftPadding: 0
    rightPadding: 0
    topPadding: 0
    spacing: 0
    bottomPadding:0
    checkState: (root.text === "0") ? Qt.Unchecked : (root.text === "1" && root.tristate) ? Qt.PartiallyChecked : Qt.Checked
    onCheckStateChanged: console.log("state:"+checkState)
    enabled: !root.readOnly
    checked: root.checkState === Qt.Checked  ? true :false
    onCheckedChanged: console.log("checked:"+checked)
    ToolTip.text: root.tooltip
    ToolTip.visible: root.tooltip.length >0 && checkbox.pressed

    indicator: Rectangle {
        id: indic
        border.color: root.borderColor
        border.width: 1
        width: root.width
        height: root.height
        Rectangle {
            id: filler
            anchors.fill: parent
            scale: 0.7
            radius: root.radius
            color: root.borderColor
            visible: root.checkState != Qt.Unchecked
            opacity: root.checkState == Qt.PartiallyChecked ? 0.5 : 1.0
        }
    }

    contentItem: Item {
    }

    /*onClicked: {
        console.log("toggled")
        if(root.tristate)
            field.value = checkState == Qt.Unchecked ? "0" : checkState == Qt.PartiallyChecked ? "1" : "2"
        else
            field.value = checked ? "1": "0"
    }*/
}

