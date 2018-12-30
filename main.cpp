#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <iostream>
#include "opencv2/calib3d/calib3d.hpp"
#include <cv.hpp>

using namespace cv;
using namespace std;


int main(int argc, char* argv[])
{

    if(argc<3)
    {
	cout << "USAGE: ./main <file_1> <file_2> ... <file_n>" <<endl <<"MAKE SURE YOU HAVE ATLEAST 2 IMAGES"<<endl;
	exit(0);
    }
    //vector<Mat>imgs;
    //vector<vector<KeyPoint> >imgkeypts;
    //vector<Mat>descriptors;
    Mat img1, img2;

    Mat finalimg;

    Ptr<Feature2D> sift=xfeatures2d::SIFT::create(750);
    img1=imread(argv[1]);

    int serial=2;
    while(serial<argc)
    {
        if(serial!=2)
        {
            img1=imread("result.jpg", 1);
            }
        img2=imread(argv[serial]);
        serial++;

        vector<KeyPoint> img1keypts, img2keypts;
        Mat descriptors1, descriptors2;

        sift->detect(img1, img1keypts);
        sift->detect(img2, img2keypts);

        sift->compute(img1, img1keypts, descriptors1);
        sift->compute(img2, img2keypts, descriptors2);


        BFMatcher matcher;
        vector<DMatch> matches;

        matcher.match(descriptors1,descriptors2, matches); //2D array with matched keypoints as rows

        //FINDING GOOD MATCHES descriptor distance is a measure of similarity

        float min_dist=matches[0].distance; //dist is always positive;

        for(int i=1; i<matches.size(); i++)
        {
            if(matches[i].distance<min_dist)
            {
                min_dist=matches[i].distance;
                }
        }

        vector<DMatch> goodmatches;
        for(int i=0; i<matches.size(); i++)
        {
           if(matches[i].distance<=3*min_dist) //consider it a good match
           {
                goodmatches.push_back(matches[i]);

           }
        }


        vector<Point2f> img1pts;
        vector<Point2f> img2pts;

        for(int i=0; i<goodmatches.size(); i++)
        {
            img1pts.push_back(img1keypts[goodmatches[i].queryIdx].pt);
            img2pts.push_back(img2keypts[goodmatches[i].trainIdx].pt);

        }

        Mat homo = findHomography(img2pts, img1pts, CV_RANSAC); //(orig plane, destination plane in final image-- leftmost, algo)


        warpPerspective(img2, finalimg, homo, Size(img1.cols+img2.cols, max(max(img1.rows, img2.rows), img2.cols)));//after this, it contains properly aligned contribution of right image.
        //Pixels which were to be derived from left image are still blank
        if(serial==3)
        {
            Mat leftsec(finalimg ,cv::Rect(0, 0, img1.cols, img1.rows)); //linking pixels of left section of resuult image to the image originally at left
            img1.copyTo(leftsec);
            imwrite("result.jpg", finalimg);

        }
        else
        {

            int startcol;//, endcol;
            ///////////////////////////////////////
            for(int i=0; i<finalimg.cols; i++)
            {
                int count=0;
                for(int j=0; j<finalimg.rows; j++)
                {
                    if((finalimg.at<Vec3b>(j, i)[0]!=0)||(finalimg.at<Vec3b>(j, i)[1]!=0)||(finalimg.at<Vec3b>(j, i)[2]!=0))
                    {
                        count++;
                        if(count>10)
                        {
                            startcol=i; goto label1;
                        }
                    }



                }

            }
            label1:
            //////////////////////////////////////

           /* for(int i=finalimg.cols-1; i>=0; i--)
            {
                int count=0;
                for(int j=0; j<finalimg.rows; j++)
                {
                    if((finalimg.at<Vec3b>(j, i)[0]!=0)||(finalimg.at<Vec3b>(j, i)[1]!=0)||(finalimg.at<Vec3b>(j, i)[2]!=0))
                    {
                        count++;
                        if(count>10)
                        {
                            endcol=i; goto label2;
                        }
                    }



                }

            }
            label2:*/
            /////////////////for loop//////////end
            //imshow("Display", img1);

            Mat leftsec(finalimg ,cv::Rect(0, 0, startcol+1, img1.rows)); //linking pixels of left section of resuult image to the image originally at left
            Mat leftsecimg1(img1, cv::Rect(0, 0, startcol+1, img1.rows));

            leftsecimg1.copyTo(leftsec);

            imwrite("result.jpg", finalimg);

        }

    }

    namedWindow("Final Image", WINDOW_AUTOSIZE);
    imshow("Final Image", finalimg);
    waitKey(0);

    return 0;
}
/*
   namedWindow("Display", WINDOW_AUTOSIZE);
    //vector<Mat>Keypimgs;
    Mat matchimg[argc-2];
    for(int i=0; i<argc-2; i++)
    {
        drawMatches(imgs[i], imgkeypts[i], imgs[i+1], imgkeypts[i+1], matches[i], matchimg[i], Scalar::all(255), Scalar::all(0), vector< char >(), DrawMatchesFlags::DEFAULT);
        imshow("Display", matchimg[i]);
        waitKey(0);
    }
    return 0;
}*/
