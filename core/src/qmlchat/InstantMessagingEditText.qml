import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.12
import InstantMessaging 1.0

Frame {
    id: root
    property alias model: selector.model
    property alias currentPersonId: selector.currentValue
    signal sendClicked(var text)
    signal focusGained()
    padding: 0

    TextWriterController {
        id: textCtrl
        text: edit.text
    }

    Connections {
        target: textCtrl
        function onTextChanged(text){ edit.text = text }
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
                        onCountChanged: {
                            if(selector.currentIndex < 0 && count > 0)
                                selector.currentIndex = 0
                        }

                        contentItem:  RowLayout {
                            Image {
                                source: "image://avatar/%1".arg(selector.currentValue)
                                fillMode: Image.PreserveAspectFit
                                Layout.fillHeight: true
                                Layout.leftMargin: 10
                                sourceSize.height: selector.height
                                sourceSize.width: selector.height
                            }
                            Label {
                                id: nameLbl
                                 leftPadding: 0
                                 rightPadding: selector.indicator.width + selector.spacing
                                 text: selector.displayText
                                 font: selector.font
                                 Layout.fillHeight: true
                                 verticalAlignment: Text.AlignVCenter
                                 horizontalAlignment: Text.AlignHCenter
                                 elide: Text.ElideRight
                                 fontSizeMode : Text.VerticalFit
                             }
                        }

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
                TextArea {
                    id: edit
                    text: textCtrl.text

                    onPressed: root.focusGained()
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
