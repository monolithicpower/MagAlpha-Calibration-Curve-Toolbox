#include <QCoreApplication>
#include <iostream>
#include <iomanip>

#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QStringList>
#include <QDebug>

#include "calibrationcurvegenerator.h"
#include "angleinterpolation.h"

int main(int argc, char *argv[])
{
    (void) argc;
    (void) argv;
    QFile file;
    QFileInfo fileInfo;
    if (argc < 2)
    {
        fileInfo.setFile("..\\input-files\\calibration_data_input_example.csv");
        QDir::setCurrent(fileInfo.path());
        std::cout << qPrintable(fileInfo.path()) << std::endl;
        file.setFileName(fileInfo.fileName());
        std::cout << "open default input file: " << qPrintable(fileInfo.absoluteFilePath()) << std::endl;
    }
    else
    {
        fileInfo.setFile(QString(argv[1]));
        QDir::setCurrent(fileInfo.path());
        file.setFileName(fileInfo.fileName());
        std::cout << "open the input file: " << qPrintable(fileInfo.absoluteFilePath()) << std::endl;
    }
    if (!file.open(QIODevice::ReadOnly)) {
        std::cout << "Error: Program not able to open the input file. " << qPrintable(file.errorString()) << std::endl;
        return 1;
    }
    QVector<float> refAngle;
    QVector<float> measuredAngle;
    QList<QByteArray> rowElement;
    QByteArray line;
    bool convserionOk;
    //read header
    line = file.readLine();
    line.chop(2);
    rowElement = line.split(',');
    const size_t MAXWIDTH = 25;
    std::cout << std::left << std::setw(MAXWIDTH*2+4) << std::setfill('-') << "-" << std::endl;
    std::cout << " "  << std::left << std::setw(MAXWIDTH) << std::setfill(' ') << qPrintable(rowElement[0]) <<
                 "| " << std::left << std::setw(MAXWIDTH) << std::setfill(' ') << qPrintable(rowElement[1]) << "|" << std::endl;
    std::cout << std::left << std::setw(MAXWIDTH*2+4) << std::setfill('-') << "-" << std::endl;
    //read the data
    while (!file.atEnd()) {
        line = file.readLine();
        line.chop(2); //remove \r\n
        rowElement = line.split(',');
        refAngle.append(rowElement[0].toFloat(&convserionOk));
        if (convserionOk == false)
        {
            std::cout << "Conversion Error! Not able to convert " << qPrintable(rowElement[0]) << " to float" << std::endl;
        }
        measuredAngle.append(rowElement[1].toFloat(&convserionOk));
        if (convserionOk == false)
        {
            std::cout << "Conversion Error! Not able to convert " << qPrintable(rowElement[1]) << " to float" << std::endl;
        }
        std::cout << " "  << std::left << std::setw(MAXWIDTH) << std::setfill(' ') << qPrintable(rowElement[0]) <<
                     "| " << std::left << std::setw(MAXWIDTH) << std::setfill(' ')<< qPrintable(rowElement[1]) << "|" << std::endl;
    }
    file.close();

    const unsigned int dataLength = refAngle.size();
    float referenceAngleArray[dataLength];
    float measuredAngleArray[dataLength];

    // The code below show an example on how to convert a raw angle value in degree
    float fullScaleValue = 360.0; //for example 512.0 is the full scale value for 9 bit data length (2^9)
    for (unsigned int i = 0; i<dataLength;++i)
    {
        referenceAngleArray[i] = (float)refAngle[i]*360.0/fullScaleValue;
        measuredAngleArray[i] = (float)measuredAngle[i]*360.0/fullScaleValue;
    }

    //Call Curve fitting function here
    float h1;
    float h2;
    float h3;
    float h4;
    float phi1;
    float phi2;
    float phi3;
    float phi4;
    float angleErrorArray[dataLength];

    extractAngleErrorHarmonics(referenceAngleArray,
                               measuredAngleArray,
                               angleErrorArray,
                               dataLength,
                               &h1,
                               &h2,
                               &h3,
                               &h4,
                               &phi1,
                               &phi2,
                               &phi3,
                               &phi4);

    float fittedAngleErrorInDegree[dataLength];
    float angleErrorConstants[dataLength];
    float angleErrorSlopes[dataLength];
//    float correctedAngleArrayInDegree[dataLength];

    generateAngleErrorLookupTableUsingFittedCurve( measuredAngleArray, fittedAngleErrorInDegree,
                                    dataLength, &h1, &h2, &h3, &h4,
                                    &phi1, &phi2, &phi3, &phi4);

    generateAngleErrorLookupTableUsingConstantsAndSlopes( measuredAngleArray, angleErrorConstants, angleErrorSlopes,
                                    dataLength, &h1, &h2, &h3, &h4,
                                    &phi1, &phi2, &phi3, &phi4);




    QFile outpuFile;
    QFileInfo outputFileInfo;
    outputFileInfo.setFile("..\\output-files\\calibration_curve.csv");
    QDir::setCurrent(outputFileInfo.path());
    outpuFile.setFileName(outputFileInfo.fileName());
    if(outpuFile.open(QFile::WriteOnly |QFile::Truncate))
    {
        QTextStream output(&outpuFile);
        output <<"Reference Angle" << "," << "Measured Angle" << "," << "Angle Error" << "," << "Angle Error Fitting" <<
                 "," << "H1" << "," << "H2" << "," << "H3" << "," << "H4" <<
                 "," << "Ph1" << "," << "Ph2" << "," << "Ph3" << "," << "Ph4" <<
                 "," << "Number of points" <<endl;
        for (int i = 0; i<refAngle.size();++i)
        {
            output << referenceAngleArray[i] << "," << measuredAngleArray[i] << "," << angleErrorArray[i] << "," << fittedAngleErrorInDegree[i] << "," <<
                      h1 << "," << h2 << "," << h3 << "," << h4 << "," <<
                      phi2 << "," << phi2 << "," << phi2 << "," << phi2 << "," <<
                      dataLength<< endl;
        }
    }
    outpuFile.close();
    return 0;
}
