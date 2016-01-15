import QtQuick 2.5
import QtQuick.Window 2.2
//import Rcse 1.0
//import "./"
import "./Rcse/"

Window {
    id:rootw
    visible: true
    x:0
    y:0

    Item {
        anchors.fill: parent
        Image {
            id:root
            property real iratio : 1.3621523579201935
            property real iratioBis : 0.7341322680869952
            property real realScale: width/2253
            width:(parent.width>parent.height*iratio)?iratio*parent.height:parent.width
            height:(parent.width>parent.height*iratio)?parent.height:iratioBis*parent.width
           /* onHeightChanged: {
                console.log(height+" "+width+ " "+parent.height+ " "+parent.width);
            }*/

            source: "ptites-sorcieres-fiche-pj-2.jpg"

            Field {
                x:200*parent.realScale
                y:100*parent.realScale
                width:200*parent.realScale
                height: 30
                color:"red"
            }

        }
    }
}
