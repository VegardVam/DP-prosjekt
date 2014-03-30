

all:
	gcc -o runDP heleSammensatt.c -lrt -lphidget21
	gcc -o runDPmedRef medRefSys.c -lrt -lphidget21
	gcc -o return60 return60medrefsys.c -lrt -lphidget21 
	gcc -o alternerende alternerendePosisjon.c -lrt -lphidget21
run:
	./runDP
	./runDPmedRef
	./return60
	./alternerende 
