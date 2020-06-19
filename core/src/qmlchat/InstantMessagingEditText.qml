import QtQuick 2.0
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import InstantMessaging 1.0

Frame {
    id: root
    property alias model: selector.model
    property alias currentPersonId: selector.currentValue
    signal sendClicked(var text)
    padding: 0

    TextWriterController {
        id: textCtrl
        text: edit.text
    }

    Connections {
        target: textCtrl
        function onTextChanged(){ edit.text = text }
    }

    ColumnLayout {
        anchors.fill: parent

        RowLayout {
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignTop
            ToolBar {
                Layout.fillWidth: true
                RowLayout {
                    ComboBox {
                        id: selector
                        textRole: "name"
                        valueRole: "uuid"
                        currentIndex: 0
                        delegate: ItemDelegate {
                            width: selector.width
                            contentItem: RowLayout {
                                Image {
                                    source: "image://avatar/%1".arg(model.uuid)
                                    fillMode: Image.PreserveAspectFit
                                    sourceSize.height: textlbl.height
                                    sourceSize.width: textlbl.height
                                }
                                Label {
                                    id: textlbl
                                    text: model.name
                                }
                            }
                            highlighted: selector.highlightedIndex === index
                        }
                    }
                    ToolButton {
                        text: "😀"
                        onClicked: popup.open()
                        Popup {
                            id: popup
                            GridLayout {
                                columns: 5
                                Repeater {
                                    model: ["😀","😁","😂","🤣","😃","😄","😅","😆","😉","😊","😋","😎","😍","😘","🥰","😗","😙","😚","☺"," 🙂","🤗","🤩","🤔","🤨","😐","😑","😶","🙄","😏","😣","😥","😮","🤐","😯","😪","😫","😴","😌","😛","😜","😝","🤤","😒","😓","😔","😕","🙃","🤑","😲","☹️","🙁","😖","😞","😟","😤","😢","😭","😦","😧","😨","😩","🤯","😬","😰","😱","🥵","🥶","😳","🤪","😵","😡","😠","🤬","😷","🤒","🤕","🤢","🤮","🤧","😇","🤠","🤡","🥳","🥴","🥺","🤥","🤫","🤭","🧐","🤓","😈","👿","👹","👺","💀","👻","👽","🤖","💩","😺","😸","😹","😻","😼","😽","🙀","😿","😾"]
                                    Button {
                                        text: modelData
                                        flat: true
                                        onClicked: edit.text += modelData
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        RowLayout {
            id: layout
            Layout.fillWidth: true
            Layout.bottomMargin: 10
            function sendMessage() {
                root.sendClicked(textCtrl.interpretedText())
                textCtrl.send()
            }
            ScrollView {
                Layout.fillWidth: true
                Layout.fillHeight: true
                clip: true
                //ScrollBar.vertical.interactive: true
                TextArea {
                    id: edit
                    text: textCtrl.text

                    Keys.onUpPressed: textCtrl.up()
                    Keys.onDownPressed: textCtrl.down()
                    Keys.onReturnPressed: {
                        if(event.modifiers === Qt.NoModifier)
                        {
                            layout.sendMessage()
                            event.accepted = true
                        }
                        else
                            event.accepted = false
                    }
                }
                background: Rectangle {
                    radius: 5
                    border.width: 1
                    border.color: "darkblue"
                }
            }
            Button {
                text: qsTr("send")
                Layout.fillHeight: true
                enabled:  edit.length > 0
                onClicked: {
                    layout.sendMessage()
                }
            }
        }
    }
}
