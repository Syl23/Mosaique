#ifndef __IMAGE_HPP__
#define __IMAGE_HPP__
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string> 
#include <cstring>
#include "image_ppm.h"
#include "Couleur.hpp"
#include "image_ppm.h"

using namespace std;

double map(double x, double b1, double b2, double b3, double b4) {
	return(((x - b1) / (b2 - b1)) * (b4 - b3) + b3);
}

class Image {
public:
	OCTET* tab = nullptr;
	int sizeX;
	int sizeY;
	bool color;

	int reference = 0;
	Image(bool color = true) {
		this->color = color;
	}

	Image(Image* clonage) {
		this->sizeX = clonage->sizeX;
		this->sizeY = clonage->sizeY;
		this->color = clonage->color;
		this->tab = new unsigned char[this->sizeX * this->sizeY * (this->color ? 3 : 1)];
		memcpy(this->tab, clonage->tab, this->sizeX * this->sizeY * (this->color ? 3 : 1));
	}


	Image(char* name, bool color = true) {
		this->color = color;
		if (color) {
			lire_nb_lignes_colonnes_image_ppm(name, &this->sizeY, &this->sizeX);
			allocation_tableau(this->tab, OCTET, this->sizeY * this->sizeX * 3);
			lire_image_ppm(name, this->tab, this->sizeY * this->sizeX);
		}
		else {
			lire_nb_lignes_colonnes_image_pgm(name, &this->sizeY, &this->sizeX);
			allocation_tableau(this->tab, OCTET, this->sizeY * this->sizeX);
			lire_image_pgm(name, this->tab, this->sizeY * this->sizeX);
		}
	}

	~Image() {
		delete[] this->tab;
	}

	void writeToFile(char* name) {
		if (this->color) {
			ecrire_image_ppm(name, this->tab, this->sizeY, this->sizeX);
		}
		else {
			ecrire_image_pgm(name, this->tab, this->sizeY, this->sizeX);
		}
	}

	double variance(Couleur moyenne) {
		double res = 0;

		if (this->color) {
			for (int i = 0; i < this->sizeX * this->sizeY; i++) {
				Couleur tmp(this->tab[i * 3 + 0], this->tab[i * 3 + 1], this->tab[i * 3 + 2]);
				res += tmp.dist(moyenne);
			}
		}
		else {
			for (int i = 0; i < this->sizeX * this->sizeY; i++) {
				Couleur tmp(this->tab[i]);
				res += tmp.dist(moyenne);
			}
		}
		res /= (double)(this->sizeX * this->sizeY);

		return res;
	}

	Couleur avg() {
		Couleur res;

		if (this->color) {
			res.color = true;
			for (int i = 0; i < this->sizeX * this->sizeY; i++) {
				Couleur tmp(this->tab[i * 3 + 0], this->tab[i * 3 + 1], this->tab[i * 3 + 2]);
				res.add(tmp);
			}
		}
		else {
			for (int i = 0; i < this->sizeX * this->sizeY; i++) {
				Couleur tmp(this->tab[i]);
				res.add(tmp);
			}
		}
		res.divide(this->sizeX * this->sizeY);

		return res;
	}

	double ressemblance(Image* reg) {
		double seuil = 10;
		Couleur a = this->avg();
		Couleur b = reg->avg();
		double cDist = a.dist(b);

		//return(1.0/square(avg(this)-avg(reg)));
		if (cDist < seuil)
			return(psnr(reg, this->sizeX, this->sizeY));
		else
			return(1.0 / cDist);
	}

	Image* scale(int tsizeX, int tsizeY) {
		Image* res = new Image();
		res->sizeX = tsizeX;
		res->sizeY = tsizeY;
		res->color = this->color;
		int colorNb = (this->color ? 3 : 1);

		if (res->tab != nullptr) {
			delete res->tab;
		}

		res->tab = new unsigned char[res->sizeX * res->sizeY * colorNb];
		for (int y = 0; y < res->sizeY; y++) {
			for (int x = 0; x < res->sizeX; x++) {
				for (int c = 0; c < colorNb; c++) {
					res->tab[(y * res->sizeX + x) * colorNb + c] = this->tab[((int)(
						(int)(map(y, 0, res->sizeY, 0, this->sizeY)) * this->sizeX +
						(int)(map(x, 0, res->sizeX, 0, this->sizeX))
						)) * colorNb + c];
				}
			}
		}

		return res;
	}


	Image* scale(Image* model) {
		Image* res = new Image();
		res->sizeX = model->sizeX;
		res->sizeY = model->sizeY;
		res->color = this->color;
		int colorNb = (this->color ? 3 : 1);

		res->tab = new unsigned char[res->sizeX * res->sizeY * colorNb];
		for (int y = 0; y < res->sizeY; y++) {
			for (int x = 0; x < res->sizeX; x++) {
				for (int c = 0; c < colorNb; c++) {
					res->tab[(y * res->sizeX + x) * colorNb + c] = this->tab[((int)(
						(int)(map(y, 0, res->sizeY, 0, this->sizeY)) * this->sizeX +
						(int)(map(x, 0, res->sizeX, 0, this->sizeX))
						)) * colorNb + c];
				}
			}
		}

		return res;
	}

	double psnr(Image* disImg, int n, int m) {
		double res = -1.0;
		if (this->color == disImg->color) {
			if (this->color) {
				int d = 255;
				float diff = 0.;
				int nW = this->sizeX;
				int nH = this->sizeY;
				for (int i = 0; i < nH * nW * 3; i++)
				{
					diff += sqrt((this->tab[i] - disImg->tab[i]) * (this->tab[i] - disImg->tab[i]));
				}

				float EQM = 1. / (nH * nW * 3) * diff;
				res = 10 * log10(d * d / EQM);
			}
			else {
				double sum = 0.0;
				int i;
				double error;

				for (i = 0; i < n * m; i++) {
					error = this->tab[i] - disImg->tab[i];
					sum = error * error;
				}

				double mse = sum / (n * m);
				res = 10.0 * log(255.0 * 255.0 / mse) / log(10.0);
			}

		}

		return res;
	}

};


#endif