
#include "LayoutAnalysis.h"

//===============ȫ�ֱ���==========================================================================
string sReadPath = "";          //��ȡ ͼ��·��

bool bAuxiliaryCode = true;                   //��������
//===============ȫ�ֱ���==========================================================================


//��ȡ���ļ���·��
void get_dirs(CString strPath, vector <CString>& dirs)
{
	vector<CString> vecFiles = {};
	//��ȡ�ļ������������ļ�����
	CString strFilePath;
	//int64    dwDirSize = 0;
	strFilePath += strPath;
	strFilePath += "//*.*";
	CFileFind finder;
	BOOL bFind = finder.FindFile(strFilePath);
	while (bFind)
	{
		bFind = finder.FindNextFile();
		if (!finder.IsDots())
		{
			CString strTempPath = finder.GetFilePath();
			if (finder.IsDirectory())
			{
				dirs.push_back(strTempPath);// �������ļ���

											//TraverseDir(strTempPath, dirs);
			}
			else
			{
				continue;
			}
		}
	}
	finder.Close();
}

//��ֱ����ͶӰ
Mat VerticalProjection(Mat srcImageBin)//��ֱ����ͶӰ  
{
	//	Mat lineImage(1, srcImageBin.cols, CV_8UC1, cv::Scalar(0, 0, 0));
	Mat lineImage(1, srcImageBin.cols, CV_16SC1, cv::Scalar(0, 0, 0));
	int value;
	for (int i = 0; i < srcImageBin.cols; i++)
	{
		int icount = 0;
		for (int j = 0; j < srcImageBin.rows; j++)
		{
			value = srcImageBin.at<uchar>(j, i);
			if (value == 0)
			{
				icount++; //ͳ��ÿ�еİ�ɫ���ص�   
			}
		}
		lineImage.at<ushort>(0, i) = icount;
	}
	return lineImage;
}

//ˮƽ����ͶӰ
Mat HorProjection(Mat srcImageBin)
{
	//Mat lineImage(srcImageBin.rows,1, CV_8UC1, cv::Scalar(0, 0, 0));
	Mat lineImage(srcImageBin.rows, 1, CV_16SC1, cv::Scalar(0, 0, 0));
	int value;
	for (int i = 0; i < srcImageBin.rows; i++)
	{
		int icount = 0;
		for (int j = 0; j < srcImageBin.cols; j++)
		{
			value = srcImageBin.at<uchar>(i, j);
			if (value == 0)
			{
				icount++; //ͳ��ÿ�еİ�ɫ���ص�   
			}
		}
		lineImage.at<ushort>(i, 0) = icount;
	}
	return lineImage;
}

//ˮƽ����ͶӰ ���غ���
void HorProjection(Mat srcImageBin, Mat &lineImage)
{
	int value;
	for (int i = 0; i < srcImageBin.rows; i++)
	{
		if (i == 385)
		{
			int a = 0;
		}
		int icount = 0;
		for (int j = 0; j < srcImageBin.cols; j++)
		{
			value = srcImageBin.at<uchar>(i, j);
			if (value == 0)
			{
				icount++; //ͳ��ÿ�еİ�ɫ���ص�   
			}
		}
		lineImage.at<ushort>(i, 0) = icount;

	}

}

//��ָ���ַ��з��ַ���
string split2(string str, char del) //���ԡ���λ����::192:168:ABC::416��->��192 168 ABC 416��
{
	stringstream ss(str);
	string tok;
	vector<string> ret;
	while (getline(ss, tok, del))
	{
		if (tok > "")
			ret.push_back(tok);
	}
	return ret[ret.size() - 1];
}

//�ָ��ַ�
vector<string> split(const string &s, const string &seperator) {
	vector<string> result;
	typedef string::size_type string_size;
	string_size i = 0;

	while (i != s.size()) {
		//�ҵ��ַ������׸������ڷָ�������ĸ��
		int flag = 0;
		while (i != s.size() && flag == 0) {
			flag = 1;
			for (string_size x = 0; x < seperator.size(); ++x)
				if (s[i] == seperator[x]) {
					++i;
					flag = 0;
					break;
				}
		}

		//�ҵ���һ���ָ������������ָ���֮����ַ���ȡ����
		flag = 0;
		string_size j = i;
		while (j != s.size() && flag == 0) {
			for (string_size x = 0; x < seperator.size(); ++x)
				if (s[j] == seperator[x]) {
					flag = 1;
					break;
				}
			if (flag == 0)
				++j;
		}
		if (i != j) {
			result.push_back(s.substr(i, j - i));
			i = j;
		}
	}
	return result;
}


//=========================================================================================
//===========����-��������λ=============================================================
//=========================================================================================


//�������־��νṹ��
struct stAncientBooksRect
{
	int x;
	int y;
	int width;
	int height;

	int iColID = 0;  //�����

	int iRowID = 0;  //�����

};


//��x����
bool MySort_x2(Rect rect1, Rect rect2)
{
	return rect1.x < rect2.x;
}

//��x����
bool MySort_x3(stAncientBooksRect rect1, stAncientBooksRect rect2)
{
	return rect1.iColID < rect2.iColID;
}

//����ڵ����
float ComputerblackRate(Mat imgrect)
{
	//����ڵ�ռ���α���
	float fCount = 0;
	float fRate = 0;

	for (int icol = 0; icol < imgrect.cols; icol++)
	{
		for (int irow = 0; irow < imgrect.rows; irow++)
		{
			int value = imgrect.at<uchar>(irow, icol);
			if (value == 0)
			{
				fCount++;
			}
		}
	}
	fRate = fCount / (imgrect.cols*imgrect.rows);

	return fRate;
}

//��ȷ�߽�
void PreciseBoundary(vector<stAncientBooksRect> &iVecRectWorld, Mat BinImage)
{
	//ת���ɲ�ɫͼ
	Mat rgbImg5;
	cvtColor(BinImage, rgbImg5, COLOR_GRAY2RGB);

	Rect MyRect;
	for (int ia = 0; ia < iVecRectWorld.size(); ia++)
	{
		MyRect.x = iVecRectWorld[ia].x;
		MyRect.y = iVecRectWorld[ia].y;
		MyRect.width = iVecRectWorld[ia].width;
		MyRect.height = iVecRectWorld[ia].height;

		Mat imgrect = BinImage(Rect(MyRect));

		//��ֱͶӰ��ˮƽͶӰ
		Mat VerImg = VerticalProjection(imgrect);
		Mat HorImg = HorProjection(imgrect);


		bool bLeft = false;
		bool bRight = false;
		bool bUp = false;
		bool bDown = false;
		int iLeft = 0;
		int iRight = 0;
		int iUp = 0;
		int iDown = 0;

		//���ұ߽�
		for (int icol = 0; icol < VerImg.cols; icol++)
		{
			if (VerImg.at<ushort>(0, icol) != 0 && bLeft == false)
			{
				iLeft = icol;
				bLeft = true;
			}
			if (VerImg.at<ushort>(0, VerImg.cols - 1 - icol) != 0 && bRight == false)
			{
				iRight = VerImg.cols - 1 - icol;
				bRight = true;
			}
			if (bLeft == true && bRight == true)
			{
				break;
			}
		}

		//���±߽�
		for (int irow = 0; irow < HorImg.rows; irow++)
		{
			if (HorImg.at<ushort>(irow, 0) != 0 && bUp == false)
			{
				iUp = irow;
				bUp = true;
			}
			if (HorImg.at<ushort>(HorImg.rows - 1 - irow, 0) != 0 && bDown == false)
			{
				iDown = HorImg.rows - 1 - irow;
				bDown = true;
			}
			if (bUp == true && bDown == true)
			{
				break;
			}
		}

		iVecRectWorld[ia].x = iVecRectWorld[ia].x + iLeft;
		iVecRectWorld[ia].y = iVecRectWorld[ia].y + iUp;
		iVecRectWorld[ia].width = iRight - iLeft;
		iVecRectWorld[ia].height = iDown - iUp;

		//����������Ӿ���
		if (bAuxiliaryCode)
		{

			Rect rect;
			rect.x = iVecRectWorld[ia].x;
			rect.y = iVecRectWorld[ia].y;
			rect.width = iVecRectWorld[ia].width;
			rect.height = iVecRectWorld[ia].height;

			rectangle(rgbImg5, rect, Scalar(0, 0, 255), 2, 1);//�þ��λ����δ�
		}

	}
	int a = 0;

}

//��λ��������
string AncientBooksText(Mat img0, string imageName)
{
	////-----------------�ü��ܱ�-------------------------------------------------------
	//Rect Myrect;
	//Myrect.x = 50;
	//Myrect.y = 200;
	//Myrect.width = img0.cols - 100;
	//Myrect.height = img0.rows - 400;
	//Mat imgrect = img0(Rect(Myrect.x, Myrect.y, Myrect.width, Myrect.height));
	////-----------------�ü��ܱ�-------------------------------------------------------

	//ת���ɲ�ɫͼ
	Mat rgbImg;
	cvtColor(img0, rgbImg, COLOR_GRAY2RGB);

	//ת���ɲ�ɫͼ
	Mat rgbImg3;
	cvtColor(img0, rgbImg3, COLOR_GRAY2RGB);

	Mat binImage, binImageCopy;
	threshold(img0, binImage, 0, 255, CV_THRESH_OTSU);
	binImageCopy = binImage.clone();

	//��ȡ�Զ����
	//Mat element = getStructuringElement(MORPH_RECT, Size(2, 2)); //��һ������MORPH_RECT��ʾ���εľ���ˣ���Ȼ������ѡ����Բ�εġ������͵�														   
	//dilate(binImageCopy, binImageCopy, element);//���Ͳ���

	//-----------------��̬ѧ��ȡֱ�߲��þ��μ�ǿ-------------------------------------
	Mat imageF1, imageF2, imageF3, imageF4, imageF5, imageF6, imageF7;

	Mat element1 = getStructuringElement(MORPH_RECT, Size(190, 1)); //��ֱ��
	Mat element3 = getStructuringElement(MORPH_RECT, Size(1, 190)); //��ֱ��

																	//--------����-------------
	morphologyEx(img0, imageF1, MORPH_CLOSE, element1);
	threshold(imageF1, imageF2, 0, 255, CV_THRESH_OTSU | CV_THRESH_BINARY_INV);//����Ӧ��ֵ

	vector< vector<Point> > contours2; //��������
	std::vector<cv::Vec4i> hierarchy2;
	findContours(imageF2, contours2, hierarchy2, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE); //Ѱ��������ͼƬ�Ǻڵװ���

	vector< vector<Point> >::iterator itr2;  //����������
	itr2 = contours2.begin();
	int iThreshold2 = 5;
	while (itr2 != contours2.end())
	{
		//������������С��Ӿ���  
		Rect rect2 = boundingRect(*itr2);

		if (rect2.width > img0.cols * 4 / 5)
		{
			rect2.x = rect2.x - iThreshold2;
			rect2.y = rect2.y - iThreshold2;
			rect2.width = rect2.width + 2 * iThreshold2;
			rect2.height = rect2.height + 2 * iThreshold2;
			rectangle(binImageCopy, rect2, Scalar(255), CV_FILLED, 1);//�þ��λ����δ�   ��ɫ
		}
		else if (rect2.width > img0.cols * 1 / 10)
		{
			rect2.x = rect2.x - iThreshold2;
			rect2.y = rect2.y - iThreshold2;
			rect2.width = rect2.width + 1.5 * iThreshold2;
			rect2.height = rect2.height + 1.5 * iThreshold2;
			rectangle(binImageCopy, rect2, Scalar(255), CV_FILLED, 1);//�þ��λ����δ�   ��ɫ
		}

		itr2++;
	} //while (itr != contours.end())

	  //--------����-------------
	morphologyEx(img0, imageF4, MORPH_CLOSE, element3);
	threshold(imageF4, imageF5, 0, 255, CV_THRESH_OTSU | CV_THRESH_BINARY_INV);

	vector< vector<Point> > contours5; //��������
	std::vector<cv::Vec4i> hierarchy5;
	findContours(imageF5, contours5, hierarchy5, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE); //Ѱ��������ͼƬ�Ǻڵװ���

	vector< vector<Point> >::iterator itr5;  //����������
	itr5 = contours5.begin();
	int iThreshold5 = 16;
	while (itr5 != contours5.end())
	{
		//������������С��Ӿ���  
		Rect rect5 = boundingRect(*itr5);

		if (rect5.height > img0.rows * 4 / 5)
		{
			rect5.x = rect5.x - iThreshold5;
			rect5.y = rect5.y - iThreshold5;
			rect5.width = rect5.width + 2 * iThreshold5;
			rect5.height = rect5.height + 2 * iThreshold5;
			rectangle(binImageCopy, rect5, Scalar(255), CV_FILLED, 1);//�þ��λ����δ�   ��ɫ
		}
		else if (rect5.height > img0.rows * 2 / 5)
		{
			rect5.x = rect5.x - iThreshold5 / 3;
			rect5.y = rect5.y - iThreshold5 / 3;
			rect5.width = rect5.width + 2 * iThreshold5 / 3;
			rect5.height = rect5.height + 2 * iThreshold5 / 3;
			rectangle(binImageCopy, rect5, Scalar(255), CV_FILLED, 1);//�þ��λ����δ�   ��ɫ
		}

		itr5++;
	} //while (itr != contours.end())

	  //bitwise_or(imageF2, imageF5, imageF7);

	  //-----------------��ֱͶӰ-------------------------------------------------------
	Mat VerImgCut(1, img0.cols, CV_8UC1, cv::Scalar(0, 0, 0));
	VerImgCut = VerticalProjection(binImageCopy); //��ֱͶӰ

												  //���������ƴ�ֱͶӰͼ��
	cv::Mat projImg(img0.rows, img0.cols, CV_8U, cv::Scalar(255));
	for (int col = 0; col < img0.cols; ++col)
	{
		cv::line(projImg, cv::Point(col, img0.rows - VerImgCut.at<ushort>(0, col)), cv::Point(col, img0.rows - 1), cv::Scalar::all(0));
	}
	//���������ƴ�ֱͶӰͼ��
	//-----------------��ֱͶӰ-------------------------------------------------------

	//---------------ȷ��ÿ�еķ�Χ-��ʼ����λ��--------------------------------------------
	vector<Rect> ivecRect;                                               //�о�����Ϣ
	vector<int> ivecBegin;                                               //��ʼλ��
	vector<int> ivecEnd;                                                 //����λ��
	Rect myRect;
	int icount = 0;                                                      //ͳ�Ʒ�ֵ��
	int ibegin = 666666;                                                 //��ʼλ��
	int iend = 666666;                                                   //����λ��
	int minVal = 16;                                                     //�жϵĻ�׼ֵ
	int minRange = 30;                                                    //�ַ���ָ����С���
	int value = 0;
	for (int i = 0; i < VerImgCut.cols; i++)
	{
		value = VerImgCut.at<ushort>(0, i);
		if (value > minVal&&ibegin == 666666)
		{
			ibegin = i;                                                  //��ʼλ��  
		}
		else if (value < minVal&&ibegin != 666666)
		{
			if (i - ibegin >= minRange)
			{
				icount++;                                                    //ͳ������1
				iend = i;                                                    //����λ��
				ivecBegin.push_back(ibegin);                                 //��������
				ivecEnd.push_back(iend);                                     //��������
				myRect.x = ibegin - 2;
				myRect.y = 0;
				myRect.width = iend - ibegin + 4;
				myRect.height = img0.rows;
				ivecRect.push_back(myRect);
				ibegin = 666666;                                             //��λ
				iend = 666666;                                               //��λ

				if (bAuxiliaryCode)
				{
					rectangle(rgbImg, myRect, Scalar(0, 0, 255), 3, 1);//�þ��λ����δ�   ��ɫ
				}

			}
			else
			{
				ibegin = 666666;                                             //��λ
				iend = 666666;                                               //��λ
			}
		}
	}  //for (int i = 0; i < lineImg.cols; i++)
	   //---------------ȷ��ÿ�еķ�Χ-��ʼ����λ��--------------------------------------------

	   //------------------��ȷ��λÿ��λ��-------------------------------------------------------
	for (int ia = 0; ia < ivecRect.size(); ia++)
	{
		if (ivecRect[ia].x < 120)
		{
			ivecRect[ia].width = ivecRect[ia].width + ivecRect[ia].x - 92;
			ivecRect[ia].x = 92;

		}
		Mat imgrect = binImageCopy(Rect(ivecRect[ia]));

		Mat HorImg = HorProjection(imgrect); //ˮƽͶӰ

											 //ȷ��������ʼ��
		int iStartRow = 0;                  //��ʼ��
		int iEndRow = HorImg.rows - 1;      //������
		int iJudgeVal = 2;                  //�ж�ֵ
		bool bStart = false;
		bool bEnd = false;
		for (int r = 0; r < HorImg.rows; r++)
		{
			int inewStartVal = HorImg.at<ushort>(r, 0); //����
			if (inewStartVal > iJudgeVal && bStart == false)
			{
				iStartRow = r;
				bStart = true;
			}
			int inewEndVal = HorImg.at<ushort>(HorImg.rows - 1 - r, 0); //����
			if (inewEndVal > iJudgeVal && bEnd == false)
			{
				iEndRow = HorImg.rows - r;
				bEnd = true;
			}
			if (bStart == true && bEnd == true)
			{
				break;
			}
		}
		//��ֱͶӰ��ȷ��������ʼ��
		ivecRect[ia].y = ivecRect[ia].y + iStartRow - 3;
		ivecRect[ia].height = iEndRow - iStartRow + 6;
		if (bAuxiliaryCode)
		{
			//����������Ӿ���
			//rectangle(rgbImg3, ivecRect[ia], Scalar(0, 255, 0), 3, 1);//�þ��λ����δ�   ��ɫ
		}
		//------------ȷ��x��width-----------------

		int b = 0;
	}
	//------------------��ȷ��λÿ��λ��-------------------------------------------------------

	//------------------���и�-----------------------------------------------------------------
	Mat LeftImg(img0.size(), CV_8UC1, cv::Scalar(255));
	Mat RightImg(img0.size(), CV_8UC1, cv::Scalar(255));

	for (int ia = 0; ia < ivecRect.size(); ia++)
	{
		Mat imgrect = img0(Rect(ivecRect[ia]));

		//���������
		if (ivecRect[ia].x + ivecRect[ia].width < 680)
		{
			Mat imageROI = LeftImg(Rect(ivecRect[ia]));

			imgrect.copyTo(imageROI, imgrect);
		}
		else //�������ұ�
		{
			Mat imageROI = RightImg(Rect(ivecRect[ia]));

			imgrect.copyTo(imageROI, imgrect);
		}
		int b = 0;
	}

	//------------------���и�-----------------------------------------------------------------

	// �ֲ���ֵ�� 
	Mat binImageLeft, binImageRight;
	int blockSize = 151;
	int constValue = 35;
	cv::adaptiveThreshold(LeftImg, binImageLeft, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, blockSize, constValue);
	cv::adaptiveThreshold(RightImg, binImageRight, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, blockSize, constValue);

	Mat HorImgLeft = HorProjection(binImageLeft); //ˮƽͶӰ

	Mat HorImgRight = HorProjection(binImageRight); //ˮƽͶӰ

	if (bAuxiliaryCode)
	{
		//--------------���������ƴ�ֱͶӰͼ��----------------------------------
		cv::Mat projImgLeft(binImageLeft.rows, binImageLeft.cols, CV_8U, cv::Scalar(255));

		for (int irow = 0; irow < binImageLeft.rows; ++irow)
		{
			cv::line(projImgLeft, cv::Point(binImageLeft.cols - HorImgLeft.at<ushort>(irow, 0), irow), cv::Point(binImageLeft.cols - 1, irow), cv::Scalar::all(0));
		}


		cv::Mat projImgRight(binImageRight.rows, binImageRight.cols, CV_8U, cv::Scalar(255));

		for (int irow = 0; irow < binImageRight.rows; ++irow)
		{
			cv::line(projImgRight, cv::Point(binImageRight.cols - HorImgRight.at<ushort>(irow, 0), irow), cv::Point(binImageRight.cols - 1, irow), cv::Scalar::all(0));
		}
		//--------------���������ƴ�ֱͶӰͼ��----------------------------------
	}

	//---------------ȷ��ÿ�еķ�Χ-��ʼ����λ��--------------------------------------------
	//----------------���-----------------------
	vector<int> ivecBeginLeft;                                                //��ʼλ��
	vector<int> ivecEndLeft;                                                 //����λ��
	int icountLeft = 0;                                                      //ͳ�Ʒ�ֵ��
	int ibeginLeft = 666666;                                                 //��ʼλ��
	int iendLeft = 666666;                                                   //����λ��
	int minValLeft = 70;                                                     //�жϵĻ�׼ֵ
	int minRangeLeft = 5;                                                    //�ַ���ָ����С���
	int valueLeft = 0;
	for (int i = 0; i < HorImgLeft.rows; i++)
	{
		if (i > 895 && i < 960)
		{
			minValLeft = 80;
		}
		valueLeft = HorImgLeft.at<ushort>(i, 0);
		if (valueLeft > minValLeft&&ibeginLeft == 666666)
		{
			ibeginLeft = i;                                                  //��ʼλ��  
		}
		else if (valueLeft < minValLeft&&ibeginLeft != 666666)
		{
			if (i - ibeginLeft >= minRangeLeft)
			{
				icountLeft++;                                                    //ͳ������1
				iendLeft = i;                                                    //����λ��
				ivecBeginLeft.push_back(ibeginLeft);                                 //��������
				ivecEndLeft.push_back(iendLeft);                                     //��������
				ibeginLeft = 666666;                                             //��λ
				iendLeft = 666666;                                               //��λ
			}
			else
			{
				ibeginLeft = 666666;                                             //��λ
				iendLeft = 666666;                                               //��λ
			}
		}
	}  //for (int i = 0; i < lineImg.cols; i++)

	vector<int> iVecLeft;
	int iValueLeft = 0;
	for (int iLeft = 0; iLeft < ivecBeginLeft.size(); iLeft++)
	{
		if (iLeft == 0)
		{
			iValueLeft = ivecBeginLeft[iLeft];
			iVecLeft.push_back(iValueLeft);
		}
		else
		{
			iValueLeft = (ivecBeginLeft[iLeft] + ivecEndLeft[iLeft - 1]) / 2;
			iVecLeft.push_back(iValueLeft);
		}
	}
	iValueLeft = ivecEndLeft[ivecEndLeft.size() - 1];
	iVecLeft.push_back(iValueLeft);

	//----------------�ұ�-----------------------
	vector<int> ivecBeginRight;                                               //��ʼλ��
	vector<int> ivecEndRight;                                                 //����λ��
	int icountRight = 0;                                                      //ͳ�Ʒ�ֵ��
	int ibeginRight = 666666;                                                 //��ʼλ��
	int iendRight = 666666;                                                   //����λ��
	int minValRight = 62;                                                     //�жϵĻ�׼ֵ
	int minRangeRight = 10;                                                   //�ַ���ָ����С���
	int valueRight = 0;
	for (int i = 0; i < HorImgRight.rows; i++)
	{
		valueRight = HorImgRight.at<ushort>(i, 0);
		if (valueRight > minValRight&&ibeginRight == 666666)
		{
			ibeginRight = i;                                                  //��ʼλ��  
		}
		else if (valueRight < minValRight&&ibeginRight != 666666)
		{
			if (i - ibeginRight >= minRangeRight)
			{
				icountRight++;                                                    //ͳ������1
				iendRight = i;                                                    //����λ��
				ivecBeginRight.push_back(ibeginRight);                                 //��������
				ivecEndRight.push_back(iendRight);                                     //��������
				ibeginRight = 666666;                                             //��λ
				iendRight = 666666;                                               //��λ
			}
			else
			{
				ibeginRight = 666666;                                             //��λ
				iendRight = 666666;                                               //��λ
			}
		}
	}  //for (int i = 0; i < lineImg.cols; i++)

	vector<int> iVecRight;
	int iValueRight = 0;
	for (int iRight = 0; iRight < ivecBeginRight.size(); iRight++)
	{
		if (iRight == 0)
		{
			iValueRight = ivecBeginRight[iRight];
			iVecRight.push_back(iValueRight);
		}
		else
		{
			iValueRight = (ivecBeginRight[iRight] + ivecEndRight[iRight - 1]) / 2;
			iVecRight.push_back(iValueRight);
		}
	}
	iValueRight = ivecEndRight[ivecEndRight.size() - 1];
	iVecRight.push_back(iValueLeft);
	//---------------ȷ��ÿ�еķ�Χ-��ʼ����λ��--------------------------------------------


	//---------------����-----------------------------------------------------------------
	//��istart��iend����Ϣ��x����
	sort(ivecRect.begin(), ivecRect.end(), MySort_x2);
	sort(iVecLeft.begin(), iVecLeft.end());
	sort(iVecRight.begin(), iVecRight.end());
	//---------------����-----------------------------------------------------------------


	//---------------ȷ��ÿ���ַ�����-------------------------------------------------------
	vector<stAncientBooksRect> iVecRectWorld;
	stAncientBooksRect MyRectWord;
	for (int ib = 0; ib < ivecRect.size(); ib++)
	{
		MyRectWord.x = ivecRect[ib].x;
		MyRectWord.width = ivecRect[ib].width;
		MyRectWord.iColID = ib;  //�����

		if (MyRectWord.x + MyRectWord.width < 680)
		{
			for (int ic = 0; ic < iVecLeft.size() - 1; ic++)
			{
				MyRectWord.y = iVecLeft[ic];
				MyRectWord.height = iVecLeft[ic + 1] - iVecLeft[ic];
				MyRectWord.iRowID = ic;  //�����


				Rect rectLeft;
				rectLeft.x = MyRectWord.x;
				rectLeft.y = MyRectWord.y;
				rectLeft.width = MyRectWord.width + 2;
				rectLeft.height = MyRectWord.height;

				Mat imgrect = binImageCopy(Rect(rectLeft));
				float fRate = ComputerblackRate(imgrect);  //����ڵ����
				if (fRate > 0.01)
				{
					iVecRectWorld.push_back(MyRectWord);

					//����������Ӿ���
					if (bAuxiliaryCode)
					{
						rectangle(rgbImg3, rectLeft, Scalar(0, 0, 255), 2, 1);//�þ��λ����δ�
					}
				}

			}
		}
		else
		{
			for (int id = 0; id < ivecBeginRight.size(); id++)
			{
				MyRectWord.y = iVecRight[id];
				MyRectWord.height = iVecRight[id + 1] - iVecRight[id];
				MyRectWord.iRowID = id;  //�����

				Rect rectRight;
				rectRight.x = MyRectWord.x;
				rectRight.y = MyRectWord.y;
				rectRight.width = MyRectWord.width;
				rectRight.height = MyRectWord.height;

				Mat imgrect = binImageCopy(Rect(rectRight));
				float fRate = ComputerblackRate(imgrect);  //����ڵ����
				if (fRate > 0.05)
				{
					iVecRectWorld.push_back(MyRectWord);

					//����������Ӿ���
					if (bAuxiliaryCode)
					{
						rectangle(rgbImg3, rectRight, Scalar(0, 0, 255), 2, 1);//�þ��λ����δ�
					}
				}


			}
		}
	}
	//---------------ȷ��ÿ���ַ�����-------------------------------------------------------

	//��ȷ�߽�
	PreciseBoundary(iVecRectWorld, binImageCopy);

	//����
	sort(iVecRectWorld.begin(), iVecRectWorld.end(), MySort_x3);


	//====================ת��string====================================================
	string strResult; //����ַ���
	stringstream ssxmlResult;
	ssxmlResult << "<ImageName>" << imageName << "</ImageName>" << "\n";
	ssxmlResult << "<LR_Column>" << 0 << "</LR_Column>" << "\n";
	ssxmlResult << "<All_Txt>" << 1 << "</All_Txt>" << "\n";
	int iColID = iVecRectWorld[0].iColID;
	bool bChange = false;
	int iCount = 0;
	for (int ia = 0; ia < iVecRectWorld.size(); ia++)
	{
		if (iColID == iVecRectWorld[ia].iColID&&iCount == 0)
		{
			ssxmlResult << "<iColID>" << iVecRectWorld[ia].iColID << "</iColID>" << "\n";
			iCount++;

		}
		else if (iColID != iVecRectWorld[ia].iColID)
		{
			iColID = iVecRectWorld[ia].iColID;
			iCount = 0;
			if (iColID == iVecRectWorld[ia].iColID&&iCount == 0)
			{
				ssxmlResult << "<iColID>" << iVecRectWorld[ia].iColID << "</iColID>" << "\n";
				iCount++;
			}

		}
		if (iColID == iVecRectWorld[ia].iColID)
		{
			if (iColID == 17)
			{
				iVecRectWorld[ia].width = iVecRectWorld[ia].width + 3;
			}
			if (iColID == 0 && iVecRectWorld[ia].iRowID == 14)
			{
				iVecRectWorld[ia].y = iVecRectWorld[ia].y - 5;
				iVecRectWorld[ia].height = iVecRectWorld[ia].height + 10;
			}
			if (iColID == 9 || iColID == 10 || iColID == 11 || iColID == 12 || iColID == 13 || iColID == 14)
			{
				if (iVecRectWorld[ia].iRowID == 14)
				{
					iVecRectWorld[ia].height = iVecRectWorld[ia].height + 8;
				}

			}
			ssxmlResult << "<rect id=" << iVecRectWorld[ia].iRowID + 1 << ">";
			ssxmlResult << iVecRectWorld[ia].x << "," << iVecRectWorld[ia].y << ","
				<< iVecRectWorld[ia].width << "," << iVecRectWorld[ia].height << "," << 1 << "</rect>" << "\n";
		}


	}
	strResult = ssxmlResult.str();
	//cout << strResult << endl;
	//====================ת��string====================================================




	//-------------------��ͼ------------------------------------------------
	////ת���ɲ�ɫͼ
	//Mat rgbImg6;
	//cvtColor(img0, rgbImg6, COLOR_GRAY2RGB);

	//Rect MyRect;
	//for (int ia = 0; ia < iVecRectWorld.size(); ia++)
	//{
	//	//����������Ӿ���
	//	if (bAuxiliaryCode)
	//	{

	//		Rect rect;
	//		rect.x = iVecRectWorld[ia].x;
	//		rect.y = iVecRectWorld[ia].y;
	//		rect.width = iVecRectWorld[ia].width;
	//		rect.height = iVecRectWorld[ia].height;
	//		rectangle(rgbImg6, rect, Scalar(255, 0, 0), 2, 1);//�þ��λ����δ�
	//	}

	//}
	//-------------------��ͼ------------------------------------------------

	////====��ͼ==========================================================================
	//if (bAuxiliaryCode)
	//{
	//	cout << strResult << endl;

	//	Mat dst2;
	//	resize(rgbImg, dst2, Size(), 0.5, 0.5);  //�����ٴ�ͼ���ٴ洢�ռ�
	//	string str = imageName; //ȥ��ĩβ�ĸ��ַ���.png��
	//	for (int i = 0; i < 4; i++)
	//	{
	//		imageName.pop_back();
	//	}
	//	stringstream ss;
	//	ss << "C:\\Users\\cnki\\Desktop\\11\\" << imageName << ".jpg";
	//	imwrite(ss.str(), dst2);
	//}
	////====��ͼ==========================================================================
	//int a1 = 0;
	return strResult;
}

void main()
{
	bool bTotalImage = false;                        //����ͼƬ�Ƿ����

	CString strDir = "D:\\sxl\\����ͼƬ\\11�ż������и�";

	vector<CString> vFilePathList;
	get_dirs(strDir, vFilePathList);

	while (!bTotalImage)
	{
		for (int ia = 0; ia < vFilePathList.size(); ia++)
		{
			//cout << "ia:" << ia << ",Dirs:" << vFilePathList.size() << endl;
			sReadPath = CW2A(vFilePathList[ia].GetString());
			//#############################################
			//�����ļ�����ͼ��
			vector<String> fileNames;
			string strImgPath = sReadPath + "\\*.jpg";
			glob(strImgPath, fileNames, false);
			for (int i = 0; i < fileNames.size(); i++)
			{
				cout << i << "\t" << fileNames.size() << endl;
				string imageName = fileNames[i];     //ͼ�����ƣ�����׺.jpg

				Mat image0, image;
				image0 = imread(imageName, 0);//��ȡͼƬ 

				if (image0.data == 0)
				{
					return;
				}
				else
				{
					imageName = split2(imageName, '\\');//����ַ���

					string strResult;
					strResult = AncientBooksText(image0, imageName); //�ı���λ
					cout << strResult << endl;
				}

			}//for (int i = 0; i < fileNames.size(); i++)
			 //#############################################
		} //for (int ia = 0; ia < vFilePathList.size(); ia++)

		  ////==========���txt==============
		  //fout.close();                  //�ر��ļ�
		  ////==========���txt==============


		bTotalImage = true; //��ʾȫ��ͼƬ�Ѿ�����
	} //while (!bTotalImage)
	system("pause");
}

//=========================================================================================
//===========����-��������λ=============================================================
//=========================================================================================



