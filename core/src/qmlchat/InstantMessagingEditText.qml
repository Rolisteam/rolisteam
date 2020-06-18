import QtQuick 2.0
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

Frame {
    id: root
    property alias model: selector.model
    signal sendClicked(var text)
    padding: 0
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
                        currentIndex: 0
                        Timer {
                            running: true
                            repeat: true
                            interval: 10000
                            onTriggered: console.log(selector.count+" "+root.model)
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
            Layout.fillWidth: true
            Layout.bottomMargin: 10
            TextArea {
                id: edit
                Layout.fillWidth: true
                Layout.fillHeight: true
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
                onClicked: root.sendClicked(edit.text)
            }
        }
    }
}
