# CMakeList.txt : fichier projet CMake de niveau supérieur, effectuez une configuration globale
# et incluez les sous-projets ici.
#
cmake_minimum_required (VERSION 3.12)

project ("Mosaique")

if(MSVC)
	add_compile_options( /debug /std:c++latest )
else()
	add_compile_options(-std=c++17 -g -fpermissive)
endif()

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/images/ DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/images/)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/DatabaseLocation DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)

# Dossier d'include, contentant des .h / .hpp
include_directories(
	lib/
	src/metier/
)

add_executable( Mosaique 
	src/main.cpp
)
