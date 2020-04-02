/*
 * slice_resource_main.c
 *
 *  Created on: Oct 17, 2019
 *      Author: mdaa
 */


#include <stdlib.h>
#include <time.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <time.h>
#include <unistd.h>

#include "getLoad.h"



//Function forward deceleration
void slice_rand_alloc (int slice_id, int runTime);
void slice_rand_req (int slice_id, int runTime);
void slice_generator (bool init, int runTime);
int check_resource_status ();
void resource_alloc (int slice_db);
void check_alloc ();



//int excess_slice [TOTAL_SLICE];
//int req_slice [TOTAL_SLICE];

//double excess_res;				//Excess resources
//double req_res;					//Amount of shortage of resources

double total_capacity = 0;			//Total capacity of the server. Calculated during 1st slice allocation iteration
double total_asking_capacity = 0;	//Total capacity asked by all slices. Calculated during every check_resource_status () iteration
double total_unallocated_res = 0;	//Total unallocated resources. Calculated during every check_resource_status () iteration

double slice_fairness[TOTAL_SLICE]; //Database for slice priority boosting

FILE *fp;

slice slice_data [TOTAL_SLICE];

//Sort slice in ascending order (highest priority slice first (db id 0))
void sort_slice ()
{
	int i, j;
	slice tmp;
    for(i=0; i<TOTAL_SLICE; i++)
    {
        for(j=i+1; j<TOTAL_SLICE; j++)
        {
            if(slice_data[i].slice_prio < slice_data[j].slice_prio)
            {
            	tmp = slice_data[i];
            	slice_data[i] = slice_data[j];
                slice_data[j] = tmp;
            }
        }
    }
}

void slice_rand_alloc (int slice_id, int runTime)
{
	srand((unsigned int) time(NULL));
	int i, p;

	for (p = 0; p < TOTAL_SLICE; p++)
	{
		if (slice_id == slice_data [p].slice_ID)
		{
			i = p;
			break;
		}
	}


	bool non_zero = true;
//	double percentage_value = (slice_data [i].data_rate_req * (-50.0)) / 100.0;
	double percentage_value = (slice_data [i].data_rate_req * 50.0) / 100.0;
//	double percentage_value = (slice_data [i].data_rate_req * 100.0) / 100.0;

	int max_range = slice_data [i].data_rate_req + percentage_value;
//	double min_range = slice_data [i].data_rate_req - percentage_value;
	while (non_zero)
	{
//		slice_data [i].data_rate_alloc = random () % 570;
		slice_data [i].data_rate_alloc = (double) (random () % max_range);
		usleep(600000);

//		if ((slice_data [i].data_rate_alloc > 0) && ((slice_data [i].data_rate_alloc <= max_range) && (slice_data [i].data_rate_alloc >= min_range)))
		if (slice_data [i].data_rate_alloc > 0)
		{
			non_zero = false;
		}
	}
}

void slice_rand_req (int slice_id, int runTime)
{
	int i;
/*	bool non_zero = true;
	while (non_zero)
	{
		for (i = 0; i < TOTAL_SLICE; i++)
		{
			if (slice_id == slice_data [i].slice_ID)
			{
				slice_data [i].data_rate_req = random () % 600;

				if (slice_data [i].data_rate_req > 0)
				{
					non_zero = false;
				}
				break;
			}
		}
	}*/

/*	if (runTime == 0)
	{
		for (i = 0; i < TOTAL_SLICE; i++)
		{
			if (slice_id == slice_data [i].slice_ID)
			{
				read_load (i);
//				read_load (slice_data [i].slice_ID);
				slice_data [i].data_rate_req = sliceLoad [i][runTime];
				break;
			}
		}
	} else
	{
		slice_data [slice_id].data_rate_req = sliceLoad [slice_id][runTime];
	}*/
	if (runTime == 0)
	{
		read_load (slice_id);
		slice_data [slice_id].data_rate_req = sliceLoad [slice_id][runTime];
	} else
	{
		int dbID, slicedb;
		for (i = 0; i < TOTAL_SLICE; i++)
		{
			if (slice_id == slice_data [i].slice_ID)
			{
				dbID = slice_data [i].DBID;
				slicedb = i;
				break;
			}
		}
		slice_data [slicedb].data_rate_req = sliceLoad [dbID][runTime];
	}


}

void slice_generator (bool init, int runTime)
{
	srand(time(NULL));

	int i;
	if (init == true)
	{
		for (i = 0; i < TOTAL_SLICE; i++)
		{
			slice_data [i].slice_ID = i;
			slice_data [i].DBID = i;

			slice_data [i].data_rate_alloc = 0;

			slice_rand_req (i, runTime);
			slice_rand_alloc (i, runTime);
			total_capacity = total_capacity + slice_data [i].data_rate_alloc;


			bool non_zero = true;
			while (non_zero)
			{
				slice_data [i].slice_prio = random () % 100;
				if (slice_data [i].slice_prio > 0)
				{
					non_zero = false;
					usleep(600000);
				}
			}

			slice_data [i].slice_prio_init = slice_data [i].slice_prio;
		}
	} else
	{
		for (i = 0; i < TOTAL_SLICE; i++)
		{
			//The slice already has priority and allocation defined. We need new request
			slice_rand_req (slice_data [i].slice_ID, runTime);
		}
	}

	//Collecting data for writing report
	for (i = 0; i < TOTAL_SLICE; i++)
	{
		slice_data [i].slice_prio_old = slice_data [i].slice_prio;
		slice_data [i].data_rate_old_alloc = slice_data [i].data_rate_alloc;
	}
}

double unused_res_allocation (double resourceToAllocate)
{
	double suffled_res = 0;
	double extra_resource = resourceToAllocate;

//Now we will allocate the extra resources to the high priority users first
	int i;
	if (extra_resource > 0)
	{
		for (i = 0; i < TOTAL_SLICE; i++)
		{
			if (slice_data [i].data_rate_req > slice_data [i].data_rate_alloc)
			{
				if ((slice_data [i].data_rate_req - slice_data [i].data_rate_alloc) <= extra_resource)
				{
					extra_resource -= (slice_data [i].data_rate_req - slice_data [i].data_rate_alloc);
					suffled_res += (slice_data [i].data_rate_req - slice_data [i].data_rate_alloc);
					slice_data [i].data_rate_alloc = slice_data [i].data_rate_req;

					slice_data [i].slice_prio = slice_data [i].slice_prio_init;

					slice_fairness[slice_data [i].slice_ID] = 0;
				} else
				{
					suffled_res += extra_resource;
					slice_data [i].data_rate_alloc += extra_resource;
					break;
				}

			} else
			{
				slice_data [i].slice_prio = slice_data [i].slice_prio_init;

				slice_fairness[slice_data [i].slice_ID] = 0;
			}
		}
	}

	return suffled_res;
}

void unused_res_find ()
{
	int i;
	double extra_resource = 0;
	double temp_res = 0, total_alloc = 0;
/*	for (i = 0; i < TOTAL_SLICE; i++)
	{
		if ((slice_data [i].data_rate_req > slice_data [i].data_rate_alloc) && (total_unallocated_res > 0))
//		if ((slice_data [i].data_rate_req > slice_data [i].data_rate_alloc))
		{
			//The current slice is suffering from lack of resources

			if ((slice_data [i].data_rate_req - slice_data [i].data_rate_alloc)> total_unallocated_res)
			{
				//The current slice requires more resources than unallocated resource availability
				slice_data [i].data_rate_alloc += total_unallocated_res;
				total_unallocated_res = 0;
				break;

			} else
			{
				//The current slice requires less resources than unallocated resource availability
				total_unallocated_res = total_unallocated_res - (slice_data [i].data_rate_req - slice_data [i].data_rate_alloc);
				slice_data [i].data_rate_alloc = slice_data [i].data_rate_req;


				slice_data [i].slice_prio = slice_data [i].slice_prio_init;

				slice_fairness[slice_data [i].slice_ID] = 0;
			}
		}

	}*/

	//We check the total allocated resource amount of all users
	for (i = 0; i < TOTAL_SLICE; i++)
	{
		total_alloc += slice_data [i].data_rate_alloc;
	}

	if (total_alloc < TOTAL_CAPACITY)
	{
		double unallocatedResource = TOTAL_CAPACITY -total_alloc;
		unused_res_allocation (unallocatedResource);
	}


	//We check the total unused resource amount of all users
	for (i = 0; i < TOTAL_SLICE; i++)
	{
		if (slice_data [i].data_rate_req < slice_data [i].data_rate_alloc)
		{
			temp_res = slice_data [i].data_rate_alloc - slice_data [i].data_rate_req;
			extra_resource += temp_res;
		}

	}



//	double suffled_res = 0;
//Now we will allocate the extra resources to the high priority users first
	if (extra_resource > 0)
	{
		double suffled_res = unused_res_allocation (extra_resource);

/*		for (i = 0; i < TOTAL_SLICE; i++)
		{
			if (slice_data [i].data_rate_req > slice_data [i].data_rate_alloc)
			{
				if ((slice_data [i].data_rate_req - slice_data [i].data_rate_alloc) <= extra_resource)
				{
					extra_resource -= (slice_data [i].data_rate_req - slice_data [i].data_rate_alloc);
					suffled_res += (slice_data [i].data_rate_req - slice_data [i].data_rate_alloc);
					slice_data [i].data_rate_alloc = slice_data [i].data_rate_req;
				} else
				{
					suffled_res += extra_resource;
					slice_data [i].data_rate_alloc += extra_resource;
					break;
				}

			}
		}*/

//Now we will trim the excess resources starting from the low priority users

		for (i = TOTAL_SLICE - 1; i >= 0; i--)
		{
			if (slice_data [i].data_rate_alloc > slice_data [i].data_rate_req)
			{
				if ((slice_data [i].data_rate_alloc - slice_data [i].data_rate_req) >= suffled_res)
				{
					slice_data [i].data_rate_alloc -= suffled_res;
					break;
				} else
				{
					suffled_res -= (slice_data [i].data_rate_alloc - slice_data [i].data_rate_req);
					slice_data [i].data_rate_alloc -= (slice_data [i].data_rate_alloc - slice_data [i].data_rate_req);
				}
			}
		}
	}
}

//In this function we will first check the unused resources and than allocated to the slices having shortage of resources
//Afterwards we will re allocate resources to the slices according to there priority

void res_shuffle ()
{
	unused_res_find ();

	check_alloc ();

/*	int i, j;
	double temp_res_req = 0, temp_res_alloc = 0;
	for (i = 0; i < TOTAL_SLICE; i++)
	{
		check_alloc ();
		if (slice_data [i].data_rate_req > slice_data [i].data_rate_alloc)
		{
			//The current slice is suffering from lack of resources
			temp_res_req = slice_data [i].data_rate_req - slice_data [i].data_rate_alloc;
			for (j = TOTAL_SLICE - 1; j >= 0; j--)
			{
				if (slice_data [j].data_rate_req < slice_data [j].data_rate_alloc)
				{
					temp_res_alloc = slice_data [j].data_rate_alloc - slice_data [j].data_rate_req;
					if (temp_res_alloc <= temp_res_req)
					{
						//The current slice has less or equal resources to share than requested
						temp_res_req -= temp_res_alloc;
						if (temp_res_req == 0)
						{
							//No more resource required. The excess resource from current slice is perfect
//							slice_data [j].data_rate_alloc = slice_data [j].data_rate_req;
							slice_data [j].data_rate_alloc = slice_data [j].data_rate_alloc - temp_res_alloc;
							slice_data [i].data_rate_alloc += temp_res_alloc;
//							req_res = req_res - temp_res_alloc;

							if (slice_data [i].data_rate_alloc != slice_data [i].data_rate_req)
							{
								printf ("Shared resource supposed fulfill the request. Please check\n");
							}
							slice_data [i].slice_prio = slice_data [i].slice_prio_init;

							slice_fairness[slice_data [i].slice_ID] = 0;

							break;
						} else
						{
							//More resources are required. We need to take the next slice. No action needed now

//							slice_data [j].data_rate_alloc = slice_data [j].data_rate_req;
							slice_data [j].data_rate_alloc = slice_data [j].data_rate_alloc - temp_res_alloc;
							slice_data [i].data_rate_alloc += temp_res_alloc;
							continue;
						}
					} else
					{
						//The current slice has more resources to share than requested. We will share as per request
//						slice_data [i].data_rate_alloc = slice_data [i].data_rate_req;
						slice_data [i].data_rate_alloc = slice_data [i].data_rate_alloc + temp_res_req;
//						slice_data [j].data_rate_alloc -= (temp_res_alloc - temp_res_req);
						slice_data [j].data_rate_alloc = (slice_data [j].data_rate_alloc - temp_res_req);
						temp_res_req = 0;
//						req_res = req_res - (temp_res_alloc - temp_res_req);

						slice_data [i].slice_prio = slice_data [i].slice_prio_init;

						slice_fairness[slice_data [i].slice_ID] = 0;

						break;
					}
				}
			}
		} else
		{
			//This slice has enough resource
			slice_data [i].slice_prio = slice_data [i].slice_prio_init;

			slice_fairness[slice_data [i].slice_ID] = 0;
		}
	}*/

	check_alloc ();
}


void resource_alloc (int slice_db)
{
	int total_prio = 0, i;
	double excess_resource = 0, required_res = 0, total_req = 0, total_alloc = 0;

	for (i = slice_db ; i < TOTAL_SLICE; i++)
	{
		total_prio += slice_data [i].slice_prio;
		total_req += slice_data [i].data_rate_req;
		total_alloc += slice_data [i].data_rate_alloc;
	}


	check_alloc ();

	for (i = slice_db ; i < TOTAL_SLICE; i++)
	{
		slice_data [i].data_rate_alloc = (double) (((double)slice_data [i].slice_prio / (double)total_prio) * total_alloc);

		if (slice_data [i].data_rate_req <= slice_data [i].data_rate_alloc)
		{
			excess_resource += slice_data [i].data_rate_alloc - slice_data [i].data_rate_req;
			slice_data [i].data_rate_alloc = slice_data [i].data_rate_req;
			slice_fairness[slice_data [i].slice_ID] = 0;
			slice_data [i].slice_prio = slice_data [i].slice_prio_init;
		} else if (slice_data [i].data_rate_req > slice_data [i].data_rate_alloc)
		{
			required_res = slice_data [i].data_rate_req - slice_data [i].data_rate_alloc;

			if (required_res <= excess_resource)
			{
				slice_data [i].data_rate_alloc += required_res;
				excess_resource = excess_resource - required_res;

				slice_fairness[slice_data [i].slice_ID] = 0;
				slice_data [i].slice_prio = slice_data [i].slice_prio_init;
			} else
			{
				slice_fairness[slice_data [i].slice_ID] = slice_fairness[slice_data [i].slice_ID] + 1.0;
				slice_data [i].data_rate_alloc += excess_resource;
				excess_resource = 0;
				//Source: https://mathbitsnotebook.com/Algebra2/Exponential/EXGrowthDecay.html
				slice_data [i].slice_prio = slice_data [i].slice_prio *
						pow ((1 + (slice_data [i].data_rate_alloc / slice_data [i].data_rate_req)),
								slice_fairness[slice_data [i].slice_ID]);
			}

		}
	}

	for (i = slice_db ; i < TOTAL_SLICE; i++)
	{
		if (excess_resource > 0)
		{
			if (slice_data [i].data_rate_req > slice_data [i].data_rate_alloc)
			{
				required_res = slice_data [i].data_rate_req - slice_data [i].data_rate_alloc;

				if (required_res <= excess_resource)
				{
					slice_data [i].data_rate_alloc += required_res;
					excess_resource = excess_resource - required_res;

					slice_fairness[slice_data [i].slice_ID] = 0;
					slice_data [i].slice_prio = slice_data [i].slice_prio_init;
				} else
				{
					slice_data [i].data_rate_alloc += excess_resource;
					excess_resource = 0;
				}
			}

		} else
		{
			break;
		}
	}

}

void res_rearrange ()
{
	int i, max_prio_sliceID;

	res_shuffle ();

	check_alloc ();

	for (i = 0; i < TOTAL_SLICE; i++)
	{
//		total_prio += slice_data [i].slice_prio;

		if (slice_data [i].data_rate_req > slice_data [i].data_rate_alloc)
		{
			max_prio_sliceID = i;
			break;
		}
	}

	resource_alloc (max_prio_sliceID);

	check_alloc ();
}


int check_resource_status ()
{
//	check_alloc();
	int i;
	double excess_res;				//Excess resources
	double req_res;					//Amount of shortage of resources
	double temp_res_alloc = 0;
	double temp_res_req = 0;
	excess_res = 0;
	req_res = 0;
	int j = 0;
	for (i = 0; i < TOTAL_SLICE; i++)
	{
		temp_res_alloc = temp_res_alloc + slice_data [i].data_rate_alloc;
		temp_res_req += slice_data [i].data_rate_req;
		if (slice_data [i].data_rate_req < slice_data [i].data_rate_alloc)
		{
			excess_res += slice_data [i].data_rate_alloc - slice_data [i].data_rate_req;
//			excess_slice [j] = slice_data [i].slice_ID;
		} else if (slice_data [i].data_rate_req > slice_data [i].data_rate_alloc)
		{
			req_res+= slice_data [i].data_rate_req - slice_data [i].data_rate_alloc;
//			req_slice [j] = slice_data [i].slice_ID;
		}
		j++;
	}

	total_asking_capacity = req_res;
	total_unallocated_res = TOTAL_CAPACITY - temp_res_alloc;

	assert (TOTAL_CAPACITY >= (temp_res_alloc-1));

	sort_slice ();

/*	if (excess_res > req_res)
	{
		//Total resource request is less than the resource allocation. We need to reallocate the excess resources to the
		//resource hungry slices
		return 30;
	} else if (excess_res < req_res)
	{
		//Total resource request is greater than the resource allocation. We need to take resources from low priority slices
		return 40;
	}*/

	if (temp_res_req <= TOTAL_CAPACITY)
	{
		//Total resource request is less than the resource allocation. We need to reallocate the excess resources to the
		//resource hungry slices
		return 30;
	} else
	{
		//Total resource request is greater than the resource allocation. We need to take resources from low priority slices
		return 40;
	}

	return 1;
}

void check_alloc ()
{
	int i;
	double total_alloc = 0;
	for (i = 0; i < TOTAL_SLICE; i++)
	{
		total_alloc += slice_data [i].data_rate_alloc;
	}

	if ((total_alloc-1) > TOTAL_CAPACITY)
		printf ("Check");

	if ((total_alloc+1) < TOTAL_CAPACITY)
		printf ("Check");
}


int main ()
{
	fp = fopen("user_alloc.csv", "w+");
	setbuf(stdout, NULL);

	fprintf (fp, "Simulation Time(s); Slice ID; Slice Priority old; Slice Request; Slice old alloc; Slice Priority new; slice new alloc\n");

	int i, p;
	bool init = true;
	for (i = 0; i < SIMULSTION_RUNTIME; i++)
	{
		slice_generator (init, i);
		check_alloc ();

		init = false;
		int slice_health = check_resource_status();
		check_alloc ();

		if (slice_health == 30)
		{
			res_shuffle ();
		} else if (slice_health == 40)
		{
			res_rearrange ();
		}

		check_alloc ();

		for (p = 0; p < TOTAL_SLICE; p++)
		{
			fprintf (fp, "%d; %d; %d; %f; %f; %d; %f;\n", i, slice_data [p].slice_ID, slice_data [p].slice_prio_old, slice_data [p].data_rate_req,
					slice_data [p].data_rate_old_alloc, slice_data [p].slice_prio, slice_data [p].data_rate_alloc);
		}

		fprintf (fp, "\n\n\n");

		printf ("Resource availability: %f\n", total_capacity);

//		sleep(0.5);
		usleep(600000);

	}


	return 0;
}
