import QtQuick
import QtQuick.Controls
import mindmap
import mindmapcpp
import Customization

MindLink {
    id: root
    property QtObject style: Theme.styleSheet("Controls")
    property alias text: label.text
    property alias visibleLabel: label.visible
    property alias opacityLabel: label.opacity
    property color colorBorder: root.style.borderColor
    property int borderWidth: 1
    property color backgroundLabel: root.style.backgroundColor
    property int radius: 5
    signal textEdited(var text)

    color: root.style.textColor
    onEditingChanged: {
        label.readOnly = false
        label.enabled = true
        focusScope.focus = true
        label.focus = true
    }

    FocusScope {
        id: focusScope
        anchors.fill: parent
        TextField {
            id: label
            x: root.horizontalOffset - width/2
            y: root.verticalOffset - height/2
            readOnly: !root.editing
            enabled: root.writable
            focus: root.editing
            onEditingFinished: {
              console.log("mindlink: "+label.text)
              root.editing = false
              label.readOnly = true
              root.textEdited(label.text)
            }
            color: root.color

            background: Rectangle {
                border.width: root.borderWidth
                border.color: root.colorBorder
                color: root.editing ? root.backgroundLabel : Qt.darker(root.backgroundLabel)
                radius: root.radius
                opacity: root.opacityLabel

            }
            onPressed: (mouse)=>{
                console.log("on pressed link")
                if(root.writable)
                    root.editing = true
                //mouse.accepted = false
            }

            /*MouseArea {
                id: mouse
                anchors.fill: parent
                acceptedButtons: label.editing ? Qt.NoButton : Qt.LeftButton
                enabled: root.editable
                propagateComposedEvents: true
                onDoubleClicked: {
                    label.readOnly = false
                    label.enabled = true
                    focusScope.focus = true
                    label.focus = true
                }
            }*/
        }
    }
}
