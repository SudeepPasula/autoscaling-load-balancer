CXX = g++
CXXFLAGS = -Wall -Werror -std=c++17

OBJS = main.o Config.o Firewall.o LoadBalancer.o Logger.o RequestGenerator.o RequestQueue.o Switch.o WebServer.o

.PHONY: all docs zip clean

all: loadbalancer

loadbalancer: $(OBJS)
	$(CXX) $(CXXFLAGS) -o loadbalancer $(OBJS)

main.o: main.cpp Config.h Firewall.h LoadBalancer.h Logger.h RequestGenerator.h Switch.h Request.h RequestQueue.h WebServer.h
	$(CXX) $(CXXFLAGS) -c main.cpp

Config.o: Config.cpp Config.h
	$(CXX) $(CXXFLAGS) -c Config.cpp

Firewall.o: Firewall.cpp Firewall.h
	$(CXX) $(CXXFLAGS) -c Firewall.cpp

LoadBalancer.o: LoadBalancer.cpp LoadBalancer.h Config.h Firewall.h Logger.h RequestQueue.h WebServer.h Request.h
	$(CXX) $(CXXFLAGS) -c LoadBalancer.cpp

Logger.o: Logger.cpp Logger.h
	$(CXX) $(CXXFLAGS) -c Logger.cpp

RequestGenerator.o: RequestGenerator.cpp RequestGenerator.h Config.h Firewall.h Request.h
	$(CXX) $(CXXFLAGS) -c RequestGenerator.cpp

RequestQueue.o: RequestQueue.cpp RequestQueue.h Request.h
	$(CXX) $(CXXFLAGS) -c RequestQueue.cpp

Switch.o: Switch.cpp Switch.h LoadBalancer.h Config.h Firewall.h Logger.h RequestQueue.h WebServer.h Request.h
	$(CXX) $(CXXFLAGS) -c Switch.cpp

WebServer.o: WebServer.cpp WebServer.h Request.h
	$(CXX) $(CXXFLAGS) -c WebServer.cpp

docs:
	doxygen Doxyfile

zip:
	zip project4.zip *.cpp *.h Makefile config*.txt Doxyfile

clean:
	rm -f loadbalancer *.o *.d project4.zip
