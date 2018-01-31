#include <uARMconst.h>
#include "const.h"
#include "pcb.h"

pcb_t pcbFree_table[MAXPROC];
pcb_t *pcbfree_h;
/*
void initPcbs(){
    pcb_t *temp;
    int i; 
    pcbfree_h -> p_next = pcbFree_table[0];
    temp = pcbFree_table[0];
    for(i = 1; i < MAXPROC; i++){
        temp -> p_next = pcbFree_table[i];
        temp = temp -> p_next;
    }  
}*/

void initPcbs(){
	int size = sizeof(pcbFree_table)/sizeof(pcbfree_h);
	if (size < 20) {
		initPcbs();
		*(pcbFree_table[size]).p_next = pcbFree_table[size+1];
		//*(pcbFree_table[size]).p_parent = NULL;
		//*(pcbFree_table[size]).p_first_child = NULL;
		//*(pcbFree_table[size]).p_sib = NULL;
	}
}
/*
void freePcb(pcb_t *p){
	int size = sizeof(pcbFree_table)/sizeof(pcbfree_h);
	if (size < 20) {
		if (*(pcbFree_table[size]).p_first_child != NULL) freePcb(p);
		else *(pcbFree_table[size]).p_first_child = p;
	}
}*/
/*
pcb_t *allocPcb(){
	
}
*/
