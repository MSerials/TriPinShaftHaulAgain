#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include "stdio.h"



//	CTime m_time;

	//��ȡ��ǰʱ������  
//	CString m_strDate = m_time.Format(_T("%x"));          //��ʽ������  
//	CString m_strTime = m_time.Format(_T("%X"));          //��ʽ��ʱ��  

 //m_strDateTime = m_time.Format(_T("%Y-%m-%d %H:%M:%S %A"));   //��ʽ������ʱ��  


class csv
{

private:
	int currentLine = 0;
	std::ofstream outFile;
	int GetLine(std::string fileName)
	{
		using namespace std;
	
		std::ifstream infile(fileName,ios::in);//���ļ�����ios::in����ķ�ʽ��
		if (!infile)// �ж��ļ��Ƿ����
			return 0;
		char s[1024];
		int Cl = 0;
		while (infile)
		{
			Cl++;
			infile.getline(s, 100);//���뵽�ļ�β
		}
		infile.close();
		return Cl;
	}

public:
	csv(std::string fileName = "") {
		system("mkdir ErrorLog");
		if (fileName.empty())
			fileName = "ErrorLog/" + GetDate() + ".csv";
		currentLine = GetLine(fileName);
		outFile.open(fileName, std::ios::out);
	};
	~csv() { outFile.close(); };

	void WriteLine(std::string line_message)
	{
		outFile << line_message + "\n";
	}

	void WriteLine_Ex(std::string line_message)
	{
		char Msg[2048] = { 0 };
		sprintf_s(Msg, "%d,%s,%s", currentLine++, GetTime().c_str(), line_message.c_str());
		outFile << std::string(Msg) << std::endl;
	}

	std::string GetDate() {

		CString DateTime = CTime::GetCurrentTime().Format(_T("%Y-%m-%d"));
		return ((char*)(LPCSTR)(CStringA)(DateTime));
	}

	std::string GetTime() {
		CString m_strTime = CTime::GetCurrentTime().Format(_T("%H:%M:%S"));          //��ʽ��ʱ��  
		return ((char*)(LPCSTR)(CStringA)(m_strTime));
	}
};

