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

   // imshow(pfad + datei, bild);

    double min, max;
    minMaxLoc(bild, &min, &max);
    cout << "kleinster Grauwert: " << min << " Groesster Grauwert: " << max << endl;
    //Test
    //Skalierung
    Mat scal;
    convertScaleAbs(bild, scal, 255.0 / (max - min), -min * (255.0 / (max - min)));

    //
    // imshow(datei, scal);

    //Bilder abspeichern
    imwrite(pfad + datei + ".png", scal);

    delete[] buffer;
    return scal;
}

/*Praktikum 2*/
Mat filter(Mat &eingabeBild, Mat &filterErgebnis, int filterWahl, int filtergroesse){
    /* 1: Gauss-Filter */
    /* 2: Median-Filter */
    /* 3: Bilateralfilter */
    if(filterWahl == 1){
        Mat filterSize = getGaussianKernel(filtergroesse,0);
        GaussianBlur(eingabeBild, filterErgebnis, filterSize.size(), 0);

        //imshow("Gaussian Filter", filterErgebnis);
        return filterErgebnis;
    }if(filterWahl == 2){
        medianBlur(eingabeBild, filterErgebnis, filtergroesse);
        //imshow("Medianfilter", filterErgebnis);
        return filterErgebnis;
    }

    if(filterWahl == 3){
        bilateralFilter(eingabeBild, filterErgebnis, 9, 50, 50);

        //imshow("Bilateral Filter", filterErgebnis);
        return filterErgebnis;
    }

    return eingabeBild;
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
    blur(histogramm, histogramm, Size (1,7));
    normalize(histogramm, histogramm, 0, histogrammBild.rows, NORM_MINMAX, -1, Mat());

    //Histogram zeichen
    for(int i = 0; i < histSize; i++){
        line( histogrammBild, Point( bin_w*(i), hoehe - cvRound(histogramm.at<float>(i)) ) ,
                   Point( bin_w*(i), hoehe), Scalar(255,0,0));
    }

    //Histogram ausgeben
    //imshow("Histogramm ", histogrammBild );
}

void segmentieren(Mat &eingabeBild, int filter, String &pfad, String &datei ){

    //maximalen Grauwert bestimmen.
    double min, max; //max == breite des Histogramms
    minMaxLoc(eingabeBild, &min, &max);
    cout << "minimum: " << min << " maximum: " << max << endl;
    //imshow("Eingabebild", eingabeBild);

    //Histogramm zeichnen
    Mat histogramm;
    Mat histogrammBild( 400, 256, CV_8UC3, Scalar( 0,0,0));

    drawHistogram(eingabeBild, histogramm, histogrammBild, 256, 400 );

    //maximale Krümmung bestimmen mit dem Algorithmus von Tsai
    //alle mögliche Grauwerte t innerhalb des Bereiches von R bis 255-R -> von min+255 bis max -255
    const int R = 25;

    Mat tsai1(256, 1, CV_32F, Scalar(0,0, 0));
    Mat tsai2(256, 1, CV_32F, Scalar(0,0, 0));

    for(int i = R; i < max - R; i++){
        for(int j = 1; j <= R; j++){
            tsai1.at<float>(i) += (histogramm.at<float>(i+j) - histogramm.at<float>(i-j)) / (2.0 * ((float) i) );
        }
        tsai1.at<float>(i) = tsai1.at<float>(i) * (1.0 / ((float) R ));
    }

    for(int i = R; i < max - R; i++){
        for(int j = 1; j <= R; j++){
            tsai2.at<float>(i) += abs(tsai1.at<float>(i+j) - tsai1.at<float>(i-j));
        }
        tsai2.at<float>(i) = (tsai2.at<float>(i) * (1.0 / ((float) R)));
    }

    //Histogramm zeichnen
    normalize(tsai2, tsai2, 0, histogrammBild.rows, NORM_MINMAX, -1, Mat());
    for(int i = R; i < max - R; i++){
        line(histogrammBild, Point(i, 400 - cvRound(tsai2.at<float>(i))), Point(i, 400 - tsai2.at<float>(i+1)), Scalar(50, 0, 250), 1);
    }

    //Histogram abspeichern
    if(filter == 1){
        imwrite(pfad + datei + "_GaussFilter" + ".png", histogrammBild);
    } else if(filter == 2){
        imwrite(pfad + datei + "_Medianfilter" + ".png", histogrammBild);
    } else if(filter == 3){
        imwrite(pfad + datei + "_Bilateral_Filter" + ".png", histogrammBild);
    } else {
        cout << "Falsche nummer für die Filter auswahl -> Bild wurde nicht abgespeichert" << endl;
    }


}

int main(int argc, char * argv[]) {

	//Datei einlesen
	String pfad = argv[1];
    Mat ausgabeBild;

    Mat histogramm;
    int breite = 256;
    int hoehe = 400;

    Mat histogrammBild( hoehe, breite, CV_8UC3, Scalar( 0,0,0) );

    /*Schleife über alle Bilder */
    for(int i = 16; i < 117; i++) {
        String datei = to_string(i);
        cout << datei << endl;
        Mat eingabeBild = readImg(pfad, datei);
        /*Schleife um Alle Filter zu benutzen. */
        for (int j = 1; j < 4; j++) {
            Mat filterergebnis = filter(eingabeBild, ausgabeBild, j, 3);
            segmentieren(filterergebnis, j, pfad, datei);

        }
    }

    waitKey();
    return 0;
}