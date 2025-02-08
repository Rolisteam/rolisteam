import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs
import Customization
import org.rolisteam.InstantMessaging

Drawer {
    id: control
    width: pane.implicitWidth
    height: pane.implicitHeight
    interactive: control.opened

    FontDialog{
        id: fontDial
        currentFont: InstantMessagerManager.ctrl.font //Theme.imFont
        onAccepted: {
            InstantMessagerManager.ctrl.font= fontDial.selectedFont //Theme.imFont
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
                        checked: InstantMessagerManager.ctrl.nightMode
                        onCheckedChanged: {
                            InstantMessagerManager.ctrl.nightMode = nightSwitch.checked
                            //Theme.nightMode = nightSwitch.checked
                        }
                    }

                    Label {
                        text: qsTr("Sound Notification")
                    }
                    Switch {
                        id: sound
                        checked: true
                        onCheckedChanged: {
                            InstantMessagerManager.ctrl.sound = sound.checked
                        }
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
                    text: InstantMessagerManager.ctrl.font.family
                }
                Label {
                    text: qsTr("Font size:")
                }
                SpinBox {
                    id: sizeId
                    from: 0
                    value: InstantMessagerManager.ctrl.font.pixelSize
                    to: 200.0
                    onValueChanged: {
                        if(InstantMessagerManager.ctrl.font.pixelSize !== sizeId.value)
                            InstantMessagerManager.ctrl.font.pixelSize = sizeId.value
                    }
                }
            }
    }
}
