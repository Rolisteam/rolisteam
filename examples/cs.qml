import QtQuick 2.4
import QtQuick.Window 2.2
//import Rcse 1.0
//import "./"
import "./Rcse/"

Window {
    id:rootw
    visible: true
    x:0
    y:0
    width:1126
    height: 827
    Item {
        anchors.fill: parent
        Image {
            id:root
            property real pratio : parent.height/parent.width
            property real iratio : 0.7341322680869952

            height:(parent.width>parent.height*iratio)?parent.height:iratio*parent.width
            width:(parent.width>parent.height*iratio)?iratio*parent.height:parent.height

            //anchors.fill: parent
            source: "ptites-sorcieres-fiche-pj-2.jpg"
            //fillMode: Image.PreserveAspectFit
            anchors.verticalCenter: parent.verticalCenter
            /*width:1126
            height: 827*/
            Field {
                x:200
                y:100
                width:200
                height: 300
                color:"red"
            }

        }
    }
}
