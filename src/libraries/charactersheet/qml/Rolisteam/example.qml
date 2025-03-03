import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtWebEngine
import QtQuick.Templates
import Qt.labs.qmlmodels
import QtCore

Item {
    ColumnLayout {
        RowLayout {
            Label {
                text: qsTr("Nom")
            }
            TextInputField {
            }
        }
        Slider {
        }
        RowLayout {
            Label {
                text: qsTr("Prenom")
            }
            TextInputField {
            }
        }
        RowLayout {
            Label {
                text: qsTr("Couleur des yeux")
            }
            TextInputField {
            }
        }
        RowLayout {
            Label {
                text: qsTr("Corpulence")
            }
            TextInputField {
            }
        }
        GridLayout {
            columns: 4

            Label {
                text: qsTr("Courage")
            }
            Label {
                text: qsTr("Malice")
            }
            Label {
                text: qsTr("Mignon")
            }
            Label {
                text: qsTr("Colère")
            }
            TextInputField {
            }
            TextInputField {
            }
            TextInputField {
            }
            TextInputField {
            }
        }
        Label {
            text: qsTr("Syndrôme")
        }
        TextAreaField {
        }
        Label {
            text: qsTr("Souvenir 1: %1").arg("")
        }
        TextAreaField {
        }
        Label {
            text: qsTr("Souvenir 2: %1").arg("")
        }
        TextAreaField {
        }
        Label {
            text: qsTr("Souvenir 3: %1").arg("")
        }
        TextAreaField {
        }
    }
    DelegateChooser {
        id: delegate

    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/
