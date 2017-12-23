import QtQuick 2.0
import QtQuick 2.4
import QtQuick.Controls 2.0

Rectangle {
    id:root
    property alias text : textArea.text
    property alias textColor: textArea.color
    property alias font: textArea.font
    property alias wrapMode : textArea.wrapMode
    property alias hAlign: textArea.horizontalAlignment
    property alias vAlign: textArea.verticalAlignment
    property bool clippedText: false
    property bool readOnly: false
    property bool backgroundVisible: true
    property alias background: textArea.background

    TextArea {
        id: textArea
        anchors.fill: parent
        selectByMouse: true
        readOnly: root.readOnly
        textFormat: TextEdit.RichText
    }
}
