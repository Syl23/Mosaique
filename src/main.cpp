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

	bool uneven = true;
	bool giga = true;

	if(argc != 5){
		
		cout<<"Il convient de mettre le nom d'un fichier d'entré, de sortie, une taille de découpage et un scale"<<endl;
		cout << "Usage : " << argv[0] << " entree.ppm sortie.ppm split/seuil scale\n";
		return 1;
	}

	Image* imageIn = new Image((char *)argv[1]);
	vector<Region*> regions;
	if (giga) {
		if (uneven) {
			regions = unevenSplit(imageIn, (double)atoi(argv[3]), 8);
			findBestImagesGiga(regions, uneven);
		}
		else {
			regions = split(imageIn, atoi(argv[3]));
			findBestImagesGiga(regions, uneven);
		}
	}
	else {
		if (uneven) {
			regions = unevenSplit(imageIn, (double)atoi(argv[3]), 8);
			unevenFindBestImages(regions);
		}
		else {
			regions = split(imageIn, atoi(argv[3]));
			findBestImages(regions);
		}
	}

	for(int i = 0 ; i < regions.size() ; i ++){
		//cout<<"psnr : "<<regions[i]->psnr<<endl;
	}

	scale(regions, atoi(argv[4]));
	replaceWithBestImg(regions);
	
	Image* out = merge(regions);
	out->writeToFile((char *)argv[2]);
	return 0;
}
