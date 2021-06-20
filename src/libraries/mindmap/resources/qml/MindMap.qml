import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import Customization 1.0

Flickable {
    id: flick
    property QtObject ctrl
    property QtObject styleSheet: Theme.styleSheet("mindmapinteral")
    signal openImage(var id)

    contentHeight: mapmind.height
    contentWidth: mapmind.width
    interactive: true
    boundsBehavior: Flickable.StopAtBounds

    ScrollBar.vertical: ScrollBar { }
    ScrollBar.horizontal: ScrollBar { }

    Shortcut {
        sequence: StandardKey.Undo
        onActivated:  ctrl.undo();
        onActivatedAmbiguously: ctrl.undo()
    }
    Shortcut {
        sequence: StandardKey.Redo
        onActivated: ctrl.redo();
        onActivatedAmbiguously: ctrl.redo()
    }

    Popup {
        id: stylePopup
        property QtObject targetNode
        property bool hasImage: targetNode != null ? stylePopup.targetNode.imageUri : false
        height: flick.styleSheet.popupHeight
        x: parent.width
        y: flick.styleSheet.popupY
        modal: true
        padding: flick.styleSheet.popupPadding
        ColumnLayout {
            anchors.fill: parent
            spacing: flick.styleSheet.popupSpacing
              Button {
                icon.source: flick.styleSheet.addIcon
                icon.color: "transparent"
                Layout.fillWidth: true
                text: stylePopup.hasImage ? qsTr("Change Image") : qsTr("Set Image")
                onClicked: flick.openImage(stylePopup.targetNode.id)
              }
              ToolButton {
                icon.name: flick.styleSheet.removeIconName
                icon.source: flick.styleSheet.removeIconSource
                icon.color: "transparent"
                Layout.fillWidth: true
                text: qsTr("Remove Image")
                enabled: stylePopup.hasImage
                onClicked: ctrl.removeImage(stylePopup.targetNode.id)
              }
            GroupBox {
                id: styleTab
                title: qsTr("Node Styles")
                Layout.fillWidth: true
                Layout.fillHeight: true

                contentWidth: flickable.contentWidth
                contentHeight: flickable.contentHeight
                ScrollView {
                    id: flickable
                    anchors.fill: parent
                    contentWidth: grid.implicitWidth+15
                    contentHeight: grid.implicitHeight
                    ScrollBar.vertical.policy: ScrollBar.AlwaysOn
                    clip: true
                      GridLayout {
                          id: grid
                          columns: flick.styleSheet.gridColumnCount
                          Repeater {
                              model: ctrl.styleModel
                              Button {
                                  id: control
                                  width: flick.styleSheet.controlWidth
                                  height: flick.styleSheet.controlHeight
                                  background: Rectangle {
                                      radius: flick.styleSheet.controlRadius
                                      border.width: flick.styleSheet.borderWidth
                                      border.color: flick.styleSheet.borderColor
                                      gradient: Gradient {
                                          GradientStop { position: 0.0; color: control.pressed ? colorTwo : colorOne }//
                                          GradientStop { position: 1.0; color: control.pressed ? colorOne : colorTwo }//
                                      }

                                  }
                                  contentItem: Text {
                                      color: textColor
                                      text: qsTr("Text")
                                  }
                                  onClicked: {
                                      stylePopup.parent.object.styleIndex = index
                                      stylePopup.close()
                                  }
                              }
                          }
                    }
                }
            }

        }

    }

    Pane {
        id: mapmind
        x: 0
        y: 0
        width: Math.max(2000, ctrl.contentRect.width)
        height: Math.max(1000, ctrl.contentRect.height)

        scale: root.viewScale
        transformOrigin: Item.Center
        MouseArea {
            anchors.fill:parent
            acceptedButtons:Qt.LeftButton
            onClicked: {
              ctrl.selectionCtrl.clearSelection()
              stylePopup.targetNode = null
            }
        }
        Repeater {
            anchors.fill: parent
            model: ctrl.linkModel
            delegate: Link {
                x: startPointRole.x
                y: startPointRole.y
                width: widthLink
                height: heightLink
                start: startPointRole
                end: endPointRole
                visible: link.visible
                text: link.text
                onTextEdited: {
                  link.text = text
                }
            }
        }
        Repeater {
            anchors.fill: parent
            model: ctrl.nodeModel
            delegate: Node {
                id: nodeItem
                nodeStyle: ctrl.getStyle(node.styleIndex)
                focus: true
                readWrite: ctrl.readWrite
                currentNode: node
                source: node.imageUri ? "image://nodeImages/%1".arg(node.imageUri) : ""
                onAddChild: ctrl.addBox(node.id)
                onOpenChanged: ctrl.nodeModel.openNode(node.id, open)
                onReparenting: ctrl.reparenting(node,id)
                foldingBtnVisible: node.hasLink
                onAddCharacter: {
                    ctrl.addCharacterBox(node.id, name, source, color)
                }
                onTextEdited: {
                  node.text = text
                }

                onSelectStyle: {
                    stylePopup.parent = nodeItem
                    stylePopup.targetNode = node
                    stylePopup.open()
                }
                onClicked: {
                    if(mouse.modifiers & Qt.ControlModifier) {
                        selected ? ctrl.selectionCtrl.removeFromSelection(node) : ctrl.selectionCtrl.addToSelection(node)
                    }
                    else if(!selected){
                        root.ctrl.selectionCtrl.clearSelection()
                        root.ctrl.selectionCtrl.addToSelection(node)
                        stylePopup.targetNode = node
                    }
                }
            }
        }

    }
}
