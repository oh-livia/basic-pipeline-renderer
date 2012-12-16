glrender_os: glrender.cc
	g++ *.cc initshader.cpp amath.h parse_obj.h bezObj.h -Wall -pedantic -framework OpenGL -framework GLUT -o glrender

glrender_pc: glrender.cc
	g++ -O2 *.cc *.h initshader.cpp -I -L /usr/lib/x86_64-linux-gnu/ -lglut -lGL -lGLU -lGLEW -lm -o glrender

.PHONY: clean
clean:
	rm -f *~ *.o glrender
