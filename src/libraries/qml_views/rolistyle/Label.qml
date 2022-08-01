import QtQuick
import QtQuick.Controls
import QtQuick.Controls.impl
import QtQuick.Templates  as T
import Customization

T.Label {
    id: control

    property QtObject style: Theme.styleSheet("Palette")

    color: control.style.windowText
    linkColor: control.style.link
}
