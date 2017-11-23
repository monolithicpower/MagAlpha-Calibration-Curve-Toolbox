#include "calibrationcurvegenerator.h"
#include "math.h"

unsigned char extractAngleErrorHarmonics(   float referenceAngleInDegree[],
                                            float measuredAngleInDegree[],
                                            float angleErrorArrayInDegree[],
                                            const unsigned int angleArraySize,
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

    for(i=0;i<angleArraySize;++i)
    {
        angleErrorArrayInDegree[i]=modulo(measuredAngleInDegree[i]-referenceAngleInDegree[i], 360.0);
        meanAngleError+=angleErrorArrayInDegree[i];
    }
    meanAngleError/=(float)angleArraySize;
    for  (i=0;i<angleArraySize;++i)
    {
        angleErrorArrayInDegree[i]-=meanAngleError;
    }
    getHarmonics(angleErrorArrayInDegree, angleArraySize, 1, pH1, pPhi1);
    getHarmonics(angleErrorArrayInDegree, angleArraySize, 2, pH2, pPhi2);
    getHarmonics(angleErrorArrayInDegree, angleArraySize, 3, pH3, pPhi3);
    getHarmonics(angleErrorArrayInDegree, angleArraySize, 4, pH4, pPhi4);
    return 0;
}

unsigned char getFittedCurve(   float measuredAngleInDegree[],
                                float angleFittingArrayInDegree[],
                                const unsigned int angleArraySize,
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
    for  (i=0; i < angleArraySize; ++i)
    {
        angleRadian = measuredAngleInDegree[i]*M_PI/180.0;
        angleFittingArrayInDegree[i]=   (*pH1)*cosf((1.0*angleRadian-(*pPhi1)))+
                                        (*pH2)*cosf((2.0*angleRadian-(*pPhi2)))+
                                        (*pH3)*cosf((3.0*angleRadian-(*pPhi3)))+
                                        (*pH4)*cosf((4.0*angleRadian-(*pPhi4)));
    }
    return 0;
}


unsigned char getHarmonics(float angleErrorArrayInDegree[],
                           const unsigned int angleArraySize,
                           unsigned int harmonicOrder,
                           float *pHarmonicAmplitude,
                           float *pHarmonicPhase)
{
    float period = (float)angleArraySize/(float)harmonicOrder;
    unsigned int i;
    float cosHarmonic;
    float sinHarmonic;
    float x;
    float y;
    float sumCosHarmonic = 0.0;
    float sumSinHarmonic = 0.0;
    for (i=0; i<angleArraySize; ++i)
    {
        cosHarmonic = cosf((float)i*2.0*M_PI/period);
        sinHarmonic = sinf((float)i*2.0*M_PI/period);
        sumCosHarmonic += angleErrorArrayInDegree[i]*cosHarmonic;
        sumSinHarmonic += angleErrorArrayInDegree[i]*sinHarmonic;
    }
    x = (2.0/(float)angleArraySize)*sumCosHarmonic;
    y = (2.0/(float)angleArraySize)*sumSinHarmonic;
    *pHarmonicAmplitude = sqrtf(powf(x,2.0)+powf(y,2.0));
    *pHarmonicPhase =atan2f(y, x);
    return 0;
}

float modulo(float x, float y)
{
    float b = x / y;
    return (b - floorf(b)) * y;
}
