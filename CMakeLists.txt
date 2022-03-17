# CMakeList.txt : fichier projet CMake de niveau supérieur, effectuez une configuration globale
# et incluez les sous-projets ici.
#
cmake_minimum_required (VERSION 3.8)

project ("Mosaique")

if(MSVC)
	add_compile_options()
else()
	add_compile_options(-std=c++17 -O3 -g)
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")

endif()

# Dossier d'include, contentant des .h / .hpp
include_directories(
	lib/
)


add_executable( Mosaique 
	src/main.cpp
)
target_compile_features(Mosaique PRIVATE cxx_std_17)
