CC           =  pgCC
CFLAGS       = -c -ta=tesla,cc30 -O3 -Minfo=accel -Mprof=lines
LDFLAGS      = -fast -acc -lrt -lm -ta=tesla,cc30 -Mprof=lines

PROGRAM_NAME = mandelbox
OBJS         = main.o print.o timing.o savebmp.o getparams.o 3d.o raymarching.o renderer.o init3D.o

$(PROGRAM_NAME): $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o $@

.cc.o:
	$(CC) $(CFLAGS) $?

clean:
	rm *.o $(PROGRAM_NAME) $(EXEEXT) *~
