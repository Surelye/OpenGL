CXXFILES = hello_window.cpp
LIBS = -lGL -lGLEW -lglfw

all:
	$(CXX) $(CXXFILES) $(LIBS) -o hello_window

clean:
	rm hello_window