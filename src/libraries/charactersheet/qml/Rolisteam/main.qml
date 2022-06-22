import QtQuick 2.10
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.13
import QtGraphicalEffects 1.0
import QtWebEngine 1.10
import QtQuick.Templates 2.14
import QtQuick.Extras 1.4

Item {


  ColumnLayout {
    RowLayout {
      Label {
        text: qsTr("Nom")
      }
      TextInputField {

      }
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




}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/
