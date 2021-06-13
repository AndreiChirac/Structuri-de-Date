#include "pozitie_libera.h"


DateIntrare* citeste_fisier_in(char *nume_fisier_intrare) {
    
    DateIntrare *date_noi = malloc(sizeof(DateIntrare)); // se aloca o structura de tipul DateIntrare

    FILE *fisier;
	
	fisier = fopen(nume_fisier_intrare,"r");
    
    int nr_max=0,nr_elemente=0;

    fscanf(fisier,"%d %d \n",&nr_max , &nr_elemente);

    date_noi->numar_intrari = nr_elemente;
    date_noi->numarul_maxim_din_intrari = nr_max;

    date_noi->intrari = (int *)malloc( ( nr_elemente ) * sizeof(int) );

    for( int i = 0 ; i < nr_elemente ; ++i )
   		{
   			int elem; 
   			
   			fscanf(fisier,"%d",&elem);
   		
   			date_noi->intrari[i] = elem;

   		}	
   	
   	fclose(fisier);

    return date_noi;
}


int* calculeaza_vector_raspuns(DateIntrare *date_intrare) {
    // TODO creeaza vectorul raspuns folosind arbori de intervale
    return date_intrare->intrari;
}


int* calculeaza_vector_raspuns_trivial(DateIntrare *date_intrare) {
    
	
    DateIntrare *date_noi = calloc(1,sizeof(DateIntrare));
	
	int max = date_intrare->numarul_maxim_din_intrari;
	int nr_intrari = date_intrare->numar_intrari;
	
	date_noi->intrari = calloc( ( max + 1 ) , sizeof(int) ); // am folosit calloc pentru a putea stii care pozitie e ocupata si care nu	    

	for( int i = 0 ; i < nr_intrari ; i++ )
    	{
    		int elem = date_intrare->intrari[i];

    		if( date_noi->intrari[elem] == 0 ) 
    			date_noi->intrari[elem] = elem;    			
    		else
    			{

    				for( int k = elem-1 ; k >= 0 ; k-- )
    					if( date_noi->intrari[k] == 0 )
    						{
    							date_noi->intrari[k] = elem;
    							break; // am luat break pentru a nu mi pune pe toate pozitiile elem 
    						}
    			}
    			
    	}
	
    return date_noi->intrari ;
}