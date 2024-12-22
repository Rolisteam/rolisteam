import QtQuick
import QtQuick3D
import QtQuick3D.Physics
import QtMultimedia
import Controllers

DynamicRigidBody {
    id: root

    required property real scaleFactor
    required property color baseCol
    required property int index
    required property int type
    required property QtObject ctrl
    required property real parentWidth
    required property real parentHeight

    property bool selected: false
    onSelectedChanged: {
        ctrl.selected = root.selected
    }

    opacity: root.ctrl.value > 0 ? 1.0 : 0.4

    property real internalScale: selected ? 1.2 : 1.
    readonly property string diceCode: Dice3DCtrl.diceTypeToCode(root.type)

    QtObject {
        id: internal
        property vector3d rotation: Qt.vector3d(0, 0, 0)
    }

    function moved() {
        if(!root.selected)
            return
        root.ctrl.elapsed()
    }

    function computeRotOffset(oldpos, newpos) {
        const offset = (Math.abs(newpos.x - oldpos.x) + Math.abs(newpos.y - oldpos.y) + Math.abs(newpos.z - oldpos.z))/3;
        ctrl.addRotationOffset(offset)
    }

    Connections {
        target: root.ctrl
        function onStableChanged() {
            root.ctrl.rotation = root.eulerRotation
            root.ctrl.position = root.position
        }
    }


    scale: Qt.vector3d(scaleFactor*internalScale, scaleFactor*internalScale, scaleFactor*internalScale)
    eulerRotation: root.ctrl.rotation
    onEulerRotationChanged: {
        if(root.ctrl.stable)
            root.ctrl.rotation = root.eulerRotation
        else if(internal.rotation.fuzzyEquals(root.eulerRotation, 0.005))
            return;
        else
        {
            moved()
            computeRotOffset(internal.rotation, root.eulerRotation)
        }
        internal.rotation = root.eulerRotation
    }

    position: root.ctrl.position
    onPositionChanged: {
        moved()
        if(root.position.y+10 < 0)
            root.position.y = 20
        if(Math.abs(root.position.x) > Math.abs(Dice3DCtrl.width/2))
            root.position.x = root.position.x > 0 ? Dice3DCtrl.width/2 : -Dice3DCtrl.width/2
        if(Math.abs(root.position.z) > Math.abs(Dice3DCtrl.height/2))
            root.position.z = root.position.z > 0 ? Dice3DCtrl.height/2 : -Dice3DCtrl.height/2

        root.ctrl.position = root.position
    }

    massMode: DynamicRigidBody.CustomDensity
    receiveContactReports: true

    onBodyContact: (body, positions, impulses, normals) => {
        moved()
        let volume = 0
        impulses.forEach(vector => {
                             volume += vector.length()
                         })
        diceSound.volume = volume / 2000
        if (!diceSound.playing)
            diceSound.play()
    }

    collisionShapes: ConvexMeshShape {
        id: diceShape
        source: "qrc:/dice3d/meshes/%1.mesh".arg(root.diceCode)
    }

    Texture {
        id: normals
        source: "qrc:/dice3d/maps/%1_Normal_OpenGL.png".arg(root.diceCode)
    }

    Texture {
        id: numberFill
        source: "qrc:/dice3d/maps/%1_Base_color.png".arg(root.diceCode)
        generateMipmaps: true
        mipFilter: Texture.Linear
    }

    physicsMaterial: PhysicsMaterial {
        id: physicsMaterial
        staticFriction: 0.0001
        dynamicFriction: 0.5
        restitution: 0.000001
    }

    Model {
        id: thisModel
        pickable: true
        source: diceShape.source
        materials: PrincipledMaterial {
            metalness: 1.0
            roughness: 0.4
            baseColor: baseCol
            emissiveMap: numberFill
            emissiveFactor: Qt.vector3d(1, 1, 1)
            normalMap: normals
            normalStrength: 0.75
        }
    }

    SoundEffect {
        id: diceSound
        loops: 0
        muted: Dice3DCtrl.muted || !Dice3DCtrl.displayed
        source: "qrc:/dice3d/sounds/onedice.wav"
    }
}
