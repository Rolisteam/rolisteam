import QtQuick 2.0
import QtQuick.Controls 2.12
import QtQuick.Controls.Universal 2.12
import RMindMap 1.0

MindLink {
    id: root
    property alias text: label.text
    property bool editable: false
    property alias visibleLabel: label.visible
    property alias opacityLabel: label.opacity
    property color colorBorder: "gray"
    property int borderWidth: 1
    property color backgroundLabel: Universal.background
    property int radius: 5
    property real opacityLabel: 0.8
    property QtObject object

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
            onEditingFinished: root.editable = false
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
