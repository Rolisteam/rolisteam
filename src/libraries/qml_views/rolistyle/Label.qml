import QtQuick
import QtQuick.Controls
import QtQuick.Controls.impl
import QtQuick.Templates as T
import Customization

T.Label {
    id: control

    property QtObject themestyle: Theme.styleSheet("Palette")

    color: control.themestyle.windowText
    linkColor: control.themestyle.link
}
