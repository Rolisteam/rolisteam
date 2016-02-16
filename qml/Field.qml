import QtQuick 2.0
import QtQuick 2.4
import QtQuick.Controls 1.3

Rectangle {
    id:root
    property string text:""
    property alias textColor: textInput.color
    property alias availableValues: selectvalues.model
    TextInput {
        id: textInput
        anchors.fill: parent
        selectByMouse: true
        //onTextChanged: root.text = textInput.text
    }
    ComboBox {
        id: selectvalues
        anchors.fill: parent
        onCurrentTextChanged: root.text = selectvalues.currentText
    }
   

    states: [
    State {
            name: ""
            PropertyChanges {target: selectvalues; visible: false}
            PropertyChanges {target: textInput; visible: false}
        },
    State {
            name: "field"
            PropertyChanges {target: selectvalues; visible: false}
            PropertyChanges {target: textInput; visible: true}
            PropertyChanges {target: textInput; text: root.text}
        },
        State {
            name: "combo"
            PropertyChanges {target: textInput; visible: false}
            PropertyChanges {target: selectvalues; visible: true}
            PropertyChanges {target: selectvalues; currentIndex: find(root.text)}
        }


    ]
}

