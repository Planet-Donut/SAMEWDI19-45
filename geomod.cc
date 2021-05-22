/*!
  \file   geomod.cc
  \author Salim Boussofara + Mohamed Ata Allah Krichene
  \date   Mars 2021
  \brief  Implémentation du module "geomod"
*/
#include <cmath>
#include <iostream>
#include "geomod.h"
#include "constantes.h"

using namespace std;

namespace{
	double	epsilon_zero;
	double maximum;
	
}

bool equal_zero(double val){
		if (abs(val)< epsilon_zero) return true;
		return false;
	}
//2.2
double getEpsilon(){
	return epsilon_zero;
}

double getMax(){
	return maximum;
}

void setMax(double x){
	maximum = abs(x);
	epsilon_zero= maximum * pow(10,-10) ;
}

//2.4
double normalisation(double c, double r){	
	if((c>=r-maximum) and (c<=maximum-r)) return c;
	if(c<r-maximum) return c+2*maximum;
	return c-2*maximum;
}

P2D normalisation(const P2D& p, double r){
	P2D point1={normalisation(p.x, r),normalisation(p.y, r)};
	return point1;
}

//2.5
double norme(const P2D& p1,const P2D& p2, Vecteur& v){
	double x(p1.x);
	double y(p1.y);
	double x_min(p2.x-2*maximum);
	double y_min=(p2.y-2*maximum);
	double norme(sqrt((x-x_min)*(x-x_min)+(y-y_min)*(y-y_min)));
	for(int i(-1);i<2;i++){
		for(int j(-1);j<2;j++){
			double x_eq(p2.x+2*i*maximum);
			double y_eq(p2.y+2*j*maximum);
			double n=sqrt((x-x_eq)*(x-x_eq)+(y-y_eq)*(y-y_eq));
			if(n<=norme){
				norme=n;
				x_min=x_eq;
				y_min=y_eq;
			}
		}
	}
	if(norme==0) v={0,0,0};
	v={(x_min-x)/norme,(y_min-y)/norme,norme};
	return abs(norme);
}	

//2.6
bool testEgalite(const P2D& p1, const P2D& p2){
	Vecteur v;
	return equal_zero(norme(p1,p2,v));
}

//2.7
bool testAppartenance(const P2D& p,const Cercle& c){
	Vecteur v;
	return norme(p,c.origine,v)<=(c.rayon-epsilon_zero);//BOULIC NE VA PAS AIMER
}

//2.8
bool testIntersection(const Cercle& c1, const Cercle& c2){
	Vecteur v;
	double n(norme(c1.origine,c2.origine,v));
	return n<c1.rayon+c2.rayon-epsilon_zero;
}

vector<P2D> rebouclement(Cercle c){
	vector<P2D> vectP2D;
	P2D p=c.origine;
	vectP2D.push_back({p.x,p.y});
	bool rebouclementCoin=false;
	if(p.x+c.rayon>maximum || p.x-c.rayon<-maximum){
		P2D p1={normalisation(p.x,c.rayon),p.y};
		vectP2D.push_back({p1.x, p1.y});
		rebouclementCoin=true;
	}
	if(p.y+ c.rayon>maximum || p.y-c.rayon<-maximum){
		P2D p1={p.x,normalisation(p.y,c.rayon)};;
		vectP2D.push_back({p1.x, p1.y});
		if(rebouclementCoin)
			vectP2D.push_back(normalisation(p,c.rayon));
	}
	return vectP2D;
}

void drawCercleGeo(Cercle c, int j){//BASE + RAYON_COMM
	vector<P2D> vect = rebouclement(c);
	for(size_t i(0);i<vect.size();i++){
		if(j==-1)
			drawCercleGraph(vect[i].x,vect[i].y,c.rayon);
		drawCercleGraph(vect[i].x,vect[i].y,c.rayon*30,j);//NE PAS LAISSER LE RAYON COMME CA, C'EST JUSTE POUR VISUALISER
	}
}

void drawGisGeo(Cercle c){
	vector<P2D> vect = rebouclement(c);
	for(size_t i(0);i<vect.size();i++)
		drawGisGraph(vect[i].x,vect[i].y,c.rayon);
}

void drawProspGeo(P2D p, unsigned int i){
	normalisation(p);//à changer le rayon
	drawTriangle(p.x, p.y, i);//A CHANGER
}

void drawTransGeo(P2D p, unsigned int i){
	normalisation(p);//à changer le rayon
	drawEtoile(p.x, p.y, i);//A CHANGER
}

void drawForageGeo(P2D p, unsigned int i){
	normalisation(p);//à changer le rayon
	drawCroix(p.x, p.y, i);//A CHANGER
}

void drawCommGeo(P2D p, unsigned int i){
	normalisation(p);
	drawCommGraph(p.x, p.y, i);//A CHANGER
}
	

