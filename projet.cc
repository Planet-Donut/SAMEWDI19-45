#include "constantes.h"
#include "simulation.h"
#include "gui.h"
using namespace std;

int main(int argc, char* argv[]){
	setMax(dim_max);
	//Simulation sim;
	//sim.lecture(argv[1]);
	//sim.run();
	if(argc>2){
		cout<<"ERREUR"<<endl;
		exit(EXIT_FAILURE);
	}
	auto app = Gtk::Application::create();
	string fichier="";
	if(argc==2) fichier=argv[1];
	Gui gui(fichier);
	return app->run(gui);
}
