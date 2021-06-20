import QtQuick 2.15
import QtQuick.Controls 2.15
import Customization 1.0

Slider {
    id: control
    property QtObject style: Theme.styleSheet("PermissionSlider")
    enum Permission {
        NoPermission,
        ReadOnlyPermission,
        WritePermission
    }

    property int permission: control.value === 0  ? PermissionSlider.Permission.NoPermission :
                             control.value === 1 ? PermissionSlider.Permission.ReadOnlyPermission :
                                                   PermissionSlider.Permission.WritePermission

    from: control.style.startValue
    to: control.style.endValue
    stepSize: 1.0
    snapMode: Slider.SnapOnRelease

    background: Rectangle {
            x: control.leftPadding
            y: control.topPadding + control.availableHeight / 2 - height / 2
            implicitWidth: control.width
            implicitHeight: control.availableHeight / 4
            width: control.availableWidth
            height: implicitHeight
            radius: control.style.radiusSize
            color: control.style.backgroundColor

            Rectangle {
                width: control.visualPosition * parent.width
                height: parent.height
                color: control.value == 1 ? control.style.readOnlyPermColor : control.style.writePermColor
                radius: control.style.radiusSize
            }
        }

}
