/*!
  \file   base.cc
  \author Salim Boussofara + Mohamed Ata Allah Krichene
  \date   Avril 2021
  \brief  Implémentation du module "Base"
*/
#include <iostream>
#include <sstream>
#include <fstream>
#include "base.h"
#include "message.h"
#include "constantes.h"
using namespace std;
using namespace message;

unsigned int Base::nbComTot=0;

Base::Base(P2D pos, double ressources, unsigned int nbP, unsigned int  nbF, 
		   unsigned int  nbT, unsigned int  nbC): nbP(nbP), nbF(nbF), nbT(nbT), 
		   nbC(nbC),ressources(ressources){
	cercleBase={pos,rayon_base};
}

Base::~Base(){	
	for(size_t i(0); i<robots.size(); ++i){
		delete robots[i];
		robots[i]=nullptr;
	}
}

bool Base::testIntersectionBases(Base* candidatbase, vector<Base*> const& bases){
	Cercle candidat=candidatbase->cercleBase;
	for(size_t i(0);i<bases.size();i++){
		Cercle cercle=bases[i]->cercleBase;
		if(testIntersection(candidat,cercle))	return true;
	}
	return false;
}

Base* Base::initBase(ifstream& fichier, string& line, vector<Base*>const& bases){
	istringstream data(line);
	double x,y,ressources;
	unsigned int nbP, nbF ,nbT ,nbC;
	data>>x>>y>>ressources>>nbP>>nbF>>nbT>>nbC;
	Base* base=new Base({x,y},ressources, nbP, nbF, nbT, nbC);
	if(testIntersectionBases(base, bases) || 
				Gisement::testIntersectionBaseGisement(base->cercleBase)){
		delete base;
		return base = nullptr;
	}
	size_t size(nbP+nbT+nbF+nbC);
	for(size_t i(0); i<size; i++){
		do{
			getline(fichier >> ws,line);
		}while(line[0]=='#');		
		if(i<nbP){
			Robot* rob=Robot::initRobot(line, {x,y}, PROSPECTEUR, base->robots);
			if(rob!=nullptr) base->robots.push_back(rob);
			else{
				delete base;
				return base=nullptr;
			}
		}else if(i<nbP+nbF){
			Robot* rob=Robot::initRobot(line, {x,y}, FORAGE, base->robots);
			if(rob!=nullptr) base->robots.push_back(rob);
			else{
				delete base;
				return base=nullptr;
			}
		}else if(i<size-nbC){
			Robot* rob=Robot::initRobot(line, {x,y}, TRANSPORT, base->robots);
			if(rob!=nullptr) base->robots.push_back(rob);
			else{
				delete base;
				return base=nullptr;
			}
		}else{
			Robot* rob= Robot::initRobot(line,{x,y}, COMMUNICATION,base->robots);
			if(rob!=nullptr) base->robots.push_back(rob);
			else{
				delete base;
				return base=nullptr;
			}
		}
	}
	size_t i(size);
	P2D origineBase=base->cercleBase.origine;
	vector<Robot*> robots=base->robots;
	bool b(true);
	if(nbC>0){
		do{
			i--;
			b=((Communication*)(robots[i]))->testCom(origineBase);
		}while(!b and i>size-nbC);
	}		
	if(nbC==0 or !b){
		cout<<missing_robot_communication(origineBase.x,origineBase.y);
		delete base;
		return base=nullptr;
	}
	base->comCentral=(Communication*)(robots[i]);
cout<<2*deltaR/deltaD<<endl;;
	base->setDroiteComm();
	return base;
}

void Base::updateVoisin(Base* base1, Base* base2){
	if(base1==nullptr || base2==nullptr) return;
	vector<Robot*> robots1=base1->robots;
	vector<Robot*> robots2=base2->robots;
	for(size_t i(0); i<robots1.size();i++){
		for(size_t j(0); j<robots2.size();j++){
			if(robots1[i]==nullptr || robots2[j]==nullptr) return;
			if(robots2[j]!=robots1[i])
				robots1[i]->setLink(robots2[j]);
		}
	}
}

void Base::reset(){
	if(robots.empty())return; //ESSAYER SI CA MARCHE SANS
	connectedRobots.clear();
	forageSurMeilleurGisement.clear();
	for(size_t i (0); i< robots.size(); ++i){
		robots[i]->clearLink();
		robots[i]->setEtat(AUTONOMOUS);
	}	
}

unsigned int Base::getNbProspecteur()const{
	return nbP;
}
unsigned int Base::getNbForage()const{
	return nbF;
}
unsigned int Base::getNbTransport()const{
	return nbT;
}
unsigned int Base::getNbCommunication()const{
	return nbC;
}
double Base::getRessources() const{
	return ressources;
}

string Base::afficheBase()const{
	string ch="";
	ch+=to_string(cercleBase.origine.x) +" "+ to_string(cercleBase.origine.y) +" "+
		to_string(ressources) +" "+ to_string(nbP)+" "+ to_string(nbF)+" "+ 
		to_string(nbT)+" "+ to_string(nbC)+"\n";
	for(size_t i(0);i<robots.size();i++){
		if(robots[i]==nullptr) return "";
		ch+="\t\t"+robots[i]->afficheRobot()+"\n";
	}
	return ch;
}

void Base::drawBase(size_t j)const{
	drawCercleGeo(cercleBase,j);
	for(size_t i(0);i<robots.size();i++){
		if(robots[i]==nullptr) return;
		robots[i]->drawRobot(j);
	}
}

void Base::updateAutonomous(){
	for(size_t i(0) ;i<robots.size();++i)
		if(robots[i]->getEtat()==AUTONOMOUS)
			robots[i]->autonomous();
}

bool Base::gisementExploitable(Gisement* gis, size_t& compteurForage, size_t& compteurTransport, bool b){
	double compteurForageTot(0);
	double compteurTransportTot=0;
	//forageSurMeilleurGisement.clear();-->A REMETTRE Peut-etre
	//cout<<"DDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD"<<endl;
	compteurForage=0;
	compteurTransport=0;
		cout<<"gFGCGGGGGGGGGGGGGG"<<gis<<endl;

	if(gis==nullptr) return false;
	cout<<"gFGCGGGGGGGGGGGGGG"<<gis<<endl;
	for(auto& robot: connectedRobots){
		if(robot->gisementBut(gis)){
			if(robot->getType()==FORAGE){
				compteurForageTot++;
				if(testEgalite(robot->getPosBase(), cercleBase.origine)){
					compteurForage++;
					forageSurMeilleurGisement.push_back((Forage*)robot);
				}
			}
			if(robot->getType()==TRANSPORT){
				compteurTransportTot++;
				if(testEgalite(robot->getPosBase(),cercleBase.origine))
					compteurTransport++;
			}
		}
		if(compteurForage==2 && compteurTransport==10) return false;
	}
	if(!b){
		compteurForage=compteurForageTot-compteurForage;
		compteurTransport=compteurTransportTot-compteurTransport;
		if(compteurForage==4 || compteurTransport==20)
			return false;
	}
	//cout<<"------------------------------------------------- "<<compteurForage<<endl;
	return true;
}

//****
InfoGisement Base::findMeilleurGisement(double& ratio){
	ratio=200;//2*deltaR/deltaD;
	InfoGisement meilleurGisement;//={{{0,0},0},0,nullptr};
	//cherche meilleurGisement
	InfoGisement resultatRecherche;
	for(size_t i(0); i<connectedRobots.size(); i++){
		if(connectedRobots[i]->getType()==PROSPECTEUR)
			resultatRecherche=((Prospecteur*)connectedRobots[i])->getGisTrouve();
		if(connectedRobots[i]->getType()==FORAGE)
			resultatRecherche=((Forage*)connectedRobots[i])->getGisForage();
		Gisement* gisTrouve = resultatRecherche.gisPtr;
		Cercle cercleg = resultatRecherche.cercleGisement;
		size_t cf,ct;//A VERIFIER SI ON PEUT PASSER UN ARGUMENT PAR REFERENCE en étant par défaut
		double candidatRatio;
		if(gisTrouve==nullptr && !gisementExploitable(resultatRecherche.gisPtr,cf,ct,false))
			continue;
			
		Vecteur v; 
		double norm=abs(norme(connectedRobots[i]->getPos(), cercleg.origine, v));
		double capaciteg=resultatRecherche.capaciteg;
		cout<<"capapcacpapcpcaCAPACITE "<<capaciteg<<endl;
		if(capaciteg<deltaR) continue;////VERIFIER!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		candidatRatio=capaciteg*capaciteg/norm;
			cout<<"****************************************-------- "<<ratio<<endl;	

		cout<<"RRREEESSSS "<<resultatRecherche.capaciteg<<endl;	
		if(candidatRatio>=ratio){
			ratio=candidatRatio;
			meilleurGisement=resultatRecherche;
			cout<<"NNNEEWW"<<endl;
		}
	}
	cout<<"**************************************** "<<ratio<<endl;	
meilleurCapaciteg=meilleurGisement.capaciteg;
	cout<<"MEILLEUR RECHERCHE "<<meilleurCapaciteg<<endl;
	return meilleurGisement;
}

bool Base::inForageSurMeilleurGisement(Robot* rob){
	for(size_t i(0); i<forageSurMeilleurGisement.size(); i++){
		if(rob==forageSurMeilleurGisement[i])
			return true;
	}
	return false;
}

bool Base::forageACreer(InfoGisement meilleurGisement, double ratio){
	size_t size=forageSurMeilleurGisement.size();
	cout<<"-/-/-/-/-/-/-/- "<<size<<endl;
	if(size>=2) return false;
	cout<<"RRRARAARATTTUIOI "<<ratio<<endl;
	if(size==1 and ratio<800) return false;
	vector<Forage*> connectedForagesPossibles;
	cout<<"SSSSSSSSSSSSSSSSSSSIIIIIIIIIIIIIIIIIIIIIIIEEEEEEEEEEEEE "<<connectedRobots.size()<<endl;
	for(size_t i(0); i<connectedRobots.size(); i++)	{
		//cout<<"TTTTYYYPPPEE "<<connectedRobots[i]->getType()<<endl;
		if(connectedRobots[i]->getType()==FORAGE){
			cout<<" SSSSUUUURRGGGIIISSS "<<inForageSurMeilleurGisement(connectedRobots[i])<<endl;
			//if(!inForageSurMeilleurGisement(connectedRobots[i])){ //////////!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! A VERIFIER
				cout<<" POSSIIBBLLE"<<connectedRobots[i]->deplacementPossible(meilleurGisement.cercleGisement.origine)<<endl;
				//if(connectedRobots[i]->deplacementPossible(meilleurGisement.cercleGisement.origine))
					if(testEgalite(connectedRobots[i]->getPosBase(),cercleBase.origine))//CETTE CHOSE SE REPETE DANS D'AUTRES FONCTIONS PEUT-ETRE EN FAIRE UNE FONCTION DANS ROBOT
						connectedForagesPossibles.push_back((Forage*)connectedRobots[i]);
			//}
		}	
	}
//		cout<<"KKKKAAAALLLLBBB"<<endl;
		if(connectedForagesPossibles.empty()){//  (!inForageSurMeilleurGisement(connectedRobots[i]))){
			cout<<"EMPTYyyyyyyyyyyyyyyyyyyyyyyyyy"<<endl;
			return true;
		}
		Forage* plusProcheForage=connectedForagesPossibles[0];
		Vecteur v, vCompare;
		P2D gis=meilleurGisement.cercleGisement.origine;
		double norm=norme(plusProcheForage->getPos(),gis,v);
		for(size_t i(1);i<connectedForagesPossibles.size();i++){
			double n=norme(connectedForagesPossibles[i]->getPos(),gis,vCompare);
			if(n<norm){
				v=vCompare;
				norm=n;
				plusProcheForage=connectedForagesPossibles[i];
			}
		}
		((Forage*)plusProcheForage)->setGisForage(meilleurGisement);		
		if(norm>norme(cercleBase.origine,gis,vCompare) && ressources>cost_forage+cost_transp){
			cout<<"TTTTTTTTTTTTTTTTTTTTTTRRRRRRRRRRRRRUUUUUEEEEEEEEEEE"<<endl;
			return true;
		}

		double angle;
		if(v.premier==0) angle=M_PI/2;
		else angle=abs(atan(v.second/v.premier));
		double butX=(plusProcheForage->getPos().x)+(norm-meilleurGisement.cercleGisement.rayon)*cos(angle)*v.premier;
		double butY=(plusProcheForage->getPos().y)+(norm-meilleurGisement.cercleGisement.rayon)*sin(angle)*v.second;
		cout<<"BUT "<<butX<<" "<<butY<<endl;
		plusProcheForage->setBut({butX, butY});
		forageSurMeilleurGisement.push_back(plusProcheForage);
		cout<<"POS "<<plusProcheForage->getPos().x<<" "<<plusProcheForage->getPos().y<<endl;
		return false;
}


int Base::envoiTransport(vector<Transport*> connectedTransports, const P2D& forage){
	cout<<"debut envoiTransport"<<endl;
			//cout<<"KKKHHRRARRARARA"<<endl;
	if(connectedTransports.empty()) return -1;
	cout<<"*///////////////////////////////////////////////////////////////"<<endl;
	Transport* plusProcheTransport=connectedTransports[0];
	for(size_t i(0); i<connectedTransports.size(); ++i){
		cout<<connectedTransports[i]->afficheRobot()<<endl;
		}
	cout<<"*///////////////////////////////////////////////////////////////"<<endl;	
//	cout<<"KKKHHRRARRARARA"<<endl;
	Vecteur v,vCompare;
	size_t indiceRetour=0;
	double norm=norme(plusProcheTransport->getPos(), forage,v);
	for(size_t i(0);i<connectedTransports.size();i++){  //******METTRE TOUTE CETTE PARTIE DANS UNE FONCTION SEPAREE PARCE QUE CA SE REPETE POUR LE FORAGE
		if(!testEgalite(connectedTransports[i]->getPos(), forage)){
			double n=norme(connectedTransports[i]->getPos(), forage,  vCompare);
						cout<<"*-*-*-*-*-*-*"<<vCompare.premier<<"  "<<vCompare.second;
			
			if(n<norm){
				norm=n;
				v=vCompare;
				
				plusProcheTransport=connectedTransports[i];
				indiceRetour=i;
			}
		}else{
			return -1;
		}
}
	norm=norme(forage,plusProcheTransport->getPos(),v);

	/*if(norm>norme(cercleBase.origine,forage,vCompare)){
		return -1;
	}*/
	cout<<"VVVVVVVVVVVVVVVVVVVVVVVVVVVV"<<v.premier<<"  "<<v.second;
	double butX=plusProcheTransport->getPos().x-norm*v.premier;
	double butY=plusProcheTransport->getPos().y-norm*v.second;
	plusProcheTransport->setBut({butX, butY});
	plusProcheTransport->setAttributs(false,false,false);
	//cout<<"BUT TTRRANSPSOSOPtrt "<<butX<<" "<<butY<<endl;
	//cout<<"BUT TTRRANSPSOSOPtrt26656556 "<<plusProcheTransport<<endl;
	return indiceRetour;
};

void Base::remoteTransport(){
	size_t size=forageSurMeilleurGisement.size();
	//cout<<"KAKKKAKAKLLLBB SIIISISII"<<forageSurMeilleurGisement.size()<<endl;
	if(size==0) return;
	vector<Transport*> connectedTransports;
	for(size_t i(0); i<connectedRobots.size(); i++)	
		if(connectedRobots[i]->getType()==TRANSPORT)
			if(testEgalite((connectedRobots[i]->getPosBase()),cercleBase.origine))
				if(!((Transport*)connectedRobots[i])->getRetour()){
				if(connectedRobots[i]->deplacementPossible(meilleurGisement.cercleGisement.origine))//C'EST PAS LE VRAI BUT MAIS JE PENSE QUE CA PASSE
					connectedTransports.push_back((Transport*)connectedRobots[i]);
				}
	int compteur=meilleurCapaciteg/(deltaR);//TROUVER LA BONNE FORMULE!!!!!!!
	//while(!(connectedTransports.empty()) and compteur>0 and meilleurCapaciteg>deltaR){
	if(!(connectedTransports.empty())){
		cout<<"RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRR"<<endl;
		size_t i(0);
		if(size==2){
			if(compteur%2==0)
				i=envoiTransport(connectedTransports, forageSurMeilleurGisement[0]->getBut());
			else i=envoiTransport(connectedTransports, forageSurMeilleurGisement[1]->getBut());
		}else i=envoiTransport(connectedTransports, forageSurMeilleurGisement[0]->getBut());
		//if(i==-1) continue;
		//swap(connectedTransports[i], connectedTransports.back());
		connectedTransports.pop_back();
		compteur--;
		
	}

}

void Base::updateRemote(){
	//bool chercheGisement=false;
	/*for(size_t i(0);i<connectedRobots.size();i++)	
		if(connectedRobots[i]->getType()==FORAGE || connectedRobots[i]->getType()==PROSPECTEUR){
			chercheGisement=true;
			break;
		}
	cout<<"HHHHRRRRRAAAAMMMM"<<endl;
	
	if(chercheGisement){*/
		//size_t compteurForage(0);
		//size_t compteurTransport(0);
		/*double ratio;
		InfoGisement meilleurGisement=findMeilleurGisement(ratio);*/
		/*cout<<"Meilleur "<<meilleurGisement.gisPtr<<endl;
		if(meilleurGisement.gisPtr!=nullptr){	
			cout<<"LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLUEJE"<<endl;
			cout<<" MeilleurGGIIIS 		"<<meilleurGisement.cercleGisement.origine.x<<"  "<<meilleurGisement.cercleGisement.origine.y<<endl;
	*/		//if(compteurTransport<=10)
				cout<<"remoteeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee"<<endl;
				remoteTransport();	
			//AU LIEU DE PASSER FORAGESURGISEMENTR PAR RÉFÉRENCE, PEUT-ETRE LE METTRE EN VARIABLE STATIQUE DE LA BASE
		//}
	//}
//	cout<<"HHHHRRRRRAAAAMMMM"<<endl;
	for(size_t i(0);i<connectedRobots.size();i++)	
		if(testEgalite((connectedRobots[i]->getPosBase()),cercleBase.origine))
			if(connectedRobots[i]->getEtat()==REMOTE){
				connectedRobots[i]->remote();
				//cout<<"PPPOOPOSSS "<<connectedRobots[i]->getType()<<"   "<<connectedRobots[i]->getEtat()<<endl;
			}
}


void Base::recDFS(vector<Robot*>& result, Robot* next){
	next->setVisited(true);
	if(testEgalite(next->getPosBase(),cercleBase.origine))
		next->setEtat(REMOTE);	
	result.push_back(next);
	for(size_t i(0);i<next->getLink().size();i++){
		if(next->getLink()[i]==nullptr)continue;
		if(!(next->getLink()[i]->getVisited())){
			recDFS(result,next->getLink()[i]);
		}
	}
}

void Base::connexion(){
	for(size_t i(0);i<robots.size();++i){
		if(robots[i]==nullptr) continue;//+
		robots[i]->setVisited(false);
//		robots[i]->setEtat(AUTONOMOUS);
	}	
	recDFS(connectedRobots,comCentral);
}

void Base::maintenance(){
	vector<Robot*> link=comCentral->getLink();
	for(size_t i(0);i<link.size();i++){
		//if(link[i]==nullptr) continue;
		if(link[i]==comCentral) continue;
		if(testEgalite(link[i]->getPosBase(),cercleBase.origine))
			if(testAppartenance(link[i]->getPos(),cercleBase)){
					if(link[i]->getType()==TRANSPORT)
						if(((Transport*)link[i])->getCharge()){
							//(Transport*)link[i]->setAttributs(
							ressources+=deltaR;
						}
							//	cout<<"RRRRRRRRRREEEEEEEEEEEESSSSSSSSSOOOOOOOOOUUUUUURRRRCCCCCCCEEEEEEEEEEE "<<ressources<<endl;
				ressources-=link[i]->maintenance();
			//	cout<<"RRRRRRRRRREEEEEEEEEEEESSSSSSSSSOOOOOOOOOUUUUUURRRRCCCCCCCEEEEEEEEEEE "<<ressources<<endl;
			}	
	}
}

bool Base::missionAccomplie()const{
	if(ressources>finR) return true;
	return false;
}

void Base::drawLinkBase(){
	for(size_t i(0); i<robots.size();i++){
		if(robots[i]==nullptr) continue;
		robots[i]->drawLinkRobots();
	}
}

void Base::drawCommBase(){
	for(size_t i(0); i<robots.size();i++){
		if(robots[i]==nullptr) continue;
		if(robots[i]->getType() != COMMUNICATION) continue;
		((Communication*)(robots[i]))->drawCercleCom();
	}
}
///////////////////////CREATION/////////////////////////////////
void Base::creation(){
	++updateCreation;
	bool forageACreer=chercheForage_Gisement();
	cout<<forageACreer<<"FFFFFFEFFEFOEFOEFOFOEFO"<<endl;
	unsigned int robots_rest=max_robots;
	if(ressources<=2*(cost_forage+cost_com+cost_prosp)) return;
	if(updateCreation<4){
		for(unsigned int i(0);i<max_robots;++i)
			if(ressources>=cost_prosp+2*cost_forage) 
				creationProsp(robots_rest);	
		return;		
		}
	if(updateCreation==4){
			if(ressources>=cost_transp + cost_forage and forageACreer)
				creationForageTransport(robots_rest);
		if(ressources>=cost_com and nbComTot<200)
			creationCommunication(robots_rest);
		return;		
	}
	
	if(updateCreation==5){
		if(ressources>=cost_transp)
				creationForageTransport(robots_rest,false);
		if(ressources>=cost_com and nbComTot<200)
			creationCommunication(robots_rest);		
	}		
	if(forageACreer)
		if(ressources>=cost_forage+cost_transp)
			creationForageTransport(robots_rest);

	if(nbC<50 and Base::nbComTot<200)
		if(ressources>=cost_com)
			creationCommunication(robots_rest);
	if(nbP<16)
		if(ressources>=cost_prosp) 
			creationProsp(robots_rest);	
	
	if(ressources>3*cost_transp)
		creationForageTransport(robots_rest, false);
}

///////////////////CREATION PROSPECTEUR//////////////////////////////


void Base::creationProsp(unsigned int& i){
	if(i==0)return;
	--i;
	ressources-=cost_prosp;
	Prospecteur* prospecteur;
	unsigned int uid=genererUid();
	P2D butProspecteur= prospecteur->marcheProspecteur(cercleBase.origine);
	prospecteur= new Prospecteur(uid,0,cercleBase.origine,butProspecteur,cercleBase.origine,false,false,false);
	++nbP;
	robots.push_back(prospecteur);
}
	
	
///////////////////CREATION FORAGE & TRANSPORT////////////////////////

bool Base::chercheForage_Gisement(){
//	cout<<"++++++++++++++++++++++++++++++++++++++"<<endl;
	bool chercheGisement=false;
	for(size_t i(0);i<connectedRobots.size();i++)	
		if(connectedRobots[i]->getType()==FORAGE || connectedRobots[i]->getType()==PROSPECTEUR){
			chercheGisement=true;
			break;
		}	
	if(chercheGisement){
		size_t compteurForage(0),compteurTransport;
		double ratio;
		InfoGisement meilleurGisement=findMeilleurGisement(ratio);
		cout<<" TTTYTTY55555555 "<<forageACreer(meilleurGisement, ratio);
		return meilleurCapaciteg>deltaR and forageACreer(meilleurGisement, ratio);
	}
}

void Base::creationForageTransport(unsigned int& i,bool b){
	Forage* newForage=nullptr;
	cout<<"FFFOOOOOOOOOOOOOOOORRRRRRRRAAAAAAAAAGGGGEEEEEEEE"<<endl;
	if(i==0)return;
	if(b && ressources>=cost_forage+cost_transp){	
			unsigned int uidf=genererUid();
			++nbF;
			Vecteur v;
			P2D pos = cercleBase.origine;
			double norm=norme(pos, meilleurGisement.cercleGisement.origine,v);
			double butX=(pos.x)+(norm-meilleurGisement.cercleGisement.rayon)*v.premier;
			double butY=(pos.y)+(norm-meilleurGisement.cercleGisement.rayon)*v.second;
			newForage = new Forage(uidf,0,pos,{butX,butY},cercleBase.origine,false);
			forageSurMeilleurGisement.push_back(newForage);
			
			robots.push_back(newForage);
			ressources-=cost_forage;
			i--;
	}
	unsigned int uidt=genererUid();
	++nbT;
	P2D but;
	if(!b){
		robots.push_back(new Transport(uidt,0,cercleBase.origine, cercleBase.origine, cercleBase.origine,false,false));	
		i--;
		ressources-=cost_transp;
	}else if(newForage!=nullptr){
		robots.push_back(new Transport(uidt,0,cercleBase.origine, but, cercleBase.origine,false,false));	
		i--;
		ressources-=cost_transp;
			cout<<"FIN CREATION FORAGE"<<endl;
	}
}



///////////////////CREATION COMMUNICATION/////////////////////////////

/**************************SUIVI***************************************/
/*
void Base::creationCommunication(unsigned int& i,Robot* robot){
	if(i==0)return;
	--i;
	ressources-=cost_com; 
	P2D but= robot->getPos();
	Communication* communication;
	unsigned int uid=genererUid();
	communication= new Communication(uid,0,cercleBase.origine,but,cercleBase.origine,false);
	++nbC;
	robots.push_back(communication);
}
*/
/**************************FRACTAL*************************************/

/*void Base::fractal(P2D a,P2D b,int times){
	--times;
	if(times==0) return;
	P2D c,d,e;
	c.x = b.x -(a.y-b.y);
	c.y = b.y -(b.x-a.x);
	d.x = a.x -(a.y - b.y);
	d.y = a.y -(b.x - a.x);
	e.x = d.x +(b.x-a.x-(a.y-b.y))/2;
	e.y = d.y -(b.x- a.x + a.y - b.y)/2;
		fracDroite.push_back(c);
		fracGauche.push_back(e);		
		fractal(d,e,times);
		fractal(e,c,times);
}*/

void Base::setDroiteComm(){
	P2D p1,p2;
	p1=p2=cercleBase.origine;
	cout<<"COMM"<<rayon_comm<<endl;
	cout<<dim_max<<endl;
	double angle=atan((rayon_comm-35)/(2*dim_max));
	cout<<"angle"<<angle<<endl;
	size_t nb=(2*dim_max)/(sin(angle)*(rayon_comm-35));
	cout<<"NB "<<nb<<endl;
	for(size_t i(0); i<2*nb; i++){
		//if(i%2==0){
			p1.x+=cos(angle)*(rayon_comm-35);
			p1.y+=sin(abs(angle))*(rayon_comm-35);
			cout<<"P1"<<p1.x<<" "<<p1.y<<endl;
			p1=normalisation(p1);
			droiteComm.push_back(p1);
		/*}else{
			p2.x-=cos(angle)*(rayon_comm-10);
			p2.y-=sin(abs(angle))*(rayon_comm-10);
			cout<<"p2"<<p2.x<<" "<<p2.y<<endl;
			p2=normalisation(p2);
			droiteComm.push_back(p2);
		}*/
	}
}

void Base::creationCommunication(unsigned int& i){
	if(i==0)return;
	P2D but;
	Communication* communication;
	unsigned int uid=genererUid();
	if(droiteComm.empty()) return;
	if((compteurComm % 2)==0) but=droiteComm.back();
	else but=droiteComm.back();
	droiteComm.pop_back();	
	cout<<"BBUBUBUBT"<<but.x<<"  "<<but.y<<endl;
	communication = new Communication(uid,0,cercleBase.origine,but,cercleBase.origine,false);
	--i;
	++nbC;
	++nbComTot;
	compteurComm++;
	ressources-=cost_com;
	robots.push_back(communication);
}
///////////////////GENERER RANDOM UID/////////////////////////////
unsigned int Base::genererUid(){
//BOUCLE POUR VERIFIER UID
	int candidatUid(-1);
	do{
		++candidatUid;
	}while(Robot::testUid(candidatUid,robots));
	cout<<"uid  "<<candidatUid<<endl;
	return candidatUid;
//FIN VERIFICATION UID
}
//FIN CREACTION ROBOTS


/*bool Base::deleteBase(){
	if(equal_zero(ressources)) return true;
	return false;	
}*/
