cmake_minimum_required(VERSION 3.10)

include_directories(../core/src)

# Helpers
add_subdirectory(helper)


#subdirs with tests
add_subdirectory(dataChapter)
add_subdirectory(dataCleverURI)
add_subdirectory(preferences )
add_subdirectory(colorButton)
add_subdirectory(paletteModel)
add_subdirectory(chat )
add_subdirectory(rolisteamtheme )
add_subdirectory(dataplayer )
add_subdirectory(picture )
add_subdirectory(session )
add_subdirectory(dataCharacter )
add_subdirectory(fieldModelTest )
#add_subdirectory(sendToServer )
add_subdirectory(vmap )
add_subdirectory(widgets )
add_subdirectory(network )
add_subdirectory(map )
add_subdirectory(musicmodel )
add_subdirectory(channelmodel)
add_subdirectory(characterstatemodel )
add_subdirectory(dicealiasmodel )
add_subdirectory(genericmodel   )
add_subdirectory(unitmodel           )

