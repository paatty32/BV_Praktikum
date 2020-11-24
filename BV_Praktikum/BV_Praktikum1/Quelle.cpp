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
	dicomDatei.seekg(0, dicomDatei.beg);

	//Ende des Header herausfinden
    int headEnde = dateiGroesse - ( (SIZE * SIZE) * 2);

    cout << "HeadEnde: " << headEnde << endl;
    cout << "Dateigroesse " << dateiGroesse << endl;

    ushort *buffer = new ushort[dateiGroesse-headEnde]; //Platz für die reinen Bilddaten

    dicomDatei.seekg(headEnde, dicomDatei.beg); //Setze den Zeiger ab dem HeadEnde

    //Buffer mit inhalt der Bilddateien befüllen
	dicomDatei.read((char*) buffer, dateiGroesse - headEnde); //Fuegt die anzahl, die im 2. Parameter angegeben ist in das array was im ersten Parameter angegeben ist

	dicomDatei.close();

	Mat bild(SIZE, SIZE,CV_16U);
	//16bit Pixel 
	int pixel;

	//Mat Objekt befuellen
	for(int i = 0; i < SIZE; i++){
	    for(int j = 0; j < SIZE; j++){
	        pixel;
	        bild.at<ushort>(i,j) = buffer[(i * SIZE) + j]; //Pixel
	    }
	}
    imshow("Normales Bild", bild);

	double min, max;
	minMaxLoc(bild, &min, &max);
	cout << "kleinster Grauwert: " << min << " Größter GRauwert: " << max << endl;

	Mat scal;
    convertScaleAbs(bild, scal, 255.0 / (max - min), -min * (255.0 / (max - min)));

	imshow("Dicom", scal);
	waitKey();
	
	//Content printen:
    //cout.write(buffer, dateiGroesse);
	


	delete[] buffer;
	return 0;
}