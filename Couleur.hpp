void writePpmToFile(Image * img, char* nom){
	ecrire_image_ppm(nom, img->tab,  img->sizeY, img->sizeX);
}

Image* loadPpmFromFile(const char* name){
	Image *res = new Image();

	OCTET* tab; 
	int tx;
	int ty;

	//lire_nb_lignes_colonnes_image_pgm(name, &ty, &tx);
    lire_nb_lignes_colonnes_image_ppm(name, &ty, &tx);

	allocation_tableau(tab, OCTET, tx*ty*3);
	lire_image_ppm(name, tab, tx*ty);

	res->tab = tab;
	res->sizeX = tx;
	res->sizeY = ty;

	return res;
}

vector<Region*> splitColor(Image * img, int nbvdiv){
	vector<Region*> res;

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
	return res;
}
Image* scaleColor(Image * in, Region * model){
	Image * res = new Image();
	res->sizeX = model->sizeX;
	res->sizeY = model->sizeY;
	allocation_tableau(res->tab, OCTET, res->sizeX*res->sizeY*3);
	for(int y = 0 ; y < res->sizeY ; y ++){
		for(int x = 0 ; x < res->sizeX ; x++){
            for(int c = 0 ; c < 3 ; c++){
                res->tab[(y*res->sizeX + x)*3+c] = in->tab[((int)(
                    (int)(map(y,0,res->sizeY,0,in->sizeY))*in->sizeX+
                    (int)(map(x,0,res->sizeX,0,in->sizeX))
                    ))*3+c];
            }
		}
	}
	return res;
}

Image* scaleColor(Image * in, Image * model){
	Image * res = new Image();
	res->sizeX = model->sizeX;
	res->sizeY = model->sizeY;
	allocation_tableau(res->tab, OCTET, res->sizeX*res->sizeY*3);
	for(int y = 0 ; y < res->sizeY ; y ++){
		for(int x = 0 ; x < res->sizeX ; x++){
            for(int c = 0 ; c < 3 ; c++){
                res->tab[(y*res->sizeX + x)*3+c] = in->tab[((int)(
                    (int)(map(y,0,res->sizeY,0,in->sizeY))*in->sizeX+
                    (int)(map(x,0,res->sizeX,0,in->sizeX))
                    ))*3+c];
            }
		}
	}
	return res;
}


float colorPsnr(OCTET* ImgIn,OCTET* ImgOut, int nH, int nW)
{
  int d =255;
  float diff =0.;

  for(int i =0; i < nW*nW*3 ; i++)
  {
    diff += sqrt((ImgIn[i]-ImgOut[i])*(ImgIn[i]-ImgOut[i]));
  }

  float EQM = 1./(nW*nW*3) * diff;
  return 10*log10(d*d / EQM);
}

struct color{
    double r;
    double g;
    double b;
};

//double colorAvgDiff()

struct color avgColor(Image * in){
	color res;

    res.r=0;
    res.g=0;
    res.b=0;

	for(int i = 0 ; i < in->sizeX*in->sizeY ; i++){
        res.r += in->tab[i*3+0];
        res.g += in->tab[i*3+1];
        res.b += in->tab[i*3+2];
	}

    res.r = res.r/(in->sizeX*in->sizeY);
    res.g = res.g/(in->sizeX*in->sizeY);
    res.b = res.b/(in->sizeX*in->sizeY);

	return res;
}
color avgColor(Region * in){
	color res;

    res.r=0;
    res.g=0;
    res.b=0;

	for(int i = 0 ; i < in->sizeX*in->sizeY ; i++){
        res.r += in->tab[i*3+0];
        res.g += in->tab[i*3+1];
        res.b += in->tab[i*3+2];
	}

    res.r = res.r/(in->sizeX*in->sizeY);
    res.g = res.g/(in->sizeX*in->sizeY);
    res.b = res.b/(in->sizeX*in->sizeY);

	return res;
}

double colorDist(color a, color b){
    return sqrt(square(a.r-b.r) + square(a.g-b.g) + square(a.b-b.b));
}

double ressemblanceColor(Image * in, Region * reg){
    double seuil = 10;
    double cDist = colorDist(avgColor(in), avgColor(reg));

	//return(1.0/square(avg(in)-avg(reg)));
	if(cDist<seuil)
		return(colorPsnr(in->tab,reg->tab, in->sizeX,in->sizeY));
	else
		return(1.0/cDist);
}

void findBestImagesColor(vector<Region*> regions){
	auto files = scanFolder("ppmColorDatabase");
	for(int j = 0 ; j < files.size() ; j++){
        const char * f = files[j].c_str();

        // cout<<" f = "<<f<<endl;

        // cout<<j<<" : "<<f.c_str()<<endl;

		
		Image * tmp = loadPpmFromFile(f);
		Image * vignette =  scaleColor(tmp, regions[0]);
		cout<<((int)(map(j,0,files.size(),0,100)*100.0))/100.0<<"%"<<endl;
		for(int i = 0 ; i < regions.size() ; i++){
			double tmPsnr = ressemblanceColor(vignette, regions[i]);
            //cout<<"tmPsnr : "<<tmPsnr<<endl;
			if(tmPsnr > regions[i]->bestPsnr){
				regions[i]->bestPsnr = tmPsnr;
				regions[i]->bestImg = f;
				//cout<<i<<" : "<<"bestPsnr : "<<tmPsnr<<endl;
			}
		}
		liberate(vignette);
		liberate(tmp);
	}
}

void replaceWithBestImgColor(vector<Region*> regions){
	for(int i = 0 ; i < regions.size() ; i++){

		Image * tmp = loadPpmFromFile((char *)regions[i]->bestImg.c_str());
		Image * vignette =  scaleColor(tmp, regions[i]);

		liberate(tmp);
		delete(regions[i]->tab);
		regions[i]->tab = vignette->tab;
	}
}

Image * mergeColor(vector<Region*> regs){
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
	allocation_tableau(res->tab, OCTET, sizeX*sizeY*3);


	for(int i = 0 ; i < regs.size() ; i ++){
		for(int y = 0 ; y < regs[i]->sizeY ; y++){
			for(int x = 0 ; x < regs[i]->sizeX ; x++){
                for(int c = 0; c < 3 ; c++){
                    res->tab[(
                        (y+regs[i]->startY)*sizeX + x + regs[i]->startX
                    )*3+c] = regs[i]->tab[(
                        y*regs[i]->sizeX + x)*3+c]
                        ;
                }
			}
		}
	}

	return res;
}

// vector<vector<string>> * importColorDatabase(){
// 	ifstream file("dataBase.dat", ios::in);

// 	vector<vector<string>> * res = new vector<vector<string>>(256);

// 	string tmpStr;
// 	int tmpInt;

// 	int size;
// 	file>>size;

// 	for(int i  = 0 ; i < size ; i++){
// 		file>>tmpInt>>tmpStr;
// 		res->at(tmpInt).push_back(tmpStr);
// 	}
// 	return res;
// }

// vector<vector<string>> * createColorDataBase(){
// 	vector<vector<string>> * res = new vector<vector<string>>(256);
// 	ofstream file("dataBase.dat", ios::out | ios::trunc);

// 	auto files = scanFolder("dataBase");
// 	file<<files.size()<<endl;
// 	for(int j = 0 ; j < files.size() ; j++){
// 		cout<<"Data-base generation : "<<((int)(map(j,0,files.size(),0,100)*100.0))/100.0<<"%"<<endl;
// 		auto f = files[j];
// 		Image * tmp = loadPgmFromFile((char *)f.c_str());
// 		int tmpAvg = round(avg(tmp));
// 		res->at(tmpAvg).push_back(f);
// 		file<<tmpAvg<<" "<<f<<endl;
// 		liberate(tmp);
// 	}

// 	return res;	
// }


// vector<vector<string>> * getColorDataBase(){
// 	ifstream fichier("dataBase.dat", ios::in);  // on ouvre en lecture
 
// 	if(fichier){  // si l'ouverture a fonctionné
// 		fichier.close();
// 		return importColorDatabase();		
// 	}
// 	else{
// 		return createColorDataBase();
// 	}

// 	return NULL;
// }