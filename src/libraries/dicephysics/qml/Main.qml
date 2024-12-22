import QtQuick
import QtQuick3D
import QtQuick3D.Physics
import QtQuick3D.Helpers
import QtQuick.Controls
import QtQuick.Layouts
import QtQml
import Controllers
import DicePhysics

ApplicationWindow {
    id: root
    x: Dice3DCtrl.x
    y: Dice3DCtrl.y
    width: Dice3DCtrl.width
    height: Dice3DCtrl.height

    property real halfWidth: width/2
    property real halfHeight: height/2
    property real ceilling: 500.
    property real side: 100.

    visible: Dice3DCtrl.displayed

    flags: Qt.WA_TranslucentBackground | Qt.WindowStaysOnTopHint | Qt.FramelessWindowHint | Qt.Popup
    color: "transparent"

    property real currDrawerWidth: menu.width * menu.position

    title: qsTr("Rolisteam 3D dice roller")

    Item
    {
        anchors.fill: parent

        PhysicsWorld {
            id: physicsWorld
            running: true
            enableCCD: true
            scene: viewport.scene
            gravity: Qt.vector3d(0, -980.7, 0)
            typicalLength: 1
            typicalSpeed: 500
            minimumTimestep: 15
            maximumTimestep: 20
        }

        View3D {
            id: viewport
            anchors.fill: parent
            camera: camera

            environment: SceneEnvironment {
                clearColor: "transparent"
                backgroundMode: SceneEnvironment.Transparent
                antialiasingMode: SceneEnvironment.MSAA
                antialiasingQuality: SceneEnvironment.High
                lightProbe: proceduralSky
            }

            Texture {
                id: proceduralSky
                textureData: ProceduralSkyTextureData {
                    sunLongitude: -115
                    groundBottomColor : Qt.rgba(0.5, 0.5, 0.5, 0.5)
                }
            }

            Node {
                id: scene

                OrthographicCamera {
                    id: camera
                    clipFar: 300
                    clipNear: 1
                    position: Qt.vector3d(0.0, 300.0, 0)
                    eulerRotation.x: -90 //: Qt.vector3d(-87, -0, 0)
                }

                DirectionalLight {
                    eulerRotation: Qt.vector3d(0, -1, 0)//Qt.vector3d(-45, 25, 0)
                    castsShadow: true
                    brightness: 1
                    shadowMapQuality: Light.ShadowMapQualityVeryHigh
                }


                StaticRigidBody {
                    id: table

                    position: Qt.vector3d(0, 0, 0)
                    scale: Qt.vector3d(root.width/100, 1., root.height/100)
                    sendContactReports: true
                    Model {
                        source: "#Cube"
                        materials: DefaultMaterial {
                            opacity: 0.0
                            diffuseColor: "#aa5555"
                        }
                    }
                    collisionShapes: BoxShape {
                        extents: Qt.vector3d(root.width, root.side, root.height)
                        enableDebugDraw: true
                    }
                }

                StaticRigidBody {
                    id: northWall

                    position: Qt.vector3d(0, root.ceilling/2, -root.halfHeight-(root.side/2))
                    scale: Qt.vector3d(root.width/100, root.ceilling/100, 1.)
                    sendContactReports: true
                    Model {
                        source: "#Cube"
                        materials: DefaultMaterial {
                            opacity: 0.2
                            diffuseColor: "#2222FF"
                        }
                    }
                    collisionShapes: BoxShape {
                        extents: Qt.vector3d(root.width, root.ceilling, root.side)
                        enableDebugDraw: true
                    }
                }

                StaticRigidBody {
                    id: southWall

                    position: Qt.vector3d(0, root.ceilling/2, root.halfHeight+(root.side/2))
                    scale: Qt.vector3d(root.width/100, root.ceilling/100, 1.)
                    sendContactReports: true
                    Model {
                        source: "#Cube"
                        materials: DefaultMaterial {
                            opacity: 0.2
                            diffuseColor: "#880000"
                        }
                    }
                    collisionShapes: BoxShape {
                        extents: Qt.vector3d(root.width, root.ceilling, root.side)
                        enableDebugDraw: true
                    }
                }

                StaticRigidBody {
                    id: westWall

                    position: Qt.vector3d(-root.halfWidth-(root.side/2), root.ceilling/2, 0)
                    scale: Qt.vector3d(1., root.ceilling/100, root.height/100)
                    sendContactReports: true
                    Model {
                        id: wwModel
                        source: "#Cube"
                        materials: DefaultMaterial {
                            opacity: 0.2
                            diffuseColor: "#00FF00"
                        }
                    }
                    collisionShapes: BoxShape {
                        extents: Qt.vector3d(root.side, root.ceilling, root.height)
                        enableDebugDraw: true
                    }
                }

                StaticRigidBody {
                    id: aestWall

                    position: Qt.vector3d(root.halfWidth+(root.side/2), root.ceilling/2, 0)
                    scale: Qt.vector3d(1., root.ceilling/100, root.height/100)
                    sendContactReports: true
                    Model {
                        id: model
                        source: "#Cube"
                        materials: DefaultMaterial {
                            opacity: 0.2
                            diffuseColor: "#888800"
                        }
                    }
                    collisionShapes: BoxShape {
                        extents: Qt.vector3d(root.side, root.ceilling, root.height)
                        enableDebugDraw: true
                    }
                }

                StaticRigidBody {
                    id: ceilling

                    position: Qt.vector3d(0, root.ceilling, 0)
                    scale: Qt.vector3d(root.width/100, 1., root.height/100)

                    sendContactReports: true
                    Model {
                        source: "#Cube"
                        materials: DefaultMaterial {
                            opacity: 0.2
                            diffuseColor: "#88EE90"
                        }
                    }

                    collisionShapes: BoxShape {
                        //source: model.source
                        extents: Qt.vector3d(root.width, root.side, root.height)
                        enableDebugDraw: true
                    }
                }

                AxisHelper {
                    //opacity: 0.3
                    visible: false
                }

                Component {
                    id: genericDiceComp
                    RegularDice {
                        scaleFactor: menu.factor
                        parentWidth: root.width
                        parentHeight: root.height
                    }
                }

                Repeater3D {
                    id: dicePool
                    model: Dice3DCtrl.model
                    delegate: genericDiceComp
                    function restore() {
                        for (let i = 0; i < count; i++) {
                            objectAt(i).restore()
                        }
                    }
                }
            } // scene

            MouseArea {
                id: ma
                anchors.fill: parent
                enabled: !menu.mouseEnabled
                property list<DynamicRigidBody> selection
                property real xvelocity: 0.0
                property real zvelocity: 0.0
                property real xpos: 0.0
                property real ypos: 0.0
                property real t: 0.0
                property bool rolling: false

                property vector3d formerPosition

                function clear() {
                    console.log("Clear selection")
                    while(ma.selection.length > 0) {
                        let target = ma.selection.pop();
                        target.selected = false
                    }
                }

                function prepareSelection(point) {
                    console.log("prepare selection")
                    ma.rolling = true
                    ma.selection.forEach(body => {
                            body.isKinematic = true
                            body.kinematicRotation = body.rotation
                            body.kinematicPosition =  Qt.vector3d(body.position.x, 50, body.position.z)
                    });
                    formerPosition = point
                }
                function releaseSelection(point, velocity) {
                    console.log("release selection")
                    ma.selection.forEach(body => {
                            body.kinematicPosition =  Qt.vector3d(point.x, 50, point.z)
                            body.isKinematic = false
                            body.applyCentralImpulse(velocity)
                            body.applyImpulse(velocity, Qt.vector3d(0, 10, 0))
                    });
                    ma.rolling = false
                }

                function moveSelection(point) {
                    const distX= point.x - formerPosition.x
                    const distZ = point.z - formerPosition.z
                    ma.selection.forEach(body => {
                            body.kinematicPosition =  Qt.vector3d(body.kinematicPosition.x + distX, body.kinematicPosition.y, body.kinematicPosition.z+ distZ)
                    });
                    formerPosition = point
                }

                function select(target) {
                    ma.selection.push(target)
                    target.selected = true
                }

                function unselect(target) {
                    const idx = ma.selection.indexOf(target)
                    if(idx < 0)
                        return;

                    ma.selection.splice(idx, 1)
                    target.selected = false
                }


                function initVelocity(point) {
                    xpos = point.x
                    ypos = point.y
                    t = Date.now()
                }
                function computeVelocity(point) {
                    var nx = point.x
                    var ny = point.y
                    var nt = Date.now()
                    var distx = nx - xpos
                    var disty = ny - ypos
                    var interval = nt - t
                    if(interval == 0)
                        return
                    //console.log("nx: ",nx," distx: ",distx)
                    ma.xvelocity = distx//Math.sqrt(distx*distx)  //interval
                    ma.zvelocity = disty//Math.sqrt(disty*disty) //interval
                    xpos = nx
                    ypos = ny
                    t = nt
                }

                onPressed: (mouse)=> {
                               console.log("inside onPressed :",mouse.modifiers)
                               var point = viewport.mapTo3DScene(Qt.vector3d(mouse.x, mouse.y, 0))
                               var result = viewport.pick(mouse.x, mouse.y)
                               if(mouse.modifiers === Qt.NoModifier)
                               {
                                   if(result.objectHit) {
                                       var target = result.objectHit.parent
                                       if(!target.selected) {
                                            ma.clear()
                                            select(target)
                                       }
                                       prepareSelection(point)
                                       initVelocity(mouse)
                                   }
                               }
                               else if(mouse.modifiers & Qt.ControlModifier)
                               {
                                   if(result.objectHit) {
                                       let target = result.objectHit.parent
                                        if(target.selected)
                                            ma.unselect(target)
                                       else
                                            ma.select(target)
                                   }
                               }
                           }
                onPositionChanged: (mouse)=>{
                        if(false === ma.rolling || ma.selection.lenght === 0)
                            return

                        var point = viewport.mapTo3DScene(Qt.vector3d(mouse.x, mouse.y, 0))
                        moveSelection(point)
                        computeVelocity(mouse)
                        Dice3DCtrl.expectRoll = true
                }
                onReleased: (mouse)=>{
                    if(false === ma.rolling || ma.selection.lenght === 0)
                        return
                    var point = viewport.mapTo3DScene(Qt.vector3d(mouse.x, mouse.y, 0))
                    const vec = Qt.vector3d(xvelocity, -0.4, zvelocity).normalized();
                    ma.releaseSelection(point, vec)
                }
            }
        }

        SideMenu {
            id: menu
        }

        RoundButton {
            id: iconOpen
            icon.source: "qrc:/dice3d/icons/menuIcon.svg"
            x: root.currDrawerWidth
            onClicked: {
                menu.open()
            }
        }

        Label {
            id: selectionCount
            text: ma.selection.length
            visible: ma.selection.length
            font.pixelSize: 30
            font.bold: true
            anchors.horizontalCenter: iconOpen.horizontalCenter
            anchors.top: iconOpen.bottom
            anchors.topMargin: 20
            horizontalAlignment: Label.AlignHCenter

            width: Math.max(implicitHeight, implicitWidth)
            height: width
            background: Rectangle {
                color: "white"
                radius: width/2
                opacity: 0.8
            }
        }
    }
}



//! [window]
