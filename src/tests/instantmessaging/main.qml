import QtQuick 2.12
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.12
import InstantMessaging 1.0


ApplicationWindow {
    visible: true
    width: 800
    height: 600

    ListModel {
        id: fakeModel
        ListElement {
            local: true
            text: "Hello!!"
            time: "18:18"
            writerName: "tata"
            writeId: "aaa"
        }
        ListElement {
            local: false
            text: "How are you ?"
            time: "18:19"
            writerName: "obi"
            writeId: "aaa"
        }
    }

    ColumnLayout {
        anchors.fill: parent
        /*Button {
            onClicked: refresher.refresh = !refresher.refresh
        }*/

        DiceMessageDelegate {
            Layout.fillWidth: true
        }

        ListView {
            id: list
            model: fakeModel
            Layout.fillWidth: true
            Layout.fillHeight: true
            delegate: TextMessageDelegate {
                width: list.width
                height: 80
            }
        }




    }

}
