/*!
  \file   robot.cc
  \author Salim Boussofara + Mohamed Ata Allah Krichene
  \date   Avril 2021
  \brief  Implémentation du module "Robot"
*/
#include <iostream>
#include <fstream>
#include <sstream>
#include "robot.h"
#include "message.h"
#include "constantes.h"

using namespace std;
using namespace message;

Robot::Robot(unsigned int uid, double dp, P2D pos, P2D but, P2D posBase,bool atteint):
				uid(uid),dp(dp),pos(pos),but(but),posBase(posBase),atteint(atteint){}

Robot::~Robot(){}		

bool Robot::testUid(unsigned int uid, vector<Robot*>const& robots){
	for(size_t i(0); i<robots.size(); i++)
		if(uid==robots[i]->uid)	return true;
	return false;
}

static bool strToBool(string s){
	if(s=="true")  return true;
	return false;
}

Robot* Robot::initRobot(string const& line, const P2D& posBase, Type_robot type,
						vector<Robot*>const& robots){							
	istringstream data(line);
	unsigned int uid;
	double dp, x, y;
	double xb, yb, xg, yg, rayong, capaciteg;
	bool atteint, retour, found;
	string a, r, f;
	data>>uid>>dp>>x>>y>>xb>>yb>>a;
	atteint=strToBool(a);
	Robot* rob = nullptr;
	switch(type){
		case PROSPECTEUR:
			data>>r>>f;
			retour=strToBool(r);
			found=strToBool(f);
			if(found) data>>xg>>yg>>rayong>>capaciteg;
			rob = new Prospecteur(uid, dp, {x,y}, {xb,yb}, posBase, atteint, retour,
									found,{{xg,yg},rayong}, capaciteg);
			break;
		case FORAGE:
			rob = new Forage(uid, dp, {x,y}, {xb,yb}, posBase, atteint);
			break;
		case TRANSPORT: 
			data>>r;
			retour=strToBool(r);
			rob = new Transport(uid, dp, {x,y}, {xb,yb}, posBase, atteint, retour);
			break;
		case COMMUNICATION:
			rob = new Communication(uid, dp, {x,y}, {xb,yb}, posBase, atteint);
			break;    
		default: break;	
	}
	if(testUid(uid,robots)){
		delete rob;
		rob=nullptr;
	}
	return rob;
}

void Robot::setLink(Robot* robot){
	vector<P2D> centres=rebouclement({pos,rayon_comm});
	for(size_t i(0); i<centres.size();i++)
		if(testAppartenance(robot->pos,{centres[i],rayon_comm})){
			link.push_back(robot);
			return;
		}
} 	

void Robot::clearLink(){
	link.clear();
}

string Robot::afficheRobot()const{
	return to_string(uid)+" "+to_string(dp)+" "+to_string(pos.x)+" "+to_string(pos.y)+
		   " "+to_string(but.x)+" "+to_string(but.y)+" "+to_string(atteint);
}

	
vector<Robot*> Robot::transportsPossibles(){
	vector<Robot*> vectTransport;
	for(size_t i(0); i<link.size(); i++)
		if(link[i]->type==TRANSPORT)
			if(!(((Transport*)link[i])->getCharge()))
				if(testEgalite(link[i]->posBase,posBase))
					if(testAppartenance(link[i]->pos,{pos,20}))
						vectTransport.push_back(link[i]);
	return vectTransport;
} 

Etat Robot::getEtat()const{
	return etat;
}	

void Robot::setEtat(Etat e){
	etat=e;
}

vector<Robot*> Robot::getLink()const{
	return link;
}

void Robot::setVisited(bool v){
	visited=v;
}

bool Robot::getVisited()const{
	return visited;
}	

Type_robot Robot::getType()const{
	return type;
}
P2D Robot::getPos()const{
	return pos;
}

P2D Robot::getPosBase()const{
	return posBase;
}

void Robot::setBut(const P2D& newBut){
	but=newBut;
}
P2D Robot::getBut()const{
	return but;
}

void Robot::avance(){
	Vecteur v;
	if(norme(pos, but, v)<=deltaD){ 
		dp+=v.norme;
		pos.x=but.x;
		pos.y=but.y;
		return;
	}
	if(v.premier==0){
		dp+=deltaD;
		pos.y+=deltaD*v.second;
		return;
	}
	double angle=atan(v.second/v.premier);
	pos.x+=deltaD*cos(angle)*v.premier;
	pos.y+=deltaD*abs(sin(angle))*v.second;
	dp+=deltaD;
}

bool Robot::mouvement(){
	if(!atteint){				
		avance();
		pos=normalisation(pos);
		if(testEgalite(pos, but))
			atteint=true;
		return true;
	} 
	return false;
}

bool Robot::gisementBut(Gisement* gis){
	InfoGisement info=gis->getInfoGis();
	return testAppartenance(but, info.cercleGisement) and info.capaciteg>deltaR;
}		

double Robot::maintenance(){
	double c=cost_repair*dp;
	if(atteint){	
		dp=0;
		atteint=false;
		return c;
	}
	return 0;
}

	
Prospecteur::Prospecteur(unsigned int uid, double dp, P2D pos, P2D but, P2D posBase,
						 bool atteint, bool retour,bool found,Cercle gisement,
						 double capaciteg):Robot(uid, dp, pos, but, posBase, atteint)
						 ,retour(retour),found(found){
	type=PROSPECTEUR;
	if(found){
		gisement=gisement;
		capaciteg=capaciteg;
	}
}

Prospecteur::~Prospecteur(){
	//delete this;
}
string Prospecteur::afficheRobot()const{
	Cercle gisement=gisTrouve.cercleGisement;
	return Robot::afficheRobot()+" "+to_string(retour)+" "+to_string(found)+" "+
					to_string(gisement.origine.x) +" "+to_string(gisement.origine.y)
					+" "+to_string(gisement.rayon)+" "+to_string(gisTrouve.capaciteg);
}

bool Prospecteur::deplacementPossible(const P2D& candidatBut)const{
	Vecteur v;
	return (maxD_prosp-dp)>norme(pos,candidatBut,v)+norme(posBase,but,v);
}

void Prospecteur::stockeInfoGisement_Redirige(){
	P2D pb;
	if(!found){
		InfoGisement infoGis=Gisement::intersectionGisRob(pos);
		if(infoGis.gisPtr==nullptr){
			pb=marcheProspecteur(pos);
		}else{
			gisTrouve=infoGis;
			found=true;
			retour=true;
			pb=posBase;
		}
	}
	if(deplacementPossible(pb)) but=pb;
		atteint=false;

}
	
void Prospecteur::autonomous(){
	if(dp>=maxD_prosp) return;
	if(Robot::mouvement()) return;
	if(atteint) stockeInfoGisement_Redirige(); 
}

void Prospecteur::remote(){
	if(dp>=maxD_prosp) return; 
	if(!atteint){
		if(found){
			retour=false;
			found=false;
			but=marcheProspecteur(pos);
			return;
		}				
		Robot::mouvement(); 
		return;
	}
	stockeInfoGisement_Redirige();
}

InfoGisement Prospecteur::getGisTrouve()const{
	return gisTrouve;
}

double Prospecteur::maintenance(){
	found=false; 
	retour=false;
	return Robot::maintenance();
}


void Prospecteur::drawRobot(unsigned int i){
	drawProspGeo(pos, i);
}


Forage::Forage(unsigned int uid, double dp, P2D pos, P2D but,P2D posBase,bool atteint):
			   Robot(uid, dp, pos, but, posBase, atteint){
	type=FORAGE;
	if(atteint){
		InfoGisement gis=Gisement::intersectionGisRob(pos);
		if(gis.gisPtr!=nullptr)	gisForage=gis;
	}	
}

Forage::~Forage(){}		

bool Forage::deplacementPossible(const P2D& candidatBut)const{
	Vecteur v;
	return (maxD_forage-dp)>norme(pos, candidatBut,v) and !atteint;
}

void Forage::autonomous(){
	if(dp>=maxD_forage) return;
	if(Robot::mouvement()) return;
	if(atteint){
		if(gisForage.capaciteg==0) return;
		vector<Robot*> vectTransport=transportsPossibles();
		if(!vectTransport.empty()){
			Transport* transport=(Transport*)vectTransport[0];
			if(gisForage.gisPtr->extraction()){
				gisForage=gisForage.gisPtr->getInfoGis();
				transport->setAttributs(false,true,true);
				transport->setBut(posBase);
				return;
			}
			transport->setAttributs(false, false, false);
		}
	}	
}

void Forage::remote(){
	autonomous();
}

void Forage::setGisForage(const InfoGisement& gis){
	gisForage=gis;
}

InfoGisement Forage::getGisForage()const{
	return gisForage;
}

void Forage::drawRobot(unsigned int i){
	drawForageGeo(pos, i);
}
		
Transport::Transport(unsigned int uid, double dp, P2D pos, P2D but, P2D posBase,
					 bool atteint, bool retour):
					 Robot(uid, dp, pos, but, posBase, atteint), retour(retour){
	type=TRANSPORT;
}

Transport::~Transport(){}

string Transport::afficheRobot()const{
	return Robot::afficheRobot()+" "+to_string(retour);
}

void Transport::setAttributs(bool at, bool ret, bool ch){
	atteint=at;
	retour=ret;
	charge=ch;
}

bool Transport::getRetour(){
	return retour;
}

bool Transport::getCharge()const{
	return charge;
}

bool Transport::deplacementPossible(const P2D& candidatBut)const{
	Vecteur v;												
	return (maxD_transp-dp)>norme(pos,candidatBut,v)+norme(posBase,but,v);
}

void Transport::autonomous(){
	if(dp>=maxD_transp) return;
	if(Robot::mouvement()) return;
	if(atteint && !retour)	but=posBase;
	if(retour) retour=false;
}

void Transport::remote(){
	autonomous();
}


double Transport::maintenance(){
 	//VERIFIER LES BOOLEENS
 	atteint=true;
 	charge=false;
	retour=false;
	return Robot::maintenance();
}


void Transport::drawRobot(unsigned int i){
	drawTransGeo(pos, i);//A COMPLETER
}

/*********************************************/
bool Transport::verifTransRetour(){
	if(retour) return true;
	return false;
}
Communication::Communication(unsigned int uid,double dp, P2D pos,P2D but, P2D posBase,
							bool atteint): Robot(uid, dp, pos, but, posBase, atteint){
	type=COMMUNICATION;
}
				
Communication::~Communication(){}

bool Communication::testCom(P2D const& base){
	return testEgalite(pos, base);
}

void Communication::autonomous(){
	if(dp>=maxD_com) return;
	Robot::mouvement();
}

void Communication::remote(){
	autonomous();
}

bool Communication::deplacementPossible(const P2D& candidatBut)const{
	Vecteur v;
	return (maxD_com-dp)>norme(pos,candidatBut,v) and !atteint;
}

void Communication::drawRobot(unsigned int i){
	drawCommGeo(pos, i);
}

void Communication::drawCercleCom(){
	drawCercleGeo({{pos.x,pos.y},rayon_comm});
}

void Robot::drawLinkRobots(){;
	for(size_t i(0) ;i<link.size();++i){
		if(link[i]==nullptr)continue;
		P2D p = link[i]->pos;
		if((pos.x-p.x)*(pos.x-p.x)+(pos.y-p.y)*(pos.y-p.y)>rayon_comm*rayon_comm)
			continue;
		drawLink(pos.x,pos.y,link[i]->pos.x,link[i]->pos.y);
	}
}

double Robot::generateRandomNumbers(int min ,int max){
	double rd=(double) rand() / RAND_MAX;
	double randomValue= rd*(max-min)+min;
	return randomValue;
}

P2D Robot::marcheProspecteur(P2D p){
	double xStepSize= generateRandomNumbers(-2*deltaD,2*deltaD);
	double yStepSize= generateRandomNumbers(-2*deltaD,2*deltaD);
	p.x +=30*xStepSize;
	p.y +=30*yStepSize;
	P2D pb={p.x,p.y};	
	return pb;
}
