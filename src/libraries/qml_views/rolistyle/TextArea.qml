import QtQuick
import QtQuick.Controls
import QtQuick.Controls.impl
import QtQuick.Templates as T
import Customization

T.TextArea {
    id: control

    property QtObject style: Theme.styleSheet("Palette")

    color: control.style.text
    implicitHeight: Math.max(contentHeight + topPadding + bottomPadding, implicitBackgroundHeight + topInset + bottomInset, placeholder.implicitHeight + topPadding + bottomPadding)
    implicitWidth: Math.max(contentWidth + leftPadding + rightPadding, implicitBackgroundWidth + leftInset + rightInset, placeholder.implicitWidth + leftPadding + rightPadding)
    leftPadding: padding + 4
    padding: 6
    placeholderTextColor: Color.transparent(control.color, 0.5)
    selectedTextColor: control.style.highlightedText
    selectionColor: control.style.highlight

    background: Rectangle {
        border.color: control.style.darkblue
        border.width: control.style.borderWidth
        color: control.style.base
        radius: control.style.radius
    }

    PlaceholderText {
        id: placeholder

        color: control.placeholderTextColor
        elide: Text.ElideRight
        font: control.font
        height: control.height - (control.topPadding + control.bottomPadding)
        renderType: control.renderType
        text: control.placeholderText
        verticalAlignment: control.verticalAlignment
        visible: !control.length && !control.preeditText && (!control.activeFocus || control.horizontalAlignment !== Qt.AlignHCenter)
        width: control.width - (control.leftPadding + control.rightPadding)
        x: control.leftPadding
        y: control.topPadding
    }
}
