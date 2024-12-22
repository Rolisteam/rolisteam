import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs
import Controllers
import DicePhysics

Drawer {
    id: drawer

    property alias mouseEnabled: mouseCtrl.checked
    property alias factor: _factor.value

    height: root.height
    width: _toolBar.width + 40

    ColorDialog {
        id: colorDialog
        property int side: 0
        parentWindow: Overlay.overlay
        onAccepted: {
            if( side == 4)
                Dice3DCtrl.fourColor = selectedColor
            else if(side == 6 )
                Dice3DCtrl.sixColor = selectedColor
            else if(side == 8 )
                Dice3DCtrl.eightColor = selectedColor
            else if(side == 10 )
                Dice3DCtrl.tenColor = selectedColor
            else if(side == 12 )
                Dice3DCtrl.twelveColor = selectedColor
            else if(side == 20 )
                Dice3DCtrl.twentyColor = selectedColor
            else if(side == 100 )
                Dice3DCtrl.oneHundredColor = selectedColor
        }
    }
    Flickable {
        anchors.fill: parent
        contentHeight: _toolBar.implicitHeight
        flickableDirection: Flickable.AutoFlickIfNeeded
        ScrollBar.vertical: ScrollBar {
            policy: Screen.height < 600 ? ScrollBar.AlwaysOn : ScrollBar.AsNeeded
        }
        ColumnLayout {
            id: _toolBar
            anchors.horizontalCenter: parent.horizontalCenter


            property int fourCount: 0
            property int sixCount: 0
            property int eightCount: 0
            property int tenCount: 0
            property int twelveCount: 0
            property int twentyCount: 0
            property int oneHundredCount: 0

            Label {
                text: qsTr("Dice Size:")
            }

            Slider {
                id: _factor
                from: 15.0
                to: 100.0
                value: Dice3DCtrl.factor
                Layout.fillWidth: true

                onValueChanged:  {
                    Dice3DCtrl.factor = _factor.value;
                    console.log("value:",_factor.value)
                }
            }

            CheckBox {
                id: mouseCtrl
                text: qsTr("disable mouseArea")
                checked: false
            }

            ListModel {
                id: colors
                ListElement {
                    side: 4
                    type: DiceController.FOURSIDE
                }
                ListElement {
                    side: 6
                    type: DiceController.SIXSIDE
                }
                ListElement {
                    side: 8
                    type: DiceController.OCTOSIDE
                }
                ListElement {
                    side: 10
                    type: DiceController.TENSIDE
                }
                ListElement {
                    side: 12
                    type: DiceController.TWELVESIDE
                }
                ListElement {
                    side: 20
                    type: DiceController.TWENTYSIDE
                }
                ListElement {
                    side: 100
                    type: DiceController.ONEHUNDREDSIDE
                }
            }

            Repeater {
                model: colors

                RowLayout {
                    Layout.fillWidth: true
                    Label {
                        text: "D%1".arg(model.side)
                        Layout.fillWidth: true
                    }

                    ToolButton {
                        icon.name: "list-remove"
                        icon.color: "transparent"
                        onClicked:  {
                            Dice3DCtrl.removeDice(model.type)
                        }
                    }


                    Label {
                        id: label
                        property int value: Dice3DCtrl.diceCount(model.type)
                        text: value
                        Connections {
                            target: Dice3DCtrl
                            function onCountChanged() {
                                label.value = Dice3DCtrl.diceCount(model.type)
                            }
                        }
                    }

                    ToolButton {
                        icon.name: "list-add"
                        icon.color: "transparent"
                        onClicked:{
                            Dice3DCtrl.addDice(model.type)
                        }
                    }

                    ToolButton {
                        Layout.preferredWidth: height
                        Layout.fillHeight: true
                        contentItem: Rectangle {
                            id: rect
                            color: Dice3DCtrl.diceColor(model.type)

                            Connections {
                                target: Dice3DCtrl
                                function onColorChanged() {
                                    rect.color = Dice3DCtrl.diceColor(model.type)
                                }
                            }
                        }
                        onClicked: {
                            colorDialog.selectedColor = Dice3DCtrl.diceColor(model.type)
                            colorDialog.side = model.side
                            colorDialog.open()
                        }
                    }
                }
            }

            RowLayout {
                Label {
                    text: qsTr("Collision sound:")
                }

                ToolButton {
                    id: mute
                    checkable: true
                    checked: Dice3DCtrl.muted
                    icon.name: checked ? "audio-volume-muted" : "audio-volume-high"
                }
            }
            RowLayout {
                Label {
                    text: qsTr("Dice Command:")
                }
                Label {
                    text: Dice3DCtrl.dicePart
                }

                TextField {
                    id: command
                    onTextEdited: {
                        Dice3DCtrl.diceCommand = text
                    }
                }
            }

            GroupBox {
                Layout.fillWidth: true
                label: CheckBox {
                    id: checkBox
                    checked: Dice3DCtrl.sharedOnline
                    text: qsTr("Share rolls")
                    ToolTip.text: qsTr("(Experimental)")
                    onCheckedChanged: {
                        Dice3DCtrl.sharedOnline = checked
                    }
                }

                ColumnLayout {
                    anchors.fill: parent
                    enabled: checkBox.checked
                    opacity: enabled ? 1.0 : 0.5
                    Label {
                        text: qsTr("Duration: %1s").arg(animationTime.value)
                        ToolTip.text: qsTr("Rolls made by remote player will been shown on your screen for few seconds.\nThen, you'll see your own 3D dice.")
                    }
                    Slider {
                        id: animationTime
                        from: 5
                        to: 60
                        value: Dice3DCtrl.animationTime/1000
                        onMoved:  {
                            Dice3DCtrl.animationTime = animationTime.value * 1000;
                        }

                        Layout.fillWidth: true
                    }
                }
            }



        }
    }
}
