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






// color varColor(Region * in){
// 	color res;

//     res.r=0;
//     res.g=0;
//     res.b=0;

// 	for(int i = 0 ; i < in->sizeX*in->sizeY ; i++){
//         res.r += in->tab[i*3+0];
//         res.g += in->tab[i*3+1];
//         res.b += in->tab[i*3+2];
// 	}

//     res.r = res.r/(in->sizeX*in->sizeY);
//     res.g = res.g/(in->sizeX*in->sizeY);
//     res.b = res.b/(in->sizeX*in->sizeY);

// 	return res;
// }


// double equartTypeColor(Region * reg){
// 	reg->avgColor = avgColor(reg);

// 	double variance = 0.0;

// 	for(int i = 0 ; i < reg->sizeX*reg->sizeY ; i ++){
// 		variance += colorSqDist(color(reg->avgColor,color(reg->tab[i*3],reg->tab[i*3+1],reg->tab[i*3+2])));
// 	}
// 	return sqrt(variance);
// }

// vector<Region*> splitColorWithUnevenZones(Image * img, double ecartType){//erase(i)
// 	vector<Region*> res;
// 	res.push_back(Region());
// 	allocation_tableau(res[0]->tab, OCTET, img->sizeX*img->sizeY*3);
// 	for(int i = 0 ; i < img->sizeX*img->sizeY*3 ; i ++){
// 		res[0] = img->tab[i];
// 	}
// 	//res[0]->avgColor   = avgColor(res[0]);
// 	res[0]->equartType = equartTypeColor(res[0]); 


// 	vector<Region*> remaining;
// 	remaining.push_back(res[0]);

// 	while(remaining.size()!=0){
// 		Region *current = remaining.back();
// 		remaining.pop_back();


// 	}
// }