#ifndef GTKMM_EXAMPLE_GRAPHIC_H
#define GTKMM_EXAMPLE_GRAPHIC_H

#include <gtkmm.h>
#include "constantes.h"
enum Couleur {RED, GREEN, BLUE, YELLOW, MAGENTA, CYAN};

void setContext(const Cairo::RefPtr<Cairo::Context>& cr);

void setDrawingSize(unsigned int);

void setDimension(unsigned int);

void drawCercleGraph(double, double, double, int=-1);

void drawGisGraph(double, double, double);

void drawEtoile(double, double, unsigned int);

void drawCroix(double, double, unsigned int);

void drawTriangle(double, double, unsigned int);

void drawCommGraph(double, double, unsigned int);

void drawLink(double, double, double, double);

void draw_rayon_com(double, double);

void setCouleur(int);

#endif 
// GTKMM_EXAMPLE_GRAPHIC_H
