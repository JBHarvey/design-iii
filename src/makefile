CC=clang
CFLAGS=`pkg-config --cflags gtk+-3.0`
LIBS=`pkg-config --libs gtk+-3.0`
ODIR=obj
DEPS=
_OBJ = station-main.o ui_app.o ui_window.o resources.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

station-main: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o
	rm -f station-main
