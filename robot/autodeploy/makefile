CC=clang
CFLAGS+= -g `pkg-config --cflags opencv` -I/usr/include/libevdev-1.0
LIBS=`pkg-config --libs opencv` -lev -lm
ODIR=obj
DEPS= Angle.h BehaviorBuilder.h Behavior.h CommunicationStructures.h Coordinates.h Flags.h Map.h network.h Object.h Objective.h Obstacle.h Pose.h Robot.h RobotReceiver.h RobotServer.h Sensor.h State.h Wheels.h WorldCamera.h
_OBJ= robot-main.o Angle.o BehaviorBuilder.o Behavior.o CommunicationStructures.o Coordinates.o Flags.o Map.o network.o Object.o Objective.o Obstacle.o Pose.o Robot.o RobotReceiver.o RobotServer.o Sensor.o State.o Wheels.o WorldCamera.o
OBJ=$(patsubst %,$(ODIR)/%,$(_OBJ))

-include $(OBJ:.o=.d)

$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)
	$(CC) -MM $(CFLAGS) $*.c > $(ODIR)/$*.d
	@cp -f $(ODIR)/$*.d $(ODIR)/$*.d.tmp
	@sed -e 's/.*://' -e 's/\\$$//' < $(ODIR)/$*.d.tmp | fmt -1 | \
	  sed -e 's/^ *//' -e 's/$$/:/' >> $(ODIR)/$*.d
	@rm -f $(ODIR)/$*.d.tmp

$(ODIR)/%.o: %.h $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)
	$(CC) -MM $(CFLAGS) $*.h > $(ODIR)/$*.d
	@cp -f $(ODIR)/$*.d $(ODIR)/$*.d.tmp
	@sed -e 's/.*://' -e 's/\\$$//' < $(ODIR)/$*.d.tmp | fmt -1 | \
	  sed -e 's/^ *//' -e 's/$$/:/' >> $(ODIR)/$*.d
	@rm -f $(ODIR)/$*.d.tmp

robot-main: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

all: robot-main 

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o $(ODIR)/*.d
	rm -f robot-main
