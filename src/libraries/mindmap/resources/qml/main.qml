import QtQuick
import QtQuick.Controls
import mindmap
import Customization
//import utils

Item {

    visible: true
    width: 800
    height: 400
    Content {
        anchors.fill: parent
        ctrl: MindmapManager.ctrl
    }

}
