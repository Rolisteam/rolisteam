import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import CustomItems 1.0
import Profile 1.0

GridLayout {
    id: _root
    columns: 3
    property alias imageData: _img.imageData
    property alias characterName: _nameField.text
    property alias color: _color.color
    property alias validInput: _nameField.validInput
    property alias isSquare: _img.isSquare
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
    }
    Label {
        text: qsTr("Name:")
    }
    RequiredTextField {
        id: _nameField
        Layout.fillWidth: true
        onEditingFinished: _root.nameEdited(name)
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
            onColorEdited: _root.colorEdited(col)
        }

    }

}
