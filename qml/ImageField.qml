import QtQuick 2.0
import QtQuick 2.4
import QtQuick.Controls 1.3

Rectangle {
    id:root
    property alias text : textArea.source
    property alias textColor: textArea.textColor
    property alias hAlign: textArea.horizontalAlignment
    property alias vAlign: textArea.verticalAlignment
    property bool clippedText: false
    Image {
        id: textArea
        anchors.fill: parent
        fillMode: Image.PreserveAspectFit
    }
}

