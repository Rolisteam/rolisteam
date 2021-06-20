import QtQuick 2.0
import QtQuick.Controls 2.12
import RMindMap 1.0
import Customization 1.0

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
    property real opacityLabel: 0.8
    property QtObject object
    signal textEdited(var text)

    color: root.style.textColor

    FocusScope {
        id: focusScope
        anchors.centerIn: parent
        TextField {
            id: label
            text: label
            anchors.centerIn: parent
            readOnly: !root.editable
            focusReason: Qt.MouseFocusReason
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
