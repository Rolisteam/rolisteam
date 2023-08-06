import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

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
        model: _missingFilesModel
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
              text: model.name
              Layout.fillWidth: true
              elide: Text.ElideRight
            }
            ToolButton {
              id: act1Btn
             ToolTip.text: model.actions[0]
             checkable: true
             icon.name: model.icons[0]
             icon.color: "transparent"
             onClicked: _dialog.setAction(0, model.index, 0)
             checked: model.action === 0
            }
            ToolButton {
             ToolTip.text: model.actions[1]
             checkable: true
             icon.name: model.icons[1]
             icon.color: "transparent"
             onClicked: _dialog.setAction(0, model.index, 1)
             checked: model.action === 1
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
        model: _unmanagedFilesModel
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
              text: model.name
              Layout.fillWidth: true
              elide: Text.ElideRight
            }
            ToolButton {
             id: act2Btn
             ToolTip.text: model.actions[0]
             icon.name: model.icons[0]
             icon.color: "transparent"
             checkable: true
  //           checked: model.action === 0
             onClicked: _dialog.setAction(1, model.index, checked ? 0 : -1)
            }
            ToolButton {
             ToolTip.text: model.actions[1]
             icon.name: model.icons[1]
             icon.color: "transparent"
             checkable: true
//             checked: model.action === 1
             onClicked: _dialog.setAction(1, model.index, checked ? 1 : -1)
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
        enabled: _dialog ? _dialog.canValidate : false
        opacity: enabled ? 1.0 : 0.4
        onClicked: _dialog.validate()
      }
      Button {
        icon.name: "cancel"
        icon.color: "transparent"
        onClicked: _dialog.refuse()
      }
    }
  }
}
