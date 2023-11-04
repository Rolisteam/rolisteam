import QtQuick
import QtQuick.Controls
import charactersheet
import Rolisteam

Menu {
    id: _root

    property SheetController ctrl


    MenuItem {
        text: qsTr("Next Page")
        enabled: _root.ctrl.currentPage < _root.ctrl.pageMax
        onClicked: _root.ctrl.nextPage()
    }

    MenuItem {
        text: qsTr("Previous Page")
        enabled: _root.ctrl.currentPage > 0
        onClicked: _root.ctrl.previousPage()
    }

    MenuSeparator{}

    MenuItem {
        id: adaptWidthToPage
        text: qsTr("Adapt Width to Window")
        checkable: true
        checked: _root.ctrl.adaptWidthToPage
        onClicked: _root.ctrl.adaptWidthToPage = !_root.ctrl.adaptWidthToPage
    }

    MenuItem {
        id: zoomMin
        text: qsTr("Zoom Min")
        onClicked: _root.ctrl.zoomLevel = 0.05
    }



    MenuItem {
        id: zoomReset
        text: qsTr("Reset Zoom")
        onClicked: _root.ctrl.zoomLevel = 1.0
    }

    MenuItem {
        id: zoomMax
        text: qsTr("Zoom Max")
        onClicked: _root.ctrl.zoomLevel = 5.
    }
    Slider {
        id: _value
        from: 0.05
        to: 5.0
        value: _root.ctrl.zoomLevel
        onMoved: {
             _root.ctrl.zoomLevel = _value.value
        }
    }
}
