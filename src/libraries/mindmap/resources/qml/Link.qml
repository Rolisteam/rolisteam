import QtQuick
import QtQuick.Controls
import mindmap
import mindmapcpp
import Customization

MindLink {
    id: root
    property QtObject style: Theme.styleSheet("Controls")
    property alias text: label.text
    property bool editable: false
    property alias visibleLabel: label.visible
    property alias opacityLabel: label.opacity
    property color colorBorder: root.style.borderColor
    property int borderWidth: 1
    property color backgroundLabel: root.style.backgroundColor
    property int radius: 5
    signal textEdited(var text)

    color: root.style.textColor

    FocusScope {
        id: focusScope
        anchors.fill: parent
        TextField {
            id: label
            x: root.horizontalOffset - width/2
            y: root.verticalOffset - height/2
            readOnly: !root.editable
            onReadOnlyChanged: focus = root.editable
            onEditingFinished: {
              console.log("mindlink: "+label.text)
              root.editable = false
              root.textEdited(label.text)
            }
            color: root.color

            background: Rectangle {
                border.width: root.borderWidth
                border.color: root.colorBorder
                color: label.readOnly ? Qt.darker(root.backgroundLabel) : root.backgroundLabel
                radius: root.radius
                opacity: root.opacityLabel

            }
            MouseArea {
                id: mouse
                anchors.fill: parent
                acceptedButtons: label.readOnly ? Qt.LeftButton : Qt.NoButton
                enabled: label.readOnly
                propagateComposedEvents: true
                onDoubleClicked: {
                    root.editable = true
                    focusScope.focus = true
                    label.focus = true
                }
            }
        }
    }
}
