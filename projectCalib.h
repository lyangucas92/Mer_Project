/********************************************************
*
*功能：投影仪标定程序
*作者：lyangucas
*说明：
*
*********************************************************/

#pragma once

#include <opencv2/highgui.hpp>
#include <vector>
#include <iostream>
#include <fstream>

#include <opencv2/core.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/calib3d.hpp>

using namespace std;
using namespace cv;

enum calibrationPattern
{
	CHESSBOARD,
	CIRCLES_GRID,
	ASYMETRIC_CIRCLES_GRID
};

void createObjectPoints(vector<Point3f> &patternCorners, Size patternSize, float squareSize,
	int patternType);

void createProjectorObjectPoints(vector<Point2f> &patternCorners, Size patternSize, float squareSize,
	int patternType);

void fromVectorToMat(vector<Point2f> v, Mat &pts);

void normalize(const Mat &pts, const int& dim, Mat& normpts, Mat &T);

void fromMatToVector(Mat pts, vector<Point2f> &v);

void saveCalibrationData(String path, vector<Mat> T1, vector<Mat> T2, vector<Mat> ptsProjCam, vector<Mat> ptsProjProj, vector<Mat> ptsProjCamN, vector<Mat> ptsProjProjN);

double calibrate(vector< vector<Point3f> > objPoints, vector< vector<Point2f> > imgPoints,
	Mat &cameraMatrix, Mat &distCoeffs, vector<Mat> &r, vector<Mat> &t, Size imgSize);

void fromCamToWorld(Mat cameraMatrix, vector<Mat> rV, vector<Mat> tV,
	vector< vector<Point2f> > imgPoints, vector< vector<Point3f> > &worldPoints);

void saveCalibrationResults(String path, Mat camK, Mat camDistCoeffs, Mat projK, Mat projDistCoeffs,
	Mat fundamental, Mat stereoR, Mat stereoT);
