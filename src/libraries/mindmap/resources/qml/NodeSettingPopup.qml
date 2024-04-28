import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Universal
import mindmap
import mindmapcpp

Popup {
    id: _root
    modal: true
    property QtObject node
    required property MindMapController ctrl
    ScrollView {
        id: flickable
        anchors.fill: parent
        contentWidth: mainLyt.implicitWidth+30
        contentHeight: mainLyt.implicitHeight
        ScrollBar.vertical.policy: ScrollBar.AlwaysOn
        clip: true
        ColumnLayout {
            id: mainLyt
            spacing: 0

            GridLayout {
                id: info
                columns: 1
                Layout.fillWidth: true
                Layout.rightMargin: 30
                Label {
                    text: qsTr("Description: ")
                }
                ScrollView {
                    id: view
                    Layout.rowSpan: 3
                    Layout.fillWidth: true
                    TextArea {
                        implicitHeight: 150
                        placeholderText: qsTr("Description")
                        text: _root.node ? _root.node.description : ""
                        onEditingFinished: _root.node.description = text
                    }
                }
                Label {
                    text: qsTr("tags:")
                }
                TextField {
                    text:_root.node ? _root.node.tagsText : ""
                    Layout.fillWidth: true
                    onEditingFinished: _root.node.tagsText = text
                }
                Label {
                    text: qsTr("Action:")
                }
                RowLayout {
                    Layout.fillWidth: true
                    Button {
                        text: qsTr("Remove Avatar")
                        onClicked: _root.ctrl.removeImageFor(_root.node.id)
                    }
                }
            }

            GroupBox {
                id: styleTab
                title: qsTr("Node Styles")
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.rightMargin: 30
                GridLayout {
                    id: grid
                    columns: 3
                    anchors.fill: parent
                    Repeater {
                        model: _root.ctrl.styleModel
                        Button {
                            id: control
                            Layout.preferredWidth: 120
                            Layout.preferredHeight: 30

                            background: Rectangle {
                                radius: 8
                                border.width: 1
                                border.color: "black"
                                gradient: Gradient {
                                    GradientStop { position: 0.0; color: control.pressed ? colorTwo : colorOne }//
                                    GradientStop { position: 1.0; color: control.pressed ? colorOne : colorTwo }//
                                }

                            }
                            contentItem: Text {
                                color: textColor
                                text: qsTr("Text")
                                horizontalAlignment: Text.AlignHCenter
                            }
                            onClicked: {
                                _root.node.styleIndex = index
                                _root.close()
                            }
                        }
                    }
                }
            }
        }
    }

}
