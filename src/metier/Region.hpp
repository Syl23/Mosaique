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

class Region : public Image {
public:
	int startX;
	int startY;

	Image* bestImg;
	double bestPsnr;


	Region() {
		startX = 0;
		startY = 0;
		bestPsnr = -1;
		bestImg = nullptr;
	}
	~Region() {

	}

	void scale(int a) {
		this->sizeX *= a;
		this->sizeY *= a;
		this->startX *= a;
		this->startY *= a;
	}

	void applyBestImage() {
		int nbColor = (this->color ? 3 : 1);
		if (this->tab != nullptr) {
			delete this->tab;
		}
		if (this->bestImg != nullptr) {
			this->tab = this->bestImg->tab;
		}
	}

	bool split(vector<Region*>* regions, double seuil, int minimal) {
		bool isFeuille = true;
		if (this->sizeX >= minimal && this->sizeY >= minimal) {
			Couleur c = this->avg();
			double variance = this->variance(c);
			if (variance > seuil) {
				isFeuille = false;
				int regSizeX = 0;
				int regSizeY = 0;

				int cumulativeRegSizeX = 0;

				for (int x = 0; x < 2; x++) {
					regSizeX = this->sizeX / 2;
					int cumulativeRegSizeY = 0;

					if (this->sizeX % 2 == 1 && x == 1) {
						regSizeX++;
					}

					for (int y = 0; y < 2; y++) {

						Region* tmp = new Region();

						regSizeY = this->sizeY / 2;
						if (this->sizeY % 2 == 1 && y == 1) {
							regSizeY++;
						}


						allocation_tableau(tmp->tab, OCTET, regSizeX * regSizeY * 3);

						tmp->startX = cumulativeRegSizeX;
						tmp->startY = cumulativeRegSizeY;

						tmp->sizeX = regSizeX;
						tmp->sizeY = regSizeY;

						for (int j = cumulativeRegSizeY; j < tmp->startY + tmp->sizeY; j++) {
							for (int i = cumulativeRegSizeX; i < tmp->startX + tmp->sizeX; i++) {
								for (int c = 0; c < 3; c++) {
									OCTET oct = this->tab[(j * this->sizeX + i) * 3 + c];
									tmp->tab[((j - tmp->startY) * tmp->sizeX + i - tmp->startX) * 3 + c] = oct;
								}
							}
						}

						tmp->startX = cumulativeRegSizeX + this->startX;
						tmp->startY = cumulativeRegSizeY + this->startY;

						if (tmp->split(regions, seuil, minimal)) {
							regions->push_back(tmp);
						}
						else {
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

void unevenFindBestImages(vector<Region*> regions) {
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
		if (j % 10 == 0) {
			cout << "Counter : " << j << endl;
		}
		bool color = regions[0]->color;
		auto f = files[j];

		Image* tmp = new Image((char*)f.c_str(), color);
		bool set = false;
		for (int i = 0; i < regions.size(); i++) {
			Image* vignette = tmp->scale(regions[i]);

			double tmPsnr = vignette->ressemblance(regions[i]);
			if (tmPsnr > regions[i]->bestPsnr) {
				regions[i]->bestPsnr = tmPsnr;
				regions[i]->bestImg = tmp;
				set = true;
			}
			delete vignette;

		}
		if (!set) {
			delete tmp;
		}
	}
}
void findBestImages(vector<Region*> regions) {

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
		auto f = files[j];

		Image* tmp = new Image((char*)f.c_str(), color);
		Image* vignette = tmp->scale(regions[0]);
		bool set = false;
		cout << ((int)(map(j, 0, files.size(), 0, 100) * 100.0)) / 100.0 << "%" << endl;
		for (int i = 0; i < regions.size(); i++) {
			double tmPsnr = vignette->ressemblance(regions[i]);
			if (tmPsnr > regions[i]->bestPsnr) {
				regions[i]->bestPsnr = tmPsnr;
				regions[i]->bestImg = tmp;
				set = true;
			}
		}
		delete vignette;
		if (!set) {
			delete tmp;
		}
	}
}

void readEntete(ifstream* file, int& nb_colonnes, int& nb_lignes) {
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

string getDataBaseFolder() {
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
	return databaseName;
}

void findBestImagesGiga(vector<Region*> regions, int scale = 1, bool even = false) {
	auto files = Database::scanFolder(getDataBaseFolder());
	for (int j = 0; j < files.size(); j++) {
		bool color = regions[0]->color;
		int nbCouleur = (color ? 3 : 1);
		ifstream gigafile;
		gigafile.open((files[j]), ifstream::in | ifstream::binary);
		if (gigafile.good()) {
			int counter = 0;
			bool end = false;
			while (gigafile.good() && !end) {
				if (counter % 100 == 0 && counter > 0) {
					cout << "Treated : " << counter << "\n";
				}

				int nb_lignes = 0;
				int nb_colonnes = 0;
				//READ EN TETE
				if (gigafile.peek() != EOF) {
					readEntete(&gigafile, nb_colonnes, nb_lignes);
					// FIN READ ENTETE
					counter++;

					if (!gigafile.eof()) {
						unsigned char* data = new unsigned char[nb_lignes * nb_colonnes * nbCouleur];

						gigafile.read((char*)data, nb_lignes * nb_colonnes * nbCouleur);
						Image* tmp = new Image(color);
						tmp->tab = data;
						tmp->sizeX = nb_colonnes;
						tmp->sizeY = nb_lignes;

						vector<Image*> savedImages;
						Image* vignette = nullptr;

						if (even) {
							savedImages.push_back(tmp->scale(regions[0]->sizeX * scale, regions[0]->sizeY * scale));
							vignette = tmp->scale(regions[0]);
						}
						for (int i = 0; i < regions.size(); i++) {
							if (!even) {
								vignette = tmp->scale(regions[i]);
							}
							double tmPsnr = vignette->ressemblance(regions[i]);
							if (tmPsnr > regions[i]->bestPsnr) {
								regions[i]->bestPsnr = tmPsnr;
								if (regions[i]->bestImg != nullptr) {
									regions[i]->bestImg->reference--;

									if (regions[i]->bestImg->reference <= 0) {
										delete regions[i]->bestImg;
									}
								}
								Image* wanted = nullptr;
								if (even) {
									wanted = savedImages[0];
									savedImages[0]->reference++;
								}
								else {
									bool found = false;
									for (int j = 0, max = savedImages.size(); j < max && !found; j++) {
										if (savedImages[j]->sizeX == regions[i]->sizeX * scale && savedImages[j]->sizeY == regions[i]->sizeY * scale) {
											found = true;
											wanted = savedImages[j];
											wanted->reference++;
										}
									}

									if (!found) {
										wanted = tmp->scale(regions[i]->sizeX * scale, regions[i]->sizeY * scale);
										wanted->reference++;
										savedImages.push_back(wanted);
									}
								}
								regions[i]->bestImg = wanted;

							}
							if (!even) {
								delete vignette;
							}
						}
						if (even) {
							delete vignette;
							if (savedImages[0]->reference <= 0) {
								Image* t = savedImages[0];
								savedImages.pop_back();
								delete t;
							}
						}

						delete tmp;
					}
				}
				else {
					end = true;
				}

			}
		}
		gigafile.close();
		if(files.size() > 1 && j % 100 == 0){
			cout << (((double)j) / (double)files.size()) * 100.0 << "%\n";
		}
	}
}

void replaceWithBestImg(vector<Region*> regions) {
	bool color = regions[0]->color;
	for (int i = 0; i < regions.size(); i++) {
		Image* vignette = regions[i]->bestImg->scale(regions[i]);

		delete regions[i]->tab;
		regions[i]->tab = vignette->tab;
	}
}

void scale(vector<Region*> regions, int size) {
	for (int i = 0; i < regions.size(); i++) {
		regions[i]->sizeX *= size;
		regions[i]->sizeY *= size;
		regions[i]->startX *= size;
		regions[i]->startY *= size;
	}
}

vector<Region*> unevenSplit(Image* img, double seuil, int minimal) {
	vector<Region*> res;
	Region* r = new Region();
	r->startX = 0;
	r->startY = 0;
	r->sizeX = img->sizeX;
	r->sizeY = img->sizeY;
	r->tab = img->tab;
	r->color = img->color;
	if (r->split(&res, seuil, minimal)) {
		res.push_back(r);
	}
	else {
		delete r;
	}
	return res;
}

vector<Region*> split(Image* img, int nbvdiv) {
	vector<Region*> res;
	int nbColor = (img->color ? 3 : 1);
	int regSizeX = img->sizeX / nbvdiv;
	int regSizeY = img->sizeY / nbvdiv;

	for (int x = 0; x < nbvdiv; x++) {
		for (int y = 0; y < nbvdiv; y++) {
			Region* tmp = new Region();
			tmp->tab = new unsigned char[regSizeX * regSizeY * nbColor];

			tmp->startX = x * regSizeX;
			tmp->startY = y * regSizeY;

			tmp->sizeX = regSizeX;
			tmp->sizeY = regSizeY;

			for (int j = tmp->startY; j < tmp->startY + tmp->sizeY; j++) {
				for (int i = tmp->startX; i < tmp->startX + tmp->sizeX; i++) {
					for (int c = 0; c < nbColor; c++) {
						tmp->tab[((j - tmp->startY) * tmp->sizeX + i - tmp->startX) * nbColor + c] =
							img->tab[(j * img->sizeX + i) * nbColor + c];
					}
				}
			}
			res.push_back(tmp);
		}
	}

	return res;
}

Image* merge(vector<Region*> regs) {
	Image* img = new Image();
	img->sizeX = 0;
	img->sizeY = 0;

	if (regs.size() > 0) {
		img->color = regs[0]->color;
	}
	else {
		return img;
	}


	for (int i = 0; i < regs.size(); i++) {
		if (regs[i]->sizeX + regs[i]->startX > img->sizeX) {
			img->sizeX = regs[i]->sizeX + regs[i]->startX;
		}
		if (regs[i]->sizeY + regs[i]->startY > img->sizeY) {
			img->sizeY = regs[i]->sizeY + regs[i]->startY;
		}
	}

	cout << "sizeX" << img->sizeX << " sizeY" << img->sizeY << endl;
	int nbColor = (img->color ? 3 : 1);
	allocation_tableau(img->tab, OCTET, img->sizeX * img->sizeY * nbColor);
	for (int i = 0; i < regs.size(); i++) {
		for (int y = 0; y < regs[i]->sizeY; y++) {
			for (int x = 0; x < regs[i]->sizeX; x++) {
				for (int c = 0; c < nbColor; c++) {
					img->tab[
						((y + regs[i]->startY) * img->sizeX + x + regs[i]->startX) * nbColor + c]
						=
							regs[i]->tab[(y * regs[i]->sizeX + x) * nbColor + c];
				}
			}
		}
	}

	return img;
}

void applyBestImages(vector<Region*> regs) {
	for (int i = 0, max = regs.size(); i < max; i++) {
		regs[i]->applyBestImage();
	}
}

#endif