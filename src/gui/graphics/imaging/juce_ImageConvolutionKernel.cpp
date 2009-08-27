/*
  ==============================================================================

   This file is part of the JUCE library - "Jules' Utility Class Extensions"
   Copyright 2004-9 by Raw Material Software Ltd.

  ------------------------------------------------------------------------------

   JUCE can be redistributed and/or modified under the terms of the GNU General
   Public License (Version 2), as published by the Free Software Foundation.
   A copy of the license is included in the JUCE distribution, or can be found
   online at www.gnu.org/licenses.

   JUCE is distributed in the hope that it will be useful, but WITHOUT ANY
   WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
   A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

  ------------------------------------------------------------------------------

   To release a closed-source product which uses JUCE, commercial licenses are
   available: visit www.rawmaterialsoftware.com/juce for more information.

  ==============================================================================
*/

#include "../../../core/juce_StandardHeader.h"

BEGIN_JUCE_NAMESPACE

#include "juce_ImageConvolutionKernel.h"


//==============================================================================
ImageConvolutionKernel::ImageConvolutionKernel (const int size_) throw()
    : size (size_)
{
    values = new float* [size];

    for (int i = size; --i >= 0;)
        values[i] = new float [size];

    clear();
}

ImageConvolutionKernel::~ImageConvolutionKernel() throw()
{
    for (int i = size; --i >= 0;)
        delete[] values[i];

    delete[] values;
}

//==============================================================================
void ImageConvolutionKernel::setKernelValue (const int x,
                                             const int y,
                                             const float value) throw()
{
    if (((unsigned int) x) < (unsigned int) size
         && ((unsigned int) y) < (unsigned int) size)
    {
        values[x][y] = value;
    }
    else
    {
        jassertfalse
    }
}

void ImageConvolutionKernel::clear() throw()
{
    for (int y = size; --y >= 0;)
        for (int x = size; --x >= 0;)
            values[x][y] = 0;
}

void ImageConvolutionKernel::setOverallSum (const float desiredTotalSum) throw()
{
    double currentTotal = 0.0;

    for (int y = size; --y >= 0;)
        for (int x = size; --x >= 0;)
            currentTotal += values[x][y];

    rescaleAllValues ((float) (desiredTotalSum / currentTotal));
}

void ImageConvolutionKernel::rescaleAllValues (const float multiplier) throw()
{
    for (int y = size; --y >= 0;)
        for (int x = size; --x >= 0;)
            values[x][y] *= multiplier;
}

//==============================================================================
void ImageConvolutionKernel::createGaussianBlur (const float radius) throw()
{
    const double radiusFactor = -1.0 / (radius * radius * 2);
    const int centre = size >> 1;

    for (int y = size; --y >= 0;)
    {
        for (int x = size; --x >= 0;)
        {
            const int cx = x - centre;
            const int cy = y - centre;

            values[x][y] = (float) exp (radiusFactor * (cx * cx + cy * cy));
        }
    }

    setOverallSum (1.0f);
}

//==============================================================================
void ImageConvolutionKernel::applyToImage (Image& destImage,
                                           const Image* sourceImage,
                                           int dx,
                                           int dy,
                                           int dw,
                                           int dh) const
{
    Image* imageCreated = 0;

    if (sourceImage == 0)
    {
        sourceImage = imageCreated = destImage.createCopy();
    }
    else
    {
        jassert (sourceImage->getWidth() == destImage.getWidth()
                  && sourceImage->getHeight() == destImage.getHeight()
                  && sourceImage->getFormat() == destImage.getFormat());

        if (sourceImage->getWidth() != destImage.getWidth()
             || sourceImage->getHeight() != destImage.getHeight()
             || sourceImage->getFormat() != destImage.getFormat())
            return;
    }

    const int imageWidth = destImage.getWidth();
    const int imageHeight = destImage.getHeight();

    if (dx >= imageWidth || dy >= imageHeight)
        return;

    if (dx + dw > imageWidth)
        dw = imageWidth - dx;

    if (dy + dh > imageHeight)
        dh = imageHeight - dy;

    const int dx2 = dx + dw;
    const int dy2 = dy + dh;

    int lineStride, pixelStride;
    uint8* pixels = destImage.lockPixelDataReadWrite (dx, dy, dw, dh, lineStride, pixelStride);
    uint8* line = pixels;

    int srcLineStride, srcPixelStride;
    const uint8* srcPixels = sourceImage->lockPixelDataReadOnly (0, 0, sourceImage->getWidth(), sourceImage->getHeight(), srcLineStride, srcPixelStride);

    if (pixelStride == 4)
    {
        for (int y = dy; y < dy2; ++y)
        {
            uint8* dest = line;
            line += lineStride;

            for (int x = dx; x < dx2; ++x)
            {
                float c1 = 0;
                float c2 = 0;
                float c3 = 0;
                float c4 = 0;

                for (int yy = 0; yy < size; ++yy)
                {
                    const int sy = y + yy - (size >> 1);

                    if (sy >= imageHeight)
                        break;

                    if (sy >= 0)
                    {
                        int sx = x - (size >> 1);
                        const uint8* src = srcPixels + srcLineStride * sy + srcPixelStride * sx;

                        for (int xx = 0; xx < size; ++xx)
                        {
                            if (sx >= imageWidth)
                                break;

                            if (sx >= 0)
                            {
                                const float kernelMult = values[xx][yy];
                                c1 += kernelMult * *src++;
                                c2 += kernelMult * *src++;
                                c3 += kernelMult * *src++;
                                c4 += kernelMult * *src++;
                            }
                            else
                            {
                                src += 4;
                            }

                            ++sx;
                        }
                    }
                }

                *dest++ = (uint8) jmin (0xff, roundFloatToInt (c1));
                *dest++ = (uint8) jmin (0xff, roundFloatToInt (c2));
                *dest++ = (uint8) jmin (0xff, roundFloatToInt (c3));
                *dest++ = (uint8) jmin (0xff, roundFloatToInt (c4));
            }
        }
    }
    else if (pixelStride == 3)
    {
        for (int y = dy; y < dy2; ++y)
        {
            uint8* dest = line;
            line += lineStride;

            for (int x = dx; x < dx2; ++x)
            {
                float c1 = 0;
                float c2 = 0;
                float c3 = 0;

                for (int yy = 0; yy < size; ++yy)
                {
                    const int sy = y + yy - (size >> 1);

                    if (sy >= imageHeight)
                        break;

                    if (sy >= 0)
                    {
                        int sx = x - (size >> 1);
                        const uint8* src = srcPixels + srcLineStride * sy + srcPixelStride * sx;

                        for (int xx = 0; xx < size; ++xx)
                        {
                            if (sx >= imageWidth)
                                break;

                            if (sx >= 0)
                            {
                                const float kernelMult = values[xx][yy];
                                c1 += kernelMult * *src++;
                                c2 += kernelMult * *src++;
                                c3 += kernelMult * *src++;
                            }
                            else
                            {
                                src += 3;
                            }

                            ++sx;
                        }
                    }
                }

                *dest++ = (uint8) roundFloatToInt (c1);
                *dest++ = (uint8) roundFloatToInt (c2);
                *dest++ = (uint8) roundFloatToInt (c3);
            }
        }
    }

    sourceImage->releasePixelDataReadOnly (srcPixels);
    destImage.releasePixelDataReadWrite (pixels);

    if (imageCreated != 0)
        delete imageCreated;
}

END_JUCE_NAMESPACE