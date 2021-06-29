import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15
import Customization 1.0

GroupBox {
    id: root
    title: qsTr("Permission")
    property QtObject styleSheet: Theme.styleSheet("PermissionFrame")
    property alias permissionToAll: everyone.permission
    property alias playerModel: contactList.model
    signal permissionForUserChanged(var id, var permission)

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
                //onPermissionChanged: ctrl.setSharingToAll(permission)
            }
        }
        Repeater {
            id: contactList

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
                    onPermissionChanged: root.permissionForUserChanged(model.uuid, permission)//ctrl.setPermissionForUser(model.uuid,permission)
                    enabled: everyone.permission === 0
                    opacity: enabled ? 1.0 : 0.5
                }
            }
        }
    }
}
