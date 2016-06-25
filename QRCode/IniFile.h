// IniFile.h: interface for the CIniFile class.
// v1.00.01  Last Updated: 2005/11/02
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INIFILE_H__6C85A903_C7CD_4745_A010_C7D75ECED91B__INCLUDED_)
#define AFX_INIFILE_H__6C85A903_C7CD_4745_A010_C7D75ECED91B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CIniFile  
{
public:
	CIniFile(LPCTSTR lpszIniFileName=NULL);
	virtual ~CIniFile();
    
public:
	int GetInt(LPCTSTR lpszSec, LPCTSTR lpszKey, int iDef=0);
	BOOL WriteInt(LPCTSTR lpszSec, LPCTSTR lpszKey, int iValue);
	BOOL WriteDword(LPCTSTR lpszSec, LPCTSTR lpszKey, DWORD dwValue);
	BOOL WriteString(LPCTSTR lpszSec, LPCTSTR lpszKey, LPCTSTR lpszValue);
	DWORD GetDword(LPCTSTR lpszSec, LPCTSTR lpszKey, DWORD dwDef=0);
	CString GetString(LPCTSTR lpszSec, LPCTSTR lpszKey, LPCTSTR lpszDef=0);

    BOOL WriteBool(LPCTSTR lpszSec, LPCTSTR lpszKey, BOOL blValue)
    { return WriteDword(lpszSec, lpszKey, blValue?1:0); }
    BOOL GetBool(LPCTSTR lpszSec, LPCTSTR lpszKey, BOOL blDef=FALSE)
    { return (GetDword(lpszSec, lpszKey, (DWORD)blDef) != 0); }

	COLORREF GetColor(LPCTSTR lpszSec, LPCTSTR lpszKey, COLORREF clrDef=0);
	BOOL MakeFileWritable();
	BOOL IsFileExist(LPCTSTR lpszPathName, LPDWORD pdwAttr);
	BOOL SetFileName(LPCTSTR lpszNewFileName);
	BOOL SetDirName(LPCTSTR lpszNewDirName=NULL);
	static CString GetExeDir(BOOL blEndWithBkSlash=TRUE);
	BOOL ClearSection(LPCTSTR lpszSec);
	BOOL ClearKey(LPCTSTR lpszSec, LPCTSTR lpszKey);
	void GetAllSectionName(CStringArray &strArry);	
	BOOL CopyAllSectionDataIn(LPCTSTR lpszTarSec, LPCTSTR lpszSrcSec/*, CString strTarPathName*/, CString strSrcPathName);
	void GetAllKeyName(LPCTSTR lpszSec, CStringArray &strArry);

public:
	// Get Dir, Set can Path + file name.
	CString m_strIniPathName;
	CString m_strIniDirName;
	CString m_strIniFileName;
};

/* Usage:

   1. The CIniFile object use the executable file path by default, and the 
      m_strIniPathName set to the path (named, AppDir) after the object 
      constructed if without specified a lpszIniFileName.
      For example,

        //The code of C:\MyApp\App1.exe
        CIniFile myIni;

        //myIni.m_strIniPathName is "C:\MyApp\" now, you can copy for future used.
        CString strAppDir = myIni.m_strPathName;
        //Append with the Ini file name
        myIni.m_strIniFileName = "MySettings.ini";
        myIni.m_strIniPathName += myIni.m_strIniFileName;

        //Of course, if the ini file is "C:\Settings\MySettings.ini" not under
        // AppDir, you can use:
        // myIni.m_strPathName = "C:\\Settings\\MySettings.ini"
        //
        //Ready for used
        BOOL blEnableHook = myIni.GetBool("Settings", "EnableHook", FALSE);
        if (blEnableHook)
        {
            ...
        }

  2. You can specified the ini file name when constructing a CiniFile object,
     If so, you cannot get the AppDir from the m_strIniPathName. For example,
     
       CIniFile myIni("MySettings.ini");
       //It can be used if the ini file is under AppDir
       if (myIni.GetBool("Settings", "Enabled", FALSE))
          ...

       //And you can change its INI pathname when old ini file do not need
       // any more. By change the m_strIniPathName member.
       myIni.m_strIniPathName = "C:\\Settings\\OtherSettings.ini";
       DWORD dwCount = myIni.GetDword("Settings", "Count");

*/
#endif // !defined(AFX_INIFILE_H__6C85A903_C7CD_4745_A010_C7D75ECED91B__INCLUDED_)
