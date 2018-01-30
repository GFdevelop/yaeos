
// Esempio di utilizzo delle liste del Kernel di Linux

#include <stdio.h>
#include <stdlib.h>
#include "listx.h"

//Elemento della lista 
typedef struct kitem {
	// Dato della lista: in questo caso, un intero 
	int elem;
	// Concatenatore della lista
	struct list_head list;

} kitem_t;



int main() {

	// Allocazione di 2 strutture dati di tipo kitem_t
	kitem_t* elem1=(kitem_t*)malloc(sizeof(kitem_t));
	kitem_t* elem2=(kitem_t*)malloc(sizeof(kitem_t));
	
	// Contenuto dei campi dati
	elem1->elem=1;
	elem2->elem=2;
	
	// Definizione elemento sentinella
	struct list_head head;
	
	//Inizializzazione della lista 
	INIT_LIST_HEAD(&head);
	
	// Test di lista vuota
	printf("Lista vuota? %d \n",list_empty(&head));

	//Aggiunta di elementi kitem_t alla lista
	list_add(&elem1->list,&head);
	list_add(&elem2->list,&head);
	
	// Test di lista vuota
	printf("Lista vuota? %d \n",list_empty(&head));
	
	// Scorrimento della lista: Metodo1: list_for_each + container_of
	struct list_head* iter;
	list_for_each(iter,&head) {
		// In iter si trova il puntatore al campo list dell'elemento corrente
		// E' necessario usare la container_of per ottenere il puntatore alla struttura kitem_t che contiene list
		kitem_t* item=container_of(iter,kitem_t,list);
		printf("Elemento i-esimo %d \n",item->elem);
	}
	

	//Scorrimento della lista: Metodo 2: list_for_each_entry
	kitem_t* item;
	list_for_each_entry(item,&head,list) {
		// item contiene il puntatore all'elemento corrente di tipo kitem_t
		printf("Elemento i-esimo %d \n",item->elem);
	}
	
	//Rimozione di elementi dalla lista
	list_del(&elem1->list);
	list_del(&elem2->list);
	
	// Test di lista vuota
	printf("Lista vuota? %d \n",list_empty(&head));
	return 1;
}
