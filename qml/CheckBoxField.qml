import QtQuick 2.0
import QtQuick 2.4
import QtQuick.Controls 1.3

Rectangle {
    id:root

    property string text : ""
    property color textColor: "black"
    property int hAlign: 0
    property int vAlign: 0
    property bool clippedText: false

    CheckBox {
        id: checkbox
        anchors.fill: parent
        checked: root.text === "X"  ? true :false
        onCheckedChanged: {
            root.text = checked ? "X": ""
        }
    }
}

