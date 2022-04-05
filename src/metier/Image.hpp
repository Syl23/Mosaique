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

double map(double x, double b1, double b2, double b3, double b4){
  return(((x-b1)/(b2-b1))*(b4-b3)+b3);
}

class Image {
public :
    OCTET* tab;
	int sizeX;
	int sizeY;
    bool color; 

    Image(bool color = true){
        this->color = color;
    }

    Image(char* name, bool color = true){
        this->color = color;
        if(color){
            lire_nb_lignes_colonnes_image_ppm(name, &this->sizeY, &this->sizeX);
            allocation_tableau(this->tab, OCTET, this->sizeY*this->sizeX * 3);
            lire_image_ppm(name, this->tab, this->sizeY*this->sizeX);
        } else {
            lire_nb_lignes_colonnes_image_pgm(name, &this->sizeY, &this->sizeX);
            allocation_tableau(this->tab, OCTET, this->sizeY*this->sizeX);
            lire_image_pgm(name, this->tab, this->sizeY*this->sizeX);
        }
    }

    ~Image(){
        delete [] this->tab;
    }

    void writeToFile(char * name){
        if(this->color){
            ecrire_image_ppm(name, this->tab,  this->sizeY, this->sizeX);
        } else {
            ecrire_image_pgm(name, this->tab,  this->sizeY, this->sizeX);
        }
    }

    Couleur avg(){
        Couleur res;

        if(this->color){
            res.color = true;
            for(int i = 0 ; i < this->sizeX*this->sizeY ; i++){
                Couleur tmp(this->tab[i*3+0], this->tab[i*3+1], this->tab[i*3+2]);
                res.add(tmp);
            }
        } else {
            for(int i = 0 ; i < this->sizeX*this->sizeY ; i++){
                Couleur tmp(this->tab[i]);
                res.add(tmp);
            }
        }
        res.divide(this->sizeX*this->sizeY);

        return res;
    }

    double ressemblance(Image * reg){
        double seuil = 10;
        Couleur a = this->avg();
        Couleur b = reg->avg();
        double cDist = a.dist(b);

        //return(1.0/square(avg(this)-avg(reg)));
        if(cDist<seuil)
            return(psnr(reg, this->sizeX,this->sizeY));
        else
            return(1.0/cDist);
    }

    Image* scale(Image * model){
        Image * res = new Image();
        res->sizeX = model->sizeX;
        res->sizeY = model->sizeY;
        res->color = this->color;
        if(this->color){
            allocation_tableau(res->tab, OCTET, res->sizeX*res->sizeY*3);
            for(int y = 0 ; y < res->sizeY ; y ++){
                for(int x = 0 ; x < res->sizeX ; x++){
                    for(int c = 0 ; c < 3 ; c++){
                        res->tab[(y*res->sizeX + x)*3+c] = this->tab[((int)(
                            (int)(map(y,0,res->sizeY,0,this->sizeY))*this->sizeX+
                            (int)(map(x,0,res->sizeX,0,this->sizeX))
                            ))*3+c];
                    }
                }
            }
        } else {
            allocation_tableau(res->tab, OCTET, res->sizeX*res->sizeY);
            for(int y = 0 ; y < res->sizeY ; y ++){
                for(int x = 0 ; x < res->sizeX ; x++){
                    res->tab[y*res->sizeX + x] = this->tab[(int)(
                        (int)(map(y,0,res->sizeY,0,this->sizeY))*this->sizeX+
                        (int)(map(x,0,res->sizeX,0,this->sizeX))
                        )];
                }
            }
        }

        return res;
    }
    double psnr(Image* disImg , int n , int m){
        double res = -1.0;
        if(this->color == disImg->color){
            if(this->color){
                int d =255;
                float diff =0.;
                int nW = this->sizeX;
                for(int i =0; i < nW*nW*3 ; i++)
                {
                    diff += sqrt((this->tab[i]-disImg->tab[i])*(this->tab[i]-disImg->tab[i]));
                }

                float EQM = 1./(nW*nW*3) * diff;
                res = 10*log10(d*d / EQM);
            } else {
                double sum = 0.0;
                int i;
                double error;

                for (i=0; i<n*m; i++){
                    error = this->tab[i] - disImg->tab[i];
                    sum = error * error;
                }

                double mse = sum/ (n*m);
                res = 10.0* log (255.0 * 255.0/mse) / log (10.0);
            }

        }

        return res;
    }

};


#endif