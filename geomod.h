/*!
  \file   geomod.h
  \author Salim Boussofara + Mohamed Ata Allah Krichene
  \date   Mars 2021
  \brief  Interface du module "geomod"
*/

#ifndef GEOMOD_H
#define GEOMOD_H

#include "graphic.h"

struct P2D{	
	double x;
	double y;
};
struct Vecteur{
	double premier;
	double second;
	double norme;
};

struct Cercle{
	P2D origine;
	double rayon;
};

bool equal_zero(double);
double getEpsilon();
double getMax();
void setMax(double);
double normalisation(double,double=0);
P2D normalisation(const P2D&,double=0);
double norme(const P2D&,const P2D&, Vecteur&);
bool testEgalite(const P2D&, const P2D&);
bool testAppartenance(const P2D&,const Cercle&);
bool testIntersection(const Cercle&,const Cercle&);
std::vector<P2D> rebouclement(Cercle);
void drawGisGeo(Cercle);
void drawCercleGeo(Cercle,int=-1);	
void drawProspGeo(P2D, unsigned int);
void drawForageGeo(P2D, unsigned int);
void drawTransGeo(P2D, unsigned int);
void drawCommGeo(P2D, unsigned int);

#endif
