/** @file main.cpp

    @brief

    <p>(c) 2016, stefan.berke@modular-audio-graphics.com</p>
    <p>All rights reserved</p>

    <p>created 7/11/2016</p>
*/

#if 0

#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}


#else

#include <iostream>
#include "rdh_bd/AudioStream.h"
#include "rdh_bd/BDOnsetStage.h"
#include "rdh_bd/BDRealTimeStage.h"

#define PRINT(arg__) std::cout << arg__ << std::endl;
#define ERROR(arg__) { PRINT(arg__); return -1; }

int main(int , char **)
{
    using namespace RDH_BD;

    BDParamsType params;
    params.init();

    CAudioStream audio;
    CDataStream onsets, temp,
                beats, tempo, period, info;

    if (S_OK != audio.LoadFromFile(
    //"/media/defgsus/Windows7_OS/_/samples/Disorgmatron live set2.2_mono.wav"
    "/home/defgsus/prog/qt_project/mo/matrixoptimizer/data/audio/Document111.wav"
                ))
        ERROR("Failed loading audio");

    PRINT(audio.infoString());

    PRINT("Getting Onsets");

    CBDOnsetStage onsetStage(params);

    if (S_OK != onsetStage.CreateOnsetStream(&audio, &onsets, &temp))
        ERROR("Failed creating onset stream");

    PRINT("onsets: " << onsets.infoString());
    PRINT("temp:   " << temp.infoString());

    PRINT("Getting beat");

    CBDRealTimeStage rtStage(params);

    if (S_OK != rtStage.CreateBeatStream(&onsets, &beats, &tempo, &period, &info))
        ERROR("Failed in RealTimeStage");

    PRINT("beats:   " << beats.infoString());
    PRINT("tempo:   " << tempo.infoString());
    PRINT("period:  " << period.infoString());
    PRINT("info:    " << info.infoString());

    for (int i=0; i<tempo.GetNumSamples(); ++i)
        std::cout << ((FLOAT*)tempo.GetData())[i] << ", ";
    std::cout << std::endl;

    return 0;
}


#endif
