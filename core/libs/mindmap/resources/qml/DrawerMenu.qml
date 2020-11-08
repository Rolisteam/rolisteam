import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15
import Customization 1.0
import CustomItems 1.0

Drawer {
    id: root
    edge: Qt.RightEdge
    property QtObject styleSheet: Theme.styleSheet("mindmap")
    property alias darkMode: nightMode.checked
    property QtObject ctrl
    ColumnLayout {
        anchors.fill: parent

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


        GroupBox {
            id: userlist
            title: qsTr("Permission")
            Layout.fillWidth: true
            ColumnLayout {
                anchors.fill: parent
                RowLayout {
                    Layout.fillWidth: true
                    Label {
                        text: qsTr("All")
                    }

                    PermissionSlider {
                        id: everyone
                        Layout.fillWidth: true
                        onPermissionChanged: ctrl.setSharingToAll(permission)
                    }
                }
                Repeater {
                    id: contactList
                    model: ctrl.playerModel
                    RowLayout {
                        Layout.fillWidth: true
                        Image {
                            source: "image://avatar/%1".arg(model.uuid)
                            fillMode: Image.PreserveAspectFit
                            sourceSize.width: 50
                            sourceSize.height: 50
                            enabled: everyone.permission === 0
                            opacity: enabled ? 1.0 : 0.5
                        }
                        Label {
                            text: model.name
                            Layout.fillWidth: true
                            enabled: everyone.permission === 0
                            opacity: enabled ? 1.0 : 0.5
                        }
                        PermissionSlider {
                            Layout.fillWidth: true
                            onPermissionChanged: ctrl.setPermissionForUser(model.uuid,permission)
                            enabled: everyone.permission === 0
                            opacity: enabled ? 1.0 : 0.5
                        }
                    }
                }
            }
        }


        Label {
           property string logs
           text: qsTr("Errors:\n%1").arg(logs)
           Layout.fillHeight: true
           Layout.fillWidth: true
        }
    }
}
