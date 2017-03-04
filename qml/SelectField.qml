import QtQuick 2.0
import QtQuick 2.4
import QtQuick.Controls 1.3

Rectangle {
    id:root
    property string text: ""
    property alias currentIndex : selectvalues.currentIndex
    property alias currentText: selectvalues.currentText
    property alias count: selectvalues.count
    property alias combo: selectvalues
    property alias style: selectvalues.style

    property color textColor:"black"
    property int hAlign: 0
    property int vAlign: 0
    property bool readOnly: false

    property alias availableValues: selectvalues.model
    property bool clippedText: false

    ComboBox {
        id: selectvalues
        anchors.fill: parent
        enabled: !root.readOnly
        onCountChanged: {
            currentIndex=selectvalues.find(root.text)
        }
    }
    onTextChanged: {
        if(selectvalues.count>0)
        {
            currentIndex=selectvalues.find(text)
        }
    }
}

