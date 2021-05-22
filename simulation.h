/*!
  \file   simulation.h
  \author Salim Boussofara + Mohamed Ata Allah Krichene
  \date   Avril 2021
  \brief  Interface du module "Simulation"
*/
#ifndef SIMULATION_H
#define SIMULATION_H

#include <iostream>
#include <fstream>
#include <sstream>
#include "base.h"

class Simulation{
public:
	~Simulation();
	bool lecture(std::string); 
	std::vector<Base*> getBases() const;
	void saveState(const std::string path);
	void drawSimulation()const;
	void run();
	void drawLinkSimulation();
	void drawCommSimulation();
private:
	std::vector<Base*> bases;
	bool decodageBase(std::ifstream&, std::string&);
	bool decodageGisement(std::ifstream&, std::string&);	
	std::string afficheSimulation()const;
	void deleteBase();
};
#endif
