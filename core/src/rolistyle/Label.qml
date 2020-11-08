import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.impl 2.15
import QtQuick.Templates 2.15 as T
import Customization 1.0

T.Label {
    id: control

    property QtObject style: Theme.styleSheet("Controls")

    color: control.style.textColor
    linkColor: control.style.linkColor
}
