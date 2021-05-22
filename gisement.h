/*!
  \file   gisement.h
  \author Salim Boussofara + Mohamed Ata Allah Krichene
  \date   Avril 2021
  \brief  Interface du module "gisement"
*/
#ifndef GISEMENT_H
#define GISEMENT_H
#include <string>
#include <vector>
#include "geomod.h"

class Gisement;

struct InfoGisement{
	Cercle cercleGisement;
	double capaciteg;
	Gisement* gisPtr;
};

class Gisement{
public:
	Gisement(double, double, double, double);
	~Gisement();
	static bool initGisement(std::string&);
	static bool testIntersectionBaseGisement(Cercle const&);
	static std::string afficheGisements();
	static void drawGisements();
	static void deleteGisements();
	Cercle getCercleGisement()const;
	double getCapacite() const;
	static InfoGisement intersectionGisRob(const P2D& pos);
	bool extraction();
	InfoGisement getInfoGis()const;
private:
	static std::vector<Gisement*> gisements;
	static bool testIntersectionGisement(Gisement*);
	InfoGisement infoGis={0, 0, 0, 0, this}; 
};

#endif
