#include <cmath>
#include <iostream>
#include "graphic.h"
namespace{
	unsigned int drawingSize;
	double dimension; 
}

static const Cairo::RefPtr<Cairo::Context>* ptcr(nullptr); 

void setContext(const Cairo::RefPtr<Cairo::Context>& cr){
	static bool init(false);
	if(!init){
		ptcr = &cr;
		init = true;
	}
}

void setDrawingSize(unsigned int guiDrawingSize){
	drawingSize=guiDrawingSize;
}

void setDimension(unsigned int guiDimension){
	dimension=guiDimension;
}

void drawCercleGraph(double x, double y, double rayon, int i){	  
	(*ptcr)->save();
	setCouleur(i); 
	(*ptcr)->arc(x, y, rayon, 0.0, 2.0 * M_PI);
	(*ptcr)->stroke();
	(*ptcr)->restore();
}

void drawGisGraph(double x, double y, double rayon){
	(*ptcr)->save();
	(*ptcr)->set_source_rgb(0,0,0);
	(*ptcr)->arc(x, y, rayon, 0.0, 2.0 * M_PI);
	(*ptcr)->fill_preserve();
	(*ptcr)->stroke();
	(*ptcr)->restore();
}

void drawLink(double x1, double y1, double x2, double y2){
	(*ptcr)->save();
	(*ptcr)->set_source_rgb(0.55,0.26,0.933);
	(*ptcr)->move_to(x1, y1);
	(*ptcr)->line_to(x2, y2);
	(*ptcr)->stroke();	
	(*ptcr)->restore();
}

void drawEtoile(double x, double y, unsigned int i){
	double rayon=20;
	(*ptcr)->save();
	setCouleur(i); 
	(*ptcr)->move_to(x, y);
	double teta(0);
	for (int i(0); i<8; ++i){
		teta = teta + M_PI/4 ;
		(*ptcr)->line_to(x + rayon*cos(teta), y + rayon*sin(teta));
		(*ptcr)->move_to(x,y);
	} 
	(*ptcr)->stroke();
	(*ptcr)->restore();
}

void drawCroix(double x, double y, unsigned int i){
	double d=12;
	(*ptcr)->save();
	setCouleur(i); 
	(*ptcr)->move_to(x-d, y-d);
	(*ptcr)->line_to(x+d, y+d);
	//draw second line
	(*ptcr)->move_to(x+d,y-d);
	(*ptcr)->line_to(x-d,y+d);
	(*ptcr)->stroke();
	(*ptcr)->restore();
}

void drawTriangle(double x, double y, unsigned int i){
	double h=15;
	(*ptcr)->save();
	setCouleur(i); 
	(*ptcr)->move_to(x, y+h);
	(*ptcr)->line_to(x-(3*h)/(2*tan(M_PI/3)), y-h/2);
	(*ptcr)->line_to(x+9/tan(M_PI/3), y-h/2);
	(*ptcr)->line_to(x, y+h);
	(*ptcr)->stroke();
	(*ptcr)->restore();
}

void drawCommGraph(double x, double y, unsigned int i){
	(*ptcr)->save();
	setCouleur(i);
	(*ptcr)->move_to(x+12, y);
	(*ptcr)->arc(x, y, 12, (M_PI / 3), (2*M_PI/3));
	(*ptcr)->move_to(x, y);
	(*ptcr)->arc(x, y, 24, (M_PI / 6), (5*M_PI/6));
	(*ptcr)->stroke();
	(*ptcr)->restore();
}

void draw_rayon_com(double x, double y){
	(*ptcr)->save();
	(*ptcr)->move_to(x+rayon_comm, y);
	(*ptcr)->set_source_rgb(0.7,0.7,0.7);
	(*ptcr)->arc(x, y, rayon_comm, 0, (2*M_PI));
	(*ptcr)->stroke();
	(*ptcr)->restore();
}
	
void setCouleur(int i){
	if(i==-1) (*ptcr)->set_source_rgb(0.7,0.7,0.7);
	switch(i%6){
		case RED: 
			(*ptcr)->set_source_rgb(1,0,0); 
			return;
		case GREEN: 
			(*ptcr)->set_source_rgb(0,1,0); 
			return;
		case BLUE: 
			(*ptcr)->set_source_rgb(0, 0, 1); 
			return;
		case YELLOW: 
			(*ptcr)->set_source_rgb(1, 1, 0); 
			return;
		case MAGENTA: 
			(*ptcr)->set_source_rgb(1, 0, 1); 
			return; 
		case CYAN: 
			(*ptcr)->set_source_rgb(0, 1, 1); 
			return;			
	}
}
