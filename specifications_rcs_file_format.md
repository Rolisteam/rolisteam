# Specification about rcs file.

The rcs file is a json file. It is gathering all information about the charactersheet. 
Data to generate the qml code, the qml code, and data about characters.

## Background

The background key is an array of object. This object has two keys:  

bin : data of the image saved in base64  
key : id of the image (used in QML to point the image)  

The key must have this scheme: "{a41e1438-0aee-46d1-8714-2a284634df77}_background_0.jpg"  
If you have many background images the only difference between key must be the last number. It should be incremented.  

Page 0 will have {a41e1438-0aee-46d1-8714-2a284634df77}_background_0.jpg as background  
Page 1 will have {a41e1438-0aee-46d1-8714-2a284634df77}_background_1.jpg as background

## Number of character:

characterCount: <int>

If the characterCount is zero that means this charactersheet describes only the structure of character sheet. A rcs file with no character is useless in Rolisteam. But you can add 
character in it with rcse.


## Array of characters

A character is defined by the following fields:

* idSheet: <string> uuid
* name: <string> name of the character
* values: <json object>

The values object has a key for each field defined in the data > items key.
each key is defining the value for this specific field for this character.

example:
```
"values": {
                "id_1": {
                    "formula": "",
                    "id": "id_1",
                    "label": "name",
                    "readonly": false,
                    "type": "field",
                    "typefield": 0,
                    "value": "Ruth Sullivan"
                }
          }
```

A value is a jsonobject with those keys:

* formula: \<string eg: "=4+5"> It should begin with '='
* id: \<string> no space, no special caracter, no uppercase. Same as the key of jsonobject
* label: \<string> 
* readonly: \<bool>
* type: \<string> always set at field
* typefield: \<int> : 0 => TextInput, 1 => TextField, 2=> TextArea, 3=> Select, 4=> Checkbox, 6=>Image, 7=>button
* value: \<string>



## Information about fields

the data key is an object called Section. 
A section may have many fields or subsections. 
A subsection is a section.

## Section definition

A section is defined by three informations.
* name : name of the section
* type : always set at "Section"
* items : an array of fields.


## Field definition:

* bgcolor: \<color>
* border: \<int> // 0 no border, 16 border all around
* clippedText: \<bool>
* font: \<font definition (eg: Ubuntu,11,-1,5,50,0,0,0,0,0)>
* formula: \<string eg: "=4+5"> It should begin with '='
* height: \<real>
* id: \<string> no space, no special caracter, no uppercase.
* label: \<string> name of the field better readability
* page: \<int> page where the field is visible
* textalign: \<int> // 1=> 2=> 
* textColor: \<color>
* type: \<string> // always set at field
* typefield: \<int> : 0 => TextInput, 1 => TextField, 2=> TextArea, 3=> Select, 4=> Checkbox, 6=>Image, 7=>button
* value: \<value>
* values: \<array of values>
* width: \<real>
* x: \<real>
* y: \<real>

### Color

The color type is defined like this:
```
{
      "a": 255,
      "b": 0,
      "g": 0,
      "r": 0
}
```
A is the alpha channel. 0 means transparent color.  
b is the blue channel.  
g is the green channel.  
r is the red channel.  


## QML

To make your charactersheet directly working on rolisteam, you must generate the qml and store it in the json file under the key: "qml".

To generate QML code, you must create the structure of the QML:

If your charactersheet has background image:

```qml
import QtQuick 2.4
import "qrc:/resources/qml/"
Item {
   id:root
       focus: true
       property int page: 0
       property int maxPage: __pageMax__
       onPageChanged: {
           page=page>maxPage ? maxPage : page<0 ? 0 : page
       }
       Keys.onLeftPressed: --page
       Keys.onRightPressed: ++page
       signal rollDiceCmd(string cmd)
       MouseArea {
            anchors.fill:parent
            onClicked: root.focus = true
        }

           Image {
               id:imagebg
               property real iratio : ___ratio___ 
               property real iratiobis : ___ratioBis___
               property real realscale: width/ ___Image_Width___
               width:(parent.width>parent.height*iratio)?iratio*parent.height:parent.width
               height:(parent.width>parent.height*iratio)?parent.height:iratiobis*parent.width
               source: "image://rcs/__key___background_%1.jpg".arg(root.page)
	       __LIST_OF_FIELD__
        
          }

  }
  ```

If your charactersheet has no background image:

```qml
import QtQuick 2.4
import "qrc:/resources/qml/"
Item {
   id:root
       focus: true
       property int page: 0
       property int maxPage: __pageMax__
       onPageChanged: {
           page=page>maxPage ? maxPage : page<0 ? 0 : page
       }
       Keys.onLeftPressed: --page
       Keys.onRightPressed: ++page
       signal rollDiceCmd(string cmd)
       MouseArea {
            anchors.fill:parent
            onClicked: root.focus = true
        }

        property real realscale: 1
       __LIST_OF_FIELD__
  }
```


All of those variables must be defined into the generated qml code.
* \_\_pageMax__ is page count
* \_\_key__ is the key of all background images
* \_\_LIST_OF_FIELD__ is the place where the code of all field must be added.


## Generate Fields

### TEXTFIELD

```qml
TextFieldField {
	id: ___id__
	text: __id__.value
	textColor: __color_text__
	x: __posX__*parent.realscale
	clippedText: true or false
	y: __posY__*parent.realscale
	width: __width__*parent.realscale
	height: __height__*parent.realscale
	color: __bgColor__
	visible: root.page === __page__ ? true : false
	readOnly: __id__.readOnly
	font.family: __font_family__
	font.bold: true or false
	font.italic: true or false
	font.underline: true or false
	font.pointSize: __font_size__
	font.overline: true or false
	font.strikeout: true or false
	hAlign: TextInput.AlignTop or TextInput.AlignVCenter or TextInput.AlignBottom
        vAlign: TextInput.AlignRight or TextInput.AlignHCenter or TextInput.AlignLeft
	onTextChanged: {
            __id__.value = text
	}


}
```

### TEXTINPUT

```qml
TextInputField {
	id: ___id__
	text: __id__.value
	textColor: __color_text__
	x: __posX__*parent.realscale
	clippedText: true or false
	y: __posY__*parent.realscale
	width: __width__*parent.realscale
	height: __height__*parent.realscale
	color: __bgColor__
	visible: root.page === __page__ ? true : false
	readOnly: __id__.readOnly
	font.family: __font_family__
	font.bold: true or false
	font.italic: true or false
	font.underline: true or false
	font.pointSize: __font_size__
	font.overline: true or false
	font.strikeout: true or false
	onTextChanged: {
            __id__.value = text
	}

}
```

### TEXTAREA

```qml
TextAreaField {
	id: ___id__
	text: __id__.value
	textColor: __color_text__
	x: __posX__*parent.realscale
	clippedText: true or false
	y: __posY__*parent.realscale
	width: __width__*parent.realscale
	height: __height__*parent.realscale
	color: __bgColor__
	visible: root.page === __page__ ? true : false
	readOnly: __id__.readOnly
	font.family: __font_family__
	font.bold: true or false
	font.italic: true or false
	font.underline: true or false
	font.pointSize: __font_size__
	font.overline: true or false
	font.strikeout: true or false
	onTextChanged: {
            __id__.value = text
	}
}
```


### CHECKBOX

```qml
CheckBoxField {
	id: ___id__
	text: __id__.value
	textColor: __color_text__
	x: __posX__*parent.realscale
	clippedText: true or false
	y: __posY__*parent.realscale
	width: __width__*parent.realscale
	height: __height__*parent.realscale
	color: __bgColor__
	visible: root.page === __page__ ? true : false
	readOnly: __id__.readOnly
	onTextChanged: {
            __id__.value = text
	}


}
```


### SELECT
```qml
SelectField {
	id: ___id__
	availableValues:["value1", "value2", "value3"]
	currentIndex: combo.find(text)
	onCurrentIndexChanged:{
		if(count>0)
		{
			__id__.value = currentText
        	}
	}
	text: __id__.value
	textColor: __color_text__
	x: __posX__*parent.realscale
	clippedText: true or false
	y: __posY__*parent.realscale
	width: __width__*parent.realscale
	height: __height__*parent.realscale
	color: __bgColor__
	visible: root.page === __page__ ? true : false
	readOnly: __id__.readOnly
}
```

### IMAGE
```qml
ImageField {
	id: ___id__
	text: __id__.value
	textColor: __color_text__
	x: __posX__*parent.realscale
	clippedText: true or false
	y: __posY__*parent.realscale
	width: __width__*parent.realscale
	height: __height__*parent.realscale
	color: __bgColor__
	visible: root.page === __page__ ? true : false
	readOnly: __id__.readOnly
	onTextChanged: {
            __id__.value = text
	}
}
```

### BUTTON
```qml
DiceButton {
	id: ___id__
	text: __id__.label
	textColor: __color_text__
	x: __posX__*parent.realscale
	clippedText: true or false
	y: __posY__*parent.realscale
	width: __width__*parent.realscale
	height: __height__*parent.realscale
	color: __bgColor__
	visible: root.page === __page__ ? true : false
	readOnly: __id__.readOnly
	font.family: __font_family__
	font.bold: true or false
	font.italic: true or false
	font.underline: true or false
	font.pointSize: __font_size__
	font.overline: true or false
	font.strikeout: true or false
	onClicked:rollDiceCmd(__id__.value)
}
```

## Details of field
* \_\_id__ : id of the field, in rcse it is generated on this scheme: id_[0-9]+. It allows to refer to dynamic data inside the model.
* \_\_color_text__ : #RRGGBB or #AARRGGBB or name from https://www.w3.org/TR/SVG/types.html#ColorKeywords
* \_\_posX__ : position on X axis of the field
* \_\_posY__ : position on Y axis of the field
* \_\_width__ : width of the current field
* \_\_height__ : height of the current field
* \_\_bgColor__ : #RRGGBB or #AARRGGBB or name from https://www.w3.org/TR/SVG/types.html#ColorKeywords
* \_\_page__ : define on which page this field must be displayed
* \_\_font_family__ : http://doc.qt.io/qt-5/qfont.html#toString
* \_\_font_size__ : size in point of the font

