import QtQuick 2.0
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Drawer {
    width: pane.implicitWidth
    height: pane.implicitHeight
    property alias nightMode: nightSwitch.checked

    Pane {
        id: pane
        ColumnLayout {
            RowLayout {
                Label {
                    text: qsTr("Night Mode")
                }
                Switch {
                    id: nightSwitch
                }
            }

            RowLayout {
                Label {
                    text: qsTr("Sound Notification")
                }
                Switch {

                }
            }

            Label {
                text: qsTr("Font size")
            }
            Slider {
                id: size
                from: 0
                to: 4
            }
        }
    }
}
