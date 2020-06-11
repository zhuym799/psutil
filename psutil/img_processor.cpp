#include "psutil.h"
#include <map>

namespace psutil
{
    vector<Rect> CImgProcessor::getMostPossibleContours(vector<Rect> rects, int error)
    {
        vector<Rect> result;
        size_t max = 0;
        //typedef pair<Rect, int> PAIR;
        map<int, vector<Rect>> statistics;
        for (size_t i = 0; i < rects.size(); i++)
        {
            int intX = rects[i].x;
            map<int, vector<Rect>>::iterator iter = statistics.find(intX);
            if (iter == statistics.end())
            {
                //if not found
                statistics[intX] = vector<Rect>();
            }
            statistics[intX].push_back(rects[i]);
        }

        map<int, vector<Rect>>::iterator iter = statistics.begin();
        while (iter != statistics.end())
        {
            if ((iter->second).size() > max)
            {
                result = iter->second;
                max = result.size();
            }
            iter++;
        }
        return result;
    }
	Rect CImgProcessor::buildOutterRect(vector<Rect> rects, Point2i brThreshold)
	{
		Rect result;
		if (rects.size() > 0)
		{
			Rect largest(brThreshold, Point2i(0, 0));
			for (size_t i = 0; i < rects.size(); i++)
			{
				if (rects[i].x < largest.x)
				{
					largest.x = rects[i].x;
				}
				if (rects[i].y < largest.y)
				{
					largest.y = rects[i].y;
				}

				int differ = rects[i].br().x - largest.br().x;
				if (differ > 0)
				{					
					 largest.width += differ;
				}

				differ = rects[i].br().y - largest.br().y;
				if (differ > 0)
				{
					largest.height += differ;
				}				
			}
			result = largest;
		}
		return result;
	}
	Mat CImgProcessor::dftImage(Mat src)
	{
		int M = getOptimalDFTSize(src.rows);
		int N = getOptimalDFTSize(src.cols);
		Mat padded;
		copyMakeBorder(src, padded, 0, M - src.rows, 0, N - src.cols, BORDER_CONSTANT, Scalar::all(0));

		Mat planes[] = { Mat_<float>(padded), Mat::zeros(padded.size(), CV_32F) };
		Mat complexImg;
		merge(planes, 2, complexImg);

		dft(complexImg, complexImg);

		// compute log(1 + sqrt(Re(DFT(img))**2 + Im(DFT(img))**2))
		split(complexImg, planes);
		magnitude(planes[0], planes[1], planes[0]);
		Mat dest = planes[0];
		dest += Scalar::all(1);
		log(dest, dest);

		// crop the spectrum, if it has an odd number of rows or columns
		dest = dest(Rect(0, 0, dest.cols & -2, dest.rows & -2));

		int cx = dest.cols / 2;
		int cy = dest.rows / 2;

		// rearrange the quadrants of Fourier image
		// so that the origin is at the image center
		Mat tmp;
		Mat q0(dest, Rect(0, 0, cx, cy));
		Mat q1(dest, Rect(cx, 0, cx, cy));
		Mat q2(dest, Rect(0, cy, cx, cy));
		Mat q3(dest, Rect(cx, cy, cx, cy));

		q0.copyTo(tmp);
		q3.copyTo(q0);
		tmp.copyTo(q3);

		q1.copyTo(tmp);
		q2.copyTo(q1);
		tmp.copyTo(q2);

		normalize(dest, dest, 0, 1, NORM_MINMAX);
		return dest;
	}

	Mat CImgProcessor::zoom(Mat src, double scale)
	{
		int	interpolation = INTER_LINEAR;
		if (scale < 1)
		{
			interpolation = INTER_AREA;
		}
		Mat dest;
		cv::resize(src, dest, Size(), scale, scale, interpolation);
		return dest;
	}

	/*keep original size,rotate*/
	Mat CImgProcessor::rotate(Mat src, double angle, Size dsize, Scalar padding)
	{
		Point center(src.cols / 2, src.rows / 2);
		Mat rotMat = getRotationMatrix2D(center, angle, 1.0);
		Mat dest;
		warpAffine(src, dest, rotMat, dsize, 1, 0, padding);
		return dest;
	}


	//Rect CImgProcessor::getEdgeRect(Mat src, Size blurSize, int intentedRectW)
	//{
	//	const int OFFSET = 50;
	//	Mat grayImage;
	//	blur(src, grayImage, blurSize);// Size(1, 30));
	//	 // border detection		
	//	threshold(grayImage, grayImage, 230, 255, THRESH_BINARY);

	//	/// find contours
	//	vector<vector<Point>> contours;
	//	vector<Vec4i> hierarchy;
	//	findContours(grayImage, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));
	//	
	//	vector<vector<Point> > contours_poly(contours.size());
	//	vector<Rect> boundRect(contours.size());
	//	Point pt1(grayImage.cols, grayImage.rows);
	//	Point pt2(0, 0);

	//	for (int i = 0; i < contours.size(); i++)
	//	{
	//		approxPolyDP(Mat(contours[i]), contours_poly[i], 1, true);
	//		boundRect[i] = boundingRect(Mat(contours_poly[i]));
	//		if (i > 0 && boundRect[i].width <= intentedRectW + OFFSET && boundRect[i].width >= intentedRectW - OFFSET)
	//		{
	//			pt1.x = MIN(boundRect[i].tl().x, pt1.x);
	//			pt1.y = MIN(boundRect[i].tl().y, pt1.y);
	//			pt2.x = MAX(boundRect[i].br().x, pt2.x);
	//			pt2.y = MAX(boundRect[i].br().y, pt2.y);
	//		}
	//	}
	//	return Rect(pt1, pt2);
	//}



	////vector<Rect> CImgProcessor::getEdgeRect(Mat srcGray, int grayThresh, Size blurSize)
	////{
	////	Rect result = Rect(0, 0, srcGray.cols, srcGray.rows);
	////	
	////	/// find contours
	////	vector<vector<Point>> contours;
	////	vector<Vec4i> hierarchy;
	////	int maxW = MIN(srcGray.rows, srcGray.cols);
	////	size_t qty = 0;
	////	Mat grayImage;

	////
	////	vector<Rect> boundRect;
	////	while (blurSize.width > 0 && blurSize.width < maxW)
	////	{	
	////		boundRect.clear();
	////		blur(srcGray, grayImage, blurSize);
	////		imwrite("D:\\testResult\\blur" + to_string(blurSize.width) + ".jpg", grayImage);
	////		threshold(grayImage, grayImage, 250, 255, THRESH_BINARY);	
	////		imwrite("D:\\testResult\\before_drawContours"+ to_string(blurSize.width) +".jpg", grayImage);
	////		
	////		findContours(grayImage, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));
	////		
	////		vector<vector<Point> > contours_poly(contours.size());
	////			
	////		for (int i = 1; i < contours.size(); i++) // contours[0] is external
	////		{
	////			if (hierarchy[i][3] == 0)
	////			{
	////				approxPolyDP(Mat(contours[i]), contours_poly[i], 1, true);
	////				Rect binding = boundingRect(Mat(contours_poly[i]));
	////				if (binding.width > srcGray.cols * 2 / 3)
	////				{
	////					boundRect.push_back(binding);				
	////				}
	////			}
	////		}





	////		int qty = boundRect.size();
	////		if (qty > 20)
	////		{
	////			blurSize.width += 10;
	////			blurSize.height += 10;// = Size(blurSize.width + 10, blurSize.height + 10);
	////		}
	////		else if (qty < 2)
	////		{
	////			blurSize.width -= 6;
	////			blurSize.height -= 6;
	////			//blurSize = Size(blurSize.width - 6, blurSize.height - 6);
	////		}
	////		else
	////		{
	////			Mat drawing(srcGray.size(), CV_8UC3, Scalar::all(255));
	////			for (int i = 1; i < qty; i++) // contours[0] is external
	////			{
	////				Scalar color = Scalar(rand() % 255, rand() % 255, rand() % 255);
	////				rectangle(drawing, boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0);
	////			}
	////			imwrite("D:\\testResult\\drawContours" + to_string(blurSize.width) + ".jpg", drawing);
	////			break;
	////		}
	////	}	
	////	
	////	return result;
	////}





	////////findContours(grayImage, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));
	/////////// 
	////////vector<vector<Point> > contours_poly(contours.size());
	////////vector<Rect> boundRect(contours.size());
	////////Rect largest(0, 0, 0, 0);
	////////Mat drawing = Mat::zeros(src.size(), CV_8UC3);
	////////for (int i = 1; i < contours.size(); i++) // contours[0] is external
	////////{
	////////	approxPolyDP(Mat(contours[i]), contours_poly[i], 1, true);
	////////	boundRect[i] = boundingRect(Mat(contours_poly[i]));
	////////	Scalar color = Scalar(rand() % 255, rand() % 255, rand() % 255);
	////////	rectangle(drawing, boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0);
	////////	if (hierarchy[i][3] == 0 && boundRect[i].width >= largest.width && boundRect[i].height >= largest.height)
	////////	{
	////////		largest = boundRect[i];
	////////	}
	////////}
	////////imwrite("D:\\testResult\\drawContours.jpg", drawing);
	////////return largest;
//}


/*pad the image to desired size*/
	Mat CImgProcessor::pad(Mat src, Size destSize, Point topleft, Scalar padding)
	{
		destSize.width = max(src.cols, destSize.width);
		destSize.height = max(src.rows, destSize.height);
		Mat padded;
		// center
		if (topleft == Point(-1, -1))
		{
			topleft = Point((destSize.height - src.rows) / 2, (destSize.width - src.cols) / 2);
		}
		int bottom = destSize.height - src.rows - topleft.y;
		int right = destSize.width - src.cols - topleft.x;
		copyMakeBorder(src, padded, topleft.y, bottom, topleft.x, right, BORDER_CONSTANT, padding);
		return padded;
	}

	/*contrast and brightness*/
	Mat CImgProcessor::contrastAndBrightness(Mat src, float alpha, float beta)
	{
		Mat new_image = Mat::zeros(src.size(), src.type());
		for (int y = 0; y < src.rows; y++)
		{
			for (int x = 0; x < src.cols; x++)
			{
				for (int c = 0; c < 3; c++)
				{
					new_image.at<Vec3b>(y, x)[c] = saturate_cast<uchar>(alpha*(src.at<Vec3b>(y, x)[c]) + beta);
				}
			}
		}
		return new_image;
	}

	/*get background color */
	Vec3b CImgProcessor::getBgColor(Mat src)
	{
        Exception exp;
		if (src.channels() < 3)
		{
            throw cv::Exception(0,"only support BGR 3 channnels imgage。","getBgColor","CImgProcessor",0);
		}
		Mat gray(src.size(), CV_8UC1);
		cvtColor(src, gray, COLOR_BGR2GRAY);
		threshold(gray, gray, 0, 255, THRESH_BINARY | THRESH_OTSU);

		int cnt = 0;
		vector<long> sum(3);
		for (int y = 0; y < src.rows; y++)
		{
			uchar* ptrGray = gray.ptr<uchar>(y);
			Vec3b* ptrColor = src.ptr<Vec3b>(y);
			for (int x = 0; x < src.cols; x++)
			{
				if (ptrGray[x] == 255)
				{
					cnt++;
                    for (int c = 0; c < 3; c++)
					{
						sum[c] += ptrColor[x][c];
					}
				}
            }
		}
		if (cnt > 0)
		{
			Vec3b avg;
			for (int c = 0; c < 3; c++)
			{
                avg[c] =static_cast<uchar>(sum[c] / cnt);
			}
			return avg;
		}
        throw cv::Exception(0,"sorry, detection on background color failed。","getBgColor","CImgProcessor",0);;
	}

	/*convert to gray image*/
	Mat CImgProcessor::convert2Gray(Mat src)
	{
		Mat gray(src.size(), CV_8UC1);
		if (src.channels() == 3)
		{
			cvtColor(src, gray, COLOR_BGR2GRAY);
		}
		else
		{
			gray = src;
		}
		return gray;
	}
}
