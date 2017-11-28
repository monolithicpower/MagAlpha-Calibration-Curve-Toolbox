/****************************************************************************
 * MIT License
 *
 * Copyright (c) 2017 Mathieu Kaelin for Monolithic Power Systems
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 ****************************************************************************/
#ifndef ANGLEINTERPOLATION_H
#define ANGLEINTERPOLATION_H

#if defined __cplusplus
extern "C" {
#endif

/**
 * @file angleinterpolation.h
 * @author Mathieu Kaelin
 * @date 2017/11/23
 * @brief Functions to interpolate the corrected angle from the calibration lookup table.
 *
 * @see https://www.monolithicpower.com/
 * @see http://sensors.monolithicpower.com/
 */


/**
 * @brief Compute the interpolated angle using the constants and
 * slopes lookup table.
 *
 * @param angleToInterpolateInDegree Angle input
 * @param angleErrorConstants Lookup table with constants values
 * @param angleErrorSlopes Lookup table with slopes values
 * @param lookupTableSize Size of the lookup table
 * @param zeroDegreeOffset Angle offset at 0 degree
 * @param pAngleError Angle Error
 * @return corrected angle
 */
float interpolateAngleFromConstantsAndSlopes(   float angleToInterpolateInDegree,
                                                float angleErrorConstants[],
                                                float angleErrorSlopes[],
                                                const unsigned int lookupTableSize,
                                                float zeroDegreeOffset,
                                                float *pAngleError);

/**
 * @brief Compute the interpolated angle using the constants and
 * slopes lookup table. Find the lookup table index using a linear search.
 *
 * @param angleToInterpolateInDegree Angle input
 * @param lookupTableAngle Lookup table with the measured angle used to compute
 * the angle error values
 * @param angleErrorConstants Lookup table with constants values
 * @param angleErrorSlopes Lookup table with slopes values
 * @param lookupTableSize Size of the lookup table
 * @param zeroDegreeOffset Angle offset at 0 degree,
 * @param pAngleError Angle Error
 * @return corrected angle
 */
float interpolateAngleFromConstantsAndSlopesUsingLinearSearch(  float angleToInterpolateInDegree,
                                                                float lookupTableAngle[],
                                                                float angleErrorConstants[],
                                                                float angleErrorSlopes[],
                                                                const unsigned int lookupTableSize,
                                                                float zeroDegreeOffset,
                                                                float *pAngleError);

/**
 * @brief Compute the interpolated angle using the linear interpolation method.
 *
 * @param angleToInterpolateInDegree Angle input
 * @param lookupTableAngle Lookup table with the input angle used to compute the error
 * @param angleErrorInDegree[] Lookup table with the angle error in degree
 * @param lookupTableSize Size of the lookup table
 * @param zeroDegreeOffset Angle offset at 0 degree
 * @param pAngleError Angle Error
 * @return corrected angle
 */
float interpolateAngleFromFittedCurve(  float angleToInterpolateInDegree,
                                        float lookupTableAngle[],
                                        float angleErrorInDegree[],
                                        const unsigned int lookupTableSize,
                                        float zeroDegreeOffset,
                                        float *pAngleError);

/**
 * @brief Compute where to estimate the value on the interpolated line.
 *
 * The mu value is 0 at the first point and 1 and the second point.
 * For interpolated values between the two points, mu ranges between 0 and 1.
 *
 * Private function only used inside interpolate functions
 * @param x1 Closest angle value smaller or equal to the measured angle
 * @param x2 Closest angle valie bigger than the measured angle
 * @param measuredAngleInDegree
 * @return mu
 */
float mu(float x1, float x2, float measuredAngleInDegree);

/**
 * @brief Compute the linear interpolation of the measured angle.
 *
 * Private function only used inside interpolate functions
 * @param y1 Angle error at x1 angle
 * @param y2 Angle error at x2 angle
 * @param mu Number between 0.0 and 1.0 which defines where to estimate the value on the interpolated line
 * @return interpolated angle error
 */
float linearInterpolate(float y1, float y2, float mu);

#if defined __cplusplus
}
#endif

#endif // ANGLEINTERPOLATION_H
