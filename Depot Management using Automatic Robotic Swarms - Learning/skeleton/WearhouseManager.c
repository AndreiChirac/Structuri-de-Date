#include <stdio.h>
#include "WearhouseManager.h"


Package *create_package(long priority, const char* destination){
	Package *newPackage = malloc(sizeof(Package));
	newPackage->priority = priority ;
	if(destination != NULL)
		{
			newPackage->destination = malloc(strlen(destination)*sizeof(char));
			strcpy(newPackage->destination,destination);
		}
	else
		newPackage->destination = NULL;
	return newPackage;
}

void destroy_package(Package* package){
	if(package == NULL)
		return;
	if(package->destination != NULL)
		free(package->destination);
	free(package);
}

Manifest* create_manifest_node(void){
	Manifest *newManifestNode = malloc(sizeof(Manifest));
	newManifestNode->package = NULL;
	newManifestNode->next = NULL;
	newManifestNode->prev = NULL; 
	return newManifestNode;
}

void destroy_manifest_node(Manifest* manifest_node){
	if(manifest_node ==NULL)
		return;
	free(manifest_node);
}

Wearhouse* create_wearhouse(long capacity){
	if(capacity == 0 )
		return NULL;
	Wearhouse * newWearhouse = malloc(sizeof(Wearhouse));
	newWearhouse->size = 0 ;
	newWearhouse->capacity = capacity ;
	newWearhouse->packages = malloc(capacity * sizeof(Package)); 
	return newWearhouse;
}

Wearhouse *open_wearhouse(const char* file_path){
	ssize_t read_size;
	char* line = NULL;
	size_t len = 0;
	char* token = NULL;
	Wearhouse *w = NULL;


	FILE *fp = fopen(file_path, "r");
	if(fp == NULL)
		goto file_open_exception;

	if((read_size = getline(&line, &len, fp)) != -1){
		token = strtok(line, ",\n ");
		w = create_wearhouse(atol(token));

		free(line);
		line = NULL;
		len = 0;
	}

	while((read_size = getline(&line, &len, fp)) != -1){
		token = strtok(line, ",\n ");
		long priority = atol(token);
		token = strtok(NULL, ",\n ");
		Package *p = create_package(priority, token);
		w->packages[w->size++] = p;

		free(line);
		line = NULL;
		len = 0;
	}

	free(line);


	fclose(fp);
	return w;

	file_open_exception:
	return NULL;
}

long wearhouse_is_empty(Wearhouse *w){
	if(w == NULL)
		return 0;
	if(w->size == 0)
		return 1;
	return 0;
}

long wearhouse_is_full(Wearhouse *w){
	if(w == NULL)
		return 0;
	if(w->size == w->capacity)
		return 1;
	return 0;
}	

long wearhouse_max_package_priority(Wearhouse *w){
	long max =-1; 
	for(int i = 0 ; i < w->size ; i++)
		if( w->packages[i]->priority > max )
			max = w->packages[i]->priority;
	return max;
}

long wearhouse_min_package_priority(Wearhouse *w){
	long min = 100000000;
	for(int i = 0 ; i < w->size ; i++)
		if( w->packages[i]->priority < min )
			min = w->packages[i]->priority ;	
	return min;
}


void wearhouse_print_packages_info(Wearhouse *w){
	for(long i = 0; i < w->size; i++){
		printf("P: %ld %s\n",
				w->packages[i]->priority,
				w->packages[i]->destination);
	}
	printf("\n");
}

void destroy_wearhouse(Wearhouse* wearhouse){
	if(wearhouse == NULL)
		return;;
	for(int i = 0 ; i < wearhouse->size ; i++)
		free(wearhouse->packages[i]);
	free(wearhouse->packages);
	free(wearhouse);
}


Robot* create_robot(long capacity){
	Robot *newRobot = malloc(sizeof(Robot));
	newRobot->size = 0 ;
	newRobot->capacity = capacity;
	newRobot->next = NULL;
	newRobot->manifest = NULL;
	return newRobot;
}

int robot_is_full(Robot* robot){
	if(robot->size == robot->capacity)
		return 1;
	return 0;
}

int robot_is_empty(Robot* robot){
	if(robot->size == 0)
		return 1;
	return 0;
}

Package* robot_get_wearhouse_priority_package(Wearhouse *w, long priority){
	for(int i = 0 ; i < w->size ; i++)
		if( w->packages[i]->priority == priority)
			return w->packages[i];
	return NULL;
}

void robot_remove_wearhouse_package(Wearhouse *w, Package* package){
	int k;
	for(int i = 0 ; i < w->size ; i++)
		{
			if( w->packages[i] ==  package )
				{
					k=i;
					break;
				}
		}
	for(int i = k ; i < w->size-1 ; i++)	
		w->packages[i] = w->packages[i+1] ; 	
	w->size = w->size - 1 ; 
}

void robot_load_one_package(Robot* robot, Package* package){
	
	if(robot == NULL ) 	
		return;
	if(package == NULL )
		return;
	if(robot_is_full(robot) == 1)
		return;

	Manifest *newManifestNode = create_manifest_node();
	newManifestNode->package =	package;

	Manifest *current = robot->manifest;

	if(robot->manifest == NULL)
		{
			robot->manifest = newManifestNode;
			robot->size = robot->size + 1;
			return;
		}

	while(	package->priority <	current->package->priority && current->next != NULL )
			current = current->next;
	
	if( package->priority == current->package->priority )
		{
			while(strcmp(package->destination,current->package->destination) > 0 && current->next != NULL)
					current = current->next;	
			if(	current->prev == NULL && strcmp(package->destination,current->package->destination) <= 0)
				{
					newManifestNode->next = current;
					current->prev = newManifestNode;
					robot->manifest = newManifestNode;		
					robot->size = robot->size + 1;
					return; 
				}
			if(current->next == NULL && strcmp(package->destination,current->package->destination) > 0 )
				{
					newManifestNode->prev = current;
					current->next = newManifestNode;
					robot->size = robot->size + 1;
					return;
				}	
			
			newManifestNode->next = current;
			newManifestNode->prev = current->prev;
			current->prev->next = newManifestNode;
			current->prev =newManifestNode;
			robot->size = robot->size + 1;
			return;			
		}	
	
	if(current->prev == NULL && package->priority > current->package->priority ) 
		{
			newManifestNode->next = current;
			current->prev = newManifestNode;
			robot->manifest = newManifestNode;		
			robot->size = robot->size + 1;
			return;
		}		
	if( current->next == NULL && package->priority < current->package->priority )
		{
			newManifestNode->prev = current;
			current->next = newManifestNode;
			robot->size = robot->size + 1;
			return;
		}
	
	newManifestNode->next = current;
	newManifestNode->prev = current->prev;
	current->prev->next = newManifestNode;
	current->prev =newManifestNode;
	robot->size = robot->size + 1;
	return;	
}

/*
Package* robot_get_wearhouse_Maxpriority_package(Wearhouse *w, long priority){
	
	if(w == NULL || w->size == 0 )
		return NULL;

	Package *auxPackage = w->packages[0];
	int i=0;
	for( i = 0 ; i < w->size ; i++ )
		{
			if( w->packages[i]->priority == priority )
				{
					auxPackage = w->packages[i];
					i++;
					break;	
				}
		}	
	for( int j=i ; j < w->size ; j++ )
		{
			if(strcmp(w->packages[j]->destination,auxPackage->destination) < 0 && w->packages[j]->priority == priority)
				auxPackage = w->packages[j];
		}	
	return auxPackage;
}

*/
long robot_load_packages(Wearhouse* wearhouse, Robot* robot){
	
	if(robot == NULL || wearhouse_is_empty(wearhouse) == 1)
		return 0;

	int cnt = 0 ;
	while(robot_is_full(robot) != 1 )
		{
			if(wearhouse_is_empty(wearhouse) == 1)
				return cnt;
			Package *maxPackage = NULL;
			long maxSize = wearhouse_max_package_priority(wearhouse);
			maxPackage = robot_get_wearhouse_priority_package(wearhouse,maxSize);
			robot_load_one_package(robot,maxPackage);
			robot_remove_wearhouse_package(wearhouse,maxPackage);
			cnt++;
		}	
	return cnt;	
}


Package* robot_get_destination_highest_priority_package(Robot* robot, const char* destination){
	if(robot_is_empty(robot) == 1 )
		return NULL;
	Manifest *current = robot->manifest;
	while( strcmp(current->package->destination,destination) != 0 && current->next != NULL)
		current = current->next;
	return current->package;
}

void destroy_robot(Robot* robot){
	if(robot->manifest == NULL)
		{
			free(robot);
			return;
		}
	Manifest *prev = NULL ;
	Manifest *current = robot->manifest;
	while( current != NULL)
		{
			prev = current;
			current = current->next;
			free(prev);
		}
	free(robot);	
}
/*
void robot_unload_packages(Truck* truck, Robot* robot){
	

	Manifest *prev = NULL;
	Manifest *current = robot->manifest;
	Manifest *currentTruck ;

	while(current != NULL )
		{

			if( strcmp(truck->destination,current->package->destination) == 0 )
				{
					Manifest *newManifestNode = create_manifest_node();
					newManifestNode->package = current->package;
					
					if( truck_is_empty(truck) == 1 )
						{
 							truck->manifest = newManifestNode;
 							currentTruck = truck->manifest;
 							truck->size++;	
						}
					else
						{
							currentTruck->next = newManifestNode;
							newManifestNode->prev = currentTruck;
							truck->size++;
							currentTruck = currentTruck->next;
						}

					if( current->next == NULL && current->prev == NULL )	
						{
							prev = current;
							robot->manifest = NULL;
							robot->size = 0;
							//free(prev);
							return;
						}
					else if( current->prev == NULL )
						{
							prev = current;
							current = current->next;
							current->prev = NULL;
							robot->manifest = current;
							robot->size--;
							//free(prev);
						}
					else if( current->next == NULL)
						{
							prev = current;
							current->prev->next = NULL;
							robot->size--;
							//free(prev);
							return;
						}
					else if( current->next != NULL && current->prev != NULL )
						{
							prev = current;
							current = current->next;
							prev->prev->next = current;
							current->prev = prev->prev;
							robot->size--;
							//free(prev);
						}	
				}
			else
				current = current->next;
		}				
	return;	
}
*/

void robot_unload_packages(Truck* truck, Robot* robot){
	
	if(truck->capacity == 0 || robot->capacity == 0 || robot->size == 0 || truck_is_full(truck) == 1 || truck == NULL || robot->manifest == NULL)
		return;

	Manifest *current = robot->manifest;
		
		while( current->next != NULL )
		{
			if(truck_is_full(truck) == 1)
				return;
			if(robot_is_empty(robot) == 1)
				return;

			if( strcmp(truck->destination,current->package->destination) == 0 )
				{
					Manifest *newManifestNode = create_manifest_node();
					newManifestNode->package = current->package;
			
					if( truck->size == 0 && truck->manifest == NULL )
						{
							truck->manifest = newManifestNode;
							truck->size++;
						}
					else
						{
							newManifestNode->next = truck->manifest;
							truck->manifest = newManifestNode;
							truck->size++;
						}		
					if(current->prev == NULL && current->next == NULL)
						{
							robot->manifest = NULL;
							robot->size--;
							return;
						}
					if( current->prev == NULL )
						{
							current = current->next;
							robot->manifest = current;
							robot->manifest->prev = NULL;
							robot->size--;		
						}
					else if( current->prev != NULL && current->next != NULL )
						{
							current = current->next;
							current->prev->prev->next = current;
							current->prev = current->prev->prev;
							robot->size--;
						}	
				}
			else
				current = current->next;
		}
	if( strcmp(truck->destination,current->package->destination) == 0 )
				{
					Manifest *newManifestNode = create_manifest_node();
					newManifestNode->package = current->package;

					if( truck->size == 0 && truck->manifest == NULL )
						{
							truck->manifest = newManifestNode;
							truck->size++;
						}
					else
						{
							newManifestNode->next = truck->manifest;
							truck->manifest = newManifestNode;
							truck->size++;
						}				
					
					if( current->next == NULL)
						{
							current->prev->next = NULL;
							robot->size--;
							return;
						}
				}
	return;					
}

// Attach to specific truck
int robot_attach_find_truck(Robot* robot, Parkinglot *parkinglot){
	int found_truck = 0;
	long size = 0;
	Truck *arrived_iterator = parkinglot->arrived_trucks->next;
	Manifest* m_iterator = robot->manifest;


	while(m_iterator != NULL){
		while(arrived_iterator != parkinglot->arrived_trucks){
			size  = truck_destination_robots_unloading_size(arrived_iterator);
			if(strncmp(m_iterator->package->destination, arrived_iterator->destination, MAX_DESTINATION_NAME_LEN) == 0 &&
					size < (arrived_iterator->capacity-arrived_iterator->size)){
				found_truck = 1;
				break;
			}

			arrived_iterator = arrived_iterator->next;
		}

		if(found_truck)
			break;
		m_iterator = m_iterator->next;
	}

	if(found_truck == 0)
		return 0;


	Robot* prevr_iterator = NULL;
	Robot* r_iterator = arrived_iterator->unloading_robots;
	while(r_iterator != NULL){
		Package *pkg = robot_get_destination_highest_priority_package(r_iterator, m_iterator->package->destination);
		if(m_iterator->package->priority >= pkg->priority)
			break;
		prevr_iterator = r_iterator;
		r_iterator = r_iterator->next;
	}

	robot->next = r_iterator;
	if(prevr_iterator == NULL)
		arrived_iterator->unloading_robots = robot;
	else
		prevr_iterator->next = robot;

	return 1;
}

void robot_print_manifest_info(Robot* robot){
	Manifest *iterator = robot->manifest;
	while(iterator != NULL){
		printf(" R->P: %s %ld\n", iterator->package->destination, iterator->package->priority);
		iterator = iterator->next;
	}

	printf("\n");
}



Truck* create_truck(const char* destination, long capacity, long transit_time, long departure_time){
	Truck *newTruck = malloc(sizeof(Truck));
	newTruck->manifest = NULL;
	newTruck->unloading_robots = NULL;
	
	if(destination != NULL)
		{
			newTruck->destination = malloc(strlen(destination)*sizeof(char));
			strcpy(newTruck->destination,destination);
		}
	else
		newTruck->destination = NULL;
	
	newTruck->size = 0;
	newTruck->capacity = capacity;
	newTruck->in_transit_time = 0;
	newTruck->transit_end_time = transit_time;
	newTruck->departure_time = departure_time ;
	newTruck->next = NULL;	
	
	return newTruck;
}

int truck_is_full(Truck *truck){
	if(truck->size == truck->capacity)
		return 1;
	return 0;
}

int truck_is_empty(Truck *truck){	
	if(truck->size == 0)
		return 1;
	return 0;
}

long truck_destination_robots_unloading_size(Truck* truck){
	if(truck == NULL || truck->capacity == 0 )
		return 0;

	long cnt = 0;
	
	Robot *firstRobot = truck->unloading_robots;
	Manifest *current ;
	while(firstRobot != NULL)
		{
			current = firstRobot->manifest ; 
			while(current != NULL)
			{
				if( strcmp(truck->destination,current->package->destination) == 0 )
					{
						cnt = cnt + firstRobot->size;
						break;
					}
				current = current->next;
			}
			firstRobot = firstRobot->next;
		}	

	return cnt;
}


void truck_print_info(Truck* truck){
	printf("T: %s %ld %ld %ld %ld %ld\n", truck->destination, truck->size, truck->capacity,
			truck->in_transit_time, truck->transit_end_time, truck->departure_time);

	Manifest* m_iterator = truck->manifest;
	while(m_iterator != NULL){
		printf(" T->P: %s %ld\n", m_iterator->package->destination, m_iterator->package->priority);
		m_iterator = m_iterator->next;
	}

	Robot* r_iterator = truck->unloading_robots;
	while(r_iterator != NULL){
		printf(" T->R: %ld %ld\n", r_iterator->size, r_iterator->capacity);
		robot_print_manifest_info(r_iterator);
		r_iterator = r_iterator->next;
	}
}


void destroy_truck(Truck* truck){
	
	Manifest *manifestCurrent = truck->manifest;
	Manifest *manifestPrev = NULL;
	
	while(manifestCurrent != NULL)
		{
			manifestPrev = manifestCurrent;
			manifestCurrent = manifestCurrent->next;
			free(manifestPrev);
		}

	Robot *robotCurrent = truck->unloading_robots;
	Robot *robotPrev = NULL ;		
	
	while(robotCurrent != NULL)
		{	
			robotPrev = robotCurrent;
			robotCurrent =robotCurrent->next;
			free(robotPrev);
		}

	free(truck);
}


Parkinglot* create_parkinglot(void){
	
	Parkinglot *newParkinglot = malloc(sizeof(Parkinglot));
	newParkinglot->arrived_trucks = create_truck(NULL,1,0,0);
	newParkinglot->arrived_trucks->next = newParkinglot->arrived_trucks;
	newParkinglot->departed_trucks = create_truck(NULL,1,0,0);
	newParkinglot->departed_trucks->next = newParkinglot->departed_trucks;
	newParkinglot->pending_robots =  create_robot(1);
	newParkinglot->pending_robots->next= newParkinglot->pending_robots;
	newParkinglot->standby_robots =  create_robot(1);
	newParkinglot->standby_robots->next = newParkinglot->standby_robots;
	return newParkinglot;
}

Parkinglot* open_parckinglot(const char* file_path){
	ssize_t read_size;
	char* line = NULL;
	size_t len = 0;
	char* token = NULL;
	Parkinglot *parkinglot = create_parkinglot();

	FILE *fp = fopen(file_path, "r");
	if(fp == NULL)
		goto file_open_exception;

	while((read_size = getline(&line, &len, fp)) != -1){
		token = strtok(line, ",\n ");
		// destination, capacitym transit_time, departure_time, arrived
		if(token[0] == 'T'){
			token = strtok(NULL, ",\n ");
			char *destination = token;

			token = strtok(NULL, ",\n ");
			long capacity = atol(token);

			token = strtok(NULL, ",\n ");
			long transit_time = atol(token);

			token = strtok(NULL, ",\n ");
			long departure_time = atol(token);

			token = strtok(NULL, ",\n ");
			int arrived = atoi(token);

			Truck *truck = create_truck(destination, capacity, transit_time, departure_time);

			if(arrived)
				truck_arrived(parkinglot, truck);
			else
				truck_departed(parkinglot, truck);

		}else if(token[0] == 'R'){
			token = strtok(NULL, ",\n ");
			long capacity = atol(token);

			Robot *robot = create_robot(capacity);
			parkinglot_add_robot(parkinglot, robot);

		}

		free(line);
		line = NULL;
		len = 0;
	}
	free(line);

	fclose(fp);
	return parkinglot;

	file_open_exception:
	return NULL;
}

/*

void parkinglot_add_robot(Parkinglot* parkinglot, Robot *robot){

	if(robot->size == 0)
		{
			if(parkinglot->standby_robots->next == parkinglot->standby_robots )
				{
					parkinglot->standby_robots->next = robot;
					robot->next = robot;
					return;
				}	

			Robot *robotPrev = parkinglot->standby_robots;
			Robot *robotCurrent = parkinglot->standby_robots->next;

			while( robotCurrent->next != parkinglot->standby_robots->next && robot->capacity < robotCurrent->capacity)
				{
					robotPrev = robotCurrent;
					robotCurrent = robotCurrent->next;
				}
			
			if( robotCurrent->next == parkinglot->standby_robots->next && robotPrev == parkinglot->standby_robots )
				{
					if( robot->capacity >= robotCurrent->capacity )
						{
							robot->next = robotCurrent ;
							robotCurrent->next = robot;
							parkinglot->standby_robots->next = robot;
							return;
						}
					else
						{
							robotCurrent->next = robot;
							robot->next = robotCurrent;
							return;
						}
				}
			
			if( robotPrev == parkinglot->standby_robots && robot->capacity >= robotCurrent->capacity)	
				{
					while( robotCurrent->next != parkinglot->standby_robots->next )
						robotCurrent = robotCurrent->next;
					robot->next = parkinglot->standby_robots->next;
					robotCurrent->next = robot;
					parkinglot->standby_robots->next = robot;
					return;
				}
			if( robotCurrent->next == parkinglot->standby_robots->next && robot->capacity < robotCurrent->capacity )
				{
					robotCurrent->next = robot;
					robot->next = parkinglot->standby_robots->next;
					return;
				}	
			
			robotPrev->next = robot;
			robot->next = robotCurrent;	
			return;
		}
////////////////////////////////////////////////////////////////////////////
	else
		{
			if(parkinglot->pending_robots->next == parkinglot->pending_robots )
				{
					parkinglot->pending_robots->next = robot;
					robot->next = robot;
					return;
				}	

			Robot *robotPrev = parkinglot->pending_robots;
			Robot *robotCurrent = parkinglot->pending_robots->next;

			while( robotCurrent->next != parkinglot->pending_robots->next && robot->size < robotCurrent->size)
				{
					robotPrev = robotCurrent;
					robotCurrent = robotCurrent->next;
				}
			
			if( robotCurrent->next == parkinglot->pending_robots->next && robotPrev == parkinglot->pending_robots )
				{
					if( robot->size >= robotCurrent->size )
						{
							robot->next = robotCurrent ;
							robotCurrent->next = robot;
							parkinglot->pending_robots->next = robot;
							return;
						}
					else
						{
							robotCurrent->next = robot;
							robot->next = robotCurrent;
							return;
						}
				}
			
			if( robotPrev == parkinglot->pending_robots && robot->size >= robotCurrent->size)	
				{
					while( robotCurrent->next != parkinglot->pending_robots->next )
						robotCurrent = robotCurrent->next;
					robot->next = parkinglot->pending_robots->next;
					robotCurrent->next = robot;
					parkinglot->pending_robots->next = robot;
					return;
				}
			if( robotCurrent->next == parkinglot->pending_robots->next && robot->size < robotCurrent->size )
				{
					robotCurrent->next = robot;
					robot->next = parkinglot->pending_robots->next;
					return;
				}	
			
			robotPrev->next = robot;
			robot->next = robotCurrent;	
			return;
		}
}

*/

void parkinglot_add_robot(Parkinglot* parkinglot, Robot *robot){
	if(parkinglot == NULL || robot == NULL) return;

	if(robot->size == 0)
		{
			if(parkinglot->standby_robots->next == parkinglot->standby_robots )
				{
					parkinglot->standby_robots->next = robot;
					robot->next = parkinglot->standby_robots;
					return;
				}	

			Robot *robotPrev = parkinglot->standby_robots;
			Robot *robotCurrent = parkinglot->standby_robots->next;

			while( robotCurrent->next != parkinglot->standby_robots && robot->capacity < robotCurrent->capacity)
				{
					robotPrev = robotCurrent;
					robotCurrent = robotCurrent->next;
				}
			
			if( robotCurrent->next == parkinglot->standby_robots && robotPrev == parkinglot->standby_robots )
				{
					if( robot->capacity >= robotCurrent->capacity )
						{
							robot->next = robotCurrent ;
							robotCurrent->next = parkinglot->standby_robots;
							parkinglot->standby_robots->next = robot;
							return;
						}
					else
						{
							robotCurrent->next = robot;
							robot->next = parkinglot->standby_robots;
							return;
						}
				}
			
			if( robotPrev == parkinglot->standby_robots && robot->capacity >= robotCurrent->capacity)	
				{
					robot->next = parkinglot->standby_robots->next;
					parkinglot->standby_robots->next = robot;
					return;
				}
			if( robotCurrent->next == parkinglot->standby_robots && robot->capacity <= robotCurrent->capacity )
				{
					robotCurrent->next = robot;
					robot->next = parkinglot->standby_robots;
					return;
				}	
			
			robotPrev->next = robot;
			robot->next = robotCurrent;	
			return;
		}
////////////////////////////////////////////////////////////////////////////
	else
		{
			if(parkinglot->pending_robots->next == parkinglot->pending_robots )
				{
					parkinglot->pending_robots->next = robot;
					robot->next = parkinglot->pending_robots;
					return;
				}	

			Robot *robotPrev = parkinglot->pending_robots;
			Robot *robotCurrent = parkinglot->pending_robots->next;

			while( robotCurrent->next != parkinglot->pending_robots && robot->size < robotCurrent->size)
				{
					robotPrev = robotCurrent;
					robotCurrent = robotCurrent->next;
				}
			
			if( robotCurrent->next == parkinglot->pending_robots && robotPrev == parkinglot->pending_robots )
				{
					if( robot->size >= robotCurrent->size )
						{
							robot->next = robotCurrent ;
							robotCurrent->next = parkinglot->pending_robots;
							parkinglot->pending_robots->next = robot;
							return;
						}
					else
						{
							robotCurrent->next = robot;
							robot->next = parkinglot->pending_robots;
							return;
						}
				}
			
			if( robotPrev == parkinglot->pending_robots && robot->capacity >= robotCurrent->capacity)	
				{
					robot->next = parkinglot->pending_robots->next;
					parkinglot->pending_robots->next = robot;
					return;
				}
			if( robotCurrent->next == parkinglot->pending_robots && robot->size <= robotCurrent->size )
				{
					robotCurrent->next = robot;
					robot->next = parkinglot->pending_robots;
					return;
				}	
			
			robotPrev->next = robot;
			robot->next = robotCurrent;	
			return;
		}

}


void parkinglot_remove_robot(Parkinglot *parkinglot, Robot* robot){
	if(parkinglot == NULL || robot == NULL) return;

	if( robot->size == 0)
		{
			if( parkinglot->standby_robots->next == parkinglot->standby_robots )
				return;

			Robot * prev = parkinglot->standby_robots;
			Robot * current = parkinglot->standby_robots->next;
			
			while( current != robot && current->next != parkinglot->standby_robots)
				{
					prev = current;
					current = current->next;
				}

			if( current->next == parkinglot->standby_robots && prev == parkinglot->standby_robots )
				{
					parkinglot->standby_robots->next = parkinglot->standby_robots;
					return;
				}
			if( prev == parkinglot->standby_robots )
				{
					parkinglot->standby_robots->next = current->next;
					return;
				}
			if( current->next == parkinglot->standby_robots )
				{
					prev->next = parkinglot->standby_robots;	
					return;
				}

			prev->next = current->next;	
			return;
		}
	else
		{
			if( parkinglot->pending_robots->next == parkinglot->pending_robots )
				return;

			Robot * prev = parkinglot->pending_robots;
			Robot * current = parkinglot->pending_robots->next;
			
			while( current != robot && current->next != parkinglot->pending_robots )
				{
					prev = current;
					current = current->next;
				}

			if( current->next == parkinglot->pending_robots && prev == parkinglot->pending_robots )
				{
					parkinglot->pending_robots->next = parkinglot->pending_robots;
					return;
				}
			if( prev == parkinglot->pending_robots )
				{
					parkinglot->pending_robots->next = current->next;
					return;
				}
			if( current->next == parkinglot->pending_robots )
				{
					prev->next = parkinglot->pending_robots;	
					return;
				}

			prev->next = current->next;	
			return;
		}
}

int parckinglot_are_robots_peding(Parkinglot* parkinglot){
	if(parkinglot->pending_robots->next == parkinglot->pending_robots)
		return 0;
	return 1;
}

int parkinglot_are_arrived_trucks_empty(Parkinglot* parkinglot){
	Truck *current = parkinglot->arrived_trucks->next;
	while( current != parkinglot->arrived_trucks)
		{
			if(current->size != 0)
				return 0;
			current = current->next;
		}
	return 1;
}


int parkinglot_are_trucks_in_transit(Parkinglot* parkinglot){
	Truck *current = parkinglot->departed_trucks->next;
	if(current == parkinglot->departed_trucks)
		return 0;
	return 1;
}


void destroy_parkinglot(Parkinglot* parkinglot){
	
	Truck * prev = parkinglot->arrived_trucks;
	Truck * current = parkinglot->arrived_trucks->next;	

	while( current != parkinglot->arrived_trucks )
		{
			prev = current;	
			current = current->next;
			destroy_truck(prev);
		}

	destroy_truck(parkinglot->arrived_trucks);

	prev = parkinglot->departed_trucks;
	current = parkinglot->departed_trucks->next;

	while( current != parkinglot->departed_trucks )
		{
			prev = current;	
			current = current->next;
			destroy_truck(prev);
		}
	
	destroy_truck(parkinglot->departed_trucks);
	
	Robot *prevr = parkinglot->standby_robots;
	Robot *currentr = parkinglot->standby_robots->next;

	while( currentr != parkinglot->standby_robots )
		{
			prevr = currentr;
			currentr = currentr->next;
			destroy_robot(prevr);
		} 		
	destroy_robot(parkinglot->standby_robots);
	

	prevr = parkinglot->pending_robots;
	currentr = parkinglot->pending_robots->next;

		while( currentr != parkinglot->pending_robots )
		{
			prevr = currentr;
			currentr = currentr->next;
			destroy_robot(prevr);
		}

	destroy_robot(parkinglot->pending_robots);
	

	free(parkinglot);
	
}

void parkinglot_print_arrived_trucks(Parkinglot* parkinglot){
	Truck *iterator = parkinglot->arrived_trucks->next;
	while(iterator != parkinglot->arrived_trucks){

		truck_print_info(iterator);
		iterator = iterator->next;
	}

	printf("\n");

}

void parkinglot_print_departed_trucks(Parkinglot* parkinglot){
	Truck *iterator = parkinglot->departed_trucks->next;
	while(iterator != parkinglot->departed_trucks){
		truck_print_info(iterator);
		iterator = iterator->next;
	}
	printf("\n");

}

void parkinglot_print_pending_robots(Parkinglot* parkinglot){
	Robot *iterator = parkinglot->pending_robots->next;
	while(iterator != parkinglot->pending_robots){
		printf("R: %ld %ld\n", iterator->size, iterator->capacity);
		robot_print_manifest_info(iterator);
		iterator = iterator->next;
	}
	printf("\n");

}

void parkinglot_print_standby_robots(Parkinglot* parkinglot){
	Robot *iterator = parkinglot->standby_robots->next;
	while(iterator != parkinglot->standby_robots){
		printf("R: %ld %ld\n", iterator->size, iterator->capacity);
		robot_print_manifest_info(iterator);
		iterator = iterator->next;
	}
	printf("\n");

}


void truck_departed(Parkinglot *parkinglot, Truck* truck){
	
	Truck * prev = parkinglot->arrived_trucks;
	Truck * current = parkinglot->arrived_trucks->next;
	
	while( current != truck && current != parkinglot->arrived_trucks )
		{
			prev = current;
			current = current->next;
		}
	if(parkinglot->arrived_trucks->next == parkinglot->arrived_trucks);
	else if(current != truck );
	else if( current->next == parkinglot->arrived_trucks && prev == parkinglot->arrived_trucks )
		{
			parkinglot->arrived_trucks->next = parkinglot->arrived_trucks;
			
		}
	else if( prev == parkinglot->arrived_trucks )
		{
			parkinglot->arrived_trucks->next = parkinglot->arrived_trucks->next->next;
			
		}
	else if( current->next == parkinglot->arrived_trucks )
		{
			prev->next = parkinglot->arrived_trucks;	
			
		}
	else
		{	
			prev->next = current->next;	
		}	

	if(parkinglot->departed_trucks->next == parkinglot->departed_trucks )
		{
			parkinglot->departed_trucks->next = truck;
			truck->next = parkinglot->departed_trucks;
			return;
		}	

	Truck *truckPrev = parkinglot->departed_trucks;
	Truck *truckCurrent = parkinglot->departed_trucks->next;

	while( truckCurrent->next != parkinglot->departed_trucks && truck->departure_time > truckCurrent->departure_time)
		{
			truckPrev = truckCurrent;
			truckCurrent = truckCurrent->next;
		}
	
	if( truckCurrent->next == parkinglot->departed_trucks && truckPrev == parkinglot->departed_trucks )
		{
			if( truck->departure_time < truckCurrent->departure_time )
				{
					truck->next = truckCurrent ;
					truckCurrent->next = parkinglot->departed_trucks;
					parkinglot->departed_trucks->next = truck;
					return;
				}
			else
				{
					truckCurrent->next = truck;
					truck->next = parkinglot->departed_trucks;
					return;
				}
		}
	
	if( truckPrev == parkinglot->departed_trucks && truck->departure_time < truckCurrent->departure_time)	
		{
			truck->next = parkinglot->departed_trucks->next;
			parkinglot->departed_trucks->next = truck;
			return;
		}
	if( truckCurrent->next == parkinglot->departed_trucks && truck->departure_time > truckCurrent->departure_time )
		{
			truckCurrent->next = truck;
			truck->next = parkinglot->departed_trucks;
			return;
		}	
	
	truckPrev->next = truck;
	truck->next = truckCurrent;	
	return;
}


void truck_arrived(Parkinglot *parkinglot, Truck* truck){
	
	if(parkinglot == NULL || truck == NULL) return;

	Truck * prev = parkinglot->departed_trucks;
	Truck * current = parkinglot->departed_trucks->next;
	
	while( current != truck && current != parkinglot->departed_trucks )
		{
			prev = current;
			current = current->next;
		}
	if(parkinglot->departed_trucks->next == parkinglot->departed_trucks);
	else if(current != truck );
	else if( current->next == parkinglot->departed_trucks && prev == parkinglot->departed_trucks )
		{
			parkinglot->departed_trucks->next = parkinglot->departed_trucks;
			
		}
	else if( prev == parkinglot->departed_trucks )
		{
			parkinglot->departed_trucks->next = parkinglot->departed_trucks->next->next;
			
		}
	else if( current->next == parkinglot->departed_trucks )
		{
			prev->next = parkinglot->departed_trucks;	
			
		}
	else
		{	
			prev->next = current->next;	
		}	

	if( parkinglot->arrived_trucks->next == parkinglot->arrived_trucks )
		{
			parkinglot->arrived_trucks->next = truck;
			truck->next = parkinglot->arrived_trucks;
			truck->size = 0;
			truck->in_transit_time = 0;
			truck->manifest = NULL;
			return;
		}

	prev =	parkinglot->arrived_trucks;
	current = parkinglot->arrived_trucks->next;
		
	while( current->next != parkinglot->arrived_trucks && strcmp(truck->destination,current->destination) > 0 )
		{
			prev = current;
			current = current->next;
		}
	if( strcmp(truck->destination,current->destination) == 0 )
		{	
			while( current->next != parkinglot->arrived_trucks && strcmp(truck->destination,current->destination) == 0 && truck->departure_time > current->departure_time )
				{
					prev = current;
					current = current->next;
				}
			if( prev == parkinglot->arrived_trucks && truck->departure_time <= current->departure_time)
				{
					prev->next = truck;
					truck->next = current;
					truck->size = 0;
					truck->in_transit_time = 0;
					truck->manifest = NULL;
					return;
				}
			if( current->next == parkinglot->arrived_trucks && truck->departure_time >= current->departure_time)
				{
					current->next = truck;
					truck->next = parkinglot->arrived_trucks;
					truck->size = 0;
					truck->in_transit_time = 0;
					truck->manifest = NULL;
					return;
				}
			prev->next = truck;
			truck->next = current;	
			truck->size = 0;
			truck->in_transit_time = 0;
			truck->manifest = NULL;
			return;	
		}

	if( prev == parkinglot->arrived_trucks && strcmp(truck->destination,current->destination) <= 0 )
		{
			prev->next = truck;
			truck->next = current;
			truck->size = 0;
			truck->in_transit_time = 0;
			truck->manifest = NULL;
			return;
		}	
	if( current->next == parkinglot->arrived_trucks && strcmp(truck->destination,current->destination) > 0)
		{
			current->next = truck;
			truck->next = parkinglot->arrived_trucks;
			truck->size = 0;
			truck->in_transit_time = 0;
			truck->manifest = NULL;
			return;
		}	
	prev->next = truck;
	truck->next = current;	
	truck->size = 0;
	truck->in_transit_time = 0;
	truck->manifest = NULL;
	return;
}

void truck_transfer_unloading_robots(Parkinglot* parkinglot, Truck* truck){
	//return;
	Robot * prev = NULL;
	Robot * current = truck->unloading_robots;
		
		while( current != NULL )
			{
				prev = current;
				current = current->next;	
				parkinglot_add_robot(parkinglot,prev);
			}
	truck->unloading_robots = NULL;
}


// Depends on parking_turck_departed
void truck_update_depatures(Parkinglot* parkinglot, long day_hour){
	
	Truck *prev = parkinglot->arrived_trucks;
	Truck * current = parkinglot->arrived_trucks->next;
	
	while( current != parkinglot->arrived_trucks )
		{
			prev = current;
			current = current->next;

			if( day_hour == prev->departure_time)
				{
					truck_transfer_unloading_robots(parkinglot,prev);
					truck_departed(parkinglot,prev);
				}
		}	
}

// Depends on parking_turck_arrived

void truck_update_transit_times(Parkinglot* parkinglot){
	
	Truck * prev = parkinglot->departed_trucks;
	Truck * current = parkinglot->departed_trucks->next;

	while( current != parkinglot->departed_trucks )
		{
			prev = current;
			current = current->next;

			prev->in_transit_time++;

			if( prev->in_transit_time != prev->transit_end_time  )
				continue;
			else	
				{
					prev->in_transit_time = 0;
					truck_arrived(parkinglot,prev);
				}
		}
	return;	
}


void robot_swarm_collect(Wearhouse *wearhouse, Parkinglot *parkinglot){
	Robot *head_robot = parkinglot->standby_robots;
	Robot *current_robot = parkinglot->standby_robots->next;
	while(current_robot != parkinglot->standby_robots){

		// Load packages from wearhouse if possible
		if(!robot_load_packages(wearhouse, current_robot)){
			break;
		}

		// Remove robot from standby list
		Robot *aux = current_robot;
		head_robot->next = current_robot->next;
		current_robot = current_robot->next;

		// Add robot to the
		parkinglot_add_robot(parkinglot, aux);
	}
}


void robot_swarm_assign_to_trucks(Parkinglot *parkinglot){

	Robot *current_robot = parkinglot->pending_robots->next;

	while(current_robot != parkinglot->pending_robots){
		Robot* aux = current_robot;
		current_robot = current_robot->next;
		parkinglot_remove_robot(parkinglot, aux);
		int attach_succeded = robot_attach_find_truck(aux, parkinglot);
		if(!attach_succeded)
			parkinglot_add_robot(parkinglot, aux);
	}
}

void robot_swarm_deposit(Parkinglot* parkinglot){
	Truck *arrived_iterator = parkinglot->arrived_trucks->next;
	while(arrived_iterator != parkinglot->arrived_trucks){
		Robot *current_robot = arrived_iterator->unloading_robots;
		while(current_robot != NULL){
			robot_unload_packages(arrived_iterator, current_robot);
			Robot *aux = current_robot;
			current_robot = current_robot->next;
			arrived_iterator->unloading_robots = current_robot;
			parkinglot_add_robot(parkinglot, aux);
		}
		arrived_iterator = arrived_iterator->next;
	}
}

