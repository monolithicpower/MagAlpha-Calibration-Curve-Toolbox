#include "calibrationcurvegenerator.h"
#include "math.h"

static float modulo(float x, float y)
{
    float b = fmodf(x,y);
    return b < 0 ? b + y : b;
}

static void getHarmonics(float angleErrorArrayInDegree[],
                           const unsigned int sizeAngleArray,
                           unsigned int harmonicOrder,
                           float *pHarmonicAmplitude,
                           float *pHarmonicPhase)
{
    float period = (float)sizeAngleArray/(float)harmonicOrder;
    unsigned int i;
    float cosHarmonic;
    float sinHarmonic;
    float x;
    float y;
    float sumCosHarmonic = 0.0;
    float sumSinHarmonic = 0.0;
    for (i=0; i<sizeAngleArray; ++i)
    {
        cosHarmonic = cosf((float)i*2.0*M_PI/period);
        sinHarmonic = sinf((float)i*2.0*M_PI/period);
        sumCosHarmonic += angleErrorArrayInDegree[i]*cosHarmonic;
        sumSinHarmonic += angleErrorArrayInDegree[i]*sinHarmonic;
    }
    x = (2.0/(float)sizeAngleArray)*sumCosHarmonic;
    y = (2.0/(float)sizeAngleArray)*sumSinHarmonic;
    *pHarmonicAmplitude = sqrtf(powf(x,2.0)+powf(y,2.0));
    *pHarmonicPhase =atan2f(y, x);
}

unsigned char extractAngleErrorHarmonics(   float referenceAngleInDegree[],
                                            float measuredAngleInDegree[],
                                            float angleErrorArrayInDegree[],
                                            const unsigned int sizeAngleArray,
                                            float *pH1,
                                            float *pH2,
                                            float *pH3,
                                            float *pH4,
                                            float *pPhi1,
                                            float *pPhi2,
                                            float *pPhi3,
                                            float *pPhi4)
{
    unsigned int i;
    float meanAngleError = 0;
    unsigned int jumpNumber = 0;
    for(i=0;i<sizeAngleArray;++i)
    {
        angleErrorArrayInDegree[i]=modulo(measuredAngleInDegree[i]-referenceAngleInDegree[i], 360.0);
        meanAngleError+=angleErrorArrayInDegree[i];
    }
    //Check for angle error jumps
    for(i=0;i<sizeAngleArray;++i)
    {
        if (i == 0)
        {
            if (fabsf(angleErrorArrayInDegree[0]-angleErrorArrayInDegree[sizeAngleArray-1]) > 180.0)
            {
                jumpNumber++;
            }
        }
        else
        {
            if (fabsf(angleErrorArrayInDegree[i]-angleErrorArrayInDegree[i-1]) > 180.0)
            {
                jumpNumber++;
            }
        }
    }
    //If jumps detected add 180 degree
    if (jumpNumber>0)
    {
        meanAngleError = 0;
        for(i=0;i<sizeAngleArray;++i)
        {
            angleErrorArrayInDegree[i]=modulo(angleErrorArrayInDegree[i]+180.0, 360.0);
            meanAngleError+=angleErrorArrayInDegree[i];
        }
    }
    //substract the mean error value to center the curve around zero
    meanAngleError/=(float)sizeAngleArray;
    for  (i=0;i<sizeAngleArray;++i)
    {
        angleErrorArrayInDegree[i]-=meanAngleError;
    }
    //get harmonics components
    getHarmonics(angleErrorArrayInDegree, sizeAngleArray, 1, pH1, pPhi1);
    getHarmonics(angleErrorArrayInDegree, sizeAngleArray, 2, pH2, pPhi2);
    getHarmonics(angleErrorArrayInDegree, sizeAngleArray, 3, pH3, pPhi3);
    getHarmonics(angleErrorArrayInDegree, sizeAngleArray, 4, pH4, pPhi4);
    return 0;
}

unsigned char generateAngleErrorLookupTableUsingFittedCurve(float angleInDegree[],
                                                            float fittedAngleErrorInDegree[],
                                                            const unsigned int sizeAngleArray,
                                                            float *pH1,
                                                            float *pH2,
                                                            float *pH3,
                                                            float *pH4,
                                                            float *pPhi1,
                                                            float *pPhi2,
                                                            float *pPhi3,
                                                            float *pPhi4)
{
    unsigned int i;
    float angleRadian;
    for  (i=0; i < sizeAngleArray; ++i)
    {
        angleRadian = angleInDegree[i]*M_PI/180.0;
        fittedAngleErrorInDegree[i]=   (*pH1)*cosf((1.0*angleRadian-(*pPhi1)))+
                                        (*pH2)*cosf((2.0*angleRadian-(*pPhi2)))+
                                        (*pH3)*cosf((3.0*angleRadian-(*pPhi3)))+
                                        (*pH4)*cosf((4.0*angleRadian-(*pPhi4)));
    }
    return 0;
}

unsigned char generateAngleErrorLookupTableUsingConstantsAndSlopes( float angleInDegree[],
                                                                    float angleErrorConstants[],
                                                                    float angleErrorSlopes[],
                                                                    const unsigned int sizeAngleArray,
                                                                    float *pH1,
                                                                    float *pH2,
                                                                    float *pH3,
                                                                    float *pH4,
                                                                    float *pPhi1,
                                                                    float *pPhi2,
                                                                    float *pPhi3,
                                                                    float *pPhi4)
{
    unsigned int i;
    float angleRadian;
    float fittedAngleErrorInDegree[sizeAngleArray];
    float shiftedAngleInDegree[sizeAngleArray];
    float shiftedfittedAngleErrorInDegree[sizeAngleArray];
    for  (i=0; i < sizeAngleArray; ++i)
    {
        angleRadian = angleInDegree[i]*M_PI/180.0;
        fittedAngleErrorInDegree[i]=   (*pH1)*cosf((1.0*angleRadian-(*pPhi1)))+
                                        (*pH2)*cosf((2.0*angleRadian-(*pPhi2)))+
                                        (*pH3)*cosf((3.0*angleRadian-(*pPhi3)))+
                                        (*pH4)*cosf((4.0*angleRadian-(*pPhi4)));
    }
    for  (i=0; i < (sizeAngleArray-1); ++i)
    {
        shiftedAngleInDegree[i]=angleInDegree[i+1];
        shiftedfittedAngleErrorInDegree[i]=fittedAngleErrorInDegree[i+1];
    }
    shiftedAngleInDegree[sizeAngleArray-1]=angleInDegree[0];
    shiftedfittedAngleErrorInDegree[sizeAngleArray-1]=fittedAngleErrorInDegree[0];
    for  (i=0; i < sizeAngleArray; ++i)
    {
        angleErrorSlopes[i]=(shiftedfittedAngleErrorInDegree[i]-fittedAngleErrorInDegree[i])/(shiftedAngleInDegree[i]-angleInDegree[i]);
        angleErrorConstants[i]=fittedAngleErrorInDegree[i]-(angleErrorSlopes[i]*angleInDegree[i]);
    }
    return 0;
}
