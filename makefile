CFLAGS=-Wall -g
WORK= serv
.PHONY: all clean

all: $(WORK)

serv: serveurDImpression.o communication/communication.o cups_filters.o
	$(CC) $(CFLAGS) $(LFLAGS) -o $@ $^ -g -lpthread 

clean:
	rm -f $(WORK) serv *.o communication/*.o