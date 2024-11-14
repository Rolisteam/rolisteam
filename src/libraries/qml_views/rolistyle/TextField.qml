import QtQuick
import QtQuick.Controls
import QtQuick.Controls.impl
import QtQuick.Templates as T
import Customization

T.TextField {
    id: control

    property QtObject style: Theme.styleSheet("Palette")

    color: control.style.text
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset, contentHeight + topPadding + bottomPadding, placeholder.implicitHeight + topPadding + bottomPadding)
    implicitWidth: implicitBackgroundWidth + leftInset + rightInset || Math.max(contentWidth, placeholder.implicitWidth) + leftPadding + rightPadding
    leftPadding: padding + 4
    padding: 6
    placeholderTextColor: Color.transparent(control.color, 0.5)
    selectedTextColor: control.style.highlightedText
    selectionColor: control.style.highlight
    verticalAlignment: TextInput.AlignVCenter

    background: Rectangle {
        border.color: control.activeFocus ? control.style.highlight : control.style.mid
        border.width: control.activeFocus ? 2 : 1
        color: control.style.base
        implicitHeight: 40
        implicitWidth: 200
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
