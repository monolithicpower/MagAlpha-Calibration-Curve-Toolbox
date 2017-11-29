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

static float modulo(float x, float y)
{
    float b = fmodf(x,y);
    return b < 0 ? b + y : b;
}

static float angleOutputWithoutCorrection(float angleOutputInDegree, float zeroDegreeOffset)
{
    return modulo(angleOutputInDegree+zeroDegreeOffset, 360.0);
}

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
    // Find harmonics parameters
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
    //Compute the fit for every measurement points (for test purpose)
    float fittedAngleErrorInDegree[dataLength];
    float angleErrorConstants[dataLength];
    float angleErrorSlopes[dataLength];
    generateAngleErrorLookupTableUsingFittedCurve( measuredAngleArray, fittedAngleErrorInDegree,
                                    dataLength, &h1, &h2, &h3, &h4,
                                    &phi1, &phi2, &phi3, &phi4);
    generateAngleErrorLookupTableUsingConstantsAndSlopes( measuredAngleArray, angleErrorConstants, angleErrorSlopes,
                                    dataLength, &h1, &h2, &h3, &h4,
                                    &phi1, &phi2, &phi3, &phi4);
    //Generate the lookup table that will be use in the MCU application
    //Define the lookup table size
    const unsigned int lookupTableSize = 32;
    float lookupTableInputAngleArray[lookupTableSize];
    float angleStep = 360.0/(float)lookupTableSize;
    std::cout << "\n\nLookup Table Angle Error" <<std::endl;
    for(unsigned int i = 0;i<lookupTableSize;++i)
    {
        lookupTableInputAngleArray[i]=(float)i*angleStep;
        std::cout << "Index[" << i << "] = " << lookupTableInputAngleArray[i] << std::endl;
    }
    float lookupTableFittedOutputAngleArray[lookupTableSize];
    generateAngleErrorLookupTableUsingFittedCurve(lookupTableInputAngleArray, lookupTableFittedOutputAngleArray,
                                                  lookupTableSize, &h1, &h2, &h3, &h4,
                                                  &phi1, &phi2, &phi3, &phi4);
    float lookupTableConstOutputAngleArray[lookupTableSize];
    float lookupTableSlopesOutputAngleArray[lookupTableSize];
    generateAngleErrorLookupTableUsingConstantsAndSlopes(lookupTableInputAngleArray, lookupTableConstOutputAngleArray, lookupTableSlopesOutputAngleArray,
                                                         lookupTableSize, &h1, &h2, &h3, &h4,
                                                         &phi1, &phi2, &phi3, &phi4);
    //Compute the interpolated points
    float correctedAngleConstSlopes[dataLength];
    float correctedAngleErrorConstSlopes[dataLength];
    float angleError;

    for(unsigned int i = 0;i<dataLength;++i)
    {
        correctedAngleConstSlopes[i]=interpolateAngleFromConstantsAndSlopes(measuredAngleArray[i],
                                                                            lookupTableConstOutputAngleArray,
                                                                            lookupTableSlopesOutputAngleArray,
                                                                            lookupTableSize,
                                                                            referenceAngleArray[0],
                                                                            &angleError);
        correctedAngleErrorConstSlopes[i]=angleError;
    }
    float correctedAngleConstSlopesLinSearch[dataLength];
    float correctedAngleErrorConstSlopesLinSearch[dataLength];
    for(unsigned int i = 0;i<dataLength;++i)
    {
        correctedAngleConstSlopesLinSearch[i]=interpolateAngleFromConstantsAndSlopesUsingLinearSearch(measuredAngleArray[i],
                                                                                                      lookupTableInputAngleArray,
                                                                                                      lookupTableConstOutputAngleArray,
                                                                                                      lookupTableSlopesOutputAngleArray,
                                                                                                      lookupTableSize,
                                                                                                      referenceAngleArray[0],
                                                                                                      &angleError);
        correctedAngleErrorConstSlopesLinSearch[i]=angleError;
    }
    float correctedAngleFittedCurve[dataLength];
    float correctedAngleErrorFittedCurve[dataLength];
    for(unsigned int i = 0;i<dataLength;++i)
    {
        correctedAngleFittedCurve[i]=interpolateAngleFromFittedCurve(measuredAngleArray[i],
                                                                     lookupTableInputAngleArray,
                                                                     lookupTableFittedOutputAngleArray,
                                                                     lookupTableSize,
                                                                     referenceAngleArray[0],
                                                                     &angleError);
        correctedAngleErrorFittedCurve[i]=angleError;
    }
    float measuredAngleWithZeroCorrection[dataLength];
    for(unsigned int i = 0;i<dataLength;++i)
    {
        measuredAngleWithZeroCorrection[i]=angleOutputWithoutCorrection(measuredAngleArray[i], referenceAngleArray[0]);
    }
    QFile outpuFile;
    QFileInfo outputFileInfo;
    outputFileInfo.setFile("..\\output-files\\calibration_curve.csv");
    QDir outputDir;
    if (!outputDir.mkpath(outputFileInfo.path()))
    {
        std::cout << "Error, Program was unamble to create the directory: " << qPrintable(outputFileInfo.path()) << std::endl;
    }
    QDir::setCurrent(outputFileInfo.path());
    outpuFile.setFileName(outputFileInfo.fileName());
    if(outpuFile.open(QFile::WriteOnly |QFile::Truncate))
    {
        QTextStream output(&outpuFile);
        output <<"Reference Angle" << "," << "Measured Angle" << "," << "Measured Angle with Zero Correction" << "," << "Angle Error" << "," << "Angle Error Fitting" <<
                 "," << "H1" << "," << "H2" << "," << "H3" << "," << "H4" <<
                 "," << "Ph1" << "," << "Ph2" << "," << "Ph3" << "," << "Ph4" <<
                 "," << "Number of points" <<
                 "," << "Corrected Angle Cst + Slope" << "," << "Angle Error after fit Cst + Slope" <<
                 "," << "Corrected Angle Cst + Slope Lin Search" << "," << "Angle Error after fit Cst + Slope Lin Search" <<
                 "," << "Corrected Angle Fitted" << "," << "Angle Error after Fit" <<
                 endl;
        for (unsigned int i = 0; i<dataLength;++i)
        {
            output << referenceAngleArray[i] << "," << measuredAngleArray[i] << "," << measuredAngleWithZeroCorrection[i] << "," << angleErrorArray[i] << "," << fittedAngleErrorInDegree[i] << "," <<
                      h1 << "," << h2 << "," << h3 << "," << h4 << "," <<
                      phi2 << "," << phi2 << "," << phi2 << "," << phi2 << "," <<
                      dataLength<< "," <<
                      correctedAngleConstSlopes[i] << "," << correctedAngleErrorConstSlopes[i] <<"," <<
                      correctedAngleConstSlopesLinSearch[i] << "," << correctedAngleErrorConstSlopesLinSearch[i] <<"," <<
                      correctedAngleFittedCurve[i] << "," << correctedAngleErrorFittedCurve[i] <<
                      endl;
        }
    }
    outpuFile.close();
    return 0;
}
