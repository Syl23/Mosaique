#ifndef __REGION_HPP__
#define __REGION_HPP__
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string> 
#include <cstring>

#include <filesystem>

#include "image_ppm.h"
#include "Couleur.hpp"
#include "Database.hpp"
#include "Image.hpp"

using namespace std;

class Region : public Image{
public :
	int startX;
	int startY;

	Image * bestImg;
	int cursorPosGiga;
	double bestPsnr;
    Region(){
		startX = 0;
		startY = 0;
		bestPsnr = 0;
    }
    ~Region(){

    }

    void scale(int a){
        this->sizeX *=a;
        this->sizeY *=a;
        this->startX *= a;
        this->startY *= a;
    }

	bool split(vector<Region *> *regions, double seuil, int minimal){
		bool isFeuille = true;
		if(this->sizeX >= minimal && this->sizeY >= minimal){
			Couleur c = this->avg();
			double variance = this->variance(c);
			if(variance > seuil){
				isFeuille = false;
				int regSizeX = 0;
				int regSizeY = 0;

				int cumulativeRegSizeX = 0;

				for(int x = 0 ; x < 2 ; x++){
					regSizeX = this->sizeX / 2;
					int cumulativeRegSizeY = 0;

					if (this->sizeX % 2 == 1 && x == 1) {
						regSizeX++;
					}

					for(int y = 0 ; y < 2 ; y++){

						Region* tmp = new Region();

						regSizeY = this->sizeY / 2;
						if (this->sizeY % 2 == 1 && y == 1) {
							regSizeY++;
						}


						allocation_tableau(tmp->tab, OCTET, regSizeX*regSizeY*3);

						tmp->startX = cumulativeRegSizeX;
						tmp->startY = cumulativeRegSizeY;

						tmp->sizeX = regSizeX;
						tmp->sizeY = regSizeY;
						
						for(int j = cumulativeRegSizeY; j < tmp->startY + tmp->sizeY ; j++){
							for(int i = cumulativeRegSizeX; i < tmp->startX + tmp->sizeX ; i++ ){
								for(int c = 0 ; c< 3 ; c++){
									OCTET oct = this->tab[(j * this->sizeX + i) * 3 + c];
									tmp->tab[((j - tmp->startY) * tmp->sizeX + i - tmp->startX) * 3 + c] = oct;
								}
							}
						}

						tmp->startX = cumulativeRegSizeX + this->startX;
						tmp->startY = cumulativeRegSizeY + this->startY;

						if(tmp->split(regions, seuil, minimal)){
							regions->push_back(tmp);
						} else {
							delete tmp;
						}

						cumulativeRegSizeY = cumulativeRegSizeY + regSizeY;
					}
					cumulativeRegSizeX = cumulativeRegSizeX + regSizeX;

				}
			}
		}
		return isFeuille;
	}
};

struct Vector2 {
	int x;
	int y;
};

void unevenFindBestImages(vector<Region*> regions){
	string databaseName = "dataBase/";
	ifstream databaseLocation ("DatabaseLocation");
	bool found = false;
	if (databaseLocation.good()) {
		found = (bool)getline(databaseLocation, databaseName);
		databaseLocation.close();
	}
	if (!found) {
		ofstream write("DatabaseLocation");
		write << databaseName;
		write.close();
	}
	auto files = Database::scanFolder(databaseName);
	for(int j = 0 ; j < files.size() ; j++){
        bool color = regions[0]->color;
		auto f = files[j];

		Image * tmp = new Image((char *)f.c_str(), color);

		cout<<((int)(map(j,0,files.size(),0,100)*100.0))/100.0<<"%"<<endl;
		for(int i = 0 ; i < regions.size() ; i++){
			Image * vignette =  tmp->scale(regions[i]);

			double tmPsnr = vignette->ressemblance(regions[i]);
			if(tmPsnr > regions[i]->bestPsnr){
				regions[i]->bestPsnr = tmPsnr;
				regions[i]->bestImg = tmp;
			}
			delete vignette;

		}
	}
}
void findBestImages(vector<Region*> regions){

	string databaseName = "dataBase/";
	ifstream databaseLocation ("DatabaseLocation");
	bool found = false;
	if (databaseLocation.good()) {
		found = (bool)getline(databaseLocation, databaseName);
		databaseLocation.close();
	}
	if (!found) {
		ofstream write("DatabaseLocation");
		write << databaseName;
		write.close();
	}
	auto files = Database::scanFolder(databaseName);
	for(int j = 0 ; j < files.size() ; j++){
        bool color = regions[0]->color;
		auto f = files[j];

		Image * tmp = new Image((char *)f.c_str(), color);
		Image * vignette =  tmp->scale(regions[0]);

		cout<<((int)(map(j,0,files.size(),0,100)*100.0))/100.0<<"%"<<endl;
		for(int i = 0 ; i < regions.size() ; i++){
			double tmPsnr = vignette->ressemblance(regions[i]);
			if(tmPsnr > regions[i]->bestPsnr){
				regions[i]->bestPsnr = tmPsnr;
				regions[i]->bestImg = tmp;
			}
		}
		delete vignette;
		delete tmp;
	}
}

void readEntete(ifstream* file, int &nb_colonnes, int &nb_lignes) {
	int octet = file->get();
	if (octet == 'P') {
		do {
			octet = file->get();
		} while (octet != 0x0A);
	}
	string str = "";
	do {
		octet = file->get();
		char c = octet;
		str.push_back(c);
	} while (octet != 0x0A && octet != 0x20);
	nb_colonnes = atoi(str.c_str());

	str = "";
	do {
		octet = file->get();
		char c = octet;
		str.push_back(c);
	} while (octet != 0x0A && octet != 0x20);
	
	nb_lignes = atoi(str.c_str());

	do {
		octet = file->get();
	} while (octet != 0x0A);
}

void findBestImagesGiga(vector<Region*> regions, bool uneven) {
	string databaseName = "dataBase/";
	ifstream databaseLocation("DatabaseLocation");
	bool found = false;
	if (databaseLocation.good()) {
		found = (bool)getline(databaseLocation, databaseName);
		databaseLocation.close();
	}
	if (!found) {
		ofstream write("DatabaseLocation");
		write << databaseName;
		write.close();
	}

	auto files = Database::scanFolder(databaseName);
	for (int j = 0; j < files.size(); j++) {
		bool color = regions[0]->color;
		int nbCouleur = (color ? 3 : 1);
		ifstream gigafile;
		gigafile.open((files[j]), ifstream::in | ifstream::binary);
		if (gigafile.good()) {
			int counter = 0;
			bool headerRead = true;
			while(gigafile.good() && headerRead) {
				if (counter % 10 == 0) {
					cout << "Counter : " << counter << "\n";
				}

				int nb_lignes = 0;
				int nb_colonnes = 0;
				//READ EN TETE
				readEntete(&gigafile, nb_colonnes, nb_lignes);
				// FIN READ ENTETE
				counter++;

				if (!gigafile.eof()) {
					while (gigafile.peek() == '\n') {
						gigafile.get();
					}
					unsigned char * data = new unsigned char[nb_lignes * nb_colonnes * nbCouleur];
					gigafile.read((char*)data, nb_lignes * nb_colonnes * nbCouleur);
					Image* tmp = new Image(color);
					tmp->tab = data;
					tmp->sizeX = nb_colonnes;
					tmp->sizeY = nb_lignes;
					Image* vignette;
					if (!uneven) {
						vignette = tmp->scale(regions[0]);
					}


					for (int i = 0; i < regions.size(); i++) {
						if (uneven) {
							vignette = tmp->scale(regions[i]);
						}
						double tmPsnr = vignette->ressemblance(regions[i]);
						if (tmPsnr > regions[i]->bestPsnr) {
							regions[i]->bestPsnr = tmPsnr;
							regions[i]->bestImg = tmp;
						}
						if (uneven) {
							delete vignette;
						}
					}
					if (!uneven) {
						delete vignette;
					}

					//delete tmp;
				}
				
			}
		}
		cout << ((int)(map(j, 0, files.size(), 0, 100) * 100.0)) / 100.0 << "%\n";
		gigafile.close();
	}
}

/*void findBestImages(vector<Region*> regions, vector<vector<string>> * dataBase){
	vector<vector<Region*>> splitedRegions = vector<vector<Region*>>(256); 
	for(int i = 0 ; i < regions.size() ; i++){
		int tmpAvg = round(avg(regions[i]));
		splitedRegions[tmpAvg].push_back(regions[i]);
	}

	int avancement = 0;
	int nbFiles = 3978;

	for(int i = 0 ; i < 256 ; i++){
		if(dataBase->at(i).size()== 0 && splitedRegions[i].size()>0){
			cout<<"i : "<<i<<endl;
			for(int k = 0 ; k < splitedRegions[i].size() ; k++){
				for(int ii = max(0,i-20) ; ii < min(256,max(0,i-10)+40) ; ii ++){
					if(i != ii && dataBase->at(ii).size() > 0)
					splitedRegions[ii].push_back(splitedRegions[i][k]);
				}
				//splitedRegions[i][k]->bestImg = "images/gerard.pgm";
			}
		}
	}1
	for(int i = 0 ; i < 256 ; i ++){
		//cout<<"splitedRegions[i].size() : "<<splitedRegions[i].size()<<endl;
		if(splitedRegions[i].size()>0){
			//cout<<"Couleur "<<i<<" nombre d'images "<<dataBase->at(i).size()<<endl;
			///////
			///////
			for(int j = 0 ; j < dataBase->at(i).size() ; j++){

				//cout<<"image : "<<dataBase->at(i)[j]<<endl;
				Image * tmp = loadPgmFromFile((char *)dataBase->at(i)[j].c_str());
				Image * vignette =  scale(tmp, regions[0]);
				cout<<((int)(map(avancement,0,nbFiles,0,100)*100.0))/100.0<<"%"<<endl;
				avancement++;
				delete tmp;

				for(int k = 0 ; k < splitedRegions[i].size() ; k++){
					double tmPsnr = ressemblance(vignette, splitedRegions[i][k]);
					if(tmPsnr > splitedRegions[i][k]->bestPsnr){
						splitedRegions[i][k]->bestPsnr = tmPsnr;
						splitedRegions[i][k]->bestImg = dataBase->at(i)[j];
					}
				}
				delete vignette;
			}
		}
	}
}*/

void replaceWithBestImg(vector<Region*> regions){
    bool color = regions[0]->color;
	for(int i = 0 ; i < regions.size() ; i++){
		Image * vignette = regions[i]->bestImg->scale(regions[i]);

		delete regions[i]->tab;
		regions[i]->tab = vignette->tab;
	}
}

void scale(vector<Region*> regions, int size){
	for(int i = 0 ; i < regions.size() ; i ++){
		regions[i]->sizeX *= size;
		regions[i]->sizeY *= size;
		regions[i]->startX *= size;
		regions[i]->startY *= size;
	}		
}

vector<Region*> unevenSplit(Image * img, double seuil, int minimal){
	vector<Region*> res;
	Region * r = new Region();
	r->startX = 0;
	r->startY = 0;
	r->sizeX = img->sizeX;
	r->sizeY = img->sizeY;
	r->tab = img->tab;
	r->color = img->color;
	if(r->split(&res, seuil, minimal)){
		res.push_back(r);
	} else {
		delete r;
	}
	return res;
}

vector<Region*> split(Image * img, int nbvdiv){
	vector<Region*> res;
	if(img->color){
		int regSizeX = img->sizeX / nbvdiv;
		int regSizeY = img->sizeY / nbvdiv;

		for(int x = 0 ; x < nbvdiv ; x++){
			for(int y = 0 ; y < nbvdiv ; y++){
				Region * tmp = new Region();
				allocation_tableau(tmp->tab, OCTET, regSizeX*regSizeY*3);
				
				tmp->startX = x*regSizeX;
				tmp->startY = y*regSizeY;

				tmp->sizeX = regSizeX;
				tmp->sizeY = regSizeY;
				
				for(int j = tmp->startY ; j < tmp->startY + tmp->sizeY ; j++){
					for(int i = tmp->startX ; i < tmp->startX + tmp->sizeX ; i++ ){
						for(int c = 0 ; c< 3 ; c++){
							// cout<<"taille : "<<regSizeX*regSizeY*3<<endl;
							// cout<<"ind    : "<<((j-tmp->startY)*tmp->sizeX + i - tmp->startX)*3+c<<endl;
							tmp->tab[((j-tmp->startY)*tmp->sizeX + i - tmp->startX)*3+c] =
							img->tab[(j*img->sizeX + i)*3+c];
						}
					}
				}
				res.push_back(tmp);
			}
		}
	} else {
		int regSizeX = img->sizeX / nbvdiv;
		int regSizeY = img->sizeY / nbvdiv;

		for(int x = 0 ; x < nbvdiv ; x++){
			for(int y = 0 ; y < nbvdiv ; y++){
				Region * tmp = new Region();
				allocation_tableau(img->tab, OCTET, regSizeX*regSizeY);
				
				tmp->startX = x*regSizeX;
				tmp->startY = y*regSizeY;

				tmp->sizeX = regSizeX;
				tmp->sizeY = regSizeY;
				
				for(int j = tmp->startY ; j < tmp->startY + tmp->sizeY ; j++){
					for(int i = tmp->startX ; i < tmp->startX + tmp->sizeX ; i++ ){
						tmp->tab[(j-tmp->startY)*tmp->sizeX + i - tmp->startX] =
						img->tab[j*img->sizeX + i];
					}
				}
				res.push_back(tmp);
			}
		}
	}
	return res;
}

Image * merge(vector<Region*> regs){
	Image * img = new Image();
	img->sizeX = 0;
	img->sizeY = 0;

	if(regs.size() > 0){
		img->color = regs[0]->color;
	}else{
		return img;
	}
	

	for(int i = 0 ; i < regs.size() ; i ++){
		if(regs[i]->sizeX + regs[i]->startX > img->sizeX){
			img->sizeX = regs[i]->sizeX + regs[i]->startX;
		}
		if(regs[i]->sizeY + regs[i]->startY > img->sizeY){
			img->sizeY = regs[i]->sizeY + regs[i]->startY;
		}
	}

	cout<<"sizeX"<<img->sizeX<<" sizeY"<<img->sizeY<<endl;

	if(img->color){
		allocation_tableau(img->tab, OCTET, img->sizeX*img->sizeY*3);
		for(int i = 0 ; i < regs.size() ; i ++){
			for(int y = 0 ; y < regs[i]->sizeY ; y++){
				for(int x = 0 ; x < regs[i]->sizeX ; x++){
					for(int c = 0; c < 3 ; c++){
						img->tab[
							((y+regs[i]->startY)*img->sizeX + x + regs[i]->startX)*3+c] 
							= 
							regs[i]->tab[(y*regs[i]->sizeX + x)*3+c];
					}
				}
			}
		}
	}else{
		allocation_tableau(img->tab, OCTET, img->sizeX*img->sizeY);
		for(int i = 0 ; i < regs.size() ; i ++){
			for(int y = 0 ; y < regs[i]->sizeY ; y++){
				for(int x = 0 ; x < regs[i]->sizeX ; x++){
					img->tab[
						(y+regs[i]->startY)*img->sizeX + x + regs[i]->startX] 
						= 
						regs[i]->tab[y*regs[i]->sizeX + x];
				}
			}
		}
	}
	return img;
}

#endif