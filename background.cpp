#include<highgui.h>
#include<vector>
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/ml/ml.hpp>
#include <opencv2/opencv.hpp>
#include<iostream>
using namespace std;
using namespace cv;

int cnt;

Mat function(Mat &frame)
{
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    vector<Vec3f> vecCircles;               
    vector<Vec3f>::iterator itrCircles;
    cvNamedWindow("jj");
    Mat im = frame.clone();
    Mat imgYCrCb(frame.rows,frame.cols,CV_8UC3);
    cvtColor(frame, imgYCrCb, CV_BGR2YCrCb);
    imshow("jj",imgYCrCb);
	Mat blur_out(frame.rows,frame.cols,CV_8UC3);
    GaussianBlur(imgYCrCb, blur_out, Size(1,1),2.0,2.0);
    Mat range_out(frame.rows,frame.cols,CV_8UC1);
    inRange(blur_out, Scalar(50, 135, 110), Scalar(250, 155, 135), range_out);
    medianBlur(range_out,range_out,5);
    findContours(range_out, contours, hierarchy, CV_RETR_EXTERNAL,  CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
    vector<vector<Point> > contours_poly( contours.size() );
	vector<Rect> boundRect( contours.size() );
	vector<Point2f>center( contours.size() );
	vector<float>radius( contours.size() );

	for( int i = 0; i < contours.size(); i++ )
	{ 
		approxPolyDP( Mat(contours[i]), contours_poly[i], 3, true );
		boundRect[i] = boundingRect( Mat(contours_poly[i]) );
	}

	//Mat drawing = Mat::zeros( range_out.size(), CV_8UC3 );
	for( int i = 0; i< contours.size(); i++ )
	{
		Scalar color = Scalar(255,255,255);
		if(contourArea(Mat(contours_poly[i]))>1000)
		drawContours( im, contours_poly, i, color, CV_FILLED, 8, vector<Vec4i>(), 0, Point() );
		int area=boundRect[i].width * boundRect[i].height;
		if(area >5000)
		{
			rectangle( im, boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0 );
		//rectangle( frame, boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0 );
			int num=cnt;
			char name[] = "possample/file0000.jpg";
			name[14] = '0'+num/1000;
			num%=1000;
			name[15] = '0'+num/100;
			num%=100;
			name[16] = '0'+num/10;
			num%=10;
			name[17] = '0'+num;
			cnt++;
			imwrite(name,frame(boundRect[i]));
		}
	}
	imshow("Boxed Image",im);
	im.release();
	blur_out.release();
	imgYCrCb.release();
	range_out.release();
    return im;
}


int main(int argc, char ** argv)
{
	cnt=1360;
	//VideoCapture cap(0);
	Mat frame;
	int k=0;
	/*while(k!=27)
	{
		cap>>frame;
		function(frame);
		imshow("Boxed",frame);
		k=cvWaitKey(1);
	}*/
	Mat im = imread(argv[1]);
	Mat bc = imread(argv[2]);
	int constx = 80;
	int consty = 80;
	for(int i =0;i<im.rows;++i)
	{
		uchar * imp = im.ptr<uchar>(i);
		uchar * bcp = bc.ptr<uchar>(i+constx);
		for(int j=0;j<3*im.cols;j=j+3)
		{
			int check = (imp[j]-imp[j+1])*(imp[j+1]-imp[j+2])*(imp[j+2]-imp[j]);
			check*=check;
			if(check<10000000000 && imp[j]>200)
			{
				imp[j] = bcp[j+consty];
				imp[j+1] = bcp[j+consty+1];
				imp[j+2] = bcp[j+consty+2];
			}
		}
	}
	imshow("hello",im);	
	cvWaitKey(0);
				
}
