/*
 * getLoad.c
 *
 *  Created on: Mar 25, 2020
 *      Author: mdaa
 */

#include<stdio.h>
#include <string.h>
#include <stdlib.h>
#include "stdbool.h"
#include "getLoad.h"

#define SKIP_LOAD_DATA	500
extern slice slice_data [TOTAL_SLICE];


void read_load (int sliceDBID)
{
	char filename[1024];

	FILE *loadFile;

	sprintf(filename,"/home/mdaa/eclipse-workspace/slice_resource/slice_load_files/sliceLoad%d.csv",sliceDBID);
//	sprintf(filename,"sliceLoad%d.csv",sliceDBID);
	loadFile = fopen (filename, "r");

    if (!loadFile) {
        printf("Can't open file\n");
        exit (0);
    }

    char buf[1024];
    int row_count = 0;
    int field_count = 0;
    bool aggregate_load = false;
    bool file_read_init = true;
    int runTimeCount = 0;
    int loadAggregationCount = 0;
    while (fgets(buf, 1024, loadFile)) {
        field_count = 0;
        row_count++;

        if (row_count <= SKIP_LOAD_DATA) {
            continue;
        }

        char *field = strtok(buf, ";");
        while (field) {
            if ((field_count == 0) && (row_count == (SKIP_LOAD_DATA+1))) {

            	file_read_init = false;
            	double timeVal;
//            	sscanf(field_count, "%lf", &timeVal);

            	char *ptr;
            	timeVal = strtod(field, &ptr);

                if (timeVal < (SKIP_LOAD_DATA-1))
                {
                	aggregate_load = true;
                }
            }

            char *ptr1;
            if ((aggregate_load == false) && (field_count == 2) && (file_read_init == false))
            {
            	sliceLoad [sliceDBID][runTimeCount] = strtod(field, &ptr1) / 1000000;
            	runTimeCount++;
            }

            if ((aggregate_load == true) && (field_count == 2) && (file_read_init == false))
            {
            	loadAggregationCount++;
            	sliceLoad [sliceDBID][runTimeCount] += strtod(field, &ptr1) / 1000000;
            	if (loadAggregationCount == 10)
            	{
            		runTimeCount++;
            		loadAggregationCount = 0;
            	}

            }

            if (runTimeCount == SIMULSTION_RUNTIME)
            {
            	break;
            }



/*            if (field_count == 1) {
                printf("Last Name:\t");
            }
            if (field_count == 2) {
                    printf("Phone Number:\t");
            }
            if (field_count == 3) {
                printf("Job Title:\t");
            }

            printf("%s\n", field);*/
            field = strtok(NULL, ";");

            field_count++;
        }
//        printf("\n");
    }

    fclose(loadFile);

}
