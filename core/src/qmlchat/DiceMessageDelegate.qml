import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.12
import Customization 1.0

Frame {
    id: frame
    property QtObject message: model.message
    property QtObject styleSheet: Theme.styleSheet("InstantMessaging")
    ColumnLayout {
        id: layout
        anchors.fill: parent
        property alias showDetails: details.checked
        Label {
            text: frame.message.comment
            visible: frame.message.comment
            Layout.fillWidth: true
        }
        RowLayout {
            Layout.fillWidth: true
            Layout.preferredHeight: Math.max(frame.styleSheet.imageSize+2*spacing, mainText.height+2*spacing)
            Layout.maximumHeight: layout.showDetails ? Number.POSITIVE_INFINITY : Math.max(frame.styleSheet.imageSize+2*spacing, mainText.height+2*spacing)

            ColumnLayout {
                id: iconPart
                Image {
                    source: "image://avatar/%1".arg(model.writerId)
                    sourceSize.width:  frame.styleSheet.imageSize
                    sourceSize.height: frame.styleSheet.imageSize
                    fillMode: Image.PreserveAspectFit
                }
                Label {
                    id: timestamp
                    text: model.time
                    font.pixelSize: root.styleSheet.fontSize*root.fontFactor
                    opacity: root.styleSheet.opacityTime
                }
            }
            Label {
                id: mainText
                text: frame.message.text
                font.bold: true
                Layout.fillWidth: true
                textFormat: Label.RichText
                antialiasing: false
                minimumPixelSize: 10;
                font.pixelSize: frame.styleSheet.imageSize*root.fontFactor
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Label.WordWrap
                ToolTip.visible: activeFocus
                ToolTip.text: frame.message.text
            }

            ToolButton {
                id: details
                icon.source: "qrc:/resources/rolistheme/add_round.png"
                icon.color: "transparent"
                checkable: true
                rotation: checked ? 45 : 0
                flat: true
            }
        }
        Label {
            text: qsTr("Details: %1").arg(frame.message.details)
            textFormat: Label.RichText
            visible: layout.showDetails
        }
        Label {
            text: qsTr("Command: %1").arg(frame.message.command)
            visible: layout.showDetails
        }

        TextArea {
            text: frame.message.json
            visible: layout.showDetails
        }

    }

    background: Rectangle {
        border.width: 1
        border.color: "black"
        color: "lightblue"
        radius: 10
        opacity: 0.5

        Image {
            source: "qrc:/resources/rolistheme/1000-symbole.png"
            anchors.fill: parent
            sourceSize.width: layout.implicitWidth
            sourceSize.height: layout.implicitHeight
            opacity: 0.4
            fillMode: Image.PreserveAspectFit
        }
    }
}
