#include "third_party\Preferences\Preferences.h"
#include "third_party\DMC1380\Dmc1380.h"
#pragma comment(lib,"third_party\\DMC1380\\x64\\Dmc1380.lib")

#include "MvCameraControl.h"
#pragma comment(lib,"third_party\\hkvision\\MvCameraControl.lib")

#include "HalconCpp.h"
using namespace HalconCpp;
//#pragma comment(lib,"third_party\\halcon12\\halconcpp10.lib")
#pragma comment(lib,"third_party\\halcon12\\halconcpp.lib")

#include <iostream>

#include <afx.h>

#include <process.h>

#include <mutex>

//背景光需要反相
#define BEIJINGGUANG

//多个气缸的版本
#define VERSION1

	//项目赶工，随便写写
	class _global {
	public:
		enum {
			STOP,CCW,CW
		};
		enum {
			ON = 0,OFF = 1
		};



	private:
		
		_global() 
			:evt_Run(FALSE, TRUE)
		{
			evt_Run.ResetEvent();
		};
		
		int ConveyorState = STOP;
		HTuple GearModel = NULL, hv_AcqHandle = NULL;
		int g_nPayloadSize = 0;
		
	public:
		static _global * GetIns() {
			static _global  _g;
			return &_g;
		}

#ifdef BEIJINGGUANG
		bool isInv = true;
#endif
		HObject  ho_Image;
		HTuple disp_hand = NULL;
		void* handle = NULL;
		CEvent evt_Run;
		CPreferences prj;
		int Init() {
		
			CString AppPath;
			::GetModuleFileName(GetModuleHandle(NULL), AppPath.GetBuffer(300), 300);
			AppPath.ReleaseBuffer();
			AppPath = AppPath.Left(AppPath.ReverseFind('\\'));
			CString SysPath = AppPath + L"\\" + L"sys";
			prj.SetIniDir(SysPath, false);
			if (0 == prj.SetIniFile(L"PrgParameter.ini"))
			{
				prj.initData();
				prj.SaveParaFile(-1);
			}
			prj.LoadParaFile(-1);

			HTuple hv_exception;
			try
			{
				ReadShapeModel((char*)(LPCSTR)(CStringA)prj.Model_Name, &GearModel);
			}
			catch (HalconCpp::HException &HDevExpDefaultException)
			{
				HDevExpDefaultException.ToHTuple(&hv_exception);
				GearModel = NULL;
				AfxMessageBox(L"读取模板失败，无法进行检测");
			}


			int quantity = d1000_board_init();
			if (0 == quantity) return quantity;
			d1000_set_pls_outmode(0, 0);
			d1000_in_enable(0, 3);
			return quantity;
		}

		int grab(HObject & h_obj)
		{
			if (NULL == handle)
				return 3;
			int nRet = MV_OK;
			MV_FRAME_OUT_INFO_EX stImageInfo = { 0 };
			memset(&stImageInfo, 0, sizeof(MV_FRAME_OUT_INFO_EX));
			static unsigned char * pData = (unsigned char *)malloc(sizeof(unsigned char) * (g_nPayloadSize));
			if (pData == NULL)
			{
				return 1;
			}
			unsigned int nDataSize = g_nPayloadSize;


				nRet = MV_CC_GetOneFrameTimeout(handle, pData, nDataSize, &stImageInfo, 1000);
				if (nRet == MV_OK)
				{
					printf("Get One Frame: Width[%d], Height[%d], nFrameNum[%d]\n", stImageInfo.nWidth, stImageInfo.nHeight, stImageInfo.nFrameNum);
				//	GenImage1Extern(&h_obj, "byte", (Hlong)stImageInfo.nWidth, stImageInfo.nHeight, (Hlong)pData, NULL);//转换关键函数，开发文档里说的很详细
					GenImage1(&h_obj, "byte", (Hlong)stImageInfo.nWidth, (Hlong)stImageInfo.nHeight, (Hlong)pData);//转换关键函数，开发文档里说的很详细
				//	GenImage1(&h_obj, "byte", (Hlong)stImageInfo.nHeight, (Hlong)stImageInfo.nWidth, (Hlong)pData);//转换关键函数，开发文档里说的很详细

					return 0;
						
				}
				else
				{
					return 2;
				}
				
				return 0;

			//free(pData);























			if (hv_AcqHandle == NULL) {
				return -1;
			}
			HTuple hv_exception;
			
			try
			{
				GrabImageStart(hv_AcqHandle, -1);
				SetFramegrabberParam(hv_AcqHandle, "TriggerSoftware", -1);
				GrabImage(&h_obj, hv_AcqHandle);

			}
				// catch (exception) 
			catch (HalconCpp::HException &HDevExpDefaultException)
			{
				return -2;
				HDevExpDefaultException.ToHTuple(&hv_exception);
				
			}
			return 0;
		}


		bool PrintDeviceInfo(MV_CC_DEVICE_INFO* pstMVDevInfo)
		{
			if (NULL == pstMVDevInfo)
			{
				printf("The Pointer of pstMVDevInfo is NULL!\n");
				return false;
			}
			if (pstMVDevInfo->nTLayerType == MV_GIGE_DEVICE)
			{
				int nIp1 = ((pstMVDevInfo->SpecialInfo.stGigEInfo.nCurrentIp & 0xff000000) >> 24);
				int nIp2 = ((pstMVDevInfo->SpecialInfo.stGigEInfo.nCurrentIp & 0x00ff0000) >> 16);
				int nIp3 = ((pstMVDevInfo->SpecialInfo.stGigEInfo.nCurrentIp & 0x0000ff00) >> 8);
				int nIp4 = (pstMVDevInfo->SpecialInfo.stGigEInfo.nCurrentIp & 0x000000ff);

				// ch:打印当前相机ip和用户自定义名字 | en:print current ip and user defined name
				printf("CurrentIp: %d.%d.%d.%d\n", nIp1, nIp2, nIp3, nIp4);
				printf("UserDefinedName: %s\n\n", pstMVDevInfo->SpecialInfo.stGigEInfo.chUserDefinedName);
			}
			else if (pstMVDevInfo->nTLayerType == MV_USB_DEVICE)
			{
				printf("UserDefinedName: %s\n", pstMVDevInfo->SpecialInfo.stUsb3VInfo.chUserDefinedName);
				printf("Serial Number: %s\n", pstMVDevInfo->SpecialInfo.stUsb3VInfo.chSerialNumber);
				printf("Device Number: %d\n\n", pstMVDevInfo->SpecialInfo.stUsb3VInfo.nDeviceNumber);
			}
			else
			{
				printf("Not support.\n");
			}

			return true;
		}




		int HKInitCamera()
		{
			int nRet = MV_OK;
		

		
				// ch:枚举设备 | en:Enum device
				MV_CC_DEVICE_INFO_LIST stDeviceList;
				memset(&stDeviceList, 0, sizeof(MV_CC_DEVICE_INFO_LIST));
				nRet = MV_CC_EnumDevices(MV_GIGE_DEVICE | MV_USB_DEVICE, &stDeviceList);
				if (MV_OK != nRet)
				{
					printf("Enum Devices fail! nRet [0x%x]\n", nRet);
					return 1;
				}

				if (stDeviceList.nDeviceNum > 0)
				{
					for (unsigned int i = 0; i < stDeviceList.nDeviceNum; i++)
					{
						printf("[device %d]:\n", i);
						MV_CC_DEVICE_INFO* pDeviceInfo = stDeviceList.pDeviceInfo[i];
						if (NULL == pDeviceInfo)
						{
							break;
						}
						PrintDeviceInfo(pDeviceInfo);
					}
				}
				else
				{
					printf("Find No Devices!\n");
					return 2;
				}

				printf("Please Intput camera index:");
				unsigned int nIndex = 0;
				//scanf("%d", &nIndex);

				if (nIndex >= stDeviceList.nDeviceNum)
				{
					printf("Intput error!\n");
					return 3;
				}

				// ch:选择设备并创建句柄 | en:Select device and create handle
				nRet = MV_CC_CreateHandle(&handle, stDeviceList.pDeviceInfo[nIndex]);
				if (MV_OK != nRet)
				{
					printf("Create Handle fail! nRet [0x%x]\n", nRet);
					return 4;
				}

				// ch:打开设备 | en:Open device
				nRet = MV_CC_OpenDevice(handle);
				if (MV_OK != nRet)
				{
					printf("Open Device fail! nRet [0x%x]\n", nRet);
					return 5;
				}

				// ch:设置触发模式为off | en:Set trigger mode as off
				nRet = MV_CC_SetEnumValue(handle, "TriggerMode", 0);
				if (MV_OK != nRet)
				{
					printf("Set Trigger Mode fail! nRet [0x%x]\n", nRet);
					return 6;
				}

				// ch:获取数据包大小 | en:Get payload size
				MVCC_INTVALUE stParam;
				memset(&stParam, 0, sizeof(MVCC_INTVALUE));
				nRet = MV_CC_GetIntValue(handle, "PayloadSize", &stParam);
				if (MV_OK != nRet)
				{
					printf("Get PayloadSize fail! nRet [0x%x]\n", nRet);
					return 7;
				}
				g_nPayloadSize = stParam.nCurValue;

				// ch:开始取流 | en:Start grab image
				nRet = MV_CC_StartGrabbing(handle);
				if (MV_OK != nRet)
				{
					printf("Start Grabbing fail! nRet [0x%x]\n", nRet);
					return 8;
				}

				/**
				unsigned int nThreadID = 0;
				void* hThreadHandle = (void*)_beginthreadex(NULL, 0, WorkThread, handle, 0, &nThreadID);
				if (NULL == hThreadHandle)
				{
					break;
				}
				*/

				return 0;


			
		
		}

		void close_camera()
		{
			// ch:停止取流 | en:Stop grab image
			int nRet = MV_CC_StopGrabbing(handle);
			if (MV_OK != nRet)
			{
				printf("Stop Grabbing fail! nRet [0x%x]\n", nRet);

			}

			// ch:关闭设备 | Close device
			nRet = MV_CC_CloseDevice(handle);
			if (MV_OK != nRet)
			{
				printf("ClosDevice fail! nRet [0x%x]\n", nRet);

			}

			// ch:销毁句柄 | Destroy handle
			nRet = MV_CC_DestroyHandle(handle);
			if (MV_OK != nRet)
			{

			}
		}


		bool InitCamera(HTuple camera_name = "CameraA")
		{
			HTuple hv_exception;
			try {
				HalconCpp::OpenFramegrabber("GigEVision", 0, 0, 0, 0, 0, 0, "progressive", -1, "default", -1,
					"false", "default", "CameraA", 0, -1, &hv_AcqHandle);
				SetFramegrabberParam(hv_AcqHandle, "ExposureTime", 5000);
				SetFramegrabberParam(hv_AcqHandle, "Gain", 0);
				SetFramegrabberParam(hv_AcqHandle, "TriggerMode", "On");
				SetFramegrabberParam(hv_AcqHandle, "TriggerSource", "Software");
				GrabImageStart(hv_AcqHandle, -1);
			}
			catch (HalconCpp::HException &HDevExpDefaultException)
			{
				HDevExpDefaultException.ToHTuple(&hv_exception);
			
				AfxMessageBox((CString)(CStringA)hv_exception.ToString());
				return false;
			}
			return true;
		}

		void open_window(CStatic * pWnd, HTuple * Disp_Hd)
		{
			HTuple hv_exception;
			try {
				using namespace HalconCpp;
				CRect objRect;
				pWnd->GetClientRect(objRect);
				CDC *pDC = pWnd->GetDC();
				HDC  hDC = pDC->GetSafeHdc();
				int nWndWidth = objRect.right - objRect.left;
				int nWndHeight = objRect.bottom - objRect.top;

				SetCheck("~father");
				OpenWindow(0, 0, nWndWidth, nWndHeight, (Hlong)pWnd->m_hWnd, "visible", "", Disp_Hd);
				SetCheck("father");
			}
			catch (HalconCpp::HException &HDevExpDefaultException)
			{
				HDevExpDefaultException.ToHTuple(&hv_exception);
				return;
			}
		}

		void disp_obj(HObject ho_Image, HTuple Window)
		{
			HTuple hv_exception;
			try
			{
				HTuple _w = 0, _h = 0, _pointer , _type ;
				GetImagePointer1(ho_Image,  &_pointer,  &_type,  &_w,  &_h);
				SetPart(Window, 0, 0, _h, _w);
				DispObj(ho_Image, Window);
			}
			catch (HalconCpp::HException &HDevExpDefaultException)
			{
				HDevExpDefaultException.ToHTuple(&hv_exception);
				return ;
			}
		}

		bool CreateGearModel(HObject ho_ModelImage,HTuple Window, double Threshold_Val,HTuple FileName = "GearModel.shm") {
			using namespace HalconCpp;
			HObject  ho_ROI, ho_ImageROI, ho_ShapeModelImage, ho_Thresholded, ho_Region_Bin;
			HObject  ho_ShapeModelRegion, ho_FilledModelRegion, ho_ShapeModel;
			HObject MaxAreaRegion;
			// Local control variables
			HTuple  hv_FGHandle, hv_Pointer, hv_Type, hv_Width;
			HTuple  hv_Height, hv_WindowHandle, hv_hv_r, hv_hv_c, hv_hv_phi;
			HTuple  hv_hv_len1, hv_hv_len2, hv_ModelRegionRow1, hv_ModelRegionColumn1;
			HTuple  hv_ModelRegionRow2, hv_ModelRegionColumn2, hv_exception;
			try {
			GetImagePointer1(ho_ModelImage, &hv_Pointer, &hv_Type, &hv_Width, &hv_Height);
			DispObj(ho_ModelImage, Window);
			

				SetColor(Window, "yellow");
		
				SetDraw(Window, "margin");
			
				SetLineWidth(Window, 2);
				hv_WindowHandle = Window;
			//-------------------  start of the application  ----------------
			//step 1: create an ROI around the arrow
#if 0
			DrawRectangle2(hv_WindowHandle, &hv_hv_r, &hv_hv_c, &hv_hv_phi, &hv_hv_len1, &hv_hv_len2);
			GenRectangle2(&ho_ROI, hv_hv_r, hv_hv_c, hv_hv_phi, hv_hv_len1, hv_hv_len2);
#else
				SetTposition(hv_WindowHandle, 10, 10);
				WriteString(hv_WindowHandle, "请画外轮廓");
				DrawCircle(hv_WindowHandle, &hv_hv_r, &hv_hv_c, &hv_hv_phi);
				GenCircle(&ho_ROI, hv_hv_r, hv_hv_c, hv_hv_phi);
				SetTposition(hv_WindowHandle, 60, 10);
				WriteString(hv_WindowHandle, "请画内轮廓");

				DispObj(ho_ROI, Window);
				HObject tmp;
				DrawCircle(hv_WindowHandle, &hv_hv_r, &hv_hv_c, &hv_hv_phi);
				GenCircle(&tmp, hv_hv_r, hv_hv_c, hv_hv_phi);
				Difference(ho_ROI, tmp, &ho_ROI);
#endif


				HObject ReducedImg,ROI_Image,ho_ConnectedRegions, ho_Selected, ho_Filled, ho_ImageRegion;
				ReduceDomain(ho_ModelImage, ho_ROI, &ReducedImg);

				if (isInv)
				{
					Threshold(ReducedImg, &ho_Thresholded, Threshold_Val,255);
				}
				else
				{
					Threshold(ReducedImg, &ho_Thresholded, 0, Threshold_Val);
				}
			Connection(ho_Thresholded, &ho_ConnectedRegions);
	
			SelectShapeStd(ho_ConnectedRegions, &ho_Selected, "max_area", 0.6);
			
			FillUp(ho_Selected,&ho_Filled);
			GetImageSize(ho_ModelImage, &hv_Width, &hv_Height);
	

			
			RegionToBin(ho_Filled, &ho_Region_Bin, 0, 255, hv_Width, hv_Height);



			//RegionToBin(ho_Thresholded, &ho_Region_Bin, 0, 255, hv_Width, hv_Height);
			


			ReduceDomain(ho_Region_Bin, ho_ROI, &ho_ImageROI);



//			InspectShapeModel(ho_ImageROI, &ho_ShapeModelImage, &ho_ShapeModelRegion, 1, 30);
			DispObj(ho_ImageROI, Window);
			InspectShapeModel(ho_ImageROI, &ho_ShapeModelImage, &ho_ShapeModelRegion, 8, 30);
			HTuple AreaModelRegions,RowModelRegions, ColumnModelRegions, HeightPyramid, NumLevels;
			AreaCenter(ho_ShapeModelRegion, &AreaModelRegions, &RowModelRegions, &ColumnModelRegions);
			CountObj(ho_ShapeModelRegion,  &HeightPyramid);

			for (HTuple i = 1; i <= HeightPyramid; i += 1)
			{
				if (0 != (int)(new HTuple(AreaModelRegions.TupleSelect(i - 1).TupleGreater(15))))
				{
					NumLevels = i;
				}
			}

			/**
			SmallestRectangle1(ho_ShapeModelRegion, &hv_ModelRegionRow1, &hv_ModelRegionColumn1,
				&hv_ModelRegionRow2, &hv_ModelRegionColumn2);

				SetDraw(Window, "fill");
			FillUp(ho_ShapeModelRegion, &ho_FilledModelRegion);
			OpeningCircle(ho_FilledModelRegion, &ho_ROI, 3.5);

			//	DispObj(ho_ModelImage, Window);
			*/


				SetColor(Window, "cyan");

				DispObj(ho_ROI, Window);
			//step 4: create the final model
			//ReduceDomain(ho_ModelImage, ho_ROI, &ho_ImageROI);


			//
			if (NULL != GearModel)
			{
				try {
					ClearShapeModel(GearModel);
				}
				catch (HalconCpp::HException &HDevExpDefaultException)
				{
					HDevExpDefaultException.ToHTuple(&hv_exception);
					
				}

			}


		
				
				CreateShapeModel(ho_ImageROI, NumLevels, 0, HTuple(360).TupleRad(), "auto", "none", "use_polarity",
					30, 10, &GearModel);

				
			//	HOperatorSet.CreateShapeModel(ho_ImageRegion, NumLevels, 0, (new HTuple(360)).TupleRad(), "auto", "none", "use_polarity", 30, "auto", out Gear_Model);

				if (GearModel.Length() > 0)
				{
					
					//GetShapeModelContours(&ho_ShapeModel, GearModel, 1);
					SetColor(Window, "blue");
					WriteShapeModel(GearModel, FileName);
					DispObj(ho_ShapeModelRegion, Window);
				}
				else
				{
					AfxMessageBox(L"没有发现轮廓");
					return false;
				}
			}
				catch (HalconCpp::HException &HDevExpDefaultException)
				{
					HDevExpDefaultException.ToHTuple(&hv_exception);
					return false;
				}
		
				return true;
		}



		bool GearCheck_ex(HObject ho_SearchImage, double hv_score_threshold, double bin_ThresHold_Var, double hv_bin_Threshold_Score, HTuple Window, bool isDisp = false) {
			using namespace HalconCpp;

			bool isOK = false;
			// Local iconic variables
			HObject  ho_ModelImage, ho_ROI, ho_ImageROI, ho_ShapeModelImage, bin_Thres,bin_Region;
			HObject  ho_ShapeModelRegion, ho_FilledModelRegion, ho_ShapeModel;
			HObject  ho_ModelAtMaxPosition, ho_ModelAtNewPosition;
			HObject  ho_Thresholded, ho_Region_Bin;
			// Local control variables
			HTuple  hv_FGHandle, hv_Pointer, hv_Type, hv_Width, hv_Hegiht;
			HTuple  hv_Height, hv_ModelRegionRow1;
			HTuple  hv_ModelRegionColumn1, hv_ModelRegionRow2, hv_ModelRegionColumn2;
			HTuple  hv_WindowHandleZoom;
			HTuple  hv_i, hv_MaxScore, hv_Row, hv_Col, hv_RowCheck;
			HTuple  hv_ColumnCheck, hv_AngleCheck, hv_Score, hv_j, hv_MovementOfObject;
			HTuple hv_exception;
			HTuple W, H;

			try {
				GetShapeModelContours(&ho_ShapeModel, GearModel, 1);
				SetDraw(Window, "fill");
				SetColor(Window, "yellow");
				SetLineWidth(Window, 2);
				hv_MaxScore = -1;
				hv_Row = 10;
				hv_Col = 10;
				GenEmptyObj(&ho_ModelAtMaxPosition);
				if (isInv)
				{
					Threshold(ho_SearchImage, &bin_Thres,  bin_ThresHold_Var,255);
					DispObj(bin_Thres, Window);
				}
				else
				{
					Threshold(ho_SearchImage, &bin_Thres, 0, bin_ThresHold_Var);
					DispObj(bin_Thres, Window);
				}

				SetDraw(Window, "margin");
				GetImageSize(ho_SearchImage, &W, &H);
				RegionToBin(bin_Thres, &bin_Region, 0, 255, W, H);
				
				FindShapeModel(bin_Region, GearModel, 0, HTuple(360).TupleRad(), 0.35, 3,

					0, "least_squares", 0, 0.7, &hv_RowCheck, &hv_ColumnCheck, &hv_AngleCheck,
					&hv_Score);
				if (0 != ((hv_Score.TupleLength()) > 0))
				{
					{

						HTuple end_val88 = (hv_Score.TupleLength()) - 1;
						HTuple step_val88 = 1;
						for (hv_j = 0; hv_j.Continue(end_val88, step_val88); hv_j += step_val88)
						{
							VectorAngleToRigid(0, 0, 0, HTuple(hv_RowCheck[hv_j]), HTuple(hv_ColumnCheck[hv_j]),
								HTuple(hv_AngleCheck[hv_j]), &hv_MovementOfObject);
							AffineTransContourXld(ho_ShapeModel, &ho_ModelAtNewPosition, hv_MovementOfObject);

							if (0 != (hv_MaxScore < HTuple(hv_Score[hv_j])))
							{

								hv_MaxScore = ((const HTuple&)hv_Score)[hv_j];
								ho_ModelAtMaxPosition = ho_ModelAtNewPosition;
								hv_Col = ((const HTuple&)hv_ColumnCheck)[hv_j];
								hv_Row = ((const HTuple&)hv_RowCheck)[hv_j];

							}

						}
					}
					if (0 != (0 < hv_MaxScore))
					{
						if (0 != (hv_bin_Threshold_Score < hv_MaxScore))
						{
							SetColor(Window, "green");
							isOK = true;
						}
						else
						{
							SetColor(Window, "red");
						}
						SetTposition(Window, 10, 10);
						if (isDisp) {
							WriteString(Window, "正反面相似度" + (hv_MaxScore.TupleString(".5")));
							DispObj(ho_ModelAtMaxPosition, Window);
						}
						if (!isOK)
							return isOK;
					}
				}
				else
				{
					SetColor(Window, "red");
					SetTposition(Window, hv_Row, hv_Col);
					if (isDisp) WriteString(Window, "未能找到模板");
					return false;
				}


				isOK = false;
				hv_MaxScore = -1;
				hv_Row = 10;
				hv_Col = 10;
				GenEmptyObj(&ho_ModelAtMaxPosition);

				if (isInv)
				{
					InvertImage(ho_SearchImage, &ho_SearchImage);
					//Threshold(, &bin_Thres, 0, bin_ThresHold_Var);
				}




				FindShapeModel(ho_SearchImage, GearModel, 0, HTuple(360).TupleRad(), 0.35, 3,

					0, "least_squares", 0, 0.7, &hv_RowCheck, &hv_ColumnCheck, &hv_AngleCheck,
					&hv_Score);
				if (0 != ((hv_Score.TupleLength()) > 0))
				{
					{

						HTuple end_val88 = (hv_Score.TupleLength()) - 1;
						HTuple step_val88 = 1;
						for (hv_j = 0; hv_j.Continue(end_val88, step_val88); hv_j += step_val88)
						{
							VectorAngleToRigid(0, 0, 0, HTuple(hv_RowCheck[hv_j]), HTuple(hv_ColumnCheck[hv_j]),
								HTuple(hv_AngleCheck[hv_j]), &hv_MovementOfObject);
							AffineTransContourXld(ho_ShapeModel, &ho_ModelAtNewPosition, hv_MovementOfObject);

							if (0 != (hv_MaxScore < HTuple(hv_Score[hv_j])))
							{
								
								hv_MaxScore = ((const HTuple&)hv_Score)[hv_j];
								ho_ModelAtMaxPosition = ho_ModelAtNewPosition;
								hv_Col = ((const HTuple&)hv_ColumnCheck)[hv_j];
								hv_Row = ((const HTuple&)hv_RowCheck)[hv_j];
								
							}

						}
					}
					if (0 != (0 < hv_MaxScore))
					{
						if (0 != (hv_score_threshold < hv_MaxScore))
						{
							SetColor(Window, "green");
							isOK = true;
						}
						else
						{
							SetColor(Window, "red");
						}
						SetTposition(Window, hv_Row, hv_Col);
						if (isDisp) {
							WriteString(Window, "相似度" + (hv_MaxScore.TupleString(".5")));
							DispObj(ho_ModelAtMaxPosition, Window);
						}
					}
				}
				else
				{
					SetColor(Window, "red");
					SetTposition(Window, hv_Row, hv_Col);
					if (isDisp) WriteString(Window, "未能找到模板");
					return false;
				}
			}
			catch (HalconCpp::HException &HDevExpDefaultException)
			{
				HDevExpDefaultException.ToHTuple(&hv_exception);
				return false;
			}
			return isOK;
		}

		bool GearCheck( HObject ho_SearchImage, double hv_score_threshold, HTuple Window, double Threshold_Val = 55) {
			return GearCheck_ex(ho_SearchImage, hv_score_threshold, _global::GetIns()->prj.Threshold, _global::GetIns()->prj.Fb_Score_Threshold, Window, true);
#if 0


			using namespace HalconCpp;
			
			bool isOK = false;
			// Local iconic variables
			HObject  ho_ModelImage, ho_ROI, ho_ImageROI, ho_ShapeModelImage;
			HObject  ho_ShapeModelRegion, ho_FilledModelRegion, ho_ShapeModel;
			HObject  ho_ModelAtMaxPosition, ho_ModelAtNewPosition;
			HObject  ho_Thresholded, ho_Region_Bin;
			// Local control variables
			HTuple  hv_FGHandle, hv_Pointer, hv_Type, hv_Width,hv_Hegiht;
			HTuple  hv_Height, hv_ModelRegionRow1;
			HTuple  hv_ModelRegionColumn1, hv_ModelRegionRow2, hv_ModelRegionColumn2;
			HTuple  hv_WindowHandleZoom;
			HTuple  hv_i, hv_MaxScore, hv_Row, hv_Col, hv_RowCheck;
			HTuple  hv_ColumnCheck, hv_AngleCheck, hv_Score, hv_j, hv_MovementOfObject;
			HTuple hv_exception;

			try {
				//disp_obj(ho_Region_Bin, Window);

				GetShapeModelContours(&ho_ShapeModel, GearModel, 1);
				SetDraw(Window, "margin");
				SetLineWidth(Window, 2);
				hv_MaxScore = -1;
				hv_Row = 20;
				hv_Col = 20;
				GenEmptyObj(&ho_ModelAtMaxPosition);















				
				GetImagePointer1(ho_SearchImage, &hv_Pointer, &hv_Type, &hv_Width, &hv_Height);
				Threshold(ho_SearchImage, &ho_Thresholded, Threshold_Val, 255);
				RegionToBin(ho_Thresholded, &ho_Region_Bin, 0, 255, hv_Width, hv_Height);
				disp_obj(ho_Region_Bin, Window);

				isOK = GearCheck_ex(ho_Region_Bin, hv_score_threshold, _global::GetIns()->prj.Threshold, _global::GetIns()->prj.Fb_Score_Threshold, Window,true);
				//NG就返回，不继续检测，因为这个是检测正反的
				return isOK;
				
				//isOK = GearCheck_ex(ho_SearchImage, hv_score_threshold, Window, true);
				return isOK;
#if 0
				FindShapeModel(ho_Region_Bin, GearModel, 0, HTuple(360).TupleRad(), 0.35, 3,
					0, "least_squares", 0, 0.7, &hv_RowCheck, &hv_ColumnCheck, &hv_AngleCheck,
					&hv_Score);
				if (0 != ((hv_Score.TupleLength()) > 0))
				{
					{
						HTuple end_val88 = (hv_Score.TupleLength()) - 1;
						HTuple step_val88 = 1;
						for (hv_j = 0; hv_j.Continue(end_val88, step_val88); hv_j += step_val88)
						{
							VectorAngleToRigid(0, 0, 0, HTuple(hv_RowCheck[hv_j]), HTuple(hv_ColumnCheck[hv_j]),
								HTuple(hv_AngleCheck[hv_j]), &hv_MovementOfObject);
							AffineTransContourXld(ho_ShapeModel, &ho_ModelAtNewPosition, hv_MovementOfObject);
							if (0 != (hv_MaxScore < HTuple(hv_Score[hv_j])))
							{
								hv_MaxScore = ((const HTuple&)hv_Score)[hv_j];
								ho_ModelAtMaxPosition = ho_ModelAtNewPosition;
								hv_Col = ((const HTuple&)hv_ColumnCheck)[hv_j];
								hv_Row = ((const HTuple&)hv_RowCheck)[hv_j];
							}

						}
					}
					if (0 != (0 < hv_MaxScore))
					{
						if (0 != (hv_score_threshold < hv_MaxScore))
						{
							SetColor(Window, "green");
							//	isOK = true;
						}
						else
						{
							SetColor(Window, "red");
							return false;
						}
			//			SetTposition(Window, hv_Row, hv_Col);
			//			WriteString(Window, "相似度" + (hv_MaxScore.TupleString(".5")));
			//			DispObj(ho_ModelAtMaxPosition, Window);

					}
					else
					{
						SetColor(Window, "red");
						SetTposition(Window, hv_Row, hv_Col);
						WriteString(Window, "未能找到模板");
						return false;
					}
				}



				hv_MaxScore = -1;
				hv_Row = 20;
				hv_Col = 20;
				GenEmptyObj(&ho_ModelAtMaxPosition);
#if 1
				FindShapeModel(ho_Region_Bin, GearModel, 0, HTuple(360).TupleRad(), 0.35, 3,	
#else
				FindShapeModel(ho_SearchImage, GearModel, 0, HTuple(360).TupleRad(), 0.35, 3,
#endif
					0, "least_squares", 0, 0.7, &hv_RowCheck, &hv_ColumnCheck, &hv_AngleCheck,
					&hv_Score);
				if (0 != ((hv_Score.TupleLength()) > 0))
				{
					{
						HTuple end_val88 = (hv_Score.TupleLength()) - 1;
						HTuple step_val88 = 1;
						for (hv_j = 0; hv_j.Continue(end_val88, step_val88); hv_j += step_val88)
						{
							VectorAngleToRigid(0, 0, 0, HTuple(hv_RowCheck[hv_j]), HTuple(hv_ColumnCheck[hv_j]),
								HTuple(hv_AngleCheck[hv_j]), &hv_MovementOfObject);
							AffineTransContourXld(ho_ShapeModel, &ho_ModelAtNewPosition, hv_MovementOfObject);
							if (0 != (hv_MaxScore < HTuple(hv_Score[hv_j])))
							{
								hv_MaxScore = ((const HTuple&)hv_Score)[hv_j];
								ho_ModelAtMaxPosition = ho_ModelAtNewPosition;
								hv_Col = ((const HTuple&)hv_ColumnCheck)[hv_j];
								hv_Row = ((const HTuple&)hv_RowCheck)[hv_j];
							}

						}
					}
					if (0 != (0 < hv_MaxScore))
					{
						if (0 != (hv_score_threshold < hv_MaxScore))
						{
							SetColor(Window, "green");
							isOK = true;
						}
						else
						{
							SetColor(Window, "red");
						}
						SetTposition(Window, hv_Row, hv_Col);
						WriteString(Window, "相似度" + (hv_MaxScore.TupleString(".5")));
						DispObj(ho_ModelAtMaxPosition, Window);
					}
					else
					{
						SetColor(Window, "red");
						SetTposition(Window, hv_Row, hv_Col);
						WriteString(Window, "未能找到模板");
					}


				}
#endif
			}
			catch (HalconCpp::HException &HDevExpDefaultException)
			{
				HDevExpDefaultException.ToHTuple(&hv_exception);
				return false;
			}
				return isOK;
#endif

		}


		long ReadPosition(short axis)
		{
			return d1000_get_command_pos(axis);
		}

		DWORD WriteOutPutBit(short BitNo, short BitData)
		{
			static std::mutex mtx;
			std::lock_guard<std::mutex> lck(mtx);
			return (!d1000_out_bit(BitNo, BitData));
		}

		DWORD ReadOutPutBit(short BitNo)
		{
			return d1000_get_outbit(BitNo);
		}

		DWORD ReadInPutBit(short BitNo)
		{
			return (!d1000_in_bit(BitNo));
		}

		bool WaitSensor(short Bits, DWORD TimeOut)
		{
			DWORD tk = GetTickCount();
			for (; d1000_in_bit(Bits);)
			{
				Sleep(1);
				if ((GetTickCount() - tk) > TimeOut)
					return false;
			}
			return true;
		}

		bool WaitSensorInv(short Bits, DWORD TimeOut)
		{
			DWORD tk = GetTickCount();
			for (; !d1000_in_bit(Bits);)		//高电平状态
			{
				Sleep(1);
				if ((GetTickCount() - tk) > TimeOut)
					return false;
			}
			return true;
		}

		UINT WaitSensorTimeOutPos(short ibit, DWORD dwSensorTime, DWORD dwOverTime)
		{
			return 0;
		}

		UINT WaitSensorTimeOutNeg(short iPort, UINT iChanIN, DWORD dwSensorTime, DWORD dwOverTime)
		{
			return 0;
		}

		UINT WaitMotorDoneTimeOut(short Axis, DWORD dwOverTime)
		{
			DWORD tk = GetTickCount();
			for (; !d1000_check_done(Axis);)		//高电平状态
			{

				Sleep(1);
				if ((GetTickCount() - tk) > dwOverTime)
					return false;
			}
			return true;
		}


		//必须经过继电器
		void ConveyorCW() {
			WriteOutPutBit(prj.OUT_MOTOR_CW,ON);
			WriteOutPutBit(prj.OUT_MOTOR_CCW, OFF);
			ConveyorState = CW;
		}

		

		void ConveyorCCW() {
			WriteOutPutBit(prj.OUT_MOTOR_CW, OFF);
			WriteOutPutBit(prj.OUT_MOTOR_CCW, ON);
			ConveyorState = CCW;
		}

		void ConveyorSTOP() {
			WriteOutPutBit(prj.OUT_MOTOR_CW, OFF);
			WriteOutPutBit(prj.OUT_MOTOR_CCW, OFF);
			ConveyorState = STOP;
		}

		int GetConveyorState() {
			return ConveyorState;
		}

		void board_close()
		{
			ConveyorSTOP();
			Sleep(100);
			d1000_board_close();
		}

	};


	 
