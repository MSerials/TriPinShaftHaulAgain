#include "../../stdafx.h"

// Preferences.cpp: implementation of the CPreferences class.
//
//////////////////////////////////////////////////////////////////////


#include "Preferences.h"
#include <fstream>

#pragma warning(disable:4996)

using namespace std;
//CPreferences *ini = NULL;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
////////////////////////////////////////////////////////////////////// 

CPreferences::CPreferences() 
{
	path = L"";
	dir = L"";
	Reset();
	initData();
}

CPreferences::~CPreferences()
{
	
}

//constructor, can specify pathname here instead of using SetPath later
void CPreferences::CreateIniFile(CString fileName)
{       
	CString strCurDir;	 
	strCurDir  = dir + L"\\" + fileName; 
	HANDLE hf = CreateFile(strCurDir, GENERIC_WRITE, FILE_SHARE_READ, NULL,CREATE_NEW, NULL, NULL);
	CloseHandle(hf);
	path = strCurDir;
}

void CPreferences::DeleteIniFile(CString fileName)
{
	CString strCurDir;	 
	strCurDir  = dir + "\\" + fileName; 
	DeleteFile(strCurDir);
}

void CPreferences::CopyIniFile(CString srcName, CString dstName)
{
	CString strOldPath,strNewPath;	 
	strOldPath  = dir + "\\" + srcName; 
	strNewPath  = dir + "\\" + dstName; 
	CopyFile(strOldPath,strNewPath,FALSE);
}

int CPreferences::SetIniFile(CString fileName)
{
	
	if (dir == "")
	{
		AfxMessageBox(L"请先设置文件目录!");
		return -1;
	}
	else
	{
		path = dir + L"\\" + fileName;
	}
	if (!PathFileExists(path))
	{
		HANDLE hf = CreateFile(path, GENERIC_WRITE, FILE_SHARE_READ, NULL,CREATE_NEW, NULL, NULL);
		CloseHandle(hf);
		
		return 0;
	}
	return 1;
}

CString CPreferences::GetAllFileName(CString suffix, int &nCnt, TCHAR separ)
{
	CString sFileName;
	CFileFind finder;	
	CString rStr;
	CString strWildcard;
	rStr.Empty();
	
	nCnt=0;
	strWildcard = dir + L"\\*" + suffix;		
	BOOL bWorking = finder.FindFile(strWildcard);
	
	if (dir == L"")
	{
		AfxMessageBox(L"请先设置文件目录!");
		return L"";
	}
	
	while (bWorking)
	{
		sFileName.Empty();		
		bWorking = finder.FindNextFile();
		if (finder.IsDots()) continue;
		sFileName = finder.GetFileName();
		sFileName = sFileName.Left(sFileName.ReverseFind('.')) + separ;
		rStr += sFileName;
		nCnt++;
	}
	rStr.TrimRight(separ);
	
	return rStr;
}

CString CPreferences::SplitString(CString srcStr, int nIndex, TCHAR separ/* = */)
{
	int i, len =0;
	CString strSplit;
	srcStr.TrimRight(separ);
	for(i=0; i<srcStr.GetLength(); i++)
	{
		if(srcStr[i] == separ)  len++;
	}
	
	if ((nIndex>len) || (srcStr==L"")) return L"";
	AfxExtractSubString(strSplit, srcStr, nIndex, separ);
	return strSplit;
}

/////////////////////////////////////////////////////////////////////
// Public Functions
/////////////////////////////////////////////////////////////////////

//sets path of ini file to read and write from
bool CPreferences::SetIniDir(CString filedir, bool bmodule/* =true */)
{
	if (bmodule)
		dir = GetModulePath() + "\\" + filedir;
	else
		dir = filedir;
	
	return CreateMultipleDirectory(dir);
}

//reads ini file specified using CIniFile::SetPath()
//returns true if successful, false otherwise
bool CPreferences::ReadFile()
{
	CFile file;
	CFileStatus status;
	if (!file.GetStatus(path,status))
		return 0;
	ifstream inifile;
	CString readinfo;
	inifile.open(path);
	int curkey = -1, curval = -1;
	if (inifile.fail())
	{
		error = "打开文件出错.";
		return 0;
	}
	CString keyname, valuename, _value;
	CString temp;
	while (getline(inifile,readinfo))
	{
		if (readinfo != "")
		{
			if (readinfo[0] == '[' && readinfo[readinfo.GetLength()-1] == ']') //if a section heading
			{
				//去掉[]
				keyname = readinfo;
				keyname.TrimLeft('[');
				keyname.TrimRight(']');
			}
			else //if a value
			{
				valuename = readinfo.Left(readinfo.Find(L"="));
				_value = readinfo.Right(readinfo.GetLength()-valuename.GetLength()-1);
				SetValue(keyname,valuename,_value);
			}
		}
	}
	inifile.close();
	return 1;
}

//writes data stored in class to ini file
void CPreferences::WriteFile()
{
	FILE *fp;
	fp=_tfopen(path,L"wt");
	if(fp==(FILE*)NULL)
	{
		return ;
	}
	for (int keynum = 0; keynum <= names.GetUpperBound(); keynum++)
	{
		if (keys[keynum].names.GetSize() != 0)
		{
			_ftprintf(fp,_T("[%s]\n"),names[keynum]);
			for (int valuenum = 0; valuenum <= keys[keynum].names.GetUpperBound(); valuenum++)
			{
				CString strtemp=keys[keynum].names[valuenum];
				CString strtemp1=keys[keynum].values[valuenum];
				_ftprintf(fp,_T("%s=%s"),strtemp,strtemp1);
				
				if (valuenum != keys[keynum].names.GetUpperBound())
					_ftprintf(fp,_T("\n"));
				else
					if (keynum < names.GetSize())
						_ftprintf(fp,_T("\n"));
			}
			if (keynum < names.GetSize())
				_ftprintf(fp,_T("\n"));
		}
	}
	fclose(fp);
}

//deletes all stored ini data
void CPreferences::Reset()
{
	keys.SetSize(0);
	names.SetSize(0);
}

//returns number of keys currently in the ini
int CPreferences::GetNumKeys()
{
	return keys.GetSize();
}

//returns number of values stored for specified key, or -1 if key found
int CPreferences::GetNumValues(CString keyname)
{
	int keynum = FindKey(keyname);
	if (keynum == -1)
		return -1;
	else
		return keys[keynum].names.GetSize();
}

//gets value of [keyname] valuename = 
//overloaded to return CString, int, and double
CString CPreferences::GetValue(CString keyname, CString valuename)
{
	int keynum = FindKey(keyname), valuenum = FindValue(keynum,valuename);
	
	if (keynum == -1)
	{
		error = L"没有此项"+keyname;
		return L"";
	}
	
	if (valuenum == -1)
	{
		error = L"没有此项"+valuename;
		return L"";
	}
	return keys[keynum].values[valuenum];
}

//gets value of [keyname] valuename = 
//overloaded to return CString, int, and double
int CPreferences::GetValueI(CString keyname, CString valuename)
{
	return _tstoi(GetValue(keyname,valuename));
}

//gets value of [keyname] valuename = 
//overloaded to return CString, int, and double
double CPreferences::GetValueF(CString keyname, CString valuename)
{
	return _tstof(GetValue(keyname, valuename));
}

CString CPreferences::GetValueStr(CString keyname, CString valuename)
{
	return GetValue(keyname, valuename);
}
bool CPreferences::GetValuebool(CString keyname,CString valuename)
{
	return (GetValue(keyname,valuename)==L"0")?false:true;
}


//sets value of [keyname] valuename =.
//specify the optional paramter as false (0) if you do not want it to create
//the key if it doesn't exist. Returns true if data entered, false otherwise
//overloaded to accept CString, int, and double
bool CPreferences::SetValue(CString keyname, CString valuename, CString value, bool create)
{
	int keynum = FindKey(keyname), valuenum = 0;
	//find key
	if (keynum == -1) //if key doesn't exist
	{
		if (!create) //and user does not want to create it,
			return 0; //stop entering this key
		names.SetSize(names.GetSize()+1);
		keys.SetSize(keys.GetSize()+1);
		keynum = names.GetSize()-1;
		names[keynum] = keyname;
	}
	
	//find value
	valuenum = FindValue(keynum,valuename);
	if (valuenum == -1)
	{
		if (!create)
			return 0;
		keys[keynum].names.SetSize(keys[keynum].names.GetSize()+1);
		keys[keynum].values.SetSize(keys[keynum].names.GetSize()+1);
		valuenum = keys[keynum].names.GetSize()-1;
		keys[keynum].names[valuenum] = valuename;
	}
	keys[keynum].values[valuenum] = value;
	return 1;
}

//sets value of [keyname] valuename =.
//specify the optional paramter as false (0) if you do not want it to create
//the key if it doesn't exist. Returns true if data entered, false otherwise
//overloaded to accept CString, int, and double
bool CPreferences::SetValueI(CString keyname, CString valuename, int value, bool create)
{
	CString temp;
	temp.Format(L"%d",value);
	return SetValue(keyname, valuename, temp, create);
}

//sets value of [keyname] valuename =.
//specify the optional paramter as false (0) if you do not want it to create
//the key if it doesn't exist. Returns true if data entered, false otherwise
//overloaded to accept CString, int, and double
bool CPreferences::SetValueF(CString keyname, CString valuename, double value, bool create)
{
	CString temp;
	temp.Format(L"%e",value);
	return SetValue(keyname, valuename, temp, create);
}

bool CPreferences::SetValueStr(CString keyname, CString valuename,CString value,bool create)
{
	return SetValue(keyname, valuename, value, create);
}
bool CPreferences::SetValuebool(CString keyname,CString valuename,bool value,bool create)
{
	CString temp;
	if(value==false)
		temp=L"0";
	else
		temp=L"1";
	return SetValue(keyname, valuename, temp, create);
}

/////////////////////////////////////////////////////////////////////
// Private Functions
/////////////////////////////////////////////////////////////////////

//returns index of specified key, or -1 if not found
int CPreferences::FindKey(CString keyname)
{
	int keynum = 0;
	while ( keynum < keys.GetSize() && names[keynum] != keyname)
		keynum++;
	if (keynum == keys.GetSize())
		return -1;
	return keynum;
}

//returns index of specified value, in the specified key, or -1 if not found
int CPreferences::FindValue(int keynum, CString valuename)
{
	if (keynum == -1)
		return -1;
	int valuenum = 0;
	while (valuenum < keys[keynum].names.GetSize() && keys[keynum].names[valuenum] != valuename)
		valuenum++;
	if (valuenum == keys[keynum].names.GetSize())
		return -1;
	return valuenum;
}

//overloaded from original getline to take CString
istream & CPreferences:: getline(istream & is, CString & str)
{
    char buf[2048];
    is.getline(buf,2048);
    str = buf;
    return is;
}


void CPreferences::OpenIniFile()
{
	::ShellExecute(NULL,L"open",path,NULL,NULL,SW_SHOW);
}

CString CPreferences::GetFilePath()
{
	return path;
}

CString CPreferences::GetFileDir()
{
	return dir;
}

CString CPreferences::GetModulePath()
{
	TCHAR szModulePath[MAX_PATH] = {0};        
	CString strCurDir;
	
	::GetModuleFileName(NULL,szModulePath,_MAX_PATH);  	
	strCurDir  = szModulePath;  
	strCurDir  = strCurDir.Left(strCurDir.ReverseFind(TEXT('\\')));
	return strCurDir;
}

bool CPreferences::CreateMultipleDirectory(CString szPath)
{
	if (szPath == _T("")) return false;
	
    CString strDir(szPath);
	
    if (strDir.GetAt(strDir.GetLength()-1)!=_T('\\'))
    {
        strDir += _T('\\');
    }
    std::vector<CString> vPath;
    CString strTemp;
    bool bSuccess = false;
	
    for (int i=0;i<strDir.GetLength();++i)
    {
        if (strDir.GetAt(i) != _T('\\')) 
        {
            strTemp += strDir.GetAt(i);
        }
        else 
        {
            vPath.push_back(strTemp);
            strTemp += _T('\\');
			
        }
    }
	
    std::vector<CString>::const_iterator vIter;
    for (vIter = vPath.begin(); vIter != vPath.end(); vIter++) 
    {
        bSuccess = CreateDirectory(*vIter, NULL) ? true : false;    
    }	
    return bSuccess;
}


int CPreferences::SelFolder(CWnd *pWnd, CString &strFolder)
{
	HWND hParent = pWnd->m_hWnd;
    LPMALLOC lpMalloc;
    strFolder.Empty();	
	
    if (::SHGetMalloc(&lpMalloc) != NOERROR) return 0;
	
    TCHAR szDisplayName[_MAX_PATH];
    TCHAR szBuffer[_MAX_PATH];
    BROWSEINFO browseInfo;
    browseInfo.hwndOwner = hParent;
    browseInfo.pidlRoot = NULL; // set root at Desktop
    browseInfo.pszDisplayName = szDisplayName;
    browseInfo.lpszTitle = L"Select a folder";
    browseInfo.ulFlags = BIF_RETURNFSANCESTORS|BIF_RETURNONLYFSDIRS;
    browseInfo.lpfn = NULL;
    browseInfo.lParam = 0;
	
    LPITEMIDLIST lpItemIDList;
    if ((lpItemIDList = ::SHBrowseForFolder(&browseInfo)) != NULL)
    {
        // Get the path of the selected folder from the    item ID list.
        if (::SHGetPathFromIDList(lpItemIDList, szBuffer))
        {
            // At this point, szBuffer contains the path the user chose.
            if (szBuffer[0] == '\0') return 0;
			
            // We have a path in szBuffer! Return it.
            strFolder = szBuffer;
            return 1;
        }
        else return 1; // strResult is empty
		
        lpMalloc->Free(lpItemIDList);
        lpMalloc->Release();
    }
    
	return 1;
}
CString CPreferences::Encrypt(CString Source, WORD Key) // 加密函数
{
	CString Result,str;
	int i,j;
	
	Result=Source; // 初始化结果字符串
	for(i=0; i<Source.GetLength(); i++) // 依次对字符串中各字符进行操作
	{
		Result.SetAt(i, Source.GetAt(i)^(Key>>8)); // 将密钥移位后与字符异或
		Key = ((BYTE)Result.GetAt(i)+Key)*C1+C2; // 产生下一个密钥
	}
	Source=Result; // 保存结果
	Result.Empty(); // 清除结果
	for(i=0; i<Source.GetLength(); i++) // 对加密结果进行转换
	{
		j=(BYTE)Source.GetAt(i); // 提取字符
		// 将字符转换为两个字母保存
		str=L"12"; // 设置str长度为2
		str.SetAt(0, SHIFT+j/26);//这里将65改大点的数例如256，密文就会变乱码，效果更好，相应的，解密处要改为相同的数
		str.SetAt(1, SHIFT+j%26);
		Result += str;
	}
	return Result;
}


CString CPreferences::Decrypt(CString Source, WORD Key) // 解密函数
{
	CString Result,str;
	int i,j;
	
	Result.Empty(); // 清除结果
	for(i=0; i < Source.GetLength()/2; i++) // 将字符串两个字母一组进行处理
	{
		j = ((BYTE)Source.GetAt(2*i)-SHIFT)*26;//相应的，解密处要改为相同的数		
		j += (BYTE)Source.GetAt(2*i+1)-SHIFT;
		str="1"; // 设置str长度为1
		str.SetAt(0, j);
		Result+=str; // 追加字符，还原字符串
	}
	Source=Result; // 保存中间结果
	for(i=0; i<Source.GetLength(); i++) // 依次对字符串中各字符进行操作
	{
		Result.SetAt(i, (BYTE)Source.GetAt(i)^(Key>>8)); // 将密钥移位后与字符异或
		Key = ((BYTE)Source.GetAt(i)+Key)*C1+C2; // 产生下一个密钥
	}
	return Result;
}


void CPreferences::LoadParaFile(UINT item)
{
	UINT mask = item;

	if (ReadFile())
	{
		////////////////////////////////////
		if (PARA_PRJ == (mask&PARA_PRJ))
		{
		
			//m_Prj_Vector = GetValue(L"projectSetting", L"PRJ_VECTOR");

		}
		if (PARA_IMAGE == (mask&PARA_IMAGE))
		{
			Threshold = GetValueF(L"IMAGESET", L"THRESHOLD");
			Score_Threshold = GetValueF(L"IMAGESET", L"SCORE");
			Snap_Delay = GetValueF(L"IMAGESET", L"SNAPDELAY");

			Cyl_Out_Delay = GetValueF(L"IMAGESET", L"CYLOUTDELAY");
			Cyl_Back_Delay = GetValueF(L"IMAGESET", L"CYLBACKDELAY");
		}



		if (PARA_IO == (mask&PARA_IO))
		{

		

		}
	
	}

}

void CPreferences::SaveParaFile(UINT item)
{
	UINT mask = item;
	////////////////////////////////////
	if (PARA_PRJ==(mask&PARA_PRJ))
	{
	
	}

	if(PARA_IMAGE==(mask&PARA_IMAGE))
	{
		 SetValueF(L"IMAGESET", L"THRESHOLD", Threshold);
		 SetValueF(L"IMAGESET", L"SCORE", Score_Threshold);
		 SetValueF(L"IMAGESET", L"SNAPDELAY", Snap_Delay);
		 SetValueF(L"IMAGESET", L"CYLOUTDELAY", Cyl_Out_Delay);
		 SetValueF(L"IMAGESET", L"CYLBACKDELAY", Cyl_Back_Delay);		
	}
	if(PARA_IO==(mask&PARA_IO))
	{

	

	}
	
	//if(PARA_SN==(mask&PARA_SN))
	//{
	//	SetValue(L"PRJ_SN",L"SN",m_password); 
	//}
	WriteFile();
}


void CPreferences::initData()
{
	//#define PARA_PRJ            (0x1<<0)
	
}