# MagAlpha-Calibration-Curve-Toolbox
Collection of tools and code examples designed to help users improving the accuracy of the MPS MagAlpha magnetic angle sensor by implementing a calibration curve.


# Features
This project contains the following features:
* Calibration curve generator
* Angle Interpolation
* Ma-Cal-Generator App

## Calibration curve generator
C source code that generate a lookup table with the angle error.

The source code can be found in [src/calibration-curve-generator](src/calibration-curve-generator)

## Angle Interpolation
C source code that compute the corrected angle using the previously generated lookup table.

The source code can be found in [src/angle-interpolation](src/angle-interpolation)


## Ma-Cal-Generator App
C++ application that embed the two aforementioned source codes inside a executable test program.  
The application take a CSV file containing the reference and measured angle as input parameter, generate a lookup table of the angle error and then use this lookup table to correct the measured angle. The application return a CSV file with the both the input parameters and the computed corrected angle. This output file will be placed on the *output-files* folder.

The source code can be found in [src/ma-cal-generator](src/ma-cal-generator)


# Installation
## Ma-Cal-Generator App
The **Ma-Cal-Generator** application use Qt Open Source Framework. You can either directly use the compiled Windows executable or you can build it yourself.

### Use the Pre-Built executable
The executable can be found in the *bin.zip* on the [*Releases*](https://github.com/monolithicpower/MagAlpha-Calibration-Curve-Toolbox/releases/latest) page. The executable is provided along the required Qt DLLs.

Unzip the files and copy the *bin* folder into the main *MagAlpha-Calibration-Curve-Toolbox* folder. You should have something like this:
```
MagAlpha-Calibration-Curve-Toolbox
├───bin
├───docs
├───input-files
└───src
    ├───angle-interpolation
    ├───calibration-curve-generator
    └───ma-cal-generator
```

### Build the project
Start by downloading and installing the [Qt Open Source Framework](https://www1.qt.io/download-open-source/).

Then open *Qt Creator*, go to *File* => *Open File or Project...* and select [src/ma-cal-generator/ma-cal-generator.pro](src/ma-cal-generator/ma-cal-generator.pro).
The last step is to built the project. The executable will be located on the *bin* folder.

> This project was built and tested with *Desktop Qt 5.7.0 MinGW 32bit for Windows* but it should works as well with the latest Qt version.

## Calibration curve generator & Angle Interpolation
These source files doesn't require any installation, simply copy the `.c` and `.h` files in you project.
They only require the `math.h` library to works and should therefore be portable to almost any microcontrollers, embedded systems and desktop environments that use C language.

# How to use it
## Ma-Cal-Generator App
Navigate to the `MagAlpha-Calibration-Curve-Toolbox\bin` folder and launch one of the following commands:

You can use the default input CSV file located in `..\input-files\calibration_data_input_example.csv`.
```
ma-cal-generator.exe
```
Or you can specify the CSV input file you want to use.
```
ma-cal-generator.exe ..\input-files\calibration_data_input_example_add_75.csv
```

In both cases the output file will be located in `MagAlpha-Calibration-Curve-Toolbox\output-files\calibration_curve.csv`.

### Input file format
The input file must use the following structure. You can use a much row as you want.

| Reference Angle [degree] | Measured Angle [degree] |
| :----------------------: | :---------------------: |
| 258.047                  | 0.703125                |
| 260.156                  | 2.10938                 |
| 262.266                  | 3.51563                 |
| 263.672                  | 5.625                   |
| 265.781                  | 7.73438                 |
| ...                      | ...                     |

Which gives the CSV format below.
```
Reference Angle [degree],Measured Angle [degree]
258.047,0.703125
260.156,2.10938
262.266,3.51563
263.672,5.625
265.781,7.73438
..., ...
```

### Output file format
The generated output file will use the following structure.

|Reference Angle|Measured Angle|Measured Angle with Zero Correction|Angle Error|Angle Error Fitting|H1|H2|H3|H4|Ph1|Ph2|Ph3|Ph4|Number of points|Corrected Angle Cst + Slope|Angle Error after fit Cst + Slope|Corrected Angle Cst + Slope Lin Search|Angle Error after fit Cst + Slope Lin Search|Corrected Angle Fitted|Angle Error after Fit|
|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
|258.047|0.703125|258.75|0.0596442|-0.461852|0.779883|3.42582|0.26283|1.64956|-1.56693|-1.56693|-1.56693|-1.56693|200|259.219|-0.469254|259.219|-0.469254|259.219|-0.469254|
|260.156|2.10938|260.156|-0.643115|-0.477579|0.779883|3.42582|0.26283|1.64956|-1.56693|-1.56693|-1.56693|-1.56693|200|260.655|-0.498378|260.655|-0.498378|260.655|-0.498378|
|262.266|3.51563|261.563|-1.34685|-0.496033|0.779883|3.42582|0.26283|1.64956|-1.56693|-1.56693|-1.56693|-1.56693|200|262.09|-0.527503|262.09|-0.527503|262.09|-0.527503|
|263.672|5.625|263.672|-0.643481|-0.530978|0.779883|3.42582|0.26283|1.64956|-1.56693|-1.56693|-1.56693|-1.56693|200|264.243|-0.571189|264.243|-0.571189|264.243|-0.571189|
|265.781|7.73438|265.781|-0.643115|-0.577405|0.779883|3.42582|0.26283|1.64956|-1.56693|-1.56693|-1.56693|-1.56693|200|266.396|-0.614876|266.396|-0.614876|266.396|-0.614876|
|...|...|...|...|...|...|...|...|...|...|...|...|...|...|...|...|...|...|...|...|

Which gives the CSV format below.
```
Reference Angle,Measured Angle,Measured Angle with Zero Correction,Angle Error,Angle Error Fitting,H1,H2,H3,H4,Ph1,Ph2,Ph3,Ph4,Number of points,Corrected Angle Cst + Slope,Angle Error after fit Cst + Slope,Corrected Angle Cst + Slope Lin Search,Angle Error after fit Cst + Slope Lin Search,Corrected Angle Fitted,Angle Error after Fit
258.047,0.703125,258.75,0.0596442,-0.461852,0.779883,3.42582,0.26283,1.64956,-1.56693,-1.56693,-1.56693,-1.56693,200,259.219,-0.469254,259.219,-0.469254,259.219,-0.469254
260.156,2.10938,260.156,-0.643115,-0.477579,0.779883,3.42582,0.26283,1.64956,-1.56693,-1.56693,-1.56693,-1.56693,200,260.655,-0.498378,260.655,-0.498378,260.655,-0.498378
262.266,3.51563,261.563,-1.34685,-0.496033,0.779883,3.42582,0.26283,1.64956,-1.56693,-1.56693,-1.56693,-1.56693,200,262.09,-0.527503,262.09,-0.527503,262.09,-0.527503
263.672,5.625,263.672,-0.643481,-0.530978,0.779883,3.42582,0.26283,1.64956,-1.56693,-1.56693,-1.56693,-1.56693,200,264.243,-0.571189,264.243,-0.571189,264.243,-0.571189
265.781,7.73438,265.781,-0.643115,-0.577405,0.779883,3.42582,0.26283,1.64956,-1.56693,-1.56693,-1.56693,-1.56693,200,266.396,-0.614876,266.396,-0.614876,266.396,-0.614876
...,...,...,...,...,...,...,...,...,...,...,...,...,...,...,...,...,...,...,...
```
## Calibration curve generator
First you need to extract the harmonics components from the angle measurements.
```c
#include "calibrationcurvegenerator.h"

//input parameters
const unsigned int sizeAngleArray = 200;         //change if needed
float referenceAngleInDegree[sizeAngleArray];    //fill with ref angle
float measuredAngleInDegree[sizeAngleArray];     //fill with sensor angle
//output parameters
float angleErrorArrayInDegree[sizeAngleArray];
float h1;
float h2;
float h3;
float h4;
float phi1;
float phi2;
float phi3;
float phi4;
extractAngleErrorHarmonics(  referenceAngleInDegree,
                             measuredAngleInDegree,
                             angleErrorArrayInDegree,
                             sizeAngleArray,
                             &h1,
                             &h2,
                             &h3,
                             &h4,
                             &phi1,
                             &phi2,
                             &phi3,
                             &phi4);
```
Define the lookup table size and angles to use.
```c
//Generate the lookup table that will be use in the final application
const unsigned int lookupTableSize = 32;    //Define the lookup table size
float angleStep = 360.0/(float)lookupTableSize;
unsigned int i;
//output parameters
float lookupTableInputAngleArray[lookupTableSize];
for(i = 0;i<lookupTableSize;++i)
{
    lookupTableInputAngleArray[i]=(float)i*angleStep;
}
```
Compute the lookup table correction coefficients using the computed harmonics parameters. Their is several methods available to generate the calibration table.
### Constants and slopes method
This method use two coefficients to represent the angle error:
* *angleErrorConstants* represent the angle error offset for a given angle input
* *angleErrorSlopes* represent the angle error curve slope for a given angle input

This method use more memory to store the lookup tables but is easier to use to interpolate the angle error of any given points.
```c
//output parameters
float angleErrorConstants[sizeAngleArray];
float angleErrorSlopes[sizeAngleArray];
generateAngleErrorLookupTableUsingConstantsAndSlopes(lookupTableInputAngleArray,
    angleErrorConstants, angleErrorSlopes, lookupTableSize,
    &h1, &h2, &h3, &h4, &phi1, &phi2, &phi3, &phi4);
```

### Fitted curve method
This method use a single coefficients to represent the angle error:
* *angleErrorInDegree* represent the angle error value for a given angle input

This method use less memory to store the lookup tables but is slightly more complex to use to interpolate the angle error of any given points.
```c
//output parameters
float angleErrorInDegree[lookupTableSize];
generateAngleErrorLookupTableUsingFittedCurve(lookupTableInputAngleArray,
    angleErrorInDegree, lookupTableSize,
    &h1, &h2, &h3, &h4, &phi1, &phi2, &phi3, &phi4);
```
## Angle Interpolation
The function used to perform the interpolation depends of the method chosen to generate the lookup table.
### Constants and slopes method
```c
//input parameters
float measuredAngleInDegree;    //measured angle to correct
float zeroDegreeOffset = 0.0;   //Reference angle when the sensor return 0 degree (use 0.0 by default)
//output parameters
float interpolatedAngleInDegree;
float interpolatedAngleErrorInDegree;
interpolatedAngleInDegree=interpolateAngleFromConstantsAndSlopes(measuredAngleInDegree,
    angleErrorConstants, angleErrorSlopes, lookupTableSize,
    zeroDegreeOffset, &interpolatedAngleErrorInDegree);
```

### Fitted curve method
```c
//input parameters
float measuredAngleInDegree;    //measured angle to correct
float zeroDegreeOffset = 0.0;   //Reference angle when the sensor return 0 degree (use 0.0 by default)
//output parameters
float interpolatedAngleInDegree;
float interpolatedAngleErrorInDegree;
interpolatedAngleInDegree=interpolateAngleFromFittedCurve(measuredAngleInDegree,
    lookupTableInputAngleArray,angleErrorInDegree, lookupTableSize,
    zeroDegreeOffset, &interpolatedAngleErrorInDegree);
```
