//
//  test.c
//  
//
//  Created by Pavin Disatapundhu on 12/5/14.
//
//

#include <stdio.h>
#include <stdlib.h>

#define MAXSIZE 

void xmlphrase(char *msend, char *proc);
int main(){
    char mrecv[MAXSIZE];
    char *proc = malloc(sizeof(char) * 10);;
    sprintf(proc,"%s","Monitor");
    xmlphrase(mrecv, proc);
    printf("%s\n",mrecv);
}

void xmlphrase(char *msend, char *proc){
    sprintf(msend, "<?xml version='1.0' encoding='UTF-8'?><compute><type>%s</type></compute>", proc);
}