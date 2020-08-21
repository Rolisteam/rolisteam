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
            Layout.preferredHeight: frame.styleSheet.imageSize+2*spacing
            ColumnLayout {
                Image {
                    source: "image://avatar/%1".arg(model.writerId)
                    sourceSize.width:  frame.styleSheet.imageSize
                    sourceSize.height: frame.styleSheet.imageSize
                    fillMode: Image.PreserveAspectFit
                }
                Label {
                    id: timestamp
                    text: model.time
                    //anchors.right: model.local ? parent.right : undefined
                    font.pixelSize: root.styleSheet.fontSize
                    opacity: root.styleSheet.opacityTime
                }
            }
            Label {
                text: frame.message.text
                font.bold: true
                Layout.fillWidth: true
                Layout.fillHeight: true
                textFormat: Label.RichText
                minimumPixelSize: 10;
                font.pixelSize: frame.styleSheet.imageSize
                horizontalAlignment: Text.AlignHCenter
                elide:  Label.ElideRight
                wrapMode: Label.WordWrap
                clip: true
                ToolTip.visible: activeFocus
                ToolTip.text: frame.message.text
            }

            ToolButton {
                id: details
                icon.source: "qrc:/resources/images/add_round.png"
                icon.color: "transparent"
                checkable: true
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
            source: "qrc:/resources/images/logo/1000-symbole.png"
            anchors.fill: parent
            sourceSize.width: layout.implicitWidth
            sourceSize.height: layout.implicitHeight
            opacity: 0.4
            fillMode: Image.PreserveAspectFit
        }
    }
}
