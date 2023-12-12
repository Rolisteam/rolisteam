import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Customization

ColumnLayout {
    id: root
    property QtObject styleSheet: Theme.styleSheet("InstantMessaging")
    property alias source: img.source
    property alias text: timestamp.text

    Image {
        id: img
        //source: "image://avatar/%1".arg(model.writerId)
        sourceSize.width:  root.styleSheet.imageSize
        sourceSize.height: root.styleSheet.imageSize
        fillMode: Image.PreserveAspectFit
    }
    Label {
        id: timestamp
        //text: "%1 - %2".arg(model.time).arg(model.writerName)
        font: Theme.imLittleFont
        opacity: root.styleSheet.opacityTime
    }
}
