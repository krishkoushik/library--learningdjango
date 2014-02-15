/*
 * Program to generate negative image samples for HOG training
 * Author : Krishnan R
 * Last Modified : Tue, 24 Dec 2013 19:17:20
 */

#include<highgui.h>
#include<stdio.h>
#include<iostream>

using namespace std;
using namespace cv;

void* gammacorr(void *arg) {
	int yq = 0;
	IplImage * imag = (IplImage *) arg;
	IplImage * imgf = cvCreateImage(cvGetSize(imag), IPL_DEPTH_32F, 3);
	IplImage * img1 = cvCreateImage(cvGetSize(imag), 8, 3);
	int lb[256] = { 0 }, cumb[256] = { 0 }, lg[256] = { 0 }, cumg[256] = { 0 },
			lr[256] = { 0 }, cumr[256] = { 0 }, totr, totg, totb;
	int x, y, i = 0, p5b, p95b, p5g, p95g, p5r, p95r;
	for (y = 0; y < imag->height; y++) {
		uchar *ptr1 = (uchar*) (imag->imageData + y * imag->widthStep);
		for (x = 0; x < imag->width; x++) {
			lb[ptr1[3 * x]]++;
			lg[ptr1[3 * x + 1]]++;
			lr[ptr1[3 * x + 2]]++;
		}
	}
	int j = 0, p5vr, p95vr, p5vg, p95vg, p5vb, p95vb;
	for (i = 0; i < 256; i++) {
		for (j = 0; j < i; j++) {
			cumb[i] += lb[j];
			cumg[i] += lg[j];
			cumr[i] += lr[j];
		}
	}
	totr = cumr[255];
	totg = cumg[255];
	totb = cumb[255];
	int pers = 3, perl = 97;
	p5b = (int) totb * pers / 100;
	p95b = (int) totb * perl / 100;
	p5g = (int) totg * pers / 100;
	p95g = (int) totg * perl / 100;
	p5r = (int) totr * pers / 100;
	p95r = (int) totr * perl / 100;
	i = 0, j = 0;
	while (i < 256) {
		if (cumr[i] >= p5r) {
			p5vr = i;
			break;
		}
		i++;
	}
	i = 0;
	while (i < 256) {
		if (cumg[i] >= p5g) {
			p5vg = i;
			break;
		}
		i++;
	}
	i = 0;
	while (i < 256) {
		if (cumb[i] >= p5b) {
			p5vb = i;
			break;
		}
		i++;
	}
	i = 0;
	while (i < 256) {
		if (cumr[i] >= p95r) {
			p95vr = i;
			break;
		}
		i++;
	}
	i = 0;
	while (i < 256) {
		if (cumg[i] >= p95g) {
			p95vg = i;
			break;
		}
		i++;
	}
	i = 0;
	while (i < 256) {
		if (cumb[i] >= p95b) {
			p95vb = i;
			break;
		}
		i++;
	}
	float ar, br, ag, bg, ab, bb;
	ar = 255.0 / (float) (p95vr - p5vr);
	br = -1.0 * p5vr / (float) (p95vr - p5vr);
	ag = 255.0 / (float) (p95vg - p5vg);
	bg = -1.0 * p5vg / (float) (p95vg - p5vg);
	ab = 255.0 / (float) (p95vb - p5vb);
	bb = -1.0 * p5vb / (float) (p95vb - p5vb);
	cvConvertScale(imag, imgf, 1.0 / 255.0);
	for (y = 0; y < imgf->height; y++) {
		float *ptr = (float*) (imgf->imageData + y * imgf->widthStep);
		for (x = 0; x < imgf->width; x++) {
			{
				ptr[3 * x] = ab * ptr[3 * x] + bb;
				ptr[3 * x + 1] = ag * ptr[3 * x + 1] + bg;
				ptr[3 * x + 2] = ar * ptr[3 * x + 2] + br;
			}
		}
	}
	cvConvertScale(imgf, img1, 255);
	cvCopy(img1, imag);
	cvReleaseImage(&imgf);
	cvReleaseImage(&img1);
}



int main()
{
	VideoCapture cap(0);
	if(!cap.isOpened())
		return -1;
	Mat frame;
	int x=0;
	int y=0;
	namedWindow("window",1);
	int i=5001;
	int j=0;
	for(;;)
	{
		cap>>frame;
		//gammacorr(frame);
		i++;
		Rect rec = Rect(Point(x+10,y+10),Point(x+330,y+410));
		rectangle(frame,Point(x,y),Point(x+340,y+420),3,8);
		char img_name[]="positivesamplesiPosit0000.jpg";
		if(i%1 == 0)
		{
			j=i;
			img_name[20] = '0' + j/1000;
			img_name[21] = '0' + (j/100)%10;
			img_name[22] = '0' + (j/10)%10;
			img_name[23] = '0' + (j)%10;
			imwrite(img_name,frame(rec));
		}
		imshow("window",frame);
		int k = waitKey(27);
		if(k==27 || i>6000)
			break;
	}
	return 0;
}
