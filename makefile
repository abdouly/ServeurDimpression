CFLAGS=-Wall -g
WORK= serveur client
.PHONY: all clean

all: $(WORK)

serveur: serveurDImpression.o communication/communication.o cups_filters.o
	$(CC) $(CFLAGS) $(LFLAGS) -o $@ $^ -g -lpthread 
client: machines_sites.o communication/communication.o
	$(CC) $(CFLAGS) $(LFLAGS) -o $@ $^ -g  -lpthread

clean:
	rm -f $(WORK) serveur client *.o communication/*.o