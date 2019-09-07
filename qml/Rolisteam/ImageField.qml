import QtQuick 2.10

Rectangle {
    id:root
    property alias source : image.source
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

