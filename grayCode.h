/********************************************************
*
*功能：格雷码解码程序
*作者：lyangucas
*说明：
*
*********************************************************/

#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <cstdlib>
#include <fstream>

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/structured_light.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/opencv_modules.hpp>

using namespace std;
using namespace cv;

#define blackThreshold 40  
#define whiteThreshold  5

#define params_width 912
#define params_height 1140


bool decoder_V1(InputArrayOfArrays patternImages, OutputArray disparityMap,
	InputArrayOfArrays blackImages, InputArrayOfArrays whitheImages, int flags);

void computeShadowMasks_V1(InputArrayOfArrays blackImages, InputArrayOfArrays whiteImages,
	OutputArrayOfArrays shadowMasks);

bool getProjPixel_V1(InputArrayOfArrays patternImages, int x, int y, Point &projPix);

int grayToDec_V1(const std::vector<uchar>& gray);
