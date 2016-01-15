import QtQuick 2.4
import QtQuick.Window 2.2
import "./Rcse/"

Window {
    id:rootw
    visible: true
    x:0
    y:0

    Item {
        anchors.fill: parent
        onWidthChanged: {
            if(parent.width>parent.height*imagebg.iratio)
            {
                imagebg.width =imagebg.iratio*parent.height
                imagebg.height=parent.height
            }
            else
            {
                imagebg.width =parent.width
                imagebg.height=imagebg.iratioBis*parent.width
            }
        }
        Image {
            id:imagebg
	       property real iratio :1.36215
	       property real iratioBis :0.734132
	       property real realScale: width/2253
	       property real realScaleH: height/1654
           //width:(parent.width>parent.height*iratio)?iratio*parent.height:parent.width
           //height:(parent.width>parent.height*iratio)?parent.height:iratioBis*parent.width
	       source: "background.jpg"
		onHeightChanged: {
			console.log(realScale+" "+realScaleH)		
		}

		Field {
			text: "key 1"
			x:283*parent.realScale
			y:107*parent.realScale
			width:313*parent.realScale
			height:22
			color: "#ffffffff"
		}
		Field {
			text: "key 2"
			x:455*parent.realScale
			y:293*parent.realScale
			width:86*parent.realScale
			height:69
			color: "#ffffffff"
		}
		Field {
			text: "key 3"
			x:437*parent.realScale
			y:392*parent.realScale
			width:120*parent.realScale
			height:50
			color: "#ffffffff"
		}
		Field {
			text: "key 4"
			x:464*parent.realScale
			y:466*parent.realScale
			width:91*parent.realScale
			height:40
			color: "#ffffffff"
		}
		Field {
			text: "key 5"
			x:443*parent.realScale
			y:583*parent.realScale
			width:127*parent.realScale
			height:24
			color: "#ffffffff"
		}

       }
   }
}
