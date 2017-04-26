#include "stdafx.h"
#include"grayCode.h"


/********************************************************
*
*功能：解码
*日期：2017-04-12
*说明：
*
*********************************************************/
bool decoder_V1(InputArrayOfArrays patternImages, OutputArray disparityMap,
	InputArrayOfArrays blackImages, InputArrayOfArrays whitheImages, int flags)
{
	std::vector<std::vector<Mat> >& acquired_pattern = *(std::vector<std::vector<Mat> >*) patternImages.getObj();

	if (flags == structured_light::DECODE_3D_UNDERWORLD)
	{
		// Computing shadows mask
		std::vector<Mat> shadowMasks;
		computeShadowMasks_V1(blackImages, whitheImages, shadowMasks);

		int cam_width = acquired_pattern[0][0].cols;
		int cam_height = acquired_pattern[0][0].rows;

		Point projPixel;
		// Storage for the pixels of the two cams that correspond to the same pixel of the projector
		std::vector<std::vector<std::vector<Point> > > camsPixels;
		camsPixels.resize(acquired_pattern.size());

		// 解码
		//数据读写
		string file_Decoder = "Decode.txt";
		ofstream outFileD(file_Decoder.c_str(), ios_base::out);
		if (!outFileD.is_open())
		{
			return -1;
		}
		outFileD << "i" << "\t\t\t\t" << "j" << "\t\t\t\t" << "projPixel.x" << "\t\t\t\t" << "projPixel.y";//每列数据用 tab 隔开
		outFileD << endl;//换行

		for (size_t k = 0; k < acquired_pattern.size(); k++)
		{
			camsPixels[k].resize(params_height * params_width);
			for (int i = 0; i < cam_width; i++)
			{
				for (int j = 0; j < cam_height; j++)
				{
					//if the pixel is not shadowed, reconstruct
					if (shadowMasks[k].at<uchar>(j, i))
					{
						//for a (x,y) pixel of the camera returns the corresponding projector pixel by calculating the decimal number
						bool error = getProjPixel_V1(acquired_pattern[k], i, j, projPixel);
						if (error)
						{
							continue;
						}
						outFileD << i << "\t\t\t\t" << j << "\t\t\t\t" << projPixel.x << "\t\t\t\t" << projPixel.y;//每列数据用 tab 隔开
						outFileD << endl;//换行

						camsPixels[k][projPixel.x * params_height + projPixel.y].push_back(Point(i, j));
					}
				}
			}
		}


		std::vector<Point> cam1Pixs, cam2Pixs;
		Mat& disparityMap_ = *(Mat*)disparityMap.getObj();
		disparityMap_ = Mat(cam_height, cam_width, CV_64F, double(0));

		double number_of_pixels_cam1 = 0;
		double number_of_pixels_cam2 = 0;

		//数据读写
		string file_name = "PixelMatch.txt";
		ofstream outFile(file_name.c_str(), ios_base::out);  
		if (!outFile.is_open())
		{
			return -1;
		}
		outFile << "p1.y" << "\t\t\t\t" << "p1.x" << "\t\t\t\t" << "sump1x" << "\t\t\t\t" << "sump2x";//每列数据用 tab 隔开
		outFile << endl;//换行  

		//视差计算
		for (int i = 0; i < params_width; i++)
		{
			for (int j = 0; j < params_height; j++)
			{
				cam1Pixs = camsPixels[0][i * params_height + j];
				cam2Pixs = camsPixels[1][i * params_height + j];

				if (cam1Pixs.size() == 0 || cam2Pixs.size() == 0)
					continue;

				Point p1;
				Point p2;

				double sump1x = 0;
				double sump2x = 0;

				number_of_pixels_cam1 += cam1Pixs.size();
				number_of_pixels_cam2 += cam2Pixs.size();
				for (int c1 = 0; c1 < (int)cam1Pixs.size(); c1++)
				{
					p1 = cam1Pixs[c1];
					sump1x += p1.x;
				}
				for (int c2 = 0; c2 < (int)cam2Pixs.size(); c2++)
				{
					p2 = cam2Pixs[c2];
					sump2x += p2.x;
				}

				sump2x /= cam2Pixs.size();
				sump1x /= cam1Pixs.size();
				for (int c1 = 0; c1 < (int)cam1Pixs.size(); c1++)
				{
					p1 = cam1Pixs[c1];
					disparityMap_.at<double>(p1.y, p1.x) = (double)(sump2x - sump1x);
				}
		
				outFile << p1.y << "\t\t\t\t" << p1.x << "\t\t\t\t" << sump1x << "\t\t\t\t" << sump2x;//每列数据用 tab 隔开
				outFile << endl;//换行  

				sump2x = 0;
				sump1x = 0;
			}
		}
		return true;
	}  
	return false;
}


/********************************************************
*
*功能：Mask的确定
*日期：2017-04-12
*说明：
*   (1)Computes the shadows occlusion where we cannot reconstruct the model
*
*********************************************************/
void computeShadowMasks_V1(InputArrayOfArrays blackImages, InputArrayOfArrays whiteImages,
	OutputArrayOfArrays shadowMasks) 
{
	std::vector<Mat>& whiteImages_ = *(std::vector<Mat>*) whiteImages.getObj();
	std::vector<Mat>& blackImages_ = *(std::vector<Mat>*) blackImages.getObj();
	std::vector<Mat>& shadowMasks_ = *(std::vector<Mat>*) shadowMasks.getObj();

	shadowMasks_.resize(whiteImages_.size());

	int cam_width = whiteImages_[0].cols;
	int cam_height = whiteImages_[0].rows;

	// TODO: parallelize for
	for (int k = 0; k < (int)shadowMasks_.size(); k++)
	{
		shadowMasks_[k] = Mat(cam_height, cam_width, CV_8U);
		for (int i = 0; i < cam_width; i++)
		{
			for (int j = 0; j < cam_height; j++)
			{
				double white = whiteImages_[k].at<uchar>(Point(i, j));
				double black = blackImages_[k].at<uchar>(Point(i, j));

				if (abs(white - black) > blackThreshold)
				{
					shadowMasks_[k].at<uchar>(Point(i, j)) = (uchar)1;
				}
				else
				{
					shadowMasks_[k].at<uchar>(Point(i, j)) = (uchar)0;
				}
			}
		}
	}
}


/********************************************************
*
*功能：读取编码值
*日期：2017-04-12
*说明：
*   (1)For a (x,y) pixel of the camera returns the corresponding projector's pixel
*
*********************************************************/
bool getProjPixel_V1(InputArrayOfArrays patternImages, int x, int y, Point &projPix)
{
	std::vector<Mat>& _patternImages = *(std::vector<Mat>*) patternImages.getObj();
	std::vector<uchar> grayCol;
	std::vector<uchar> grayRow;

	bool error = false;
	int xDec, yDec;

	size_t numOfPatternImages;// The number of images of the pattern
	size_t numOfRowImgs;// The number of row images of the pattern
	size_t numOfColImgs;// The number of column images of the pattern

	numOfColImgs = (size_t)ceil(log(double(params_width)) / log(2.0));
	numOfRowImgs = (size_t)ceil(log(double(params_height)) / log(2.0));
	numOfPatternImages = 2 * numOfColImgs + 2 * numOfRowImgs;

	// process column images
	for (size_t count = 0; count < numOfColImgs; count++)
	{
		// get pixel intensity for regular pattern projection and its inverse
		double val1 = _patternImages[count * 2].at<uchar>(Point(x, y));
		double val2 = _patternImages[count * 2 + 1].at<uchar>(Point(x, y));

		// check if the intensity difference between the values of the normal and its inverse projection image is in a valid range
		if (abs(val1 - val2) < whiteThreshold)
			error = true;

		// determine if projection pixel is on or off
		if (val1 > val2)
			grayCol.push_back(1);
		else
			grayCol.push_back(0);
	}
	xDec = grayToDec_V1(grayCol);

	// process row images
	for (size_t count = 0; count < numOfRowImgs; count++)
	{
		// get pixel intensity for regular pattern projection and its inverse
		double val1 = _patternImages[count * 2 + numOfColImgs * 2].at<uchar>(Point(x, y));
		double val2 = _patternImages[count * 2 + numOfColImgs * 2 + 1].at<uchar>(Point(x, y));

		// check if the intensity difference between the values of the normal and its inverse projection image is in a valid range
		if (abs(val1 - val2) < whiteThreshold)
			error = true;

		// determine if projection pixel is on or off
		if (val1 > val2)
			grayRow.push_back(1);
		else
			grayRow.push_back(0);
	}
	yDec = grayToDec_V1(grayRow);

	if ((yDec >= params_height || xDec >= params_width))
	{
		error = true;
	}

	projPix.x = xDec;
	projPix.y = yDec;

	return error;
}


/********************************************************
*
*功能：格雷码值的转换
*日期：2017-04-12
*说明：
*
*********************************************************/
int grayToDec_V1(const std::vector<uchar>& gray)
{
	int dec = 0;

	uchar tmp = gray[0];

	if (tmp)
		dec += (int)pow((float)2, int(gray.size() - 1));

	for (int i = 1; i < (int)gray.size(); i++)
	{
		// XOR operation
		tmp = tmp ^ gray[i];
		if (tmp)
			dec += (int)pow((float)2, int(gray.size() - i - 1));
	}

	return dec;
}