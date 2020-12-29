/*Patrick Boadu Boafo*/
#include <stdio.h>
#include <opencv2/opencv.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include <fstream>


using namespace std;
using namespace cv;

int SIZE = 512;

Mat readImg(String &pfad, String &datei) {
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
    return scal;
}

void filter(Mat &eingabeBild, Mat &filterErgebnis, int filterWahl, int filtergroesse){
    /* 1: Gauss-Filter */
    /* 2: Median-Filter */
    /* 3: Bilateralfilter */
    if(filterWahl == 1){
        Mat filterSize = getGaussianKernel(filtergroesse,0);
        GaussianBlur(eingabeBild, filterErgebnis, filterSize.size(), 0);

        imshow("Gaussian Filter", filterErgebnis);
    }

    if(filterWahl == 2){
        medianBlur(eingabeBild, filterErgebnis, filtergroesse);
        imshow("Medianfilter", filterErgebnis);
    }

    if(filterWahl == 3){
        bilateralFilter(eingabeBild, filterErgebnis, 9, 50, 50);
        imshow("Bilateral Filter", filterErgebnis);
    }
}

void drawHistogram(Mat &eingabeBild, Mat &histogramm, Mat &histogrammBild, int breite, int hoehe ){
    int histSize = 256;

    //Bereiche des Histogramms setzen
    float range[] {0, 256};
    const float *histRange = {range};

    //Histogram berechnen
    calcHist(&eingabeBild, 1, 0, Mat(), histogramm, 1, &histSize, &histRange, true, false);

    //Histogram groesse festlegen
    int bin_w = (int) ( (double) breite/histSize );

    //Histogram normalisieren
    normalize(histogramm, histogramm, 0, histogrammBild.rows, NORM_MINMAX, -1, Mat());

    //Histogram zeichen
    for(int i = 0; i < histSize; i++){
        line( histogrammBild, Point( bin_w*(i), hoehe - cvRound(histogramm.at<float>(i)) ) ,
                   Point( bin_w*(i), hoehe), Scalar(255,0,0));
    }

    //Histogram ausgeben
    //imshow("Histogramm ", histogrammBild );
}
int main(int argc, char * argv[]) {

	//Datei einlesen
	String pfad = argv[1];
	/*Name der Dateien, die im Pfad liegen */
	String datei = "16";
    String datei1 = "1";
    String datei2 = "2";

    Mat eingabeBild = readImg(pfad, datei);
    Mat ausgabeBild;

    Mat histogramm;
    int breite = 256;
    int hoehe = 400;
    Mat histogrammBild( hoehe, breite, CV_8UC3, Scalar( 0,0,0) );


    //readImg(pfad, datei1);
    //readImg(pfad, datei2);

    /*Schleife um Alle Filter zu benutzen. */
    for(int i = 1; i < 4; i++) {
        filter(eingabeBild, ausgabeBild, i, 9);
    }

    drawHistogram(eingabeBild, histogramm, histogrammBild, breite, hoehe );
    imshow("Histogram", histogrammBild);

    waitKey();
    return 0;
}