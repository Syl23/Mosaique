#ifndef __DATABASE_HPP__
#define __DATABASE_HPP__
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <filesystem>

#include "Image.hpp"

using namespace std;
namespace fs = std::filesystem;


class Database{
public:
	bool color;
	Database(bool color = false){
		this->color = color;
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
			Image * tmp = new Image((char *)f.c_str());
			if(color){
				//TODO
			} else {
				int tmpAvg = round(tmp->avg().r);
				res->at(tmpAvg).push_back(f);
				file<<tmpAvg<<" "<<f<<endl;
			}

			delete tmp;
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

	static vector<string> scanFolder(string path){
		std::cout << "Oui" << std::endl;
		vector<string> res = vector<string>();
		for(const auto & entry : fs::directory_iterator(path)){
			res.push_back(entry.path().string());
		}
		std::cout << "non ?" << std::endl;

		return res;
	}

};

#endif