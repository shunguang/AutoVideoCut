/*
*-------------------------------------------------------------------
* test_cv_peopleDetect.cpp
* Copyright(c) 2019, The RgbVideoEditor Author (Shunguang Wu).
* All Rights Reserved.
* You may not use this file except in compliance with the License:
*      GNU GENERAL PUBLIC LICENSE (Version 3), 29 June 2007
*
* You may obtain a copy of the License at
*
*     https://www.gnu.org/licenses/gpl-3.0.en.html
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*-------------------------------------------------------------------
*/
#include <opencv2/objdetect.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <iostream>
#include <iomanip>

using namespace cv;
using namespace std;

int test_peopledetect1(int argc, char** argv);
int test_peopledetect2(int argc, char** argv);
class Detector
{
	enum Mode { Default, Daimler } m;
	HOGDescriptor hog, hog_d;
public:
	Detector() : m(Default), hog(), hog_d(Size(48, 96), Size(16, 16), Size(8, 8), Size(8, 8), 9)
	{
		hog.setSVMDetector(HOGDescriptor::getDefaultPeopleDetector());
		hog_d.setSVMDetector(HOGDescriptor::getDaimlerPeopleDetector());
	}
	void toggleMode() { m = (m == Default ? Daimler : Default); }
	string modeName() const { return (m == Default ? "Default" : "Daimler"); }
	vector<Rect> detect(InputArray img)
	{
		// Run the detector with default parameters. to get a higher hit-rate
		// (and more false alarms, respectively), decrease the hitThreshold and
		// groupThreshold (set groupThreshold to 0 to turn off the grouping completely).
		vector<Rect> found;
		if (m == Default) {
			hog.detectMultiScale(img, found, 0, Size(8, 8), Size(32, 32), 1.05, 2, false);
			hog.detectMultiScale(img, found, 0, Size(8, 8), Size(16, 16), 1.03, 2, false);
		}
		else if (m == Daimler) {
			hog_d.detectMultiScale(img, found, 0.5, Size(8, 8), Size(32, 32), 1.05, 2, true);
			hog_d.detectMultiScale(img, found, 0.0, Size(8, 8), Size(16, 16), 1.01, 2, false);
		}
		return found;
	}
	void adjustRect(Rect & r) const
	{
		// The HOG detector returns slightly larger rectangles than the real objects,
		// so we slightly shrink the rectangles to get a nicer output.
		r.x += cvRound(r.width*0.1);
		r.width = cvRound(r.width*0.8);
		r.y += cvRound(r.height*0.07);
		r.height = cvRound(r.height*0.8);
	}
};

static const string keys = "{ help h   |   | print help message }"
"{ camera c | 0 | capture video from camera (device index starting from 0) }"
"{ video v  |   | use video as input }";


int test_cv_peopleDetect(int argc, char** argv)
{
	test_peopledetect1(argc, argv);
	return 0;
}

//from vidoe file
int test_peopledetect1(int argc, char** argv)
{
	string f = "C:/swu/JustinGameVideos/2018-05-12_BA02_DC_United.mp4";
	string fout = "C:/swu/JustinGameVideos/2018-05-12_BA02_DC_United_opencv_boxes.txt";
	FILE *fp = fopen(fout.c_str(), "w");

	VideoCapture cap(f);
	if (!cap.isOpened()) {
		cout << "Can not open video stream: " << f << endl;
		return 2;
	}
	cout << "Press 'q' or <ESC> to quit." << endl;
	cout << "Press <space> to toggle between Default and Daimler detector" << endl;
	uint32_t nTotFrms = cap.get(cv::CAP_PROP_FRAME_COUNT);
	uint32_t w0 = cap.get(cv::CAP_PROP_FRAME_WIDTH);
	uint32_t h0 = cap.get(cv::CAP_PROP_FRAME_HEIGHT);
	uint32_t w1 = w0 / 2, w2 = w0 / 4;
	uint32_t h1 = h0 / 2, h2 = h0 / 4;


	Detector detector;
	Mat frame0, frame1, frame;

	string window_name = "People detector";
	cv::namedWindow(window_name, CV_WINDOW_AUTOSIZE);
	uint32_t fn = 0;
	for (;;)
	{
		cap >> frame0;
		fn++;
		if (frame0.empty())
		{
			cout << "Finished reading: empty frame0" << endl;
			break;
		}

		cv::pyrDown(frame0, frame, Size(w1, h1));
		//cv::pyrDown(frame1, frame,  Size(w2, h2));

		int64 t = getTickCount();
		vector<Rect> found = detector.detect(frame);
		t = getTickCount() - t;

		// show the window
		{
			ostringstream buf;
			buf << fn << "/" << nTotFrms << ", mode: " << detector.modeName() << " ||| "
				<< "FPS: " << fixed << setprecision(1) << (getTickFrequency() / (double)t);
			putText(frame, buf.str(), Point(5, 20), FONT_HERSHEY_PLAIN, 1.0, Scalar(0, 0, 255), 1, LINE_AA);
		}

		fprintf(fp, "fn=%d,nRects=%d\n", fn, found.size());
		for (vector<Rect>::iterator i = found.begin(); i != found.end(); ++i)
		{
			Rect &r = *i;
			detector.adjustRect(r);
			rectangle(frame, r.tl(), r.br(), cv::Scalar(0, 255, 0), 2);
			fprintf(fp, "%d,%d,%d,%d\n", r.x, r.y, r.width, r.height);
		}
		cv::imshow(window_name, frame);

		// interact with user
		const char key = (char)waitKey(30);
		if (key == 27 || key == 'q') // ESC
		{
			cout << "Exit requested" << endl;
			break;
		}
		else if (key == ' ')
		{
			detector.toggleMode();
		}
	}
	fclose(fp);
	return 0;
}

//from camera
int test_peopledetect2(int argc, char** argv)
{
	CommandLineParser parser(argc, argv, keys);
	parser.about("This sample demonstrates the use ot the HoG descriptor.");
	if (parser.has("help"))
	{
		parser.printMessage();
		return 0;
	}
	int camera = parser.get<int>("camera");
	string file = parser.get<string>("video");
	if (!parser.check())
	{
		parser.printErrors();
		return 1;
	}

	VideoCapture cap;
	if (file.empty())
		cap.open(camera);
	else
		cap.open(file.c_str());
	if (!cap.isOpened())
	{
		cout << "Can not open video stream: '" << (file.empty() ? "<camera>" : file) << "'" << endl;
		return 2;
	}

	cout << "Press 'q' or <ESC> to quit." << endl;
	cout << "Press <space> to toggle between Default and Daimler detector" << endl;
	Detector detector;
	Mat frame;
	for (;;)
	{
		cap >> frame;
		if (frame.empty())
		{
			cout << "Finished reading: empty frame" << endl;
			break;
		}
		int64 t = getTickCount();
		vector<Rect> found = detector.detect(frame);
		t = getTickCount() - t;

		// show the window
		{
			ostringstream buf;
			buf << "Mode: " << detector.modeName() << " ||| "
				<< "FPS: " << fixed << setprecision(1) << (getTickFrequency() / (double)t);
			putText(frame, buf.str(), Point(10, 30), FONT_HERSHEY_PLAIN, 2.0, Scalar(0, 0, 255), 2, LINE_AA);
		}
		for (vector<Rect>::iterator i = found.begin(); i != found.end(); ++i)
		{
			Rect &r = *i;
			detector.adjustRect(r);
			rectangle(frame, r.tl(), r.br(), cv::Scalar(0, 255, 0), 2);
		}
		imshow("People detector", frame);

		// interact with user
		const char key = (char)waitKey(30);
		if (key == 27 || key == 'q') // ESC
		{
			cout << "Exit requested" << endl;
			break;
		}
		else if (key == ' ')
		{
			detector.toggleMode();
		}
	}
	return 0;
}
