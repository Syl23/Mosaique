# CMakeList.txt : fichier projet CMake de niveau supérieur, effectuez une configuration globale
# et incluez les sous-projets ici.
#
cmake_minimum_required (VERSION 3.8)

project ("Mosaique")

if(MSVC)
	add_compile_options(/permissive /debug /std:c++17)
else()
	add_compile_options(-std=c++17 -g -fpermissive)
endif()


# Dossier d'include, contentant des .h / .hpp
include_directories(
	lib/
)

add_executable( Mosaique 
	src/Couleur.hpp
	src/main.cpp
)
