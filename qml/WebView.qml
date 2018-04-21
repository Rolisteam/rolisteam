import QtQuick 2.7
import QtWebEngine 1.5

WebEngineView {
    id: root
    property bool readOnly: false
    enabled: !readOnly
    Keys.onPressed: {
        if(event.key === Qt.Key_F5)
        {
            root.reload()
            event.accepted = true;
        }
    }
}
