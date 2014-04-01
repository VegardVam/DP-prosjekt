#include <stdio.h>
#include <stdlib.h>
#include <phidget21.h>
#include <math.h>
#include <unistd.h>
#include <time.h>

/* 
	---------------------------------------------------------------------------
	DP-Program    			Modul: Posisjon      			No.:1
	---------------------------------------------------------------------------

	Hensikt :
	Skriver VTFfeilen som GLview trenger for å visualisere båten bevegelser. 
	Metode :
	Tar inn to vektorer og et heltall. En vektor med posisjonloggen til båten 
	og en med den ruten båten etterstreber å følge. Heltallet er for å få
	lengden til for-løkken som skriver posisjonene i VTFfilen. Bruker fopen til 
	

	I N T E R N E   V A R I A B L E :
	*VTFfile 			filpeker til VTFfilen
	line        			teller til hver linje i VTFfilen
	step					teller til hvert step i VTFfilen

	F U K S J O N E R :
	fprintf				skriver til fil


	Programmert av 		Vegard Vamråk
	Dato			   		03/31/14 22:16:44 

	***************************************************************************
*/


void produceVtfFile(double *currentPositionTable, double *desiredPositionTable, int arraySize)
{
 
	FILE *vtfFile = fopen("/home/lokal/Ceetron/GLvInova_9.1-3_x86_64/tutorials/simulering.vtf","w");
  
	int line, step;
	
	fprintf(vtfFile,"*VTF-1.00\n\n\n");
	fprintf(vtfFile,"*NODES 1\n");
	fprintf(vtfFile,"0 14.5 0\n0 36.5 0\n77 14.5 0\n77 36.5 0\n0 14.5 10\n0 36.5 10\n77 14.5 10\n77 36.5 10\n\n\n");
	fprintf(vtfFile,"*NODES 2\n");
	fprintf(vtfFile,"0 0 0\n0 51 0\n149 51 0\n149 0 0\n\n\n");
	fprintf(vtfFile,"*ELEMENTS 1\n%%NODES #1\n%%QUADS\n1 2 4 3\n5 6 8 7\n1 2 6 5\n3 4 8 7\n1 3 7 5\n2 4 8 6\n\n\n"); 
	fprintf(vtfFile,"*ELEMENTS 2\n%%NODES #2\n%%QUADS\n1 2 3 4\n\n");
	fprintf(vtfFile,"*GLVIEWGEOMETRY 1\n%%ELEMENTS\n1 2\n\n");
 
	for ( line=0 ; line < arraySize ; line++ )
	{
		fprintf( vtfFile,"*RESULTS %d\n", line+1 );
      	fprintf( vtfFile,"%%DIMENSION 3\n" );
 	    fprintf( vtfFile,"%%PER_NODE #1\n" );
 	    for ( step = 0 ; step < 8 ; step++ )
 	    {
	        fprintf( vtfFile,"%f 0 0\n", currentPositionTable[line] );
        }
	    fprintf(vtfFile,"\n");
	}

	fprintf(vtfFile,"*GLVIEWVECTOR 1\n");
    fprintf(vtfFile,"%%NAME \n");
  
    for ( line=0 ; line < arraySize ; line++ )
    {
        fprintf(vtfFile,"%%STEP %d\n %d\n",line,line);
    }
    return;
}


