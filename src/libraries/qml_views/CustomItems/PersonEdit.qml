import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts
import CustomItems
import Profile

GridLayout {
    id: _root
    columns: 3
    property alias imageData: _img.imageData
    property alias characterName: _nameField.text
    property alias color: _color.color
    property alias validInput: _nameField.validInput
    property alias isSquare: _img.isSquare
    property alias validImg: _img.hasImage
    signal clicked
    signal nameEdited(string name)
    signal colorEdited(color col)

    property alias line : _colorLine.data

   // implicitHeight: childrenRect.height//Math.max(_img.height,_nameField.height+_colorLine.implicitHeight)
    ImageSelector {
        id: _img
        Layout.rowSpan: 2
        Layout.preferredHeight: 80
        Layout.preferredWidth: 80
        onMouseClicked:  _root.clicked()

        Rectangle {
            anchors.centerIn: parent
            width: parent.width+2
            height: parent.height+2
            color: "transparent"
            border.width: 1
            border.color: "red"
            visible: !_img.isSquare
            Image {
                anchors.centerIn: parent
                width: parent.width-2
                height: parent.height-2
                source: "qrc:/resources/rolistheme/contact.svg"
            }

            Label {
                text: qsTr("No Image")
                font.pixelSize: 9
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 20
                anchors.horizontalCenter: parent.horizontalCenter

            }
        }
    }
    Label {
        text: qsTr("Name:")
    }
    RequiredTextField {
        id: _nameField
        Layout.fillWidth: true
        onTextEdited: _root.nameEdited(_nameField.text)
    }
    Label {
        text: qsTr("Color:")
    }
    RowLayout {
        id: _colorLine
        ColorPicker {
            id: _color
            Layout.preferredHeight: 30
            Layout.preferredWidth: 30
            onColorEdited: (col)=> _root.colorEdited(col)
        }
    }

}
