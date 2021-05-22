/*!
  \file   simulation.cc
  \author Salim Boussofara + Mohamed Ata Allah Krichene
  \date   Avril 2021
  \brief  Implémentation du module "Simulation"
*/
#include <iostream>
#include <fstream>
#include <sstream>
#include "simulation.h"
#include "message.h"
using namespace std;

Simulation::~Simulation(){
	for(size_t i(0); i<bases.size(); i++){
		delete bases[i];
		bases[i]=nullptr;
	}
	Gisement::deleteGisements();
}	

bool Simulation::lecture(string nomFichier) {     
	string line;     
	ifstream fichier(nomFichier);  
	string elem="gisement";    
	if(!fichier.fail()){     
		while(getline(fichier >> ws,line)){    
			if(line[0]=='#')  continue; 
			if(elem=="gisement"){
				if(!decodageGisement(fichier, line)) return false; 
				elem="base";
			}
			else if(!decodageBase(fichier, line)) return false;    
		}  
		cout << message::success();    
	}    
	return true;
} 

void Simulation::saveState(const string path){
	ofstream file(path); //open in constructor
    string data(afficheSimulation());
    file << data;
}

bool Simulation::decodageGisement(ifstream& fichier, string& line){  
	istringstream data(line);
	static unsigned int nbG(0);
	data >> nbG;
	for(size_t i(0); i<nbG; i++){
		do{
			getline(fichier >> ws,line);
		}while(line[0]=='#');
		if(!(Gisement::initGisement(line))) return false;
	}
	return true;
}

bool Simulation::decodageBase(ifstream& fichier, string& line){
	istringstream data(line);
	static unsigned int nbB(0);
	data >> nbB;
	for(size_t i(0); i<nbB; i++){
		do{
			getline(fichier >> ws,line);
		}while(line[0]=='#');
		Base* base=Base::initBase(fichier,line, bases);
		if(base==nullptr) return false;
		bases.push_back(base);
	}
	return true;
}

vector<Base*> Simulation::getBases()const{return bases;}

string Simulation::afficheSimulation()const{
	string ch="";
	ch+=Gisement::afficheGisements();
	ch+=to_string(bases.size())+"\n";
	for(size_t i(0);i<bases.size();i++)
		if(bases[i] != nullptr)
			ch+="\t"+bases[i]->afficheBase()+"\n";
	return ch;
}

void Simulation::drawSimulation()const{
	Gisement::drawGisements();
	for(size_t i(0);i<bases.size();i++)
		if(bases[i]!=nullptr) bases[i]->drawBase(i);
}

void Simulation::run(){
	size_t size=bases.size();
	for(size_t i(0);i<size;i++)
		if(bases[i]!=nullptr) bases[i]->reset();	
	for(size_t i(0);i<size;i++){
		if(bases[i]==nullptr) continue;
		for(size_t j(0); j<size; j++){
			if(bases[j]==nullptr) continue;
			Base::updateVoisin(bases[i],bases[j]);
		}
	}	
	for(size_t i(0);i<size;i++){
		if(bases[i]==nullptr) continue;
		if(bases[i]->missionAccomplie()) continue;
		bases[i]->connexion();
		bases[i]->maintenance();
		bases[i]->creation();
		bases[i]->updateAutonomous();
		bases[i]->updateRemote();
		if(bases[i]->getRessources()<0.005){//A VOIR
			delete bases[i];
			bases[i]= nullptr; 
		}  
	}
} 
void Simulation::drawLinkSimulation(){
	for(size_t i(0);i<bases.size();i++)
		if(bases[i]!=nullptr) bases[i]->drawLinkBase();	
}

void Simulation::drawCommSimulation(){
	for(size_t i(0);i<bases.size();i++)
		if(bases[i]!=nullptr) bases[i]->drawCommBase();					
}

