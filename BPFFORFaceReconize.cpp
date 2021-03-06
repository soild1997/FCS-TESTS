// BPFFORFaceReconize.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <opencv2/ml.hpp>  
#include <opencv2/opencv.hpp>
#include <windows.h>  
#include <iostream>
#include <string>
#define FILE_BUFFER_LENGTH 60000
using namespace cv;
using namespace ml;
using namespace std;

static int sample_mun_perclass = 3;//训练字符每类数量  
static int class_mun = 15;//训练样本类数  
static int image_cols = 100;
static int image_rows = 100;
//Ptr<ANN_MLP> bp = ANN_MLP::create();
Ptr<ANN_MLP> bp = ANN_MLP::load("bpnet.xml");
Mat labelsMat;

char* WcharToChar(const wchar_t* wp)
{
	char *m_char;
	int len = WideCharToMultiByte(CP_ACP, 0, wp, wcslen(wp), NULL, 0, NULL, NULL);
	m_char = new char[len + 1];
	WideCharToMultiByte(CP_ACP, 0, wp, wcslen(wp), m_char, len, NULL, NULL);
	m_char[len] = '\0';
	return m_char;
}

wchar_t* CharToWchar(const char* c)
{
	wchar_t *m_wchar;
	int len = MultiByteToWideChar(CP_ACP, 0, c, strlen(c), NULL, 0);
	m_wchar = new wchar_t[len + 1];
	MultiByteToWideChar(CP_ACP, 0, c, strlen(c), m_wchar, len);
	m_wchar[len] = '\0';
	return m_wchar;
}

wchar_t* StringToWchar(const string& s)
{
	const char* p = s.c_str();
	return CharToWchar(p);
}

void labelset() 
{
	float labels[45][15] = { 0 };//训练样本标签  
	// 设置标签数据Mat输出  
	for (int i = 0; i < class_mun; i++)
	{
		for (int j = 0; j < sample_mun_perclass; j++)
		{
			for (int k = 0; k<class_mun; k++)
			{
				if (k == i)
					labels[i*sample_mun_perclass + j][k] = 1;
				else labels[i*sample_mun_perclass + j][k] = 0;
			}
		}
	}
	Mat labelsMat(class_mun*sample_mun_perclass, class_mun, CV_32FC1, labels);//设置标签数据Mat  
	cout << "labelsMat:" << endl;
	cout << labelsMat << endl;
	cout << "labelsMat——OK！" << endl;
}


void bpnettrain () 
{
	string fileform = "*.bmp";//文件格式  
	string perfileReadPath = "sample";
	string fileReadPath;
	char temp[256] = {0};
	float trainingData[45][10000] = { 0 };//每一行一个训练样本  

	for (int i = 0; i <=class_mun - 1;++i)//不同类  
	{
		//读取每个类文件夹下所有图像  
		int  j = 0;//每一类下读取图像计数个数  
		sprintf_s(temp, "%d", i);//按顺序读图  
		fileReadPath = perfileReadPath + "/" + temp + "/" + fileform;//文件读取路径  
		cout << "文件夹" << i << endl;
		HANDLE hFile;
		LPCTSTR lpFileName = StringToWchar(fileReadPath);//指定搜索目录和文件类型  
		WIN32_FIND_DATA pNextInfo;//搜索得到的文件信息将储存在pNextInfo中;  
		hFile = FindFirstFile(lpFileName, &pNextInfo);
		if (hFile == INVALID_HANDLE_VALUE)//搜索失败  
		{
			exit(-1);
		}
		//循环读取  
		do
		{
			if (pNextInfo.cFileName[0] == '.')//过滤.和..  
				continue;
			j++;//读取一张图  
			printf("%s\n", WcharToChar(pNextInfo.cFileName));
			//对读入的图片进行处理  
			Mat srcImage = imread(perfileReadPath + "/" + temp + "/" + WcharToChar(pNextInfo.cFileName), CV_LOAD_IMAGE_GRAYSCALE);//读入图像  
			Mat resizeImage;
			Mat trainImage;
			resize(srcImage, resizeImage, Size(image_cols, image_rows), (0, 0), (0, 0), CV_INTER_AREA);//使用象素关系重采样。当图像缩小时候，该方法可以避免波纹出现  
			//threshold(resizeImage, trainImage, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);//对灰度图像进行阈值操作得到二值图像  
			for (int k = 0; k<image_rows*image_cols; ++k)//每个图片  
			{
				trainingData[i*sample_mun_perclass + (j - 1)][k] = (float)resizeImage.data[k];//将数据拷贝到trainingData中  
			}
		}
		while (FindNextFile(hFile, &pNextInfo) && j<sample_mun_perclass);//如果设置读入的图片数量，则以设置的为准，如果图片不够，则读取文件夹下所有图片  
	}
	// 设置训练数据Mat输入  
	Mat trainingDataMat(class_mun*sample_mun_perclass, image_rows*image_cols, CV_32FC1, trainingData);// 设置训练数据Mat  
	//cout<<trainingDataMat<<endl;  
	cout << "trainingDataMat——OK！" << endl;
	//设置bp神经网络的参数  
	Mat layerSizes = (Mat_<int>(1, 4) << image_rows * image_cols, 100,100, class_mun);//设置各层的神经元个数
	bp->setLayerSizes(layerSizes);
	bp->setActivationFunction(ANN_MLP::SIGMOID_SYM, 1.0, 1.0);//激活函数
	bp->setTrainMethod(ANN_MLP::BACKPROP, 0.01, 0.1);//设置训练方法、权值更新率和权值更新冲量
	TermCriteria termcriteria(TermCriteria::EPS, 10000, 0.00001);
	bp->setTermCriteria(termcriteria);//设置结束条件term_crit：终止条件，
	Ptr<TrainData> Data = TrainData::create(trainingDataMat, ROW_SAMPLE, labelsMat);
	cout << "training...." << endl;
	bp->train(Data);
	bp->save("bpnet.xml"); //保存训练 
	cout << "training finish...bpnet.xml saved " << endl;
}


void testphoto(string testfilename) 
{
	Mat test_image = imread(testfilename, CV_LOAD_IMAGE_GRAYSCALE);//读入测试图  
	Mat test_temp;
	resize(test_image, test_temp, Size(image_cols, image_rows), (0, 0), (0, 0), CV_INTER_AREA);//使用象素关系重采样。当图像缩小时候，该方法可以避免波纹出现  
	//threshold(test_temp, test_temp, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);//二值化  
	Mat_<float>sampleMat(1, image_rows*image_cols);
	sampleMat = test_temp.reshape(0, 1);
	//cout << sampleMat;
	Mat responseMat;
	bp->predict(sampleMat,responseMat); //过调用predict函数，我们得到一个输出向量，它是一个1*nClass的行向量
	cout << responseMat << endl;
	Point maxLoc;
	double maxVal = 0;
	minMaxLoc(responseMat, NULL, &maxVal, NULL, &maxLoc);//最小最大值  
	string judge = "";
	if (maxLoc.x == 0)
	judge = "第0个人的脸";
	if (maxLoc.x == 1)
	judge = "第1个人的脸";
	if (maxLoc.x == 2)
	judge = "第2个人的脸";
	if (maxLoc.x == 3)
	judge = "第3个人的脸";
	if (maxLoc.x == 4)
	judge = "第4个人的脸";
	if (maxLoc.x == 5)
	judge = "第5个人的脸";
	if (maxLoc.x == 6)
	judge = "第6个人的脸";
	if (maxLoc.x == 7)
	judge = "第7个人的脸";
	if (maxLoc.x == 8)
	judge = "第8个人的脸";
	if (maxLoc.x == 9)
	judge = "第9个人的脸";
	if (maxLoc.x == 10)
	judge = "第10个人的脸";
	if (maxLoc.x == 11)
	judge = "第11个人的脸";
	if (maxLoc.x == 12)
	judge = "第12个人的脸";
	if (maxLoc.x == 13)
	judge = "第13个人的脸";
	if (maxLoc.x == 14)
	judge = "第14个人的脸";
	cout << "识别结果：" << judge << endl;
	namedWindow("test", CV_WINDOW_AUTOSIZE);
	imshow("test", test_image);
	waitKey(0);
}
	

void testcurrent() {
	double current = 0.0;
	double currentnum = 0.0;
	for (int i = 1; i <= 165; i++)
	{
		char filename[165];
		sprintf_s(filename, "test/s%d.bmp", i);
		Mat test_image = imread(filename, CV_LOAD_IMAGE_GRAYSCALE);
		//cout << test_image;
		Mat test_temp;
		resize(test_image, test_temp, Size(image_cols, image_rows), (0, 0), (0, 0), CV_INTER_AREA);
		Mat_<float>sampleMat(1, image_rows*image_cols);
		sampleMat = test_temp.reshape(0, 1);
		Mat responseMat;
		bp->predict(sampleMat, responseMat);
		Point maxLoc;
		double maxVal = 0;
		minMaxLoc(responseMat, NULL, &maxVal, NULL, &maxLoc);
		if (maxLoc.x == (i - 1) / 11)
		currentnum++;
		//cout << currentnum << endl;
	}
	current = currentnum / 165;
	cout << "正确率为" << current << endl;
}



int main()
{
	string testfilename;
	labelset();
	//bpnettrain();
	//cout << "请输入测试图片" << endl;
    //cin >> testfilename;
	//testphoto(testfilename);
	testcurrent();
	return 0;
}

