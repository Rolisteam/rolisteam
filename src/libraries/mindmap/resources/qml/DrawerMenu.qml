import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import Customization
import CustomItems

Drawer {
    id: root
    edge: Qt.RightEdge
    property QtObject styleSheet: Theme.styleSheet("mindmap")
    property alias darkMode: nightMode.checked
    property QtObject ctrl
    ColumnLayout {
        anchors.fill: parent

        RowLayout {
            Layout.fillWidth: true
            Label {
                text: qsTr("Name")
            }
            TextField {
                text: ctrl.name
                onEditingFinished: ctrl.name = text
                Layout.fillWidth: true
            }
        }
        Switch {
            id: nightMode
            text: qsTr("Night Mode")
            checked: false
        }

        RowLayout {
            Layout.fillWidth: true
            Label {
                text: qsTr("Default Style")
            }
            ComboBox {
                id: combo
                model: root.ctrl.styleModel
                currentIndex: 0
                onCurrentIndexChanged: ctrl.defaultStyleIndex = currentIndex

               contentItem: Rectangle {
                    radius: root.styleSheet.styleButtonRadius
                    width: root.styleSheet.styleButtonWidth
                    height: root.styleSheet.styleButtonHeight
                    border.width: root.styleSheet.borderSize
                    border.color: root.styleSheet.borderColor
                    gradient: Gradient {
                        GradientStop { position: 0.0; color: ctrl.getStyle(combo.currentIndex).colorOne }
                        GradientStop { position: 1.0; color: ctrl.getStyle(combo.currentIndex).colorTwo }
                    }
                    Text {
                        anchors.centerIn: parent
                        color: root.ctrl.getStyle(combo.currentIndex).textColor
                        text: qsTr("Text")
                    }
                }


                delegate: ItemDelegate {
                    width: combo.width
                    height: root.styleSheet.delegateHeight

                    Rectangle {
                        radius: root.styleSheet.styleButtonRadius
                        width: root.styleSheet.styleButtonWidth
                        height: root.styleSheet.styleButtonHeight
                        anchors.centerIn: parent
                        border.width: root.styleSheet.borderSize
                        border.color: root.styleSheet.borderColor
                        gradient: Gradient {
                            GradientStop { position: 0.0; color: colorOne }
                            GradientStop { position: 1.0; color: colorTwo }
                        }
                        Label {
                            anchors.centerIn: parent
                            color: model.textColor
                            text: qsTr("Text")
                        }
                    }
                }
            }
        }

        PermissionFrame {
          id: userlist
          Layout.fillWidth: true
          visible: ctrl.hasNetwork
          playerModel: ctrl.remotePlayerModel
          onPermissionToAllChanged: ctrl.setSharingToAll(permissionToAll)
          onPermissionForUserChanged: ctrl.setPermissionForUser(id, permission)
        }

        Label {
           property string logs
           text: qsTr("Errors:\n%1").arg(logs)
           Layout.fillHeight: true
           Layout.fillWidth: true
        }
    }
}
