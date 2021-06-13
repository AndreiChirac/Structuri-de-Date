#include "intersectii.h"


// functie ajutatoare
void afiseaza_lista_intervale(ListaIntervale2D *lista) {
    int i;
    for (i = 0; i < lista->dimensiune; i++) {
        Interval2D *interv = lista->intervale[i];
        printf("{punct stanga x: %d, punct stanga y: %d, punct dreapta x: %d, punct dreapta y: %d}\n",
                    interv->punct_stanga->x, interv->punct_stanga->y,
                    interv->punct_dreapta->x, interv->punct_dreapta->y);
    }
}

void afiseaza_lista_intervaleNou(ListaIntervale2DNou *lista) {
    int i;
    for (i = 0; i < lista->dimensiune; i++) {
        Interval2DNou *interv = lista->intervale[i];
        printf("{punct stanga x: %d, punct stanga y: %d, punct dreapta x: %d, punct dreapta y: %d}\n",
                    interv->punct_stanga->x, interv->punct_stanga->y,
                    interv->punct_dreapta->x, interv->punct_dreapta->y);
        printf(" %d \n", interv->ok );
    }
}

ListaIntervale2D* citeste_intrare(char *nume_fisier_intrare) {
	
	ListaIntervale2D *nouaLista = malloc(sizeof(ListaIntervale2D)); // se aloca o lista 

	FILE *fisier;
	
	fisier = fopen(nume_fisier_intrare,"r");

	int nrSegmente,x1,y1,x2,y2;

	fscanf(fisier,"%d",&nrSegmente);	
	nouaLista->dimensiune = nrSegmente;
	nouaLista->intervale  = malloc(nrSegmente * sizeof(Interval2D)); // se aloca dinamoic " o matrice " de intervale
	
	for(int i = 0 ;i < nrSegmente ; i++)
		{
		
			fscanf(fisier,"%d %d %d %d",&x1,&y1,&x2,&y2); 
			
			nouaLista->intervale[i] = malloc(sizeof(Interval2D)); // se aloca pt fiecare interval in parte memorie 
			Interval2D *interv = nouaLista->intervale[i];
				
			interv->punct_stanga = malloc(sizeof(Punct2D)); // se aloca memoreie pentru structura care contine cordonatele stanga
			interv->punct_dreapta = malloc(sizeof(Punct2D)); // se aloca memoreie pentru structura care contine cordonatele dreapta

			interv->punct_stanga->x = x1; 
			interv->punct_stanga->y = y1;
	        interv->punct_dreapta->x = x2;
	        interv->punct_dreapta->y = y2;		
		
		}	

	fclose(fisier);	
    // TODO citeste intervalele din fisierul dat ca parametru
    // si le salveaza in lista de intervale
    return nouaLista;
}

// ! Functie pentru actualizarea in arbore
// ! O veti da ca parametru cand initializati arborele
void actualizare_cu_delta(Nod *nod, int v2) {
    nod->info += v2;
}

// ! Functie pentru combinarea raspunsurilor in arbore
// ! O veti da ca parametru cand initializati arborele
int suma_raspunsurilor(int r1, int r2) {
    return r1 + r2;
}


int y_maxim(ListaIntervale2D *lista){
	
	int y_max = -100000;

    for(int i = 0 ; i < lista->dimensiune ; i++) // se determina cea mai mare valoare de pe axa OY 
    	{
	    	if( lista->intervale[i]->punct_stanga->y > y_max )
	    		{
	    			y_max = lista->intervale[i]->punct_stanga->y;
	    		}
	    	if( lista->intervale[i]->punct_dreapta->y > y_max )
	    		{
	    			y_max = lista->intervale[i]->punct_dreapta->y;
	    		}
    	}	
	
	return y_max;
}

ListaIntervale2DNou * creazanouaLista( ListaIntervale2D *lista ){ // functie care creaza o noua lista care are in componeta ei lista data de doua ori 

	ListaIntervale2DNou * listaNoua = malloc(sizeof(ListaIntervale2DNou));

	listaNoua->dimensiune = ( lista->dimensiune ) * 2 ;
	listaNoua->intervale  = malloc( listaNoua->dimensiune * sizeof(Interval2DNou) );

	for( int i = 0 ;i < listaNoua->dimensiune ; i++ )
		{

			listaNoua->intervale[i] = malloc(sizeof(Interval2DNou));
			
			listaNoua->intervale[i]->punct_stanga = malloc(sizeof(Punct2D));
			listaNoua->intervale[i]->punct_dreapta = malloc(sizeof(Punct2D));

			listaNoua->intervale[i]->punct_stanga = lista->intervale[i%lista->dimensiune]->punct_stanga; // noua lista are dimensiune dubla asa ca ma voi folosi de impartirea cu rest
			listaNoua->intervale[i]->punct_dreapta = lista->intervale[i%lista->dimensiune]->punct_dreapta; 

			if( i >= lista->dimensiune ) // daca deja am trecut de "prima lista" voi modifica structura astfel incat componetna dreapta sa devina componenta stanga
				{

					listaNoua->intervale[i]->ok = 1 ; // noua lista contine un nou camp care imi va spune ca acesta este capatul dreapta al segmentului

					Punct2D *aux = listaNoua->intervale[i]->punct_stanga;
					listaNoua->intervale[i]->punct_stanga = listaNoua->intervale[i]->punct_dreapta;
					listaNoua->intervale[i]->punct_dreapta = aux;
					
				}
			else
				{
					
					listaNoua->intervale[i]->punct_stanga = lista->intervale[i%lista->dimensiune]->punct_stanga;
					listaNoua->intervale[i]->punct_dreapta = lista->intervale[i%lista->dimensiune]->punct_dreapta;

					listaNoua->intervale[i]->ok = 0; // noua lista contine un nou camp care imi va spune ca acesta este capatul stanga al segmentului
 
				}
		}

	return listaNoua; 
}

ListaIntervale2DNou * sorteazalista( ListaIntervale2DNou * lista ){

	for(int i = 0 ; i < lista->dimensiune-1 ; i++)
		
		for(int j = i+1 ; j<lista->dimensiune ; j++)
			
			if(lista->intervale[j]->punct_stanga->x < lista->intervale[i]->punct_stanga->x)
				{
					
					Interval2DNou * aux  = lista->intervale[i];
					lista->intervale[i] = lista->intervale[j];
					lista->intervale[j] = aux;
				
				}

	for(int i = 0 ; i < lista->dimensiune ; i++)
		{
			if( lista->intervale[i]->ok == 1 ) // daca am dat de un capat de final imi voi interschimba capetel pentru a obtine forma initiala 
				{
					
					Punct2D *aux = lista->intervale[i]->punct_stanga;
					lista->intervale[i]->punct_stanga = lista->intervale[i]->punct_dreapta;
					lista->intervale[i]->punct_dreapta = aux;
				
				}	
		}		
	
	return lista;		
}

int calculeaza_numar_intersectii(ListaIntervale2D *lista) {
       
    ListaIntervale2DNou * listaN = creazanouaLista(lista);
    sorteazalista(listaN);
    
    int y_max = y_maxim(lista);

    ArboreDeIntervale *arbore = construieste_arbore(0, y_max, 0, actualizare_cu_delta, suma_raspunsurilor);
    
    int v_actualizazre;
    int cnt = 0 ;

	for( int i = 0 ; i < listaN->dimensiune ; i++ )
		{

			if( listaN->intervale[i]->punct_stanga->x == listaN->intervale[i]->punct_dreapta->x && listaN->intervale[i]->ok == 1 ); // penrtu ca in lista mea se afla 2 * segemnte vertical le voi lua in considerare doar  prima aparitie 

			else
				{	
					
					Interval2DNou *interv = listaN->intervale[i];
					Interval * intervalAux = malloc(sizeof(Interval)); 

					if(	interv->punct_stanga->y == interv->punct_dreapta->y ) // am dat de un capat stanga asa ca voi realiza actualizarea in arbore adaugand 1 
						{	

							if( interv->ok == 0 )
								{
									intervalAux->capat_stanga = interv->punct_stanga->y;
									intervalAux->capat_dreapta = interv->punct_dreapta->y;

									v_actualizazre = 1;
									actualizare_interval_in_arbore(arbore,intervalAux,v_actualizazre);
								
								}	
							else // am dat de un capat dreapta asa ca voi "scoate" intervalul din arbore actualizand cu -1 
								{
									intervalAux->capat_stanga = interv->punct_stanga->y;
									intervalAux->capat_dreapta = interv->punct_dreapta->y;
									
									v_actualizazre = -1;
									actualizare_interval_in_arbore(arbore,intervalAux,v_actualizazre);
									
								}
						}			
					else if ( interv->punct_stanga->x == interv->punct_dreapta->x ) // am dat de un segment vertical ceea ce presupune ca trebuie sa vad cu cate segmente orizontale se intersecteaza
						{
							
							intervalAux->capat_stanga = interv->punct_stanga->y;
							intervalAux->capat_dreapta = interv->punct_dreapta->y;
							
							cnt = cnt + interogare_interval_in_arbore(arbore,intervalAux); // prin procesul de interogare aflu cu cate segmete se intersectea 

						} 
					
					}
				
		}			
    
    return cnt;
}


int calculeaza_numar_intersectii_trivial(ListaIntervale2D *lista) {
    
    int cnt = 0 ;

    for(int i = 0 ; i < lista->dimensiune ; i++ )
    	{
	    	for(int j = i+1 ; j < lista->dimensiune ; j++ )
	    		{
	    			
		    		if(lista->intervale[i]->punct_stanga->x == lista->intervale[i]->punct_dreapta->x) // am gasit o drepata verticala asa ca ma voi uita doar la dreptele orizontale 
		    			{
		    				if( lista->intervale[j]->punct_stanga->y == lista->intervale[j]->punct_dreapta->y &&  // astfel pentru ca doua drepte sa se intersecteze trebuie ca dreapta orizontala sa contina cordonate x care 
		    					lista->intervale[j]->punct_stanga->x <= lista->intervale[i]->punct_stanga->x &&   // sa cuprinda cordonata x a segmentului vertical  , iar y ul segemntului orizontal sa fie inclus intre valorile
								lista->intervale[j]->punct_dreapta->x >= lista->intervale[i]->punct_stanga->x &&  // de tip y ale segmentului vertical   					
								lista->intervale[i]->punct_stanga->y <= lista->intervale[j]->punct_stanga->y  &&
								lista->intervale[i]->punct_dreapta->y >= lista->intervale[j]->punct_dreapta->y )
		    						{
		    						
		    							cnt++;
		    						
		    						} 
		    			}

					if(lista->intervale[i]->punct_stanga->y == lista->intervale[i]->punct_dreapta->y) // am gasit o drepata orizontale asa ca ma voi uita doar la dreptele verticale   			
		    			{
		    				if( lista->intervale[j]->punct_stanga->x == lista->intervale[j]->punct_dreapta->x && // astfel pentru ca doua drepte sa se intersecteze trebuie ca dreapta orizontala sa contina cordonate x care 
								lista->intervale[j]->punct_stanga->y <= lista->intervale[i]->punct_stanga->y &&	 // sa cuprinda cordonata x a segmentului vertical  , iar y ul segemntului orizontal sa fie inclus intre valorile	
								lista->intervale[j]->punct_dreapta->y >= lista->intervale[i]->punct_stanga->y && // de tip y ale segmentului vertical    					
								lista->intervale[i]->punct_stanga->x <= lista->intervale[j]->punct_stanga->x  &&
								lista->intervale[i]->punct_dreapta->x >= lista->intervale[j]->punct_dreapta->x	)
		    						{

		    							cnt++;

		    						}	  
		    					
		    					
		    			}
	    		}
    	}
    
    return cnt;
}
