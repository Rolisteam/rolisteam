import QtQuick 2.0
import QtQuick 2.4
import QtQuick.Controls 2.0
import QtQuick.Controls.Styles 1.4

Item {
    id:root

    property string text : ""
    property color textColor: "black"
   // property alias style: checkbox.style
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

    CheckBox {
        id: checkbox
        anchors.fill: parent
        checked: root.text === "1"  ? true :false
        enabled: !root.readOnly
        leftPadding: 0
        rightPadding: 0
        topPadding: 0
        bottomPadding:0
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

        onCheckedChanged: {
            root.text = checked ? "1": "0"
        }
    }
}

