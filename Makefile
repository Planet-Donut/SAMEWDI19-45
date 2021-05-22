OUT = projet
CXX = g++
CXXFLAGS = -Wall -std=c++11 
LINKING = `pkg-config --cflags gtkmm-3.0`
LDLIBS = `pkg-config --libs gtkmm-3.0`
OFILES = projet.o gui.o graphic.o geomod.o base.o gisement.o simulation.o robot.o message.o

all: $(OUT)

gui.o: gui.cc gui.h graphic.h
	$(CXX) $(CXXFLAGS) $(LINKING) -c $< -o $@ $(LINKING)
	
graphic.o: graphic.cc graphic.h
	$(CXX) $(CXXFLAGS) $(LINKING) -c $< -o $@ $(LINKING)
	
projet.o: projet.cc constantes.h simulation.h base.h robot.h geomod.h \
 gisement.h gui.h
	$(CXX) $(CXXFLAGS) $(LINKING) -c $< -o $@ $(LINKING)
geomod.o: geomod.cc geomod.h
	$(CXX) $(CXXFLAGS) $(LINKING) -c $< -o $@ $(LINKING)
base.o: base.cc base.h robot.h geomod.h gisement.h message.h constantes.h
	$(CXX) $(CXXFLAGS) $(LINKING) -c $< -o $@ $(LINKING)
gisement.o: gisement.cc gisement.h geomod.h constantes.h message.h
	$(CXX) $(CXXFLAGS) $(LINKING) -c $< -o $@ $(LINKING)
simulation.o: simulation.cc simulation.h base.h robot.h geomod.h gisement.h message.h
	$(CXX) $(CXXFLAGS) $(LINKING) -c $< -o $@ $(LINKING)
robot.o: robot.cc robot.h geomod.h message.h constantes.h
	$(CXX) $(CXXFLAGS) $(LINKING) -c $< -o $@ $(LINKING)
message.o: message.cc message.h
	$(CXX) $(CXXFLAGS) $(LINKING) -c $< -o $@ $(LINKING)


$(OUT): $(OFILES)
	$(CXX) $(CXXFLAGS) $(LINKING) $(OFILES) -o $@ $(LDLIBS)

clean:
	@echo "Cleaning compilation files"
	@rm *.o $(OUT) *.cc~ *.h~
