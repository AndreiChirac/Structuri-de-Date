#ifndef _INTERSECTII_H_
#define _INTERSECTII_H_

#include <stdio.h>
#include <stdlib.h>
#include "arbori_intervale.h"


typedef struct punct {
    int x;
    int y;
} Punct2D;


typedef struct interval_2d {
    Punct2D *punct_stanga;
    Punct2D *punct_dreapta;
} Interval2D;


typedef struct lista_intervale {
    int dimensiune;
    Interval2D **intervale; 
} ListaIntervale2D;


typedef struct interval_2d_nou {
    Punct2D *punct_stanga;
    Punct2D *punct_dreapta;
    int ok;
} Interval2DNou; // strtuctura noua care sa imi contina un evidentiator pentru a sti daca este un capatul stanga sau dreapta al unui segment 


typedef struct lista_intervale_nou {
    int dimensiune;
    Interval2DNou **intervale; 
} ListaIntervale2DNou;



// functie care citeste intervalele dintr-un fisier si le intoarce intr-un
// pointer la structura ListaIntervale2D
ListaIntervale2D* citeste_intrare(char *nume_fisier_intrare);

// funcție care calculeaza numarul de intersectii folosind
// ! arbori de intervale !
int calculeaza_numar_intersectii(ListaIntervale2D *lista);

// functie care calculeaza numarul de intersectii in varianta triviala
int calculeaza_numar_intersectii_trivial(ListaIntervale2D *lista);

// functie ajutatoare pentru afisare
void afiseaza_lista_intervale(ListaIntervale2D *lista);


#endif