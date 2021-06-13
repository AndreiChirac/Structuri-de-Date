#include "arbori_intervale.h"

Nod *construieste_nod(int capat_stanga , int capat_dreapta){

	Nod * noulNod = (Nod *)malloc(sizeof(Nod)); 
	noulNod->info = 0;
	Interval *nouInterval = malloc(sizeof(Interval)); 
	nouInterval->capat_stanga = capat_stanga;
	nouInterval->capat_dreapta = capat_dreapta;
	noulNod->interval = nouInterval;	
	noulNod->copil_stanga = NULL;
	noulNod->copil_dreapta =  NULL;

	if( capat_stanga < capat_dreapta )
		{
			int mijloc = (capat_stanga + capat_dreapta) / 2;
			noulNod->copil_stanga =  construieste_nod(capat_stanga,mijloc);
			noulNod->copil_dreapta = construieste_nod(mijloc+1,capat_dreapta);
		}
	
	return noulNod;	
}

int afla_dimenisune(Nod * radacina){
	if(radacina == NULL)
		return 0 ;
	return(afla_dimenisune(radacina->copil_stanga) + 1 + 
			afla_dimenisune(radacina->copil_dreapta));

}

ArboreDeIntervale *construieste_arbore(int capat_stanga, int capat_dreapta,
        int valoare_predifinita_raspuns_copil,
        void (*f_actualizare)(Nod *nod, int v_actualizare),
        int (*f_combinare_raspunsuri_copii)(int raspuns_stanga, int raspuns_dreapta)) {
    
	ArboreDeIntervale * noulAdi = malloc(sizeof(ArboreDeIntervale));
	
	noulAdi->radacina = construieste_nod(capat_stanga,capat_dreapta);
	noulAdi->dimensiune = afla_dimenisune(noulAdi->radacina);
	noulAdi->valoare_predifinita_raspuns_copil = valoare_predifinita_raspuns_copil;
	noulAdi->f_actualizare = f_actualizare;
	noulAdi->f_combinare_raspunsuri_copii = f_combinare_raspunsuri_copii;
    
    return noulAdi;
}


void actualizare_interval_pentru_nod(ArboreDeIntervale *arbore, Nod *nod,
                                        Interval *interval, int v_actualizare) {
   
	if( interval->capat_stanga <= nod->interval->capat_stanga && 
   								nod->interval->capat_dreapta <= interval->capat_dreapta){
   		arbore->f_actualizare(nod,v_actualizare);
	}	
   	else
   	{
   		int mijloc = (nod->interval->capat_stanga + nod->interval->capat_dreapta) /2 ;
   		
   		if( interval->capat_stanga <= mijloc )
   			actualizare_interval_pentru_nod(arbore,nod->copil_stanga,interval,v_actualizare);
   		
   		if ( mijloc < interval->capat_dreapta )
   			actualizare_interval_pentru_nod(arbore,nod->copil_dreapta,interval,v_actualizare);
   		
   		arbore->f_actualizare(nod,v_actualizare);
   	}

    // TODO implementati functia de actualizare pentru un nod
    // Hint: pentru a actualiza un nod cand este cazul folositi arbore->f_actualizare(nod, v_actualizare);
}


// Functia este deja implementata, se cheama functia de mai sus cu radacina arborelui
void actualizare_interval_in_arbore(ArboreDeIntervale *arbore, 
                                        Interval *interval, int v_actualizare) {
    actualizare_interval_pentru_nod(arbore, arbore->radacina, interval, v_actualizare);
}


int interogare_interval_pentru_nod(ArboreDeIntervale *arbore, Nod *nod, Interval *interval) {

	int raspuns_copil_stanga=0,raspuns_copil_drepta = 0 ;

	if( interval->capat_stanga <= nod->interval->capat_stanga &&
								nod->interval->capat_dreapta <= interval->capat_dreapta){
		return nod->info;
	}
	else
		{
			int mijloc = (nod->interval->capat_stanga + nod->interval->capat_dreapta) / 2;
			
			if( interval->capat_stanga <= mijloc )
				raspuns_copil_stanga = interogare_interval_pentru_nod(arbore,nod->copil_stanga,interval);
			
			if( mijloc < interval->capat_dreapta )
				raspuns_copil_drepta = interogare_interval_pentru_nod(arbore,nod->copil_dreapta,interval);
			
			return(arbore->f_combinare_raspunsuri_copii(raspuns_copil_stanga,raspuns_copil_drepta));
		}
    // TODO implementati functia de interogare pentru un nod prezentata in enunt
    // Hint: cand vreti sa calculati combinarea raspunsurilor folositi
    // arbore->f_combinare_raspunsuri_copii(raspuns_copil_stanga, raspuns_copil_drepta)
}


// Functia este deja implementata, se cheama functia de mai sus cu radacina arborelui
int interogare_interval_in_arbore(ArboreDeIntervale *arbore, Interval *interval) {
    return interogare_interval_pentru_nod(arbore, arbore->radacina, interval);
}



// ----- DOAR pentru bonus si DOAR daca considerati ca e necesara ----- //
void seteaza_info_in_nod_la_valoare_capat_dreapta(Nod* nod) {
    // TODO cred ca e destul de clar ce vrea sa faca functia asta
}


void seteaza_info_in_arbore_la_valoare_capat_dreapta(ArboreDeIntervale* arbore) {
    seteaza_info_in_nod_la_valoare_capat_dreapta(arbore->radacina);
}
