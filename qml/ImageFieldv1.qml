import QtQuick 2.0
import QtQuick 2.4
import QtQuick.Controls 1.3

Rectangle {
    id:root
    property alias text : image.source
    property color textColor : "black"
    property alias hAlign: image.horizontalAlignment
    property alias vAlign: image.verticalAlignment
    property bool clippedText: false
    property bool readOnly: true //by default, the edit mode will not be made in 1.8.
    color: "black"

    Image {
        id: image
        anchors.fill: parent
        fillMode: Image.PreserveAspectFit
    }
}

