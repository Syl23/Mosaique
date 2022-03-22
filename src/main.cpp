#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string> 
#include <cstring>
#include <filesystem>

#include "lib/image_ppm.h"




using namespace std;
namespace fs = std::filesystem;

struct color{
    double r;
    double g;
    double b;
	// color(int i, int ii, int iii){
	// 	r = i;
	// 	g = ii;
	// 	b = iii;
	// }
};

struct Image{
	OCTET* tab;
	int sizeX;
	int sizeY;
};

struct Region{
	OCTET* tab;
	int sizeX;
	int sizeY;

	int startX;
	int startY;

	string bestImg;
	double bestPsnr = 0;

	// color avgColor;
	// double equartType;
};




// void free(Image * img){
// 	delete(img->tab);
// 	delete(img);
// }

// void free(Region * img){
// 	delete(img->tab);
// 	delete(img);
// }


/////////////////////////////////
void liberate(Image * img){
	delete [] img->tab;
	delete img;
} 
  
void liberate(Region * img){
	delete [] img->tab;
	delete img;
} 
/////////////////////////////////


double map(double x, double b1, double b2, double b3, double b4){
  return(((x-b1)/(b2-b1))*(b4-b3)+b3);
}

double square(double x){
	return x*x;
}

Image* loadPgmFromFile(char* name){
	Image *res = new Image();

	OCTET* tab;
	int tx;
	int ty;

	lire_nb_lignes_colonnes_image_pgm(name, &ty, &tx);

	allocation_tableau(tab, OCTET, tx*ty);
	lire_image_pgm(name, tab, tx*ty);

	res->tab = tab;
	res->sizeX = tx;
	res->sizeY = ty;

	return res;
}

void writePgmToFile(Image * img, char* nom){
	ecrire_image_pgm(nom, img->tab,  img->sizeY, img->sizeX);
}


vector<Region*> split(Image * img, int nbvdiv){
	vector<Region*> res;

	int regSizeX = img->sizeX / nbvdiv;
	int regSizeY = img->sizeY / nbvdiv;

	for(int x = 0 ; x < nbvdiv ; x++){
		for(int y = 0 ; y < nbvdiv ; y++){
			Region * tmp = new Region();
			allocation_tableau(tmp->tab, OCTET, regSizeX*regSizeY);
			
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
	return res;
}

Image * merge(vector<Region*> regs){
	int sizeX = 0;
	int sizeY = 0;

	for(int i = 0 ; i < regs.size() ; i ++){
		if(regs[i]->sizeX + regs[i]->startX > sizeX){
			sizeX = regs[i]->sizeX + regs[i]->startX;
		}
		if(regs[i]->sizeY + regs[i]->startY > sizeY){
			sizeY = regs[i]->sizeY + regs[i]->startY;
		}
	}

	Image * res = new Image();
	res->sizeX = sizeX;
	res->sizeY = sizeY;
	allocation_tableau(res->tab, OCTET, sizeX*sizeY);


	for(int i = 0 ; i < regs.size() ; i ++){
		for(int y = 0 ; y < regs[i]->sizeY ; y++){
			for(int x = 0 ; x < regs[i]->sizeX ; x++){
				res->tab[
					(y+regs[i]->startY)*sizeX + x + regs[i]->startX
					] = regs[i]->tab[
					y*regs[i]->sizeX + x]
					;
			}
		}
	}

	return res;
}

double psnr(OCTET* origImg , OCTET* DisImg , int n , int m){
	double sum = 0.0;
	int i;
	double error;

	for (i=0; i<n*m; i++){
		error = origImg[i] - DisImg[i];
		sum = error * error;
	}

	double mse = sum/ (n*m);

	return 10.0* log (255.0 * 255.0/mse) / log (10.0);


}

double avg(Image * in){
	double res = 0;
	for(int i = 0 ; i < in->sizeX*in->sizeY ; i++){
		res+=in->tab[i];
	}
	return res/(in->sizeX*in->sizeY);
}
double avg(Region * in){
	double res = 0;
	for(int i = 0 ; i < in->sizeX*in->sizeY ; i++){
		res+=in->tab[i];
	}
	return res/(in->sizeX*in->sizeY);
}

double ressemblance(Image * in, Region * reg){
	//return(1.0/square(avg(in)-avg(reg)));
	if(round(avg(in)) == round(avg(reg)))
		return(psnr(in->tab,reg->tab, in->sizeX,in->sizeY));
	else
		return(1.0/square(avg(in)-avg(reg)));
}

Image* scale(Image * in, Region * model){
	Image * res = new Image();
	res->sizeX = model->sizeX;
	res->sizeY = model->sizeY;
	allocation_tableau(res->tab, OCTET, res->sizeX*res->sizeY);
	for(int y = 0 ; y < res->sizeY ; y ++){
		for(int x = 0 ; x < res->sizeX ; x++){
			res->tab[y*res->sizeX + x] = in->tab[(int)(
				(int)(map(y,0,res->sizeY,0,in->sizeY))*in->sizeX+
				(int)(map(x,0,res->sizeX,0,in->sizeX))
				)];
		}
	}
	return res;
}

Image* scale(Image * in, Image * model){
	Image * res = new Image();
	res->sizeX = model->sizeX;
	res->sizeY = model->sizeY;
	allocation_tableau(res->tab, OCTET, res->sizeX*res->sizeY);
	for(int y = 0 ; y < res->sizeY ; y ++){
		for(int x = 0 ; x < res->sizeX ; x++){
			res->tab[y*res->sizeX + x] = in->tab[(int)(
				(int)(map(y,0,res->sizeY,0,in->sizeY))*in->sizeX+
				(int)(map(x,0,res->sizeX,0,in->sizeX))
				)];
		}
	}
	return res;
}

vector<string> scanFolder(string path){
	vector<string> res = vector<string>();
	for(const auto & entry : fs::directory_iterator(path)){
		res.push_back(entry.path());
		//cout<<entry.path()<<endl;
	}

	return res;
}

void findBestImages(vector<Region*> regions){
	auto files = scanFolder("dataBase");
	for(int j = 0 ; j < files.size() ; j++){
		auto f = files[j];
		Image * tmp = loadPgmFromFile((char *)f.c_str());
		Image * vignette =  scale(tmp, regions[0]);
		cout<<((int)(map(j,0,files.size(),0,100)*100.0))/100.0<<"%"<<endl;
		for(int i = 0 ; i < regions.size() ; i++){
			double tmPsnr = ressemblance(vignette, regions[i]);
			if(tmPsnr > regions[i]->bestPsnr){
				regions[i]->bestPsnr = tmPsnr;
				regions[i]->bestImg = f;
				//cout<<"bestPsnr : "<<tmPsnr<<endl;
			}
		}
		free(vignette);
		free(tmp);
	}
}

void findBestImages(vector<Region*> regions, vector<vector<string>> * dataBase){
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
	}


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
				free(tmp);

				for(int k = 0 ; k < splitedRegions[i].size() ; k++){
					double tmPsnr = ressemblance(vignette, splitedRegions[i][k]);
					if(tmPsnr > splitedRegions[i][k]->bestPsnr){
						splitedRegions[i][k]->bestPsnr = tmPsnr;
						splitedRegions[i][k]->bestImg = dataBase->at(i)[j];
					}
				}
				free(vignette);
			}
		}
	}
}

void replaceWithBestImg(vector<Region*> regions){
	for(int i = 0 ; i < regions.size() ; i++){

		Image * tmp = loadPgmFromFile((char *)regions[i]->bestImg.c_str());
		Image * vignette =  scale(tmp, regions[i]);

		free(tmp);
		delete(regions[i]->tab);
		regions[i]->tab = vignette->tab;
	}
}

void replaceWithBestImgButBig(vector<Region*> regions){
	//Image * model = loadPgmFromFile((char *)regions[0]->bestImg.c_str());
	Region * model = regions[0];

	// double multX = ((double)model->sizeX)/((double)regions[0]->sizeX);
	// double multY = ((double)model->sizeY)/((double)regions[0]->sizeY);

	double multX = 2;
	double multY = 2;

	model->sizeX *= multX;
	model->sizeY *= multY;


	for(int i = 0 ; i < regions.size() ; i++){
		Image * tmp = loadPgmFromFile((char *)regions[i]->bestImg.c_str());
		Image * vignette =  scale(tmp, model);

		free(tmp);
		delete(regions[i]->tab);
		regions[i]->tab = vignette->tab;

		regions[i]->sizeX*=multX;
		regions[i]->startX*=multX;

		regions[i]->sizeY*=multY;
		regions[i]->startY*=multY;
	}
}

vector<vector<string>> * importDatabase(){
	ifstream file("dataBase.dat", ios::in);

	vector<vector<string>> * res = new vector<vector<string>>(256);

	string tmpStr;
	int tmpInt;

	int size;
	file>>size;

	for(int i  = 0 ; i < size ; i++){
		file>>tmpInt>>tmpStr;
		res->at(tmpInt).push_back(tmpStr);
	}
	return res;
}

vector<vector<string>> * createDataBase(){
	vector<vector<string>> * res = new vector<vector<string>>(256);
	ofstream file("dataBase.dat", ios::out | ios::trunc);

	auto files = scanFolder("dataBase");
	file<<files.size()<<endl;
	for(int j = 0 ; j < files.size() ; j++){
		cout<<"Data-base generation : "<<((int)(map(j,0,files.size(),0,100)*100.0))/100.0<<"%"<<endl;
		auto f = files[j];
		Image * tmp = loadPgmFromFile((char *)f.c_str());
		int tmpAvg = round(avg(tmp));
		res->at(tmpAvg).push_back(f);
		file<<tmpAvg<<" "<<f<<endl;
		free(tmp);
	}

	return res;	
}

vector<vector<string>> * getDataBase(){
	ifstream fichier("dataBase.dat", ios::in);  // on ouvre en lecture
 
	if(fichier){  // si l'ouverture a fonctionné
		fichier.close();
		return importDatabase();		
	}
	else{
		return createDataBase();
	}

	return NULL;
}


void scale(vector<Region*> regions, int size){
	for(int i = 0 ; i < regions.size() ; i ++){
		regions[i]->sizeX *= size;
		regions[i]->sizeY *= size;
		regions[i]->startX *= size;
		regions[i]->startY *= size;
	}		
}


// int main(int argc, char const *argv[]){
// 	if(argc != 2){
// 		cout<<"Il convient de mettre le nom d'un fichier"<<endl;
// 		return 1;
// 	}
// 	int nb = 0;
// 	Image* imageIn = loadPgmFromFile((char *)argv[1]);

// 	vector<vector<string>> * dataBase =  getDataBase();

// 	// for(int i = 0 ; i < dataBase->size() ; i++){
// 	// 	cout<<i<<" : "<<dataBase->at(i).size()<<endl;
// 	// } 


// 	auto regions = split(imageIn, 40);
// 	findBestImages(regions, dataBase);
// 	//findBestImages(regions);

// 	replaceWithBestImg(regions);

// 	//replaceWithBestImgButBig(regions);
// 	//scale(regions, 2);
// 	Image* out = merge(regions);

// 	writePgmToFile(out, (char *)"results/mosaique.pgm");

// 	cout<<"psnr : "<<psnr(imageIn->tab,out->tab, out->sizeX,out->sizeY)<<endl;

// 	return 0;
// }

#include "Couleur.hpp"


// void infiniteZoomGif(char imgFile[]){
// 	Image* imageIn = loadPpmFromFile((char *)imgFile);
// 	Image model;
// 	model.sizeX = 1025;
// 	model.sizeY = 1025;
// 	Image * goodSizedImgIn = scaleColor(imageIn, &model);
// 	auto regions = splitColor(goodSizedImgIn, goodSizedImgIn->sizeX);
// 	findBestImagesColor(regions);

// 	for(int i = 0 ; i < regions.size() ; i ++){
// 		if(regions[i]->startX == 512 && regions[i]->startY == 512 ){
// 			regions[i]->bestImg = imgFile;
// 		}
// 	}

// 	scale(regions, 2);
// 	replaceWithBestImgColor(regions);
// 	Image* out = mergeColor(regions);

	

// 	int step = 1;
// 	for(int itt = 0 ; itt<1025 ; itt ++){
// 		replaceWithBestImgColor(regions);
// 		Image* out = mergeColor(regions);

// 		string p1 = ".tmp/";
// 		string p2 = itoa(itt);
// 		string p3 = ".ppm";
// 		string nom = p1+p2+p3;

// 		writePpmToFile(out, (char *)nom.c_str());
// 		free(out);

// 		// for(int i = 0 ; i < regions.size() ; i ++){
// 		// 	if(regions[i]->startX+regions[i]->sizeX < itt*step){
// 		// 		free(regions[i]);
// 		// 		regions.erase(i);
// 		// 	}
// 		// 	if(regions[i]->startX > 1025 - itt*step){
// 		// 		free(regions[i]);
// 		// 		regions.erase(i);
// 		// 	}
// 		// 	if(regions[i]->Y+regions[i]->sizeY < itt*step){
// 		// 		free(regions[i]);
// 		// 		regions.erase(i);
// 		// 	}
// 		// 	if(regions[i]->startY > 1025 - itt*step){
// 		// 		free(regions[i]);
// 		// 		regions.erase(i);
// 		// 	}
// 		// }





// 	}
// }

int main(int argc, char const *argv[]){

	if(argc != 5){
		cout<<"Il convient de mettre le nom d'un fichier d'entré, de sortie, une taille de découpage et un scale"<<endl;
		return 1;
	}

	Image* imageIn = loadPpmFromFile((char *)argv[1]);

	auto regions = splitColor(imageIn, atoi(argv[3]));

	findBestImagesColor(regions);

	for(int i = 0 ; i < regions.size() ; i ++){
		cout<<"psnr : "<<regions[i]->bestPsnr<<endl;
	}

	scale(regions, atoi(argv[4]));
	replaceWithBestImgColor(regions);
	
	Image* out = mergeColor(regions);

	writePpmToFile(out, (char *)argv[2]);
	return 0;
}