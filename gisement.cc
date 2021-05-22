/*!
  \file   gisement.cc
  \author Salim Boussofara + Mohamed Ata Allah Krichene
  \date   Avril 2021
  \brief  Implémentation du module "Gisement"
*/
#include <cmath>
#include <iostream>
#include <sstream>
#include "gisement.h"
#include "constantes.h"
#include "message.h"
using namespace std;
using namespace message;

vector<Gisement*> Gisement::gisements;

Gisement::Gisement(double x,double y,double rayon,double capaciteg){
	infoGis={x,y,rayon,capaciteg,this};
}

Gisement::~Gisement(){
	//delete this;
}

bool Gisement::testIntersectionGisement(Gisement* candidatGisement){
	Cercle candidat=candidatGisement->infoGis.cercleGisement;
	for(size_t i(0);i<gisements.size();i++){
		Cercle cercle=gisements[i]->infoGis.cercleGisement;
		if(testIntersection(candidat, cercle))	
			return true;
	}
	return false;
}

bool Gisement::initGisement(string& line){
	istringstream data(line);
	double xg,yg,rayong,capaciteg;
	data>>xg>>yg>>rayong>>capaciteg;
	Gisement* candidatGisement=new Gisement(xg,yg,rayong,capaciteg);
	if(testIntersectionGisement(candidatGisement)){
		delete candidatGisement;
		candidatGisement=nullptr;
		return false;
	}
	gisements.push_back(candidatGisement);
	return true;
}

bool Gisement::testIntersectionBaseGisement(Cercle const& candidatBase){
	for(size_t i(0);i<gisements.size();i++){
		Cercle cercle=gisements[i]->infoGis.cercleGisement;
		if(testIntersection(candidatBase, cercle))	return true;
	}
	return false;
}

string Gisement::afficheGisements(){
	string ch=to_string(gisements.size())+"\n";
	for(auto& gisement: gisements){
		Cercle cercle=gisement->infoGis.cercleGisement;
		ch+="\t"+to_string(cercle.origine.x) +" "+ to_string(cercle.origine.y) +" "+
		   to_string(cercle.rayon) +" "+ to_string(gisement->infoGis.capaciteg)+"\n";
	}
	return ch;
}

void Gisement::drawGisements(){
	for(size_t i(0);i<gisements.size();i++)
		drawGisGeo(gisements[i]->infoGis.cercleGisement);
}

InfoGisement Gisement::intersectionGisRob(const P2D& pos){
	for(size_t i(0); i<gisements.size();i++)
		if(testAppartenance(pos,gisements[i]->infoGis.cercleGisement)){
			InfoGisement infoGisement={gisements[i]->infoGis.cercleGisement,
				gisements[i]->infoGis.capaciteg, gisements[i]};
			return infoGisement;
		}
	return {0,0,0};
}

void Gisement::deleteGisements(){
	for(size_t i (0); i<gisements.size(); ++i){
		delete gisements[i];
		gisements[i]=nullptr;
	}
	gisements.clear();
}

bool Gisement::extraction(){	
	if(infoGis.capaciteg<=deltaR) return false; 
	infoGis.capaciteg-=deltaR;
	return true;
}

InfoGisement Gisement::getInfoGis() const{
	return infoGis;
}
