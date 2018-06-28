// KNN.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include<opencv2/opencv.hpp>
#include<cv.h>
#include <highgui.h>
#include <windows.h>  
#include<iostream>
using namespace std;
using namespace cv;
#define FILE_BUFFER_LENGTH 60000
static int K = 31;
static double simila[1000];
static int num[1000];
static int id[10] ;
static int tagid[10] = { 0,1,2,3,4,5,6,7,8,9 };

void sim(char* filename) {
	string strSrcImageName1 = filename;
	Mat src_test, hsv_test;
	src_test = imread(filename, 1);
	for (int n = 0; n < 1000; n++)
	{
		char buf[1000];
		sprintf_s(buf, "corel/%d.jpg", n);
		Mat src_train, hsv_train;
		src_train = imread(buf, 1);

		/// 转换到 HSV
		cvtColor(src_test, hsv_test, CV_BGR2HSV);
		cvtColor(src_train, hsv_train, CV_BGR2HSV);


		/// 构建直方图xy坐标划分
		int h_bins = 64;
		int s_bins = 60;
		int histSize[] = { h_bins, s_bins };

		// 构建ranges取值范围
		float h_ranges[] = { 0, 256 };
		float s_ranges[] = { 0, 180 };
		const float* ranges[] = { h_ranges, s_ranges };

		// 使用H、S通道
		int channels[] = { 0, 1 };


		MatND hist_test;
		MatND hist_train;

		//计算HSV图像的直方图并归一化
		calcHist(&hsv_test, 1, channels, Mat(), hist_test, 2, histSize, ranges, true, false);
		//normalize(hist_test, hist_test, 0, 1, NORM_MINMAX, -1, Mat());
		calcHist(&hsv_train, 1, channels, Mat(), hist_train, 2, histSize, ranges, true, false);
		//normalize(hist_train, hist_train, 0, 1, NORM_MINMAX, -1, Mat());
		//计算相似度
		simila[n] = compareHist(hist_test, hist_train, CV_COMP_BHATTACHARYYA);
		//cout << simila[n] << endl;

	}

	/*for ( int n = 0; n<1000; n++)
	{
	char buf[1000];
	sprintf_s(buf, "corel/%d.jpg", n);//把不同的第i个图片名称
	Mat image = imread(buf, 0);//读入一张图
	if (!image.data)   //判读读取图片是否成功
	{
	printf("Oh，no，读取img图片文件错误~！ \n");
	}
	string name = buf;
	string strSrcImageName2 = name;
	cv::Mat mat1, mat2, matSrc1, matSrc2;
	mat1 = cv::imread(strSrcImageName1, CV_LOAD_IMAGE_COLOR);
	mat2 = cv::imread(strSrcImageName2, CV_LOAD_IMAGE_COLOR);
	CV_Assert(mat1.channels() == 3);
	CV_Assert(mat2.channels() == 3);
	//cv::resize(mat1, matSrc1, cv::Size(256, 384), 0, 0, cv::INTER_NEAREST);
	//cv::resize(mat2, matSrc2, cv::Size(256, 384), 0, 0, cv::INTER_LANCZOS4);
	cv::Mat matDst1, matDst2;
	cv::resize(mat1, matDst1, cv::Size(8, 8), 0, 0, cv::INTER_CUBIC);
	cv::resize(mat2, matDst2, cv::Size(8, 8), 0, 0, cv::INTER_CUBIC);
	cv::cvtColor(matDst1, matDst1, CV_BGR2GRAY);
	cv::cvtColor(matDst2, matDst2, CV_BGR2GRAY);
	int iAvg1 = 0, iAvg2 = 0;
	int arr1[64], arr2[64];
	for (int i = 0; i < 8; i++) {
	uchar* data1 = matDst1.ptr<uchar>(i);
	uchar* data2 = matDst2.ptr<uchar>(i);
	int tmp = i * 8;
	for (int j = 0; j < 8; j++) {
	int tmp1 = tmp + j;
	arr1[tmp1] = data1[j] / 4 * 4;
	arr2[tmp1] = data2[j] / 4 * 4;
	iAvg1 += arr1[tmp1];
	iAvg2 += arr2[tmp1];
	}
	}
	iAvg1 /= 64;
	iAvg2 /= 64;
	for (int i = 0; i < 64; i++) {
	arr1[i] = (arr1[i] >= iAvg1) ? 1 : 0;
	arr2[i] = (arr2[i] >= iAvg2) ? 1 : 0;
	}
	int iDiffNum = 0;
	for (int i = 0; i < 64; i++)
	if (arr1[i] != arr2[i])
	++iDiffNum;
	simila[n] = iDiffNum;
	//cout << simila[n] << endl;
	//cout << "iDiffNum = " << iDiffNum << endl;
	//if (iDiffNum <= 5)
	//cout << n << endl;
	//cout << "two images are very similar!" << endl;
	//else if (iDiffNum > 10)
	//cout << "they are two different images!" << endl;
	//else
	//cout << "two image are somewhat similar!" << endl;

	}*/
}

void KNN(char* filename) {

	for (int m = 0; m < 1000; m++)
	{
		num[m] = m;
	}

	/*for (int i = 0; i < 1000; i++)
	{
		cout << num[i];
	}*/
	//从小到大排序
	int i = 0;
	int j = 0;
	int t = 0;
	for (i = 0; i<1000; i++)
	{
		t = i;
		for (j = i + 1; j<1000; j++)
		{
			if (simila[t]>simila[j])
			{
				t = j;
			}
		}
		if (i != t)
		{
			simila[i] = simila[i] + simila[t];
			simila[t] = simila[i] - simila[t];
			simila[i] = simila[i] - simila[t];
			num[i] = num[i] + num[t];
			num[t] = num[i] - num[t];
			num[i] = num[i] - num[t];
		}
	}
	/*for (int i = 0; i < K; i++)
	{
		cout << num[i]<<endl;
	}*/
	
}

void TAG() 
{
    int c = 0;
	cout << "The most similar K photos and their TAG are:" << endl;
	for (int x = 1; x < K; x++)
	{
		c = num[x];
		cout << c ;
		if ((0 <= c) && (c <= 99))
		{
			id[0] = id[0] + 1;
		    cout << "blacks\t" ;
		}
		else if ((100 <= c) && (c <= 199))
		{
			id[1] = id[1] + 1;
			cout << "beach\t";
		}
		else if ((200 <= c) && (c <= 299))
		{
			id[2] = id[2] + 1;
			cout << "buliding\t" ;
		}
		else if ((300 <= c) && (c <= 399))
		{
			id[3] = id[3] + 1;
			cout << "bus\t" ;
		}
		else if ((400 <= c) && (c <= 499))
		{
			id[4] = id[4] + 1;
			cout << "dinosaur\t" ;
		}
		else if ((500 <= c) && (c <= 599))
		{
			id[5] = id[5] + 1;
			cout << "elephant\t" ;
		}
		else if ((600 <= c) && (c <= 699))
		{
			id[6] = id[6] + 1;
			cout << "flwoer\t" ;
		}
		else if ((700 <= c) && (c <= 799))
		{
			id[7] = id[7] + 1;
			cout << "horse\t";
		}
		else if ((800 <= c) && (c <= 899))
		{
			id[8] = id[8] + 1;
			cout << "mountain\t";
		}
		else
		{
			id[9] = id[9] + 1;
			cout << "food\t";
		}
	}
	cout << endl;
	//由大到小排序
	int i = 0;
	int j = 0;
	int t = 0;
	for (i = 0; i<K; i++)
	{
		t = i;
		for (j = i + 1; j<K; j++)
		{
			if (id[t]<id[j])
			{
				t = j;
			}
		}
		if (i != t)
		{
			id[i] = id[i] + id[t];
			id[t] = id[i] - id[t];
			id[i] = id[i] - id[t];
			tagid[i] = tagid[i] + tagid[t];
			tagid[t] = tagid[i] - tagid[t];
			tagid[i] = tagid[i] - tagid[t];
		}
	}

	int judge = tagid[0];
	switch (judge)
	{
	case 0: cout << "This photo is 'blacks'" << endl;
	break;
	case 1: cout << "This photo is 'beach'" << endl;
	break;
	case 2: cout << "This photo is 'buliding'" << endl;
	break;
	case 3: cout << "This photo is 'bus'" << endl;
	break;
	case 4: cout << "This photo is 'dinosaur'" << endl;
	break;
	case 5: cout << "This photo is 'elephant'" << endl;
	break;
	case 6: cout << "This photo is 'flwoer'" << endl;
	break;
	case 7: cout << "This photo is 'horse'" << endl;
	break;
	case 8: cout << "This photo is 'mountain'" << endl;
	break;
	case 9: cout << "This photo is 'food'" << endl;
	break;
	default:
	break;
	}

}


int main()
{
	char* filename = new char;
	cout << "请输入图片名:" << endl;
	cin >> filename;
	sim(filename);
	KNN(filename);
	TAG();
	system("pause");
    return 0;
}

