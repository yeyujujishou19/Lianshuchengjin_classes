
#include "LayoutAnalysis.h"


//=========================================================================================
//===========test==========================================================================
//=========================================================================================

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

//ȥ�������� 
void RemoveBigRegion(Mat &Src, Mat &Dst, int AreaLimit, int CheckMode, int NeihborMode)
{
	int RemoveCount = 0;
	//�½�һ����ǩͼ���ʼ��Ϊ0���ص㣬Ϊ�˼�¼ÿ�����ص����״̬�ı�ǩ��0����δ��飬1�������ڼ��,2�����鲻�ϸ���Ҫ��ת��ɫ����3������ϸ������   
	//��ʼ����ͼ��ȫ��Ϊ0��δ���  
	Mat PointLabel = Mat::zeros(Src.size(), CV_8UC1);
	if (CheckMode == 1)//ȥ��С��ͨ����İ�ɫ��  
	{
		//cout << "ȥ��С��ͨ��.";
		for (int i = 0; i < Src.rows; i++)
		{
			for (int j = 0; j < Src.cols; j++)
			{
				if (Src.at<uchar>(i, j) < 10)
				{
					PointLabel.at<uchar>(i, j) = 3;//��������ɫ����Ϊ�ϸ�����Ϊ3  
				}
			}
		}
	}
	else//ȥ���׶�����ɫ������  
	{
		//cout << "ȥ���׶�";
		for (int i = 0; i < Src.rows; i++)
		{
			for (int j = 0; j < Src.cols; j++)
			{
				if (Src.at<uchar>(i, j) > 10)
				{
					PointLabel.at<uchar>(i, j) = 3;//���ԭͼ�ǰ�ɫ���򣬱��Ϊ�ϸ�����Ϊ3  
				}
			}
		}
	}


	vector<Point2i>NeihborPos;//������ѹ������  
	NeihborPos.push_back(Point2i(-1, 0));
	NeihborPos.push_back(Point2i(1, 0));
	NeihborPos.push_back(Point2i(0, -1));
	NeihborPos.push_back(Point2i(0, 1));
	if (NeihborMode == 1)
	{
		//cout << "Neighbor mode: 8����." << endl;
		NeihborPos.push_back(Point2i(-1, -1));
		NeihborPos.push_back(Point2i(-1, 1));
		NeihborPos.push_back(Point2i(1, -1));
		NeihborPos.push_back(Point2i(1, 1));
	}
	else int a = 0;//cout << "Neighbor mode: 4����." << endl;
	int NeihborCount = 4 + 4 * NeihborMode;
	int CurrX = 0, CurrY = 0;
	//��ʼ���  
	for (int i = 0; i < Src.rows; i++)
	{
		for (int j = 0; j < Src.cols; j++)
		{
			if (PointLabel.at<uchar>(i, j) == 0)//��ǩͼ�����ص�Ϊ0����ʾ��δ���Ĳ��ϸ��  
			{   //��ʼ���  
				vector<Point2i>GrowBuffer;//��¼������ص�ĸ���  
				GrowBuffer.push_back(Point2i(j, i));
				PointLabel.at<uchar>(i, j) = 1;//���Ϊ���ڼ��  
				int CheckResult = 0;

				for (int z = 0; z < GrowBuffer.size(); z++)
				{
					for (int q = 0; q < NeihborCount; q++)
					{
						CurrX = GrowBuffer.at(z).x + NeihborPos.at(q).x;
						CurrY = GrowBuffer.at(z).y + NeihborPos.at(q).y;
						if (CurrX >= 0 && CurrX<Src.cols&&CurrY >= 0 && CurrY<Src.rows)  //��ֹԽ��    
						{
							if (PointLabel.at<uchar>(CurrY, CurrX) == 0)
							{
								GrowBuffer.push_back(Point2i(CurrX, CurrY));  //��������buffer    
								PointLabel.at<uchar>(CurrY, CurrX) = 1;           //���������ļ���ǩ�������ظ����    
							}
						}
					}
				}
				if (GrowBuffer.size()<AreaLimit) //�жϽ�����Ƿ񳬳��޶��Ĵ�С����1Ϊδ������2Ϊ����    
					CheckResult = 2;
				else
				{
					CheckResult = 1;
					RemoveCount++;//��¼�ж�������ȥ��  
				}

				for (int z = 0; z < GrowBuffer.size(); z++)
				{
					CurrX = GrowBuffer.at(z).x;
					CurrY = GrowBuffer.at(z).y;
					PointLabel.at<uchar>(CurrY, CurrX) += CheckResult;//��ǲ��ϸ�����ص㣬����ֵΪ2  
				}
				//********�����õ㴦�ļ��**********    
			}
		}
	}


	CheckMode = 255 * (1 - CheckMode);
	//��ʼ��ת�����С������    
	for (int i = 0; i < Src.rows; ++i)
	{
		for (int j = 0; j < Src.cols; ++j)
		{
			if (PointLabel.at<uchar>(i, j) == 2)
			{
				Dst.at<uchar>(i, j) = CheckMode;
			}
			else if (PointLabel.at<uchar>(i, j) == 3)
			{
				Dst.at<uchar>(i, j) = Src.at<uchar>(i, j);

			}
		}
	}
	//cout << RemoveCount << " objects removed." << endl;
}
//��λ��������
string ExtractText(Mat img0, string imageName)
{

	Mat binImage,binImageCopy;
	threshold(img0, binImage, 0, 255, CV_THRESH_OTSU);
	binImageCopy = binImage.clone();

	//ת���ɲ�ɫͼ
	//Mat rgbImg(img0.size(), CV_8UC3);

	 //��ȡ�Զ����
	Mat element = getStructuringElement(MORPH_RECT, Size(36, 2)); //��һ������MORPH_RECT��ʾ���εľ���ˣ���Ȼ������ѡ����Բ�εġ������͵�
	erode(binImage, binImage, element);//��ʴ�������԰�ɫ���ָ�ʴ���Ժ�ɫ��������

	RemoveBigRegion(binImage, binImage, 100000, 0, 1);    //ȥ���ϴ����ͨ��

	binImage = 255 - binImage; //��ɫ���ڵװ���
	vector< vector<Point> > contours; //��������
	std::vector<cv::Vec4i> hierarchy;
	findContours(binImage, contours, hierarchy, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE); //Ѱ��������ͼƬ�Ǻڵװ���


	////ת���ɲ�ɫͼ
	//Mat dstImage(img0.size(), CV_8UC3);
	//int index = 0;
	//for (; index >= 0; index = hierarchy[index][0]) {
	//	cv::Scalar color(rand() & 255, rand() & 255, rand() & 255);
	//	cv::drawContours(dstImage, contours, index, color, 8, 8, hierarchy);
	//}
	
	//-----------------��ֵ��������ȡ����----------------------------------
	//--------�洢��������-------------------
	vector< vector<Point> >::iterator itr;  //����������
	itr = contours.begin();

	vector <Rect> iRectVec; //��������

	//if (bAuxiliaryCode)
	//{
	//	cvtColor(img0, rgbImg, COLOR_GRAY2BGR);
	//}
	while (itr != contours.end())
	{
		//������������С��Ӿ���  
		Rect rect = boundingRect(*itr);

		if (rect.height > 42 && rect.height < 90 && rect.width>30 && rect.width<3000)//��ɸѡ
		{
			//����ڵ�ռ���α���
			float fCount = 0;
			float fRate = 0;
			Mat imgrect = binImageCopy(rect);
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
			if (fRate > 0.15) //ϸɸѡ
			{
				if (rect.width / rect.height>3&&rect.width > 200 && ( (rect.x+rect.width)<img0.cols/8
						                || ((rect.x + rect.width)>img0.cols * 5 / 6 && (rect.y + rect.height)>img0.rows * 3 / 5)
						                || (rect.y + rect.height)>img0.rows * 3 / 5) ) //��ɸѡ
				{
					if (rect.x > img0.cols * 8.5 / 10 && rect.y > img0.rows * 8.5 / 10)
					{

					}
					else
					{
						//if (bAuxiliaryCode)
						//{
						//	//����������Ӿ���
						//	rectangle(rgbImg, rect, Scalar(0, 255, 0), 8, 1);//�þ��λ����δ�   ��ɫ
						//}

						iRectVec.push_back(rect);
					}
				
				}
				else if(rect.width / rect.height>1.5 && rect.width <= 200 && ( (rect.x + rect.width)<img0.cols / 8
						                    || ((rect.x + rect.width)>img0.cols*5 / 6 && (rect.y + rect.height)>img0.rows * 3 / 5 )
						                    || (rect.y + rect.height)>img0.rows * 3 / 5 ))//��ɸѡ
				{
					//if (bAuxiliaryCode)
					//{
					//	//����������Ӿ���
					//	rectangle(rgbImg, rect, Scalar(0, 255, 0), 8, 1);//�þ��λ����δ�   ��ɫ
					//}

					iRectVec.push_back(rect);
				}

			} //if (fRate > 0.15)

		} //if (rect.height > 30 && rect.height < 100 && rect.width>100 && rect.width<2500

		itr++;
	} //while (itr != contours.end())

	
	//====================ת��string====================================================
	string strResult; //����ַ���
	stringstream ssxmlResult;
	ssxmlResult << "<ImageName>" << imageName << "</ImageName>" << "\n";
	ssxmlResult << "<LR_Column>" << 0 << "</LR_Column>" << "\n";
	ssxmlResult << "<All_Txt>" << 1 << "</All_Txt>" << "\n";
	for (int ia = 0; ia < iRectVec.size(); ia++)
	{
		ssxmlResult << "<rect id=" << ia+1 << ">";
		ssxmlResult << iRectVec[ia].x << "," << iRectVec[ia].y << ","
			<< iRectVec[ia].width << "," << iRectVec[ia].height << "," << 1 << "</rect>" << "\n";
	}
	strResult = ssxmlResult.str();
	cout << strResult << endl;
	//====================ת��string====================================================


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
	//////====��ͼ==========================================================================
	////int a1 = 0;

	return strResult;
}

void main()
{
	string sReadPath = "";          //��ȡ ͼ��·��
	bool bTotalImage = false;                        //����ͼƬ�Ƿ����

	CString strDir = "D:\\sxl\\����ͼƬ\\ˮУ����";

	vector<CString> vFilePathList;
	get_dirs(strDir, vFilePathList);

	while (!bTotalImage)
	{
		for (int ia = 0; ia < vFilePathList.size(); ia++)
		{
			sReadPath = CW2A(vFilePathList[ia].GetString());
			//#############################################
			//�����ļ�����ͼ��
			vector<String> fileNames;
			string strImgPath = sReadPath + "\\*.tif";
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
					strResult = ExtractText(image0, imageName); //�ı���λ������У���õ�ͼ��
				}

			}//for (int i = 0; i < fileNames.size(); i++)
			 //#############################################
		} //for (int ia = 0; ia < vFilePathList.size(); ia++)



		bTotalImage = true; //��ʾȫ��ͼƬ�Ѿ�����
	} //while (!bTotalImage)
	system("pause");
}