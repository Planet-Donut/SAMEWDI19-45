/*!
  \file   base.h
  \author Salim Boussofara + Mohamed Ata Allah Krichene
  \date   Avril 2021
  \brief  Interface du module "Base"
*/
#ifndef BASE_H
#define BASE_H
#include <vector>
#include <string>
#include "robot.h"
#include "gisement.h"

class Base{
public:
	Base(P2D,double,unsigned int,unsigned int, unsigned int,unsigned int);
	~Base();
	static Base* initBase(std::ifstream&, std::string&, std::vector<Base*>const&);
	static void updateVoisin(Base*, Base*);
	void reset();
	unsigned int getNbProspecteur()const;
	unsigned int getNbForage()const;
	unsigned int getNbTransport()const;
	unsigned int getNbCommunication()const;
	double getRessources() const;
	std::string afficheBase() const;
	void drawBase(size_t i)const;
	////bool deleteBase(); ////////////
	//bool gisementExploitable(Cercle c);	
	void updateAutonomous();
	void updateRemote();
	void connexion();
	void mise_a_zero(Robot*);//+	
	void maintenance();	//+	
	void drawLinkBase();//+
	void drawCommBase();//+
	bool missionAccomplie()const;
	void creation();


//FIN CREATION				
private:
	static unsigned int nbComTot;
	int updateCreation=-1;
	Cercle cercleBase;
	unsigned int  nbP=0 ,nbF=0 ,nbT=0 ,nbC=0; ////////////oussama
	double ressources;
	Communication* comCentral;
	std::vector<Robot*> robots;
	std::vector<Robot*> connectedRobots;
	std::vector<Forage*> forageSurMeilleurGisement={};
	double meilleurCapaciteg;
	std::vector<P2D> droiteComm={};
	unsigned int compteurComm=0;
	static bool testIntersectionBases(Base*,std::vector<Base*>const&);
	InfoGisement findMeilleurGisement(double&);//***
	InfoGisement meilleurGisement;
	bool forageACreer(InfoGisement, double);//***
	int envoiTransport(std::vector<Transport*>, const P2D&);
	bool chercheForage_Gisement();
	void remoteTransport();//***
	bool gisementExploitable(Gisement*, size_t &, size_t &, bool=true);//***
	void recDFS(std::vector<Robot*>&, Robot*);
	void setDroiteComm();
	unsigned int genererUid();
			//CREATION 	
	void creationProsp(unsigned int&);
	void creationForageTransport(unsigned int&,bool=true);
	//void creationCommunication(unsigned int&,Robot*);
	void creationCommunication(unsigned int&);
	bool inForageSurMeilleurGisement(Robot* rob);



};
#endif
