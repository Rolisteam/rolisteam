import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Customization

Frame {
    id: frame
    property QtObject message: model.message
    property QtObject styleSheet: Theme.styleSheet("InstantMessaging")

    padding: frame.styleSheet.padding+10
    Layout.minimumWidth: iconPart.implicitWidth + mainText.implicitWidth + details.implicitWidth + 2 * spacing + 50
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
            Layout.preferredHeight: Math.max(iconPart.implicitHeight,Math.max(frame.styleSheet.imageSize+2*spacing, mainText.height+2*spacing))
            Layout.maximumHeight: layout.showDetails ? Number.POSITIVE_INFINITY : Math.max(frame.styleSheet.imageSize+2*spacing, mainText.height+2*spacing)

            AvatarPanel {
                id: iconPart
                source:"image://avatar/%1".arg(model.writerId)
                text: "%1 - %2".arg(model.time).arg(model.writerName)
            }

            Label {
                id: mainText
                text: frame.message.result
                font: Theme.imBigFont
                //font.bold: true
                Layout.fillWidth: true
                textFormat: Label.RichText
                antialiasing: false
                minimumPixelSize: 10;
                //font.pixelSize: frame.styleSheet.imageSize*frame.factor
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Label.WordWrap
                ToolTip.visible: activeFocus
                ToolTip.text: frame.message.result
            }

            ToolButton {
                id: details
                Layout.rightMargin: frame.styleSheet.rightMarge

                background: Image {
                    source: "qrc:/resources/rolistheme/up_arrow.svg"
                    sourceSize.height: frame.styleSheet.detailImageSize
                    sourceSize.width: frame.styleSheet.detailImageSize
                }


                checkable: true
                ToolTip.text: qsTr("Details")
                rotation: checked ? 0 : 180
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
    }

    background: Rectangle {
        border.width: 1
        border.color: "black"
        color: "lightblue"
        radius: 10
        opacity: 0.5

        Image {
            source: "qrc:/resources/rolisteam/logo/1000-symbole.png"
            anchors.fill: parent
            sourceSize.width: layout.implicitWidth
            sourceSize.height: layout.implicitHeight
            opacity: 0.4
            fillMode: Image.PreserveAspectFit
        }
    }
}
