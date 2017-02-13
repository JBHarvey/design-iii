CC=clang
CFLAGS= -g `pkg-config --cflags gtk+-3.0 opencv`
LIBS=`pkg-config --libs gtk+-3.0 opencv`
ODIR=obj
RDIR=station-resources
DEPS=
_OBJ=station-main.o resources.o
OBJ=$(patsubst %,$(ODIR)/%,$(_OBJ))
_RES=ui.gresource.xml
RES=$(patsubst %,$(RDIR)/%,$(_RES))

resources.c: $(RES)
	glib-compile-resources --target=resources.c --generate-source $(RES)

$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

station-main: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

all: resources.c station-main 

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o
	rm -f station-main
	rm -f resources.c
