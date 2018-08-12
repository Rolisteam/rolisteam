import QtQuick 2.4
import QtQuick.Controls 2.3



Button {
    id:root
    property bool readOnly: false
    property string tooltip: ""
    property bool next: false
    property alias showImage: icon.visible

    display: AbstractButton.TextBesideIcon
    Image {
        id: icon
        anchors.fill: parent
        fillMode: Image.PreserveAspectFit
        source: root.next ? "image://rcs/nextpagebtn.png" : "image://rcs/previouspagebtn.png";
        horizontalAlignment: root.next ? Image.AlignLeft : Image.AlignRight
    }
}
