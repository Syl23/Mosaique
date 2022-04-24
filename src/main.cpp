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


int main(int argc, char const* argv[]) {

	if (argc != 6) {
		cout <<"Usage even split : "<<endl;
		cout <<"\t"<< argv[0] << " 0 entree.ppm sortie.ppm nbSplit scale\n";
		cout <<"Usage un-even split : "<<endl;
		cout <<"\t"<< argv[0] << " 1 entree.ppm sortie.ppm standard_deviation_threshold scale\n";
		return 1;
	}

	bool uneven = atoi(argv[1]);
	

	Image* imageIn = new Image((char*)argv[2]);
	vector<Region*> regions;

	if (uneven) {
		
		double seuil = (double)atoi(argv[4]);
		cout<<"uneven, seuil : "<<seuil<<endl;
		seuil *= seuil;

		regions = unevenSplit(imageIn, seuil, 1);
	}
	else {
		cout<<"even"<<endl;
		regions = split(imageIn, atoi(argv[4]));
	}
	findBestImagesGiga(regions, atoi(argv[5]), !uneven);
	scale(regions, atoi(argv[5]));
	applyBestImages(regions);


	for (int i = 0; i < regions.size(); i++) {
		//cout<<"psnr : "<<regions[i]->psnr<<endl;
	}


	Image* out = merge(regions);
	out->writeToFile((char*)argv[3]);
	return 0;
}
