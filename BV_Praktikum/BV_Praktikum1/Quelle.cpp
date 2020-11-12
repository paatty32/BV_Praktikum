/*Patrick Boadu Boafo*/
#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <fstream>


using namespace std;
using namespace cv;

int main(int argc, char * argv[]) {

	const int SIZE = 512;

	//Datei einlesen
	ifstream dicomDatei(argv[1]);

	//Groesse der Datei herausfinden
	dicomDatei.seekg(0, dicomDatei.end); //Setzt den zeiger ausgehend von der ersten Zeile aufs Ende der Datei
	int dateiGroesse = dicomDatei.tellg();
	dicomDatei.seekg(0, dicomDatei.beg); //Zeiger wieder am anfang setzen

	char* buffer = new char[dateiGroesse];

	dicomDatei.read(buffer, dateiGroesse); //Fügt die anzahl, die im 2. Parameter angegeben ist in das array was im ersten Parameter angegeben ist 
	dicomDatei.close();


	//Ende des Header herausfinden
	int headEnde = dateiGroesse - (SIZE * SIZE * 2);
	cout << "HeadEnde: " << headEnde << endl;
	cout << "Dateigroesse " << dateiGroesse << endl;

	//Buffer für die reinen Bilddateien
	/*char* buffer2 = new char[dateiGroesse - headEnde];

	
	for (int i = 0; i < dateiGroesse - headEnde; i++) {
		buffer2[i] = buffer[headEnde];
		headEnde = headEnde + 1;
	}
	*/
	
	Mat bild(SIZE, SIZE,CV_16U);
	//16bit Pixel 
	short pixel;
	
	//Mat Objekt befüllen
	for (int k = headEnde; k < 525628; k++) {
		for (int i = 0; i < SIZE; i++) {
			for (int j = 0; j< SIZE; j++) {
				pixel = (uchar) buffer[k] << 8 | (uchar) buffer[k + 1]; //aus zwei 8bit werten einen 16 bit wert machen 
				//cout << "SIZEOF PIXEL: " << pixel;
				bild.at<ushort>(i, j) = pixel;
			}
		} 
	}
	Mat scal;

	convertScaleAbs(bild, scal, 1, 0);

	imshow("Dicom", scal);
	waitKey();
	
	//Content printen:
    //cout.write(buffer, dateiGroesse);
	


	delete[] buffer;
	//delete[] buffer2;
	

	return 0;
}