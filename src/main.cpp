#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string> 
#include <cstring>

#include "image_ppm.h"
#include "Image.hpp"
#include "Couleur.hpp"
#include "Region.hpp"

using namespace std;




int main(int argc, char const *argv[]){

	if(argc != 5){
		cout<<"Il convient de mettre le nom d'un fichier d'entré, de sortie, une taille de découpage et un scale"<<endl;
		return 1;
	}

	Image* imageIn = new Image((char *)argv[1]);

	auto regions = split(imageIn, atoi(argv[3]));

	findBestImages(regions);

	for(int i = 0 ; i < regions.size() ; i ++){
		//cout<<"psnr : "<<regions[i]->psnr<<endl;
	}

	scale(regions, atoi(argv[4]));
	replaceWithBestImg(regions);
	
	Image* out = merge(regions);
	out->writeToFile((char *)argv[2]);
	return 0;
}