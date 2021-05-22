#ifndef GTKMM_EXAMPLE_MYAREA_H
#define GTKMM_EXAMPLE_MYAREA_H

#include <gtkmm/drawingarea.h>

struct Frame{ // Framing and window parameters
	double xMin; // frame parameters
	double xMax;
	double yMin;
	double yMax;
	double asp;  // frame aspect ratio
	int height;  // window height
	int width;   // window width
};

class MyArea : public Gtk::DrawingArea{
public:
	MyArea();
	virtual ~MyArea();
	void setFrame(Frame x); 
private:
    void adjust_frame();
	Frame frame_ref;  //initialized once ; serves as a reference afterwards
	Frame frame;
protected:
	//Override default signal handler:
	bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override;
};

#endif // GTKMM_EXAMPLE_MYAREA_H
