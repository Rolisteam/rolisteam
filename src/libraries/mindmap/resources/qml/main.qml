import QtQuick
import QtQuick.Controls
import mindmap
import Customization
//import utils

ApplicationWindow {

    visible: true
    title: qsTr("RMindMap")
    width: 800
    height: 400
    visibility: ApplicationWindow.Windowed
    Content {
        anchors.fill: parent
        ctrl: MindmapManager
    }

}
