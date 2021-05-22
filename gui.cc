#include <cstdlib>
#include <thread>
#include <iostream>
#include <chrono>
#include <thread>
#include "constantes.h"
#include "gui.h"
#include "graphic.h"
#include <cairomm/context.h>

using namespace std;

#define GTK_COMPATIBILITY_MODE
#ifdef GTK_COMPATIBILITY_MODE
namespace Gtk
{
  template<class T, class... T_Args>
  auto make_managed(T_Args&&... args) -> T*
  {
    return manage(new T(std::forward<T_Args>(args)...));
  }
}
#endif
namespace{
	Simulation* simulation;
}
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
 
void MyArea::draw(bool toggleLink, bool toggleRange,Simulation* s){
	this->toggleLink=toggleLink;
	this->toggleRange=toggleRange;
	refresh();
}
	
void MyArea::refresh(){
	auto win = get_window();
	if(win){
	  Gdk::Rectangle r(0,0,get_allocation().get_width(),get_allocation().get_height());
	  win->invalidate_rect(r,false);
	}
}		

bool MyArea::on_draw(const Cairo::RefPtr<Cairo::Context>& cr){
	// adjust the frame (cadrage) to prevent distortion
	adjust_frame();
	// the conversion uses the frame info ; it is done only once with this call
	orthographic_projection(cr, frame);
	setContext(cr);
	cr->rectangle(frame_ref.xMin,frame.yMin,2*frame_ref.xMax,2*frame_ref.yMax);
	cr->stroke();
	if(simulation!=nullptr) simulation->drawSimulation();

	// draw liens
	if(simulation!=nullptr && toggleLink )
		simulation->drawLinkSimulation();
	//draw cercles com	
	if(simulation!=nullptr && toggleRange)
		simulation->drawCommSimulation();
	return true;	
}	
void MyArea::adjust_frame()
{
	Gtk::Allocation allocation = get_allocation();
	const int width = allocation.get_width();
	const int height = allocation.get_height();
	
	frame.width  = width;
	frame.height = height;

	double new_aspect_ratio((double)width/height);
    if(new_aspect_ratio > frame_ref.asp){
		// keep yMax and yMin. Adjust xMax and xMin
	    frame.yMax = frame_ref.yMax ;
	    frame.yMin = frame_ref.yMin ;	
	  
	    double delta(frame_ref.xMax - frame_ref.xMin);
	    double mid((frame_ref.xMax + frame_ref.xMin)/2);
        // the new frame is centered on the mid-point along X
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
									Frame frame)
{
	// déplace l'origine au centre de la fenêtre
	cr->translate(frame.width/2, frame.height/2);
  
	// normalise la largeur et hauteur aux valeurs fournies par le cadrage
	// ET inverse la direction de l'axe Y
	cr->scale(frame.width/(frame.xMax - frame.xMin), 
             -frame.height/(frame.yMax - frame.yMin));
	// décalage au centre du cadrage
	cr->translate(-(frame.xMin + frame.xMax)/2, -(frame.yMin + frame.yMax)/2);
}

// =========================================
Gui::Gui(string nomFichier):
	mBox(Gtk::ORIENTATION_VERTICAL,10), mBoxSup(Gtk::ORIENTATION_HORIZONTAL,10),
	mBoxInf(Gtk::ORIENTATION_HORIZONTAL,10), 
	mBoxBoutons(Gtk::ORIENTATION_VERTICAL, 10),
	mBoxDessin(Gtk::ORIENTATION_VERTICAL, 10), 
	mBoxCommande(Gtk::ORIENTATION_VERTICAL, 10),
	mBoxSimulation(Gtk::ORIENTATION_VERTICAL, 10), mBoutonExit("Exit"), 
	mBoutonOpen("Open"), mBoutonSave("Save"), mBoutonStartStop("Start"), 
	mBoutonStep("Step"), mBoutonToggleLink("Toggle link"), 
	mBoutonToggleRange("Toogle Range"), started (false), toggleLink(true),
	toggleRange(false), nomFichier(nomFichier) {	
	set_title("Planet Donut Demo");
	set_border_width(5); 
	add(mBox);
	//Put the inner boxes and the separator in the outer box:
	mBox.pack_start(mBoxSup);
	mBox.pack_start(mBoxInf);

	mBoxSup.pack_start(mBoxBoutons);
	mBoxSup.pack_start(mBoxDessin);

	mArea.set_size_request(400,400);
	mBoxDessin.pack_start(mArea);

	mBoxBoutons.pack_start(mBoxCommande);
	mBoxBoutons.pack_start(mBoxSimulation);

	mBoxCommande.pack_start(mBoutonExit);
	mBoxCommande.pack_start(mBoutonOpen);
	mBoxCommande.pack_start(mBoutonSave);   
	mBoxCommande.pack_start(mBoutonStartStop);
	mBoxCommande.pack_start(mBoutonStep);

	mBoxSimulation.pack_start(mBoutonToggleLink);
	mBoxSimulation.pack_start(mBoutonToggleRange);

	// Connect the signal handlers
	mBoutonOpen.signal_clicked().connect(sigc::mem_fun(*this,
			  &Gui::on_button_clicked_Open));
	mBoutonSave.signal_clicked().connect(sigc::mem_fun(*this,
			  &Gui::on_button_clicked_Save));
	mBoutonStartStop.signal_clicked().connect(sigc::mem_fun(*this,
			  &Gui::on_button_clicked_Start_Stop));
	mBoutonStep.signal_clicked().connect(sigc::mem_fun(*this,
			  &Gui::on_button_clicked_Step));
	mBoutonExit.signal_clicked().connect(sigc::mem_fun(*this,
			  &Gui::on_button_clicked_Exit));       
	mBoutonToggleLink.signal_clicked().connect(sigc::mem_fun(*this,
			  &Gui::on_button_clicked_Toggle_link));       
	mBoutonToggleRange.signal_clicked().connect(sigc::mem_fun(*this,
			  &Gui::on_button_clicked_Toggle_range)); 

	//idle signal handler		  
	Glib::signal_idle().connect( sigc::mem_fun(*this, &Gui::on_idle) );
		
	mBoxInf.add(scrolledWindow);
	scrolledWindow.set_size_request(-1, 200);  
	scrolledWindow.add(treeView);
	scrolledWindow.set_policy(Gtk::PolicyType::POLICY_AUTOMATIC,
							  Gtk::PolicyType::POLICY_AUTOMATIC);
	scrolledWindow.set_hexpand();

	treeView.append_column("nbP", columns.col_nbP);
	treeView.append_column("nbF", columns.col_nbF);
	treeView.append_column("nbT", columns.col_nbT);
	treeView.append_column("nbC", columns.col_nbC);
	treeView.append_column_numeric("Amount resource", columns.col_resource, "%.2f");

	auto cell = Gtk::make_managed<Gtk::CellRendererProgress>();
	int cols_count = treeView.append_column("Mission completeness", *cell);

	auto progress_col = treeView.get_column(cols_count - 1);
	if(progress_col)
	progress_col->add_attribute(cell->property_value(),
								columns.col_resource_percentage);
	show_all_children();
	cout<<nomFichier<<endl;
	if(nomFichier!=""){
		simulation=new Simulation();
	}
	if(!(simulation->lecture(nomFichier))){
		delete simulation;
		simulation=nullptr;
	}
	mArea.draw(toggleLink,toggleRange,simulation);		
	tree_view_update();	
}

Gui::~Gui(){
	delete simulation;
	simulation = nullptr;
};

void Gui::on_button_clicked_Open(){
	if(started) return;
	Gtk::FileChooserDialog dialog("Please choose a file",
	Gtk::FILE_CHOOSER_ACTION_OPEN);
	dialog.set_transient_for(*this);
	//Add response buttons the the dialog:
	dialog.add_button("_Cancel", Gtk::RESPONSE_CANCEL);
	dialog.add_button("_Open", Gtk::RESPONSE_OK);
	//Show the dialog and wait for a user response:
	int result = dialog.run();
	string fichier="";
	//Handle the response:
	switch(result){
		case(Gtk::RESPONSE_OK):
			cout << "Open clicked." << endl;
			fichier = dialog.get_filename();
			cout << "File selected: " <<  fichier << endl;
			break;
		case(Gtk::RESPONSE_CANCEL):
			  cout << "Cancel clicked." << endl;
			  break;
		default:
			cout << "Unexpected button clicked." << endl;
			break;
	}
	//pas de selection ou choix du mauvais fichier
	if(fichier=="" || fichier.find(".txt")==string::npos){
		cout<<"not a txt file"<<endl;
		return ; 
	}
	if(simulation!=nullptr)
		delete simulation; 
	nomFichier=fichier;	  
	simulation=new Simulation();
	if(!(simulation->lecture(nomFichier))){
		delete simulation;
		simulation=nullptr;
	}
	mArea.draw(toggleLink,toggleRange,simulation);
	tree_view_update(); 
}

void Gui::on_button_clicked_Exit(){
	exit(0);
}

void Gui::on_button_clicked_Save(){
	if(started) return;
	Gtk::FileChooserDialog dialog("Please choose a file",
				Gtk::FILE_CHOOSER_ACTION_SAVE);
	dialog.set_transient_for(*this);
	//Add response buttons the the dialog:
	dialog.add_button("_Save", Gtk::RESPONSE_OK);
	dialog.add_button("_Cancel", Gtk::RESPONSE_CANCEL);
	//Show the dialog and wait for a user response:
	int result = dialog.run();
	//Handle the response:
	string fichier;
	switch(result){
		case(Gtk::RESPONSE_OK):
			cout << "Save clicked." << endl;
			fichier = dialog.get_filename();
			cout << "File selected: " <<  fichier << endl;
			break;
		case(Gtk::RESPONSE_CANCEL):
			  cout << "Cancel clicked." << endl;
			  break;
		default:
			cout << "Unexpected button clicked." << endl;
			break;
	}
	if(simulation!=nullptr)
		simulation->saveState(fichier+".txt");//voir si on laisse le .txt	
}

void Gui::on_button_clicked_Start_Stop(){
	//cout<<simulation<<endl;
	if(simulation!=nullptr){
		if(started && simulation!=nullptr) {
			started=false;
			mBoutonStartStop.set_label("Start");
			return;
		}
		started=true;
		mBoutonStartStop.set_label("Stop");
	}
}
	
void Gui::on_button_clicked_Step(){
	if(!started && simulation!=nullptr){
		simulation->run();
	    mArea.draw(toggleLink,toggleRange,simulation);
		tree_view_update();
	}
}

void Gui::on_button_clicked_Toggle_link(){
	toggleLink =! toggleLink;
}

void Gui::on_button_clicked_Toggle_range(){
	toggleRange =! toggleRange;
}		

bool Gui::on_idle(){
	this_thread::sleep_for(chrono::milliseconds(25));
	if(started && simulation!=nullptr)  {
		simulation->run();
		mArea.draw(toggleLink,toggleRange,simulation);
		tree_view_update();
	}
    return true;  // return false when done
}
// Keyboard signal handler:
bool Gui::on_key_press_event(GdkEventKey* key_event){
	if(key_event->type == GDK_KEY_PRESS){
		switch(gdk_keyval_to_unicode(key_event->keyval)){
			case 's':
				on_button_clicked_Start_Stop();
				break;
			case '1':
				on_button_clicked_Step();
				break;
		}
	}
	return Gtk::Window::on_key_press_event(key_event);
}

void Gui::tree_view_update(){
	Glib::RefPtr<Gtk::ListStore> ref_tree_model = Gtk::ListStore::create(columns);
	treeView.set_model(ref_tree_model);
	if(simulation!=nullptr){
		std::vector <Base*>report=simulation->getBases();
		for(size_t i = 0 ; i <report.size() ; ++i){
		  if(report[i]==nullptr) continue;
		  auto row =*(ref_tree_model->append());
		  row[columns.col_nbP] =report[i]->getNbProspecteur();
		  row[columns.col_nbF] =report[i]->getNbForage();
		  row[columns.col_nbT] =report[i]->getNbTransport();
		  row[columns.col_nbC] =report[i]->getNbCommunication();
		  row[columns.col_resource] =report[i]->getRessources();
		  if((report[i]->getRessources())*100/finR>100)
			row[columns.col_resource_percentage] =100;
		  else row[columns.col_resource_percentage] =(report[i]->getRessources())*100/finR;
		}
	}
}
