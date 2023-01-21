import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs
import mindmap
import Customization
import utils
import "qrc:/resources/qml"

ApplicationWindow {
    id: _app
    width: 1920
    height: 1080
    visible: true
    title: "%1 - mindmap".arg(MindmapManager.name)

    property QtObject styleSheet: Theme.styleSheet("mindmapbin")

    FileDialog {
      id: mapSelector
      property string uuid
      fileMode: FileDialog.OpenFile
      nameFilters: [qsTr("MindMap (*.rmindmap *.rmap)")]
      onAccepted: {
          MindmapManager.openFile(mapSelector.selectedFile)
      }
    }

    Content {
        id: _content
        anchors.fill: parent
        ctrl: MindmapManager
        actions: [_content.openMap]

        property Action openMap: Action {
            text: qsTr("Open MindMap")
            icon.source: _app.styleSheet.openIcon
            onTriggered: mapSelector.open()
        }

    }

}
