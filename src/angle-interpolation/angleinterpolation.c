#include "angleinterpolation.h"
#include "math.h"

static float modulo(float x, float y)
{
    float b = fmodf(x,y);
    return b < 0 ? b + y : b;
}

static int moduloInt(int x, int y)
{
    int b = x % y;
    return b < 0 ? b + y : b;
}

float interpolateAngleFromConstantsAndSlopes(   float angleToInterpolateInDegree,
                                                float angleErrorConstants[],
                                                float angleErrorSlopes[],
                                                const unsigned int lookupTableSize,
                                                float zeroDegreeOffset,
                                                float *pAngleError)
{
    float angleError;
    unsigned int lookupTableIndex;
    lookupTableIndex = modulo(floorf((angleToInterpolateInDegree/360.0)*lookupTableSize), lookupTableSize);
    angleError = angleErrorConstants[lookupTableIndex]+(angleErrorSlopes[lookupTableIndex]*angleToInterpolateInDegree);
    //return the corrected angle
    *pAngleError=angleError;
    return modulo((angleToInterpolateInDegree-angleError)+zeroDegreeOffset, 360.0);
}

float interpolateAngleFromConstantsAndSlopesUsingLinearSearch(  float angleToInterpolateInDegree,
                                                                float lookupTableAngle[],
                                                                float angleErrorConstants[],
                                                                float angleErrorSlopes[],
                                                                const unsigned int lookupTableSize,
                                                                float zeroDegreeOffset,
                                                                float *pAngleError)
{
    float angleError;
    unsigned int lookupTableIndex;
    unsigned int i;
    lookupTableIndex = lookupTableSize-1;
    for (i=0; i<lookupTableSize; ++i)
    {
        if (lookupTableAngle[i]>angleToInterpolateInDegree)
        {
            break;
        }
        else
        {
            lookupTableIndex=i;
        }
    }
    angleError = angleErrorConstants[lookupTableIndex]+(angleErrorSlopes[lookupTableIndex]*angleToInterpolateInDegree);
    //return the corrected angle
    *pAngleError=angleError;
    return modulo((angleToInterpolateInDegree-angleError)+zeroDegreeOffset, 360.0);
}

float interpolateAngleFromFittedCurve(  float angleToInterpolateInDegree,
                                        float lookupTableAngle[],
                                        float angleErrorInDegree[],
                                        const unsigned int lookupTableSize,
                                        float zeroDegreeOffset,
                                        float *pAngleError)
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
    *pAngleError=angleError;
    return modulo((angleToInterpolateInDegree-angleError)+zeroDegreeOffset, 360.0);
}

float mu(float x1, float x2, float measuredAngleInDegree)
{
   return (measuredAngleInDegree-x1)/(modulo(x2-x1,360.0));
}

float linearInterpolate(float y1, float y2, float mu)
{
   return y1*(1-mu)+y2*mu;
}
