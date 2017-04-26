#include "stdafx.h"
#include "projectCalib.h"


void createObjectPoints(vector<Point3f> &patternCorners, Size patternSize, float squareSize,
	int patternType)
{
	switch (patternType)
	{
	case CHESSBOARD:
	case CIRCLES_GRID:
		for (int i = 0; i < patternSize.height; ++i)
		{
			for (int j = 0; j < patternSize.width; ++j)
			{
				patternCorners.push_back(Point3f(float(i*squareSize), float(j*squareSize), 0));
			}
		}
		break;
	case ASYMETRIC_CIRCLES_GRID:
		break;
	}
}


void createProjectorObjectPoints(vector<Point2f> &patternCorners, Size patternSize, float squareSize,
	int patternType)
{
	switch (patternType)
	{
	case CHESSBOARD:
	case CIRCLES_GRID:
		for (int i = 1; i <= patternSize.height; ++i)
		{
			for (int j = 1; j <= patternSize.width; ++j)
			{
				patternCorners.push_back(Point2f(float(j*squareSize), float(i*squareSize)));
			}
		}
		break;
	case ASYMETRIC_CIRCLES_GRID:
		break;
	}
}

void fromVectorToMat(vector<Point2f> v, Mat &pts)
{
	int nbrOfPoints = (int)v.size();

	if (pts.empty())
		pts.create(2, nbrOfPoints, CV_32F);

	for (int i = 0; i < nbrOfPoints; ++i)
	{
		pts.at<float>(0, i) = v[i].x;
		pts.at<float>(1, i) = v[i].y;
	}
}


void normalize(const Mat &pts, const int& dim, Mat& normpts, Mat &T)
{
	float averagedist = 0;
	float scale = 0;

	//centroid

	Mat centroid(dim, 1, CV_32F);
	Scalar tmp;

	if (normpts.empty())
	{
		normpts = Mat(pts.rows, pts.cols, CV_32F);
	}

	for (int i = 0; i < dim; ++i)
	{
		tmp = mean(pts.row(i));
		centroid.at<float>(i, 0) = (float)tmp[0];
		subtract(pts.row(i), centroid.at<float>(i, 0), normpts.row(i));
	}

	//average distance

	Mat ptstmp;
	for (int i = 0; i < normpts.cols; ++i)
	{
		ptstmp = normpts.col(i);
		averagedist = averagedist + (float)norm(ptstmp);
	}
	averagedist = averagedist / normpts.cols;
	scale = (float)(sqrt(dim) / averagedist);

	normpts = normpts * scale;

	T = cv::Mat::eye(dim + 1, dim + 1, CV_32F);
	for (int i = 0; i < dim; ++i)
	{
		T.at<float>(i, i) = scale;
		T.at<float>(i, dim) = -scale*centroid.at<float>(i, 0);
	}
}


void fromMatToVector(Mat pts, vector<Point2f> &v)
{
	int nbrOfPoints = pts.cols;

	for (int i = 0; i < nbrOfPoints; ++i)
	{
		Point2f temp;
		temp.x = pts.at<float>(0, i);
		temp.y = pts.at<float>(1, i);
		v.push_back(temp);
	}
}


void saveCalibrationData(String path, vector<Mat> T1, vector<Mat> T2, vector<Mat> ptsProjCam, vector<Mat> ptsProjProj, vector<Mat> ptsProjCamN, vector<Mat> ptsProjProjN)
{
	FileStorage fs(path + ".yml", FileStorage::WRITE);

	int size = (int)T1.size();
	fs << "size" << size;
	for (int i = 0; i < (int)T1.size(); ++i)
	{
		ostringstream nbr;
		nbr << i;
		fs << "TprojCam" + nbr.str() << T1[i];
		fs << "TProjProj" + nbr.str() << T2[i];
		fs << "ptsProjCam" + nbr.str() << ptsProjCam[i];
		fs << "ptsProjProj" + nbr.str() << ptsProjProj[i];
		fs << "ptsProjCamN" + nbr.str() << ptsProjCamN[i];
		fs << "ptsProjProjN" + nbr.str() << ptsProjProjN[i];
	}
	fs.release();
}


double calibrate(vector< vector<Point3f> > objPoints, vector< vector<Point2f> > imgPoints,
	Mat &cameraMatrix, Mat &distCoeffs, vector<Mat> &r, vector<Mat> &t, Size imgSize)
{
	int calibFlags = 0;

	double rms = calibrateCamera(objPoints, imgPoints, imgSize, cameraMatrix,
		distCoeffs, r, t, calibFlags);

	return rms;
}

void fromCamToWorld(Mat cameraMatrix, vector<Mat> rV, vector<Mat> tV,
	vector< vector<Point2f> > imgPoints, vector< vector<Point3f> > &worldPoints)
{
	int s = (int)rV.size();
	Mat invK64, invK;
	invK64 = cameraMatrix.inv();
	invK64.convertTo(invK, CV_32F);

	for (int i = 0; i < s; ++i)
	{
		Mat r, t, rMat;
		rV[i].convertTo(r, CV_32F);
		tV[i].convertTo(t, CV_32F);

		Rodrigues(r, rMat);
		Mat transPlaneToCam = rMat.inv()*t;

		vector<Point3f> wpTemp;
		int s2 = (int)imgPoints[i].size();
		for (int j = 0; j < s2; ++j){
			Mat coords(3, 1, CV_32F);
			coords.at<float>(0, 0) = imgPoints[i][j].x;
			coords.at<float>(1, 0) = imgPoints[i][j].y;
			coords.at<float>(2, 0) = 1.0f;

			Mat worldPtCam = invK*coords;
			Mat worldPtPlane = rMat.inv()*worldPtCam;

			float scale = transPlaneToCam.at<float>(2) / worldPtPlane.at<float>(2);
			Mat worldPtPlaneReproject = scale*worldPtPlane - transPlaneToCam;

			Point3f pt;
			pt.x = worldPtPlaneReproject.at<float>(0);
			pt.y = worldPtPlaneReproject.at<float>(1);
			pt.z = 0;
			wpTemp.push_back(pt);
		}
		worldPoints.push_back(wpTemp);
	}
}

void saveCalibrationResults(String path, Mat camK, Mat camDistCoeffs, Mat projK, Mat projDistCoeffs,
	Mat fundamental)
{
	FileStorage fs(path + ".yml", FileStorage::WRITE);
	fs << "camIntrinsics" << camK;
	fs << "camDistCoeffs" << camDistCoeffs;
	fs << "projIntrinsics" << projK;
	fs << "projDistCoeffs" << projDistCoeffs;
	fs << "fundamental" << fundamental;
	fs.release();
}
