import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Dialogs 1.3

Button {
    id: _root
    property color color
    signal colorEdited(color col)


    ColorDialog {
        id: _dialog
        currentColor: _root.color
        color: _root.color
        onAccepted: _root.colorEdited(_dialog.color)
    }

    background: Rectangle {
        id: _back
        implicitWidth: 30
        implicitHeight: 30
        visible: !_root.flat || _root.down || _root.checked || _root.highlighted
        color: _root.down ? Qt.lighter(_root.color) : _root.color
        border.width: 0
        radius: 2
    }

    onClicked: _dialog.open()
}
