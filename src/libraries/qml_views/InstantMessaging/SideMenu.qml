import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs
import Customization

Drawer {
    id: control
    width: pane.implicitWidth
    height: pane.implicitHeight
    interactive: control.opened
    property alias nightMode: nightSwitch.checked
    property alias sound: sound.checked

    FontDialog{
        id: fontDial
        currentFont: Theme.imFont
        onAccepted: {
            Theme.imFont = fontDial.selectedFont
        }
    }

    Pane {
        id: pane
        GridLayout {
            columns: 2
                Label {
                    text: qsTr("Night Mode")
                }
                Switch {
                    id: nightSwitch
                }



                Label {
                    text: qsTr("Sound Notification")
                }
                Switch {
                    id: sound
                    checked: true
                }


            Button {
                Layout.columnSpan: 2
                Layout.alignment: Qt.AlignHCenter
                text: qsTr("Select Font…")
                onClicked: {
                    fontDial.open()
                }
            }

            Label {
                text: qsTr("Font Family:")
            }
            Label {
                text: Theme.imFont.family
            }
            Label {
                text: qsTr("Font size:")
            }
            SpinBox {
                id: sizeId
                from: 0
                value: Theme.imFont.pixelSize
                to: 200.0
                onValueChanged: {
                    if(Theme.imFont.pixelSize !== sizeId.value)
                        Theme.imFont.pixelSize = sizeId.value
                }
            }
        }
    }
}
