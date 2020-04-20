# What's next in Rolisteam v1.10

This document presents all main features schedules for **Rolisteam**
This list can evolve quickly. Keep an eye on this document.

Please also look at: https://docs.google.com/spreadsheets/d/18jDGViuOm6KjqEAumW1RU2qccQQ4-TxiXPtAg0X_M2o/edit#gid=1726144196 to see what task must be done first.

# Refactoring

The logic behind that is to split the cleverness of the code and the view.
The "cleverness" should be managed into controller. And views must obey to controllers.
Network communication must be set to update property change on controller.
The view and the controller must not send network data. Only listener of controller signals must send network message.
That the idealic solution.


# Android Client in QML

The refactoring should make easier a new client implementation in QML only.
The client is dedicated to Android platform but It will be distributed with the rolisteam client.
So client can pick the one they want (on desktop).

# Instant messaging New UI

 The instant messaging UI is poor. I would like something better, with avatar for each character/player who is texting.
 A better notifications system.

 Instead of many separate window, I would prefer one window/panel with the common chat room at first. All the other chat room are tabs inside this panel. We can close extract chat room, add new chat room.
 It should be possible to extract the chat room into a new mdiwindow/window.
 * One panel with tabs: default chat room : the global, and one per player.
 * Add/close extract tabs
 * A better UI for diceroll result
 * emoji support
 * mention user with \@username
 * Message in model, so they can be edited (just like discord for example)
 * support of HTML: link and image



# Mindmap editor

Add a new media in rolisteam, a mindmap. The current code of the mind map is here: git@github.com:obiwankennedy/rmindmap.git - branch qml_node.

https://github.com/obiwankennedy/rmindmap/tree/qml_node

Task todo on mindmap:

* Support of image inside node
* Support of package (several nodes are child of one node)
* Share the map via network.

# Vectorial map

The vectorial map will be the only map system.

* Improvement about management of combat turn.
* Distance per turn management on vmap
* Shape must also be able to change some property.
* whatever improvement in mind.
* One day vmap must be ported to QML (may be out of scope for 1.10)

# NPC token

* Token must be managed as resources.
* easier to edit/load and save.

# Charactersheet

* Give helping control to manage character's properties from the charactersheet automatically (no code edition).
