import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs
import mindmap
import Customization
import Utils
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
    FileDialog {
        id: saveDialog
        fileMode: FileDialog.SaveFile
        nameFilters: [qsTr("MindMap (*.rmindmap *.rmap)")]
        onAccepted: {
            MindmapManager.url = saveDialog.selectedFile
            MindmapManager.saveFile()
        }
    }

    Content {
        id: _content
        anchors.fill: parent
        ctrl: MindmapManager
        actions: [_content.openMap]

        onSaveMap: {
            console.log("Menu save map: ",MindmapManager.url)
            if(MindmapManager.url.toString())
            {
                console.log("Menu valid: ",MindmapManager.url)
                ctrl.saveFile();
            }
            else
            {
                console.log("Menu invalid: ",MindmapManager.url)
                saveDialog.open();
            }
        }

        property Action openMap: Action {
            text: qsTr("Open MindMap")
            icon.source: _app.styleSheet.openIcon
            onTriggered: mapSelector.open()
        }

    }

}
