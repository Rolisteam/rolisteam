import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import mindmap
import Customization
import CustomItems
import mindmapcpp

Drawer {
    id: _drawer
    property QtObject styleSheet: Theme.styleSheet("mindmapinteral")
    property alias darkMode: darkModeSwitch.checked
    property alias linkVisibility: linkVisible.checked
    property alias defaultStyle: combo.currentIndex
    required property MindMapController mediaCtrl

    SideMenuController {
        id: ctrl
        controller: _drawer.mediaCtrl
        criteria: criteria.currentIndex
        pattern: textField.text
    }
    Pane {
        anchors.fill: parent


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
                id: darkModeSwitch
                text: qsTr("Night Mode")
            }
            Switch {
                id: linkVisible
                text: qsTr("Link label visible:")
                checked: _drawer.mediaCtrl.linkLabelVisibility
                onCheckedChanged:  {
                    _drawer.mediaCtrl.linkLabelVisibility = linkVisible.checked
                }
            }
            Switch {
                id: emptyLabel
                text: qsTr("Hide empty label")
                enabled:  _drawer.mediaCtrl.linkLabelVisibility
                checked: _drawer.mediaCtrl.hideEmptyLabel
                onCheckedChanged:  {
                    _drawer.mediaCtrl.hideEmptyLabel = emptyLabel.checked
                }
            }
            RowLayout {
                Layout.fillWidth: true
                Label {
                    text: qsTr("Default Style")
                }
                ComboBox {
                    id: combo
                    model: _drawer.mediaCtrl.styleModel
                    currentIndex: 0

                    contentItem: Rectangle {
                        radius: 8
                        width: 80
                        height: 15
                        border.width: 1
                        border.color: "black"
                        gradient: Gradient {
                            GradientStop { position: 0.0; color: _drawer.mediaCtrl.style(combo.currentIndex).colorOne }
                            GradientStop { position: 1.0; color: _drawer.mediaCtrl.style(combo.currentIndex).colorTwo }
                        }
                        Text {
                            anchors.centerIn: parent
                            color: _drawer.mediaCtrl.style(combo.currentIndex).textColor
                            text: qsTr("Text")
                        }
                    }


                    delegate: ItemDelegate {
                        width: combo.width
                        height: 20

                        Rectangle {
                            radius: 8
                            width: 80
                            height: 15
                            anchors.centerIn: parent
                            border.width: 1
                            border.color: "black"
                            gradient: Gradient {
                                GradientStop { position: 0.0; color: model.colorOne }
                                GradientStop { position: 1.0; color: model.colorTwo }
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
                visible: !mediaCtrl.remote
                playerModel: mediaCtrl.remotePlayerModel
                onPermissionToAllChanged: mediaCtrl.setSharingToAll(permissionToAll)
                onPermissionForUserChanged: mediaCtrl.setPermissionForUser(id, permission)
            }
            GridLayout {
                columns: 2
                Layout.fillWidth: true
                Label {
                    text: qsTr("Criteria")
                }
                ComboBox {
                    id: criteria
                    model: ["None", "Name", "Tag", "Parent"]
                    onCurrentIndexChanged: ctrl.criteria = currentIndex
                }
                Label {
                    text: qsTr("Pattern")
                }
                TextField {
                    id: textField
                    Layout.fillWidth: true
                }
            }

            ListView {
                id: _list
                model: ctrl.model
                Layout.fillWidth: true
                clip: true
                Layout.fillHeight: true
                delegate:  GridLayout {
                    columns: 4
                    width:  _list.width

                    property QtObject item:  object
                    enabled: mediaCtrl.readWrite
                    Image {
                        Layout.column: 0
                        source: hasPicture ? "image://nodeImages/%1".arg(item.id) : ""
                        sourceSize.width: 100
                        sourceSize.height: 100
                        Layout.preferredWidth: Math.min(_drawer.width/4, 100)
                        fillMode: Image.PreserveAspectFit
                        Layout.rowSpan: 2
                    }
                    TextField {
                        Layout.column: 1
                        text: item.text
                        placeholderText: qsTr("Name")
                        onEditingFinished: item.text = text
                        Layout.fillWidth: true
                        enabled: mediaCtrl.readWrite
                    }
                    ScrollView {
                        Layout.column: 2
                        Layout.rowSpan: 2
                        Layout.fillHeight: true
                        Layout.preferredWidth: _drawer.width/4
                        Layout.maximumWidth: _drawer.width/4
                        clip: true
                        TextArea {
                            text: item.description
                            placeholderText: qsTr("Description")
                            onEditingFinished: item.description = text
                            enabled: mediaCtrl.readWrite
                        }
                    }
                    CheckBox {
                        text: qsTr("Select")
                        enabled: mediaCtrl.readWrite
                        Layout.column: 3
                        onCheckedChanged:{
                            if(checked)
                                MindmapManager.ctrl.selectionCtrl.addToSelection(item)
                            else
                                MindmapManager.ctrl.selectionCtrl.removeFromSelection(item)
                        }
                    }
                    TextField {
                        text: item.tagsText
                        placeholderText: qsTr("Tags")
                        enabled: mediaCtrl.readWrite
                        onEditingFinished: item.tagsText = text
                        Layout.fillWidth: true
                        Layout.column: 1
                        Layout.row :1
                    }

                    Button {
                        text: "apply to all"
                        Layout.column: 3
                        enabled: mediaCtrl.readWrite
                        Layout.row :1
                    }
                    Rectangle {
                        Layout.preferredHeight: 3
                        Layout.column: 0
                        Layout.row :2
                        Layout.columnSpan: 4
                        Layout.fillWidth: true
                        color: _drawer.styleSheet.textColor
                    }
                }

            }
        }
        /*Label {
            id: logs
            Layout.fillHeight: true
            Layout.fillWidth: true
        }*/
    }
}
