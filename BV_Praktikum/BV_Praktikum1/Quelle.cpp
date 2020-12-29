/*Patrick Boadu Boafo*/
#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <fstream>


using namespace std;
using namespace cv;

int SIZE = 512;

void readImg(String &pfad, String &datei) {
    ifstream dicomDatei(pfad + datei + ".dcm");

    //Groesse der Datei herausfinden
    dicomDatei.seekg(0, dicomDatei.end); //Setzt den zeiger ausgehend von der ersten Zeile aufs Ende der Datei
    int dateiGroesse = dicomDatei.tellg();
    //dicomDatei.seekg(0, dicomDatei.beg);

    //Ende des Header herausfinden
    if(dateiGroesse - ( (SIZE * SIZE) * 2) < 0){
        SIZE = 256;
    }

    int headEnde = dateiGroesse - ( (SIZE * SIZE) * 2);
    dicomDatei.seekg(headEnde, dicomDatei.beg); //Setze den Zeiger ab dem HeadEnde

    cout << "HeadEnde: " << headEnde << endl;
    cout << "Dateigroesse " << dateiGroesse << endl;

    ushort *buffer = new ushort[dateiGroesse-headEnde]; //Platz für die reinen Bilddaten

    //Buffer mit inhalt der Bilddateien befüllen
    dicomDatei.read( (char*) buffer, (dateiGroesse - headEnde)); //Fuegt die anzahl, die im 2. Parameter angegeben ist in das array was im ersten Parameter angegeben ist
    if(dicomDatei){
        cout << "Alles korrekt" << endl;
    } else {
        cout << "Fehler beim lesen" << endl;
    }
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
    imshow(pfad + datei, bild);

    double min, max;
    minMaxLoc(bild, &min, &max);
    cout << "kleinster Grauwert: " << min << " Groesster Grauwert: " << max << endl;
    //Test
    //Skalierung
    Mat scal;
    convertScaleAbs(bild, scal, 255.0 / (max - min), -min * (255.0 / (max - min)));

    imshow(datei, scal);

    //Bilder abspeichern
    imwrite(pfad + datei + ".png", scal);

    delete[] buffer;
}

int main(int argc, char * argv[]) {

	//Datei einlesen
	String pfad = argv[1];
	/*Name der Dateien, die im Pfad liegen */
	String datei = "0";
    String datei1 = "1";
    String datei2 = "2";

    readImg(pfad, datei);
    readImg(pfad, datei1);
    readImg(pfad, datei2);
    waitKey();
    return 0;
}