/*!
  \file   robot.h
  \author Salim Boussofara + Mohamed Ata Allah Krichene
  \date   Avril 2021
  \brief  Interface du module "Robot"
*/
#ifndef ROBOT_H
#define ROBOT_H
#include <vector>
#include <string>
#include "geomod.h"
#include "gisement.h"

enum Type_robot{PROSPECTEUR, FORAGE, TRANSPORT,COMMUNICATION};
enum Etat{AUTONOMOUS, REMOTE};

class Robot{
public:
	Robot(unsigned int, double, P2D, P2D, P2D, bool);
	virtual ~Robot();
	static Robot* initRobot(std::string const&, const P2D&, Type_robot
								,std::vector<Robot*>const&);
	void setLink(Robot*);
	void clearLink();
	Etat getEtat()const;
	void setEtat(Etat);
	virtual std::string afficheRobot()const;
	virtual void drawRobot(unsigned int)=0 ;
	std::vector<Robot*> getLink()const;
	void setVisited(bool);
	bool getVisited()const;
	Type_robot getType()const;
	P2D getPos()const;
	P2D getPosBase()const;
	P2D getBut()const;
	virtual void autonomous()=0;
	bool mouvement();
	void setBut(const P2D&);
	virtual void remote()=0;
	virtual bool deplacementPossible(const P2D&)const=0; 
	P2D marcheProspecteur(P2D);
	double generateRandomNumbers(int,int);
	bool gisementBut(Gisement*); 
	virtual double maintenance();
	void drawLinkRobots(); 
	static bool testUid(unsigned int, std::vector<Robot*>const&); 	
protected:
	void avance();
	unsigned int uid;
	double dp;
	P2D pos;
	P2D but;
	P2D posBase;
	bool atteint;
	bool visited=false;
	Type_robot type;
	Etat etat=AUTONOMOUS; 
	std::vector<Robot*> link;
	std::vector<Robot*> transportsPossibles(); 
};

class Prospecteur: public Robot{
public:
	Prospecteur(unsigned int, double, P2D, P2D, P2D, bool, bool, bool
						,Cercle gisement={{0,0},0},double capaciteg=0);
	~Prospecteur();
	std::string afficheRobot()const;	
	void autonomous();

	bool deplacementPossible(const P2D&)const;
	void drawRobot(unsigned int i);	
	void remoteProspecteur();
	void stockeInfoGisement_Redirige();
	InfoGisement getGisTrouve()const;
	double maintenance();
	void remote();
	void setRentreMaintenance(bool);			
private: 
	bool retour;
	bool found;
	bool rentreMaintenance;	
	InfoGisement gisTrouve={{{0,0},0},0,nullptr};
};

class Forage: public Robot{
public:
	Forage(unsigned int, double, P2D, P2D, P2D, bool);
	~Forage();
	void autonomous();
	void remote();
	void setRetour(bool);
	void drawRobot(unsigned int);
	bool deplacementPossible(const P2D&)const;
	void setGisForage(const InfoGisement&);
	InfoGisement getGisForage()const;
	
private: 
	InfoGisement gisForage={{{0,0},0},0,nullptr};
};

class Transport: public Robot{
public:
	Transport(unsigned int, double, P2D, P2D, P2D, bool, bool);
	~Transport();
	std::string afficheRobot()const;
	bool deplacementPossible(const P2D&)const;
	void autonomous();
	void remote();
	bool getRetour(); 
	double maintenance();
	void setAttributs(bool, bool, bool);
	bool getCharge()const;
	void drawRobot(unsigned int);
	bool verifTransRetour();
private:
	bool retour;
	bool charge;

};

class Communication: public Robot{
public:
	Communication(unsigned int, double, P2D, P2D, P2D, bool);
	~Communication();
	bool testCom(P2D const&);
	void autonomous();
	bool deplacementPossible(const P2D&)const;
	void remote();
	void drawRobot(unsigned int);
	void drawCercleCom();
};
#endif
