#include "angleinterpolation.h"
#include "math.h"

static float modulo(float x, float y)
{
    float b = x / y;
    return (b - floorf(b)) * y;
}

static unsigned int moduloInt(float x, float y)
{
    float b = x / y;
    return (int)roundf((b - floorf(b)) * y);
}

float interpolateAngleFromConstantsAndSlopes(   float angleToInterpolateInDegree,
                                                float angleErrorConstants[],
                                                float angleErrorSlopes[],
                                                const unsigned int lookupTableSize)
{
    float angleError;
    unsigned int lookupTableIndex;
    lookupTableIndex = modulo(floorf((angleToInterpolateInDegree/360.0)*lookupTableSize), lookupTableSize);
    angleError = angleErrorConstants[lookupTableIndex]+(angleErrorSlopes[lookupTableIndex]*angleToInterpolateInDegree);
    //return the corrected angle
    return angleToInterpolateInDegree-angleError;
}

float interpolateAngleFromConstantsAndSlopesUsingLinearSearch(  float angleToInterpolateInDegree,
                                                                float lookupTableAngle[],
                                                                float angleErrorConstants[],
                                                                float angleErrorSlopes[],
                                                                const unsigned int lookupTableSize)
{
    float angleError;
    unsigned int lookupTableIndex;
    unsigned int i;
    lookupTableIndex = lookupTableSize-1;
    for (i=0; i<lookupTableSize; ++i)
    {
        if (angleToInterpolateInDegree>=lookupTableAngle[i])
        {
            lookupTableIndex=i;
            break;
        }
    }
    angleError = angleErrorConstants[lookupTableIndex]+(angleErrorSlopes[lookupTableIndex]*angleToInterpolateInDegree);
    //return the corrected angle
    return angleToInterpolateInDegree-angleError;
}

float interpolateAngleFromFittedCurve(  float angleToInterpolateInDegree,
                                        float lookupTableAngle[],
                                        float angleErrorInDegree[],
                                        const unsigned int lookupTableSize)
{
    float angleError;
    unsigned int lookupTableIndex;
    float x1, x2, y1, y2;
    unsigned int nPlus1Index;
    float muValue;
    lookupTableIndex = modulo(floorf((angleToInterpolateInDegree/360.0)*lookupTableSize), lookupTableSize);
    nPlus1Index = moduloInt(lookupTableIndex+1, lookupTableSize);
    x1 = lookupTableAngle[lookupTableIndex];
    x2 = lookupTableAngle[nPlus1Index];
    y1 = angleErrorInDegree[lookupTableIndex];
    y2 = angleErrorInDegree[nPlus1Index];
    muValue = mu(x1, x2, angleToInterpolateInDegree);
    angleError = linearInterpolate(y1, y2, muValue);
    return angleToInterpolateInDegree-angleError;
}

float mu(float x1, float x2, float measuredAngleInDegree)
{
   return (measuredAngleInDegree-x1)/(modulo(x2-x1,360.0));
}

float linearInterpolate(float y1, float y2, float mu)
{
   return modulo(y1*(1-mu)+y2*mu, 360.0);
}
