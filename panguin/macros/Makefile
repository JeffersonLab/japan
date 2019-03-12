make testComp:
	g++ -g -c asymmetryPlot_compile.C -o comp.o `root-config --cflags --glibs`
	g++ comp.o -o compTest `root-config --cflags --glibs`
	rm comp.o

