/*
 * getLoad.h
 *
 *  Created on: Mar 25, 2020
 *      Author: mdaa
 */

#ifndef GETLOAD_H_
#define GETLOAD_H_

#define TOTAL_SLICE   			20
#define SIMULSTION_RUNTIME		60
//#define TOTAL_CAPACITY			600.0
#define TOTAL_CAPACITY			520.0

double sliceLoad [TOTAL_SLICE][SIMULSTION_RUNTIME];


typedef struct slices {
	int DBID;					//Stores the database ID
	int slice_ID;				//Stores the slice ID
	int slice_prio;				//Stores the priority of the slice
	int slice_prio_old;			//Stores slice priority before modification
	int slice_prio_init;		//This is the initial slice priority, which will constant
	double data_rate_req;		//Stores the required data_rate
	double data_rate_alloc;		//Stores the allocated data rate
	double data_rate_old_alloc; //Allocation of resources before modifying it
} slice;

//Function decleration
void read_load (int sliceDBID);


#endif /* GETLOAD_H_ */
