

/* --- put this in main ----*/
double ys[]={0,0,0}; 	// < output from reference system
double us[]={30,30,30}; // < input from reference system
double y_ref;

// inne i lokka:
y_ref = getReference(ys,us);
/*--------------------------*/

/* This function filters the constant reference to obtain a smooth reference to the PI controller */
double getReference( double ys[3], double us[3]  ){
	double num ={ #num# };
	double denum ={ #denum# };   
	y[2]=y[1];
    y[1]=y[0];
    y[0]= -denum[1]*y[1] -denum[2]*y[2] + num[1]*u[1] + num[2]*u[2];
	return y[0];
}
