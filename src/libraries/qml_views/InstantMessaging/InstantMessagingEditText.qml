import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import InstantMessaging
import Customization

Frame {
    id: root
    property alias model: selector.model
    property alias currentPersonId: selector.currentValue
    property alias currentPersonName: selector.currentText
    signal sendClicked(string text, url imageLink)
    signal focusGained()
    padding: 0

    TextWriterController {
        id: textCtrl
        text: edit.text
        onTextComputed: {
            root.sendClicked(textCtrl.text, textCtrl.imageLink)
        }
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
                                source: selector.currentValue ? "image://avatar/%1".arg(selector.currentValue) : ""
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
                        font: Theme.imFont
                        onClicked: popup.open()
                        Popup {
                            id: popup
                            width: 200
                            height: 100
                            ScrollView {
                                anchors.fill: parent
                                contentWidth: pane.width
                                contentHeight: pane.height
                                Pane {
                                    id: pane
                                    padding: 0
                                    width: gridLyt.implicitWidth+10
                                    height: gridLyt.implicitHeight
                                    GridLayout {
                                        id: gridLyt

                                        columns: 5
                                        columnSpacing: 1
                                        rowSpacing: 1
                                        Repeater {
                                            model: ["😀","😁","😂","😃","😄","😅","😆","😉","😊","😋","😎","😍","😘",
                                                    "😗","😙","😚","☺","😐","😑","😶","😏","😣","😥","😮","😯","😪",
                                                    "😫","😴","😌","😛","😜","😝","😒","😓","😔","😕","🙃","😲","☹️",
                                                    "😖","😞","😟","😢","😭","😦","😧","😨","😩","😰","😱",
                                                    "😳","😵","😡","😠","🤬","😷","😇","😈","👽","🤖","💩","😺","😸","😹","😻","😼","😽","🙀","😿","😾"]
                                            ToolButton {
                                                text: modelData
                                                flat: true
                                                objectName: "debug%1".arg(index)
                                                font: Theme.imFont
                                                padding: 0
                                                onClicked: {
                                                    edit.text += modelData
                                                }
                                            }
                                        }
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
                textCtrl.computeText()
            }
            ScrollView {
                Layout.fillWidth: true
                Layout.fillHeight: true
                clip: true
                TextArea {
                    id: edit
                    text: textCtrl.text
                    onPressed: root.focusGained()
                    font: Theme.imFont
                    Keys.onUpPressed: textCtrl.up()
                    Keys.onDownPressed: textCtrl.down()
                    Keys.onReturnPressed: event => {
                        if(event.modifiers === Qt.NoModifier)
                        {
                            layout.sendMessage()
                            event.accepted = true
                        }
                        else
                            event.accepted = false
                    }
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
