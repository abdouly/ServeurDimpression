CFLAGS=-Wall -g
WORK= serveur client imprimante
.PHONY: all clean

all: $(WORK)

serveur: serveurDImpression.o communication/communication.o cups_filters.o
	$(CC) $(CFLAGS) $(LFLAGS) -o $@ $^ -g -lpthread 
client: machines_sites.o communication/communication.o
	$(CC) $(CFLAGS) $(LFLAGS) -o $@ $^ -g -lpthread
imprimante: imprimante_distante.o communication/communication.o
	$(CC) $(CFLAGS) $(LFLAGS) -o $@ $^ -g
clean:
	rm -f $(WORK) serveur client imprimante *.o communication/*.o
