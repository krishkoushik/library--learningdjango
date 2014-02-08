/**
 * @file:   main.cpp
 * @author: Jan Hendriks (dahoc3150 [at] yahoo.com)
 * @date:   Created on 2. Dezember 2012
 * @brief:  Example program on how to train your custom HOG detecting vector
 * for use with openCV <code>hog.setSVMDetector(_descriptor)</code>;
 * 
 * For the paper regarding Histograms of Oriented Gradients (HOG), @see http://lear.inrialpes.fr/pubs/2005/DT05/
 * You can populate the positive samples dir with files from the INRIA person detection dataset, @see http://pascal.inrialpes.fr/data/human/
 * This program uses SVMlight as machine learning algorithm (@see http://svmlight.joachims.org/), but is not restricted to it
 * Tested in Ubuntu Linux 64bit 12.04 "Precise Pangolin" with openCV 2.3.1, SVMlight 6.02, g++ 4.6.3
 * and standard HOG settings, training images of size 64x128px.
 * 
 * What this program basically does:
 * 1. Read positive and negative training sample image files from specified directories
 * 2. Calculate their HOG features and keep track of their classes (pos, neg)
 * 3. Save the feature map (vector of vectors/matrix) to file system
 * 4. Read in and pass the features and their classes to a machine learning algorithm, e.g. SVMlight
 * 5. Train the machine learning algorithm using the specified parameters
 * 6. Use the calculated support vectors and SVM model to calculate a single detecting descriptor vector
 * 
 * Build by issuing:
 * g++ `pkg-config --cflags opencv` -c -g -MMD -MP -MF main.o.d -o main.o main.cpp
 * gcc -c -g `pkg-config --cflags opencv` -MMD -MP -MF svmlight/svm_learn.o.d -o svmlight/svm_learn.o svmlight/svm_learn.c
 * gcc -c -g `pkg-config --cflags opencv` -MMD -MP -MF svmlight/svm_hideo.o.d -o svmlight/svm_hideo.o svmlight/svm_hideo.c
 * gcc -c -g `pkg-config --cflags opencv` -MMD -MP -MF svmlight/svm_common.o.d -o svmlight/svm_common.o svmlight/svm_common.c
 * g++ `pkg-config --cflags opencv` -o trainhog main.o svmlight/svm_learn.o svmlight/svm_hideo.o svmlight/svm_common.o `pkg-config --libs opencv`
 * 
 * Warning:
 * Be aware that the program may consume a considerable amount of main memory, hard disk memory and time, dependent on the amount of training samples.
 * Also be aware that (esp. for 32bit systems), there are limitations for the maximum file size which may take effect when writing the features file.
 * 
 * Terms of use:
 * This program is to be used as an example and is provided on an "as-is" basis without any warranties of any kind, either express or implied.
 * Use at your own risk.
 * For used third-party software, refer to their respective terms of use and licensing.
 */

#include <stdio.h>
#include <dirent.h>
#include <ios>
#include <fstream>
#include <stdexcept>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/ml/ml.hpp>

#include "svmlight/svmlight.h"

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


// <editor-fold defaultstate="collapsed" desc="Parameter definitions">
/* Parameter definitions */

// Directory containing positive sample images
static string posSamplesDir = "possample/";
// Directory containing negative sample images
static string negSamplesDir = "negativesamples/";
// Set the file to write the features to
static string featuresFile = "genfiles/features.dat";
// Set the file to write the SVM model to
static string svmModelFile = "genfiles/svmlightmodel.dat";
// Set the file to write the resulting detecting descriptor vector to
static string descriptorVectorFile = "descriptorvector6480.dat";

// HOG parameters for training that for some reason are not included in the HOG class
static const Size trainingPadding = Size(0, 0);
static const Size winStride = Size(8, 8);
// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="Helper functions">
/* Helper functions */

static string toLowerCase(const string& in) {
    string t;
    for (string::const_iterator i = in.begin(); i != in.end(); ++i) {
        t += tolower(*i);
    }
    return t;
}

static void storeCursor(void) {
    printf("\033[s");
}

static void resetCursor(void) {
    printf("\033[u");
}

/**
 * Saves the given descriptor vector to a file
 * @param descriptorVector the descriptor vector to save
 * @param _vectorIndices contains indices for the corresponding vector values (e.g. descriptorVector(0)=3.5f may have index 1)
 * @param fileName
 * @TODO Use _vectorIndices to write correct indices
 */
static void saveDescriptorVectorToFile(vector<float>& descriptorVector, vector<unsigned int>& _vectorIndices, string fileName) {
    printf("Saving descriptor vector to file '%s'\n", fileName.c_str());
    string separator = " "; // Use blank as default separator between single features
    fstream File;
    float percent;
    File.open(fileName.c_str(), ios::out);
    if (File.good() && File.is_open()) {
        printf("Saving descriptor vector features:\t");
        storeCursor();
        for (int feature = 0; feature < descriptorVector.size(); ++feature) {
            if ((feature % 10 == 0) || (feature == (descriptorVector.size()-1)) ) {
                percent = ((1 + feature) * 100 / descriptorVector.size());
                printf("%4u (%3.0f%%)", feature, percent);
                fflush(stdout);
                resetCursor();
            }
            File << descriptorVector.at(feature) << separator;
        }
        printf("\n");
        File << endl;
        File.flush();
        File.close();
    }
}

/**
 * For unixoid systems only: Lists all files in a given directory and returns a vector of path+name in string format
 * @param dirName
 * @param fileNames found file names in specified directory
 * @param validExtensions containing the valid file extensions for collection in lower case
 * @return 
 */
static void getFilesInDirectory(const string& dirName, vector<string>& fileNames, const vector<string>& validExtensions) {
    printf("Opening directory %s\n", dirName.c_str());
    struct dirent* ep;
    size_t extensionLocation;
    DIR* dp = opendir(dirName.c_str());
    if (dp != NULL) {
        while ((ep = readdir(dp))) {
            // Ignore (sub-)directories like . , .. , .svn, etc.
            if (ep->d_type & DT_DIR) {
                continue;
            }
            extensionLocation = string(ep->d_name).find_last_of("."); // Assume the last point marks beginning of extension like file.ext
            // Check if extension is matching the wanted ones
            string tempExt = toLowerCase(string(ep->d_name).substr(extensionLocation + 1));
            if (find(validExtensions.begin(), validExtensions.end(), tempExt) != validExtensions.end()) {
                printf("Found matching data file '%s'\n", ep->d_name);
                fileNames.push_back((string) dirName + ep->d_name);
            } else {
                printf("Found file does not match required file type, skipping: '%s'\n", ep->d_name);
            }
        }
        (void) closedir(dp);
    } else {
        printf("Error opening directory '%s'!\n", dirName.c_str());
    }
    return;
}

/**
 * This is the actual calculation from the (input) image data to the HOG descriptor/feature vector using the hog.compute() function
 * @param imageFilename file path of the image file to read and calculate feature vector from
 * @param descriptorVector the returned calculated feature vector<float> , 
 *      I can't comprehend why openCV implementation returns std::vector<float> instead of cv::MatExpr_<float> (e.g. Mat<float>)
 * @param hog HOGDescriptor containin HOG settings
 */
static void calculateFeaturesFromInput(const string& imageFilename, vector<float>& featureVector, HOGDescriptor& hog) {
    /** for imread flags from openCV documentation, 
     * @see http://docs.opencv.org/modules/highgui/doc/reading_and_writing_images_and_video.html?highlight=imread#Mat imread(const string& filename, int flags)
     * @note If you get a compile-time error complaining about following line (esp. imread),
     * you either do not have a current openCV version (>2.0) 
     * or the linking order is incorrect, try g++ -o openCVHogTrainer main.cpp `pkg-config --cflags --libs opencv`
     */
	const char * ch = imageFilename.c_str();
	IplImage * imageData2 = cvLoadImage(ch);
    gammacorr(imageData2);
	Rect r = Rect(Point(8,10),Point(imageData2->width-8,imageData2->height-22));
	Mat imageData1(imageData2);
	Mat imageData;
    cv::resize(imageData1,imageData,Size(64,80));

    if (imageData.empty()) {
        featureVector.clear();
        printf("Error: HOG image '%s' is empty, features calculation skipped!\n", imageFilename.c_str());
        return;
    }
    // Check for mismatching dimensions
    if (imageData.cols != hog.winSize.width || imageData.rows != hog.winSize.height) {
        featureVector.clear();
        printf("Error: Image '%s' dimensions (%u x %u) do not match HOG window size (%u x %u)!\n", imageFilename.c_str(), imageData.cols, imageData.rows, hog.winSize.width, hog.winSize.height);
        return;
    }
    vector<Point> locations;
    hog.compute(imageData, featureVector, winStride, trainingPadding, locations);
    imageData.release(); // Release the image again after features are extracted
	imageData1.release();
	cvReleaseImage(&imageData2);
}

/**
 * Shows the detections in the image
 * @param found vector containing valid detection rectangles
 * @param imageData the image in which the detections are drawn
 */
static void showDetections(const vector<Rect>& found, Mat& imageData) {
    vector<Rect> found_filtered;
    size_t i, j;
    for (i = 0; i < found.size(); ++i) {
        Rect r = found[i];
        for (j = 0; j < found.size(); ++j)
            if (j != i && (r & found[j]) == r)
                break;
        if (j == found.size())
            found_filtered.push_back(r);
    }
    for (i = 0; i < found_filtered.size(); i++) {
        Rect r = found_filtered[i];
        rectangle(imageData, r.tl(), r.br(), Scalar(64, 255, 64), 3);
    }
}

/**
 * Test detection with custom HOG description vector
 * @param hog
 * @param imageData
 */
static void detectTest(const HOGDescriptor& hog, Mat& imageData) {
    vector<Rect> found;
    int groupThreshold = 2;
    Size padding(Size(32, 32));
    Size winStride(Size(8, 8));
    double hitThreshold = 0.; // tolerance
    hog.detectMultiScale(imageData, found, hitThreshold, winStride, padding, 1.05, groupThreshold);
    showDetections(found, imageData);
}
// </editor-fold>

/**
 * Main program entry point
 * @param argc
 * @param argv
 * @return EXIT_SUCCESS (0) or EXIT_FAILURE (1)
 */
int main(int argc, char** argv) {

    // <editor-fold defaultstate="collapsed" desc="Init">
    HOGDescriptor hog; // Use standard parameters here
    hog.winSize = Size(64, 80);  
	static vector<string> positiveTrainingImages;
    static vector<string> negativeTrainingImages;
    static vector<string> validExtensions;
    validExtensions.push_back("jpg");
    validExtensions.push_back("png");
    validExtensions.push_back("ppm");
    // </editor-fold>

    // <editor-fold defaultstate="collapsed" desc="Read image files">
    getFilesInDirectory(posSamplesDir, positiveTrainingImages, validExtensions);
    getFilesInDirectory(negSamplesDir, negativeTrainingImages, validExtensions);
    /// Retrieve the descriptor vectors from the samples
    unsigned long overallSamples = positiveTrainingImages.size() + negativeTrainingImages.size();
    // </editor-fold>
    
    // <editor-fold defaultstate="collapsed" desc="Calculate HOG features and save to file">
    // Make sure there are actually samples to train
    if (overallSamples == 0) {
        printf("No training sample files found, nothing to do!\n");
        return EXIT_SUCCESS;
    }

    /// @WARNING: This is really important, some libraries (e.g. ROS) seems to set the system locale which takes decimal commata instead of points which causes the file input parsing to fail
    setlocale(LC_ALL, "C"); // Do not use the system locale
    setlocale(LC_NUMERIC,"C");
    setlocale(LC_ALL, "POSIX");

    printf("Reading files, generating HOG features and save them to file '%s':\n", featuresFile.c_str());
    float percent;
    /**
     * Save the calculated descriptor vectors to a file in a format that can be used by SVMlight for training
     * @NOTE: If you split these steps into separate steps: 
     * 1. calculating features into memory (e.g. into a cv::Mat or vector< vector<float> >), 
     * 2. saving features to file / directly inject from memory to machine learning algorithm,
     * the program may consume a considerable amount of main memory
     */ 
    fstream File;
    File.open(featuresFile.c_str(), ios::out);
    if (File.good() && File.is_open()) {
		// Remove following line for libsvm which does not support comments
        // File << "# Use this file to train, e.g. SVMlight by issuing $ svm_learn -i 1 -a weights.txt " << featuresFile.c_str() << endl;
        // Iterate over sample images
        for (unsigned long currentFile = 0; currentFile < overallSamples; ++currentFile) {
            storeCursor();
            vector<float> featureVector;
            // Get positive or negative sample image file path
            const string currentImageFile = (currentFile < positiveTrainingImages.size() ? positiveTrainingImages.at(currentFile) : negativeTrainingImages.at(currentFile - positiveTrainingImages.size()));
            // Output progress
            if ( (currentFile+1) % 10 == 0 || (currentFile+1) == overallSamples ) {
                percent = ((currentFile+1) * 100 / overallSamples);
                printf("%5lu (%3.0f%%):\tFile '%s'", (currentFile+1), percent, currentImageFile.c_str());
                fflush(stdout);
                resetCursor();
            }
            // Calculate feature vector from current image file
            calculateFeaturesFromInput(currentImageFile, featureVector, hog);
            if (!featureVector.empty()) {
                /* Put positive or negative sample class to file, 
                 * true=positive, false=negative, 
                 * and convert positive class to +1 and negative class to -1 for SVMlight
                 */
                File << ((currentFile < positiveTrainingImages.size()) ? "+1" : "-1");
                // Save feature vector components
                for (unsigned int feature = 0; feature < featureVector.size(); ++feature) {
                    File << " " << (feature + 1) << ":" << featureVector.at(feature);
                }
                File << endl;
            }
        }
        printf("\n");
        File.flush();
        File.close();
    } else {
        printf("Error opening file '%s'!\n", featuresFile.c_str());
        return EXIT_FAILURE;
    }
    // </editor-fold>

    // <editor-fold defaultstate="collapsed" desc="Pass features to machine learning algorithm">
    /// Read in and train the calculated feature vectors
    printf("Calling SVMlight\n");
    SVMlight::getInstance()->read_problem(const_cast<char*> (featuresFile.c_str()));
    SVMlight::getInstance()->train(); // Call the core libsvm training procedure
    printf("Training done, saving model file!\n");
    SVMlight::getInstance()->saveModelToFile(svmModelFile);
    // </editor-fold>

    // <editor-fold defaultstate="collapsed" desc="Generate single detecting feature vector from calculated SVM support vectors and SVM model">
    printf("Generating representative single HOG feature vector using svmlight!\n");
    vector<float> descriptorVector;
    vector<unsigned int> descriptorVectorIndices;
    // Generate a single detecting feature vector (v1 | b) from the trained support vectors, for use e.g. with the HOG algorithm
    SVMlight::getInstance()->getSingleDetectingVector(descriptorVector, descriptorVectorIndices);
    // And save the precious to file system
    saveDescriptorVectorToFile(descriptorVector, descriptorVectorIndices, descriptorVectorFile);
    // </editor-fold>

    // <editor-fold defaultstate="collapsed" desc="Test detecting vector">
    printf("Testing custom detection using camera\n");
    hog.setSVMDetector(descriptorVector); // Set our custom detecting vector
    VideoCapture cap(0); // open the default camera
    if(!cap.isOpened()) { // check if we succeeded
        printf("Error opening camera!\n");
        return EXIT_FAILURE;
    }
    Mat testImage;
    while ((cvWaitKey(10) & 255) != 27) {
        cap >> testImage; // get a new frame from camera
//        cvtColor(testImage, testImage, CV_BGR2GRAY); // If you want to work on grayscale images
        detectTest(hog, testImage);
        imshow("HOG custom detection", testImage);
    }
    // </editor-fold>

    return EXIT_SUCCESS;
}
