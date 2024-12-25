import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Integrity

Item {
  id: root
  ColumnLayout {
    anchors.fill: parent
    GroupBox {
      title: qsTr("Missing files:")
      Layout.fillWidth: true
      Layout.fillHeight: true

      ListView {
        id: view
        clip: true
        anchors.fill: parent
        model: Controller.missingFiles
        delegate: Pane {
          width: view.width
          height: lyt.implicitHeight
          padding: 0
          ButtonGroup {
              buttons: lyt.children
          }
          RowLayout {
            id: lyt
            anchors.fill: parent
            Label {
              id: lbl1
              text: modelData
              Layout.fillWidth: true
              elide: Text.ElideRight
            }
            ToolButton {
              id: act1Btn
             ToolTip.text: qsTr("Remove file from campaign history")
             icon.name: "edit-delete"
             icon.color: "transparent"
             onClicked: Controller.performAction(modelData, Controller.RemoveFromCampaign)
            }
            ToolButton {
             ToolTip.text: qsTr("Create blank document")
             checkable: true
             icon.name: "document-new"
             icon.color: "transparent"
             onClicked: Controller.performAction(modelData, Controller.CreateBlank)
            }
          }
        }
      }
    }
    GroupBox {

      title: qsTr("Unmanaged files:")
      Layout.fillWidth: true
      Layout.fillHeight: true
      ListView {
        id: view2
        anchors.fill: parent
        clip: true
        model: Controller.unmanagedFile
        delegate: Pane {
          width: view2.width
          height: lyt2.implicitHeight
          padding: 0
          ButtonGroup {
              buttons: lyt2.children
          }
          RowLayout {
            id: lyt2
            anchors.fill: parent
            Label {
              id: lbl2
              text: modelData
              Layout.fillWidth: true
              elide: Text.ElideRight
            }
            ToolButton {
             id: act2Btn
             ToolTip.text: qsTr("Add file into project")
             icon.name: "list-add"
             icon.color: "transparent"
             onClicked: Controller.performAction(modelData, Controller.AddToCampaign)
            }
            ToolButton {
             ToolTip.text: qsTr("Delete file from Disk")
             icon.name: "list-remove"
             icon.color: "transparent"
             onClicked: Controller.performAction(modelData, Controller.RemoveFromDisk)
            }
          }
        }
      }
    }
    RowLayout {
      Item {
        Layout.fillWidth: true
      }
      Button {
        icon.name: "apply"
        icon.color: "transparent"
        enabled: true
        onClicked: Controller.accept()
      }
    }
  }
}
