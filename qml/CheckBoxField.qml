import QtQuick 2.0
import QtQuick 2.4
import QtQuick.Controls 2.1
import QtQuick.Controls.Styles 1.4

Rectangle {
    id:root

    property string text : ""
    property color textColor: "black"
    property alias style: checkbox.style
    property int hAlign: 0
    property int vAlign: 0
    property bool clippedText: false
    property bool readOnly: false
    property alias checked: checkbox.checked

    CheckBox {
        id: checkbox
        anchors.fill: parent
        checked: root.text === "1"  ? true :false
        enabled: !root.readOnly
        onCheckedChanged: {
            root.text = checked ? "1": "0"
        }
    }
}

