import QtQuick
import QtQuick.Controls.impl
import QtQuick.Controls.Fusion
import QtQuick.Controls.Fusion.impl
import Customization

Rectangle {
    id: panel

    property Item control
    property bool highlighted: control.highlighted
    property QtObject style: Theme.styleSheet("Palette")

    visible: !control.flat || control.down || control.checked

    color: Theme.buttonColor(panel.style.button, panel.style.highlight, panel.highlighted, control.down || control.checked,
                              enabled && control.hovered)
    gradient: control.down || control.checked ? null : buttonGradient

    Gradient {
        id: buttonGradient
        property color base: Theme.buttonColor(panel.style.button, panel.style.highlight, panel.highlighted,
                                               panel.control.down, panel.enabled && panel.control.hovered)
        GradientStop {
            position: 0
            color: Theme.nightMode ? Qt.darker(buttonGradient.base, 150/100) : Qt.lighter(buttonGradient.base, 124/100)
        }
        GradientStop {
            position: 1
            color: Theme.nightMode ? Qt.darker(buttonGradient.base, 102/100) : Qt.lighter(buttonGradient.base, 102/100)
        }
    }

    radius: 2
    border.color: Theme.buttonOutline(panel.style.highlight, panel.style.window, panel.highlighted || control.visualFocus, control.enabled)

    Rectangle {
        x: 1; y: 1
        width: parent.width - 2
        height: parent.height - 2
        border.color: Fusion.innerContrastLine
        color: "transparent"
        radius: 2
    }
}
