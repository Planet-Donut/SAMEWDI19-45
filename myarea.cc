#include "myarea.h"
#include <cairomm/context.h>

static void orthographic_projection(const Cairo::RefPtr<Cairo::Context>& cr, 
									Frame frame);


MyArea::MyArea(){
}

MyArea::~MyArea(){
}

void MyArea::setFrame(Frame x){
	frame_ref = x;
	frame     = frame_ref;
} 

bool MyArea::on_draw(const Cairo::RefPtr<Cairo::Context>& cr){ 
	// adjust the frame (cadrage) to prevent distortion
	adjust_frame();
	// the conversion uses the frame info ; it is done only once with this call
	orthographic_projection(cr, frame); 
	//set width and color 
	cr->set_line_width(0.1);
	cr->set_source_rgb(0.8, 0.0, 0.0);
	// the shape is assymmetric to be sure that not mirror transformation occurs
	// please note that the coordinates (x,y) of the shape
	// are from the Model space and not from the window space.
	cr->move_to(2, 10);
	cr->line_to(4.5, 13);
	//draw second line
	cr->move_to(4,10);
	cr->line_to(2,12);
	cr->stroke();
	return true;
}

void MyArea::adjust_frame(){
	Gtk::Allocation allocation = get_allocation();
	const int width = allocation.get_width();
	const int height = allocation.get_height();
	
	frame.width  = width;
	frame.height = height;
	
 
    double new_aspect_ratio((double)width/height);
    if( new_aspect_ratio > frame_ref.asp)
    { // keep yMax and yMin. Adjust xMax and xMin
	    frame.yMax = frame_ref.yMax ;
	    frame.yMin = frame_ref.yMin ;	
	  
	    double delta(frame_ref.xMax - frame_ref.xMin);
	    double mid((frame_ref.xMax + frame_ref.xMin)/2);
	    
	    frame.xMax = mid + 0.5*(new_aspect_ratio/frame_ref.asp)*delta ;
	    frame.xMin = mid - 0.5*(new_aspect_ratio/frame_ref.asp)*delta ;		  	  
    }else{
		// keep xMax and xMin. Adjust yMax and yMin
	    frame.xMax = frame_ref.xMax ;
	    frame.xMin = frame_ref.xMin ;
	  	  
 	    double delta(frame_ref.yMax - frame_ref.yMin);
	    double mid((frame_ref.yMax + frame_ref.yMin)/2);
        // the new frame is centered on the mid-point along Y
	    frame.yMax = mid + 0.5*(frame_ref.asp/new_aspect_ratio)*delta ;
	    frame.yMin = mid - 0.5*(frame_ref.asp/new_aspect_ratio)*delta ;		  	  
    }
}

static void orthographic_projection(const Cairo::RefPtr<Cairo::Context>& cr, 
									Frame frame){
	// déplace l'origine au centre de la fenêtre
	cr->translate(frame.width/2, frame.height/2);
  
	// normalise la largeur et hauteur aux valeurs fournies par le cadrage
	// ET inverse la direction de l'axe Y
	cr->scale(frame.width/(frame.xMax - frame.xMin), 
             -frame.height/(frame.yMax - frame.yMin));
  
	// décalage au centre du cadrage
	cr->translate(-(frame.xMin + frame.xMax)/2, -(frame.yMin + frame.yMax)/2);
}

