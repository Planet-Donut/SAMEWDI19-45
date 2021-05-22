#ifndef GUI_H
#define GUI_H
#include <gtkmm.h>
#include "simulation.h"



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
    void draw(bool, bool, Simulation*);
    void refresh();
    void setFrame(Frame); 
protected:	
    bool on_draw(const Cairo::RefPtr<Cairo::Context>&) override;
private:
	void adjust_frame();
	Frame frame_ref={-dim_max,dim_max,-dim_max,dim_max,1,8000,8000};//initialized once
	Frame frame;
    bool toggleLink,toggleRange;
};
	
	
class Gui :public Gtk::Window{
public:
	Gui(std::string filename);
	virtual ~Gui();
	void tree_view_update();
protected:
    //signal handlers
    bool on_idle();
    void on_button_clicked_Exit();
    void on_button_clicked_Open();
	void on_button_clicked_Save();
	void on_button_clicked_Start_Stop();
	void on_button_clicked_Step();
	void on_button_clicked_Toggle_link();
	void on_button_clicked_Toggle_range();
	// Keyboard signal handler:
    bool on_key_press_event(GdkEventKey * key_event);	
	//Member Data
    Gtk::Box mBox, mBoxSup, mBoxInf, mBoxBoutons, mBoxStart, mBoxDessin,
			 mBoxCommande, mBoxSimulation;
	MyArea mArea;
    Gtk::Button mBoutonExit, mBoutonOpen, mBoutonSave, mBoutonStartStop, 
				mBoutonStep, mBoutonToggleLink, mBoutonToggleRange; 
    bool toggleLink,toggleRange;
private: 
    std::string nomFichier;
	bool started; //step;
    class Model_columns : public Gtk::TreeModel::ColumnRecord {
	public:
		Model_columns(){
			add(col_id);
			add(col_nbP);
			add(col_nbF);
			add(col_nbT);
			add(col_nbC);
			add(col_resource);
			add(col_resource_percentage);
		 }
		Gtk::TreeModelColumn<int> col_id;
		Gtk::TreeModelColumn<int> col_nbP;
		Gtk::TreeModelColumn<int> col_nbF;
		Gtk::TreeModelColumn<int> col_nbT;
		Gtk::TreeModelColumn<int> col_nbC;
		Gtk::TreeModelColumn<double> col_resource;
		Gtk::TreeModelColumn<int> col_resource_percentage;
		};
	Model_columns columns;
    Gtk::ScrolledWindow scrolledWindow;
    Gtk::TreeView treeView;	
};
#endif
