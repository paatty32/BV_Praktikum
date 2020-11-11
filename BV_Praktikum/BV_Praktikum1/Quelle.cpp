/*Patrick Boadu Boafo*/
#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <fstream>


using namespace std;
using namespace cv;

int main(int argc, char * argv[]) {

	const int SIZE = 256;

	//Datei einlesen
	ifstream dicomDatei(argv[1]);

	//Groesse der Datei herausfinden
	dicomDatei.seekg(0, dicomDatei.end); //Setzt den zeiger ausgehend von der ersten Zeile aufs Ende der Datei
	int dateiGroesse = dicomDatei.tellg();
	dicomDatei.seekg(0, dicomDatei.beg); //Zeiger wieder am anfang setzen

	char* buffer = new char[dateiGroesse];

	dicomDatei.read(buffer, dateiGroesse); //Fügt die anzahl, die im 2. Parameter angegeben ist in das array was im ersten Parameter angegeben ist 
	dicomDatei.close();

	buffer = (ushort)buffer;

	//Ende des Header herausfinden
	int headEnde = dateiGroesse - (SIZE * SIZE);

	
	//Content printen:
    cout.write(buffer, dateiGroesse);
	


	delete[] buffer;
	

	return 0;
}