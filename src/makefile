CC=clang
CFLAGS= -g `pkg-config --cflags gtk+-3.0 opencv gmodule-2.0`
LIBS=`pkg-config --libs gtk+-3.0 opencv gmodule-2.0`
ODIR=obj
RDIR=station-resources
DEPS=ui_builder.h ui_event.h world_vision.h
_OBJ=station-main.o resources.o ui_builder.o ui_event.o world_vision.o
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
