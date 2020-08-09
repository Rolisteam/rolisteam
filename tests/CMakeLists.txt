cmake_minimum_required(VERSION 3.10)

include_directories(../core/src)
include_directories(../core/src/diceparser/include)

# Helpers
add_subdirectory(helper)


#subdirs with tests
add_subdirectory(audioplayer)
#add_subdirectory(dataChapter)
add_subdirectory(dataCleverURI)
add_subdirectory(preferences )
add_subdirectory(colorButton)
add_subdirectory(contentmodel)
add_subdirectory(paletteModel)
#add_subdirectory(chat )
add_subdirectory(rolisteamtheme )
add_subdirectory(dataplayer )
add_subdirectory(picture )
add_subdirectory(session )
add_subdirectory(dataCharacter )
add_subdirectory(fieldModelTest )
add_subdirectory(updater )
add_subdirectory(controller )
add_subdirectory(playermodel)
#add_subdirectory(sendToServer )
add_subdirectory(vmap )
add_subdirectory(serialization )
add_subdirectory(widgets )
add_subdirectory(network )
add_subdirectory(musicmodel)
add_subdirectory(channelmodel)
add_subdirectory(characterstatemodel)
add_subdirectory(dicealiasmodel)
add_subdirectory(genericmodel)
add_subdirectory(charactermodel)
add_subdirectory(unitmodel)



