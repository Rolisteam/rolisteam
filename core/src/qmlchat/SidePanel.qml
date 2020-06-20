import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Drawer {
    id: root
    property alias model: contactList.model
    width: frame.implicitWidth

    Frame {
        id: frame
        ColumnLayout {
            id: layout
            anchors.fill: parent
            RowLayout {
                Layout.fillWidth: true
                Label {
                    text: qsTr("Title")
                }
                TextField {
                    id: title
                }
            }
            RowLayout {
                Layout.fillWidth: true
                Label{
                    text: qsTr("Everybody")
                }
                Switch {
                    id: all
                    checked: true
                }
            }
            Frame {
                id: recipiants
                Layout.fillWidth: true
                enabled: !all.checked
                property int checkedItem: 0
                ColumnLayout {
                    anchors.fill: parent
                    Repeater {
                        id: contactList
                        RowLayout {
                            Layout.fillWidth: true
                            Image {
                                source: "image://avatar/%1".arg(model.uuid)
                                fillMode: Image.PreserveAspectFit
                                sourceSize.width: 50
                                sourceSize.height: 50
                                opacity: all.checked ? 0.5 : 1.0
                            }
                            Label {
                                text: model.name
                                Layout.fillWidth: true
                            }
                            Switch {
                                enabled: !all.checked
                                onCheckedChanged: recipiants.checkedItem += checked ? 1 : -1
                            }
                        }
                    }
                }
            }

            Button {
                text: qsTr("Add Chatroom")
                enabled: title.text.length > 0 && (all.checked || recipiants.checkedItem > 0 )
            }
        }
    }
}
