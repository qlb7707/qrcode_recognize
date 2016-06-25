// IniFile.cpp: implementation of the CIniFile class.
// v1.00.01 2005/11/02
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "IniFile.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define MAX_ALLSECTIONS	65535
#define MAX_ALLKEYS		65535

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIniFile::CIniFile(LPCTSTR lpszIniFileName /*NULL*/)
:m_strIniDirName(""), m_strIniFileName(""), m_strIniPathName("")
{
	SetDirName();
    //If Ini file name specified
    if (lpszIniFileName != NULL)
    {
		SetFileName(lpszIniFileName);
    }
}

CIniFile::~CIniFile()
{

}

BOOL CIniFile::IsFileExist(LPCTSTR lpszPathName, LPDWORD pdwAttr)
{
	WIN32_FIND_DATA wfd;
	HANDLE hFind;

	if(lpszPathName == NULL)
		hFind = FindFirstFile(m_strIniPathName, &wfd);
	else
		hFind = FindFirstFile(lpszPathName, &wfd);

    FindClose(hFind);
    //If file/directory found
    if (hFind != INVALID_HANDLE_VALUE)
    {
        //If need to output File Attribute
        if (pdwAttr != 0)
        {
            //Check if the buffer is valid
            if (::AfxIsValidAddress((const void*) pdwAttr, sizeof(DWORD)))
            {
                //Output the file attribute
                *pdwAttr = wfd.dwFileAttributes;
            }
        }

        return TRUE;
    }
    return FALSE;
}

CString CIniFile::GetString(LPCTSTR lpszSec, LPCTSTR lpszKey, LPCTSTR lpszDef)
{
    TCHAR szBuf[4096] = {0};
	DWORD nSize = 0;
	nSize = sizeof(szBuf);

    DWORD dwRetChars = GetPrivateProfileString(lpszSec, lpszKey, lpszDef, szBuf, nSize, m_strIniPathName);
    if (dwRetChars == 0)
        return CString("");
    else 
        return CString(szBuf);
}

DWORD CIniFile::GetDword(LPCTSTR lpszSec, LPCTSTR lpszKey, DWORD dwDef)
{
    UINT wValue = GetPrivateProfileInt(lpszSec, lpszKey, (INT)dwDef,
                                       m_strIniPathName);

    return (DWORD) wValue;
}

BOOL CIniFile::WriteString(LPCTSTR lpszSec, LPCTSTR lpszKey, LPCTSTR lpszValue)
{
    return WritePrivateProfileString(lpszSec, lpszKey, lpszValue, m_strIniPathName);
}

BOOL CIniFile::WriteDword(LPCTSTR lpszSec, LPCTSTR lpszKey, DWORD dwValue)
{
    CString str;
    str.Format("%lu", dwValue);
    return WritePrivateProfileString(lpszSec, lpszKey, str, m_strIniPathName);
}

BOOL CIniFile::MakeFileWritable()
{
    //HANDLE hFile = ::CreateFile(m_strIniPathName, GENERIC_WRITE, 0, );
    return FALSE;
}

BOOL CIniFile::WriteInt(LPCTSTR lpszSec, LPCTSTR lpszKey, int iValue)
{
    CString str;
    str.Format("%d", iValue);
    return WritePrivateProfileString(lpszSec, lpszKey, str, m_strIniPathName);
}

int CIniFile::GetInt(LPCTSTR lpszSec, LPCTSTR lpszKey, int iDef /*0*/)
{
    int iValue = GetPrivateProfileInt(lpszSec, lpszKey, iDef,
                                       m_strIniPathName);
    return iValue;
}

BOOL CIniFile::SetFileName(LPCTSTR lpszNewFileName)
{
	m_strIniFileName = lpszNewFileName;
	//Update the PathName
	m_strIniPathName = m_strIniDirName + m_strIniFileName;
	return TRUE;
}

BOOL CIniFile::SetDirName(LPCTSTR lpszNewDirName /*NULL*/)
{
	//Apply with the same dir. with current running .exe 
	if (lpszNewDirName == NULL)
		m_strIniDirName = GetExeDir(TRUE); //end with '\'
	else
	{
		m_strIniDirName = lpszNewDirName;
		//Make sure end with '\'
		if (m_strIniDirName.Right(1) != "\\")
			m_strIniDirName += "\\";
	}

	//Update the PathName
	m_strIniPathName = m_strIniDirName + m_strIniFileName;

	return TRUE;
}

CString CIniFile::GetExeDir(BOOL blEndWithBkSlash)
{
    TCHAR szDir[MAX_PATH] = {0};
    GetModuleFileName(NULL, szDir, MAX_PATH);
    CString strDir = szDir;
    int iEos = strDir.ReverseFind('\\');
    if (iEos >= 0)
	{
		int iCutPos = (blEndWithBkSlash ? (iEos+1) : (iEos));
        strDir = strDir.Left(iCutPos);
	}

	return strDir;
}

//the value string in INI file must be format of: "R,G,B" 
//for example, "12,127,96" for R=12, G=127, and B=96
COLORREF CIniFile::GetColor(LPCTSTR lpszSec, LPCTSTR lpszKey, COLORREF clrDef/* =0 */)
{
	//Get the string
	CString strRgb = GetString(lpszSec, lpszKey);
	//Not specifies, return default
	if (strRgb.IsEmpty())
		return clrDef;

	int iStart=0;
	CString strR = strRgb.Tokenize(_T(","), iStart);
	CString strG = strRgb.Tokenize(_T(","), iStart);
	CString strB = strRgb.Tokenize(_T(","), iStart);

	TCHAR *ptr;
	int iR = strtol(strR.GetBuffer(0), &ptr, 10);
	int iG = strtol(strG.GetBuffer(0), &ptr, 10);
	int iB = strtol(strB.GetBuffer(0), &ptr, 10);

	if ((iR>=0) && (iR<=255) && (iG>=0) && (iG<=255) && (iB>=0) && (iB<=255))
		return RGB((BYTE)iR, (BYTE)iG, (BYTE)iB);

	return clrDef;
}

//Clear entire section keys
BOOL CIniFile::ClearSection(LPCTSTR lpszSec)
{
	return WritePrivateProfileStruct(lpszSec, NULL, NULL, 0, m_strIniPathName);
}

BOOL CIniFile::CopyAllSectionDataIn(LPCTSTR lpszTarSec, LPCTSTR lpszSrcSec/*, CString strTarPathName*/, CString strSrcPathName)
{
	BOOL blRet = TRUE;
	DWORD dwRet, i, j, iPos=0;

	TCHAR chAllKeysAndValues[MAX_ALLKEYS];
	TCHAR chTempKeyAndValues[MAX_PATH];
	CString strTepKey;

	ZeroMemory(chAllKeysAndValues, MAX_ALLKEYS);
	ZeroMemory(chTempKeyAndValues, MAX_PATH);

	dwRet = GetPrivateProfileSection(
		lpszSrcSec,
		chAllKeysAndValues,
		MAX_ALLKEYS,
		strSrcPathName);

	for (i=0; i<MAX_ALLKEYS; i++)
	{
		if (chAllKeysAndValues[i] == NULL)
			if (chAllKeysAndValues[i] == chAllKeysAndValues[i+1])
				break;
	}

	i++;

	for (j=0; j<i; j++)
	{
		chTempKeyAndValues[iPos++] = chAllKeysAndValues[j];
		if (chAllKeysAndValues[j] == NULL)
		{
			strTepKey = chTempKeyAndValues;
			CString strKey = strTepKey.Left(strTepKey.Find('='));
			CString strValue = strTepKey.Mid(strTepKey.Find('=')+1);

			//WriteString(lpszTarSec, strKey, strValue);
			blRet = WritePrivateProfileString(lpszTarSec, ( LPCTSTR)strKey, (LPCTSTR)strValue, m_strIniPathName/*strTarPathName*/);
			if (!blRet)
				break;	

			iPos = 0;
		}
	}
	
	return blRet;
}

//Clear entire section keys
BOOL CIniFile::ClearKey(LPCTSTR lpszSec, LPCTSTR lpszKey)
{
	return WritePrivateProfileStruct(lpszSec, lpszKey, NULL, 0, m_strIniPathName);
}

void CIniFile::GetAllSectionName(CStringArray &strArry)
{
	strArry.RemoveAll();
	TCHAR szBuf[4096] = {0};
	DWORD nSize = 0;
	nSize = sizeof(szBuf);

	DWORD dwRet = GetPrivateProfileSectionNames(szBuf, nSize, m_strIniPathName);

	CString strSectionName;
	strSectionName.Empty();
	for(int i=0; i<dwRet; i++)
	{
		if(szBuf[i] != '\0') 
		{
			strSectionName = strSectionName + szBuf[i];
		} 
		else 
		{
			if(!strSectionName.IsEmpty())
			{
				strArry.Add(strSectionName);
			}
			strSectionName.Empty();
		}
	}

}

void CIniFile::GetAllKeyName(LPCTSTR lpszSec, CStringArray &strArry)
{	
	TCHAR szBuf[4096] = {0};
	DWORD nSize = 0;
	nSize = sizeof(szBuf);

	CString strRet;
	DWORD dwRetChars = GetPrivateProfileString(lpszSec, NULL, "", szBuf, nSize, m_strIniPathName);

	CString strKeyName;
	strKeyName.Empty();

	for(int i=0; i<dwRetChars; i++)
	{
		if(szBuf[i] != '\0') 
		{
			strKeyName = strKeyName + szBuf[i];
		} 
		else 
		{
			if(!strKeyName.IsEmpty())
			{
				strArry.Add(strKeyName);
			}
			strKeyName.Empty();
		}
	}

}
