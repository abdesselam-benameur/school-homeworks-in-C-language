#ifndef FONCTIONS_H_INCLUDED
#define FONCTIONS_H_INCLUDED
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int encrypter(char* cle, char* fichier, char* fichier_enc);
int decrypter(char* cle, char* fichier, char* fichier_dec);
void decryptage_noraml(unsigned char* c, char cle, unsigned char secret, int i, int j);
int xor_cle(char* cle);
void menu(void);
int comparer(char* fic1, char* fic2);
void viderBuffer(void);
int lire(char *chaine, int longueur);
void menu(void);
void bienvenue(void);

#endif // FONCTIONS_H_INCLUDED
