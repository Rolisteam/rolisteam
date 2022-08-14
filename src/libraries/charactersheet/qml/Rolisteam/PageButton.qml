import QtQuick
import QtQuick.Controls

Button {
    id:root
    property bool readOnly: false
    property string tooltip: ""
    property bool next: false
    property bool showImage: true

    display: showImage ? AbstractButton.TextBesideIcon : AbstractButton.TextOnly

    icon.color: "transparent"
    icon.source: root.next ? "image://rcs/nextpagebtn.png" : "image://rcs/previouspagebtn.png"
}
