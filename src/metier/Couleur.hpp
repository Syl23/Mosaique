#ifndef __COULEUR_HPP__
#define __COULEUR_HPP__


double square(double x){
	return x*x;
}

class Couleur{
public:
	double r,g,b;
	bool color;

	Couleur(double r, double g, double b){
		this->color = true;
		this->r = r;
		this->g = g;
		this->b = b;
	}

	Couleur(double r){
		this->color = false;
		this->r = this->g = this->b = r;
	}

	Couleur(){
		this->color = false;
		this->r = this->g = this->b = 0;
	}

	~Couleur(){

	}

	void divide(double a){
		if(a != 0){
			if(this->color){
				this->r /=a;
				this->g /=a;
				this->b /=a;
			}else {
				this->r = this->g = this->b = this->r / a;
			}

		}
	}

	void add(const Couleur &c){
		if(this->color){
			this->r += c.r;
			this->g += c.g;
			this->b += c.b;
		} else {
			this->r = this->g = this->b = this->r + c.r;
		}

	}

	double dist(const Couleur &c){
		if(this->color){
			return square(this->r-c.r) + square(this->g-c.g) + square(this->b-c.b);
		} else {
			return square(this->r-c.r);
		}
	}
};
#endif