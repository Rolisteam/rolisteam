import QtQuick 2.10

Item {
   id:root
   property int page: 0
   signal rollDiceCmd(string cmd)
   Image {
       id:imagebg
       property real iratio :1.36215
       property real iratiobis :0.734132
       property real realscale: width/2253
       width:(parent.width>parent.height*iratio)?iratio*parent.height:parent.width
       height:(parent.width>parent.height*iratio)?parent.height:iratiobis*parent.width
       source: "/home/renaud/documents/background.jpg"
	Field {
	    id:id2
	    text: "key1"
	    x:296*imagebg.realscale
	    y:96*imagebg.realscale
	    width:157*imagebg.realscale
	    height:43*imagebg.realscale
	    color: "#ffff99ff"
	    state:"field"
	    visible: true
	}
 
  }
}
