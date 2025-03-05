// mbswrapper.h
// Copyright (C) 2011 Suguru Kawamoto
// マルチバイト文字ワイド文字APIラッパー

#ifndef __MBSWRAPPER_H__
#define __MBSWRAPPER_H__

#include <windows.h>
#if defined(__cplusplus)
extern "C" {
#endif
int MtoW(LPWSTR pDst, int size, LPCSTR pSrc, int count);
int WtoM(LPSTR pDst, int size, LPCWSTR pSrc, int count);
int AtoW(LPWSTR pDst, int size, LPCSTR pSrc, int count);
int WtoA(LPSTR pDst, int size, LPCWSTR pSrc, int count);
int TerminateStringM(LPSTR lpString, int size);
size_t GetMultiStringLengthW(LPCWSTR lpString);
size_t GetMultiStringLengthA(LPCSTR lpString);
int WtoMMultiString(LPSTR pDst, int size, LPCWSTR pSrc);
int AtoWMultiString(LPWSTR pDst, int size, LPCSTR pSrc);
char* AllocateStringM(int size);
wchar_t* AllocateStringW(int size);
char* AllocateStringA(int size);
wchar_t* DuplicateMtoW(LPCSTR lpString, int c);
DWORD GetNextCharM(LPCSTR lpString, LPCSTR pLimit, LPCSTR* ppNext);
int PutNextCharM(LPSTR lpString, LPSTR pLimit, LPSTR* ppNext, DWORD Code);
DWORD GetNextCharW(LPCWSTR lpString, LPCWSTR pLimit, LPCWSTR* ppNext);
int PutNextCharW(LPWSTR lpString, LPWSTR pLimit, LPWSTR* ppNext, DWORD Code);
int GetCodeCountW(LPCWSTR lpString, int CharCount);
BOOL FixStringM(LPSTR pDst, LPCSTR pSrc);
BOOL CheckStringM(LPCSTR lpString);
void FreeDuplicatedString(void* p);
// UTF-8 UTF-16 LE間の変換処理でWindows XPのエミュレーションを行う
int MultiByteToWideCharAlternative(UINT CodePage, DWORD dwFlags, LPCSTR lpMultiByteStr, int cbMultiByte, LPWSTR lpWideCharStr, int cchWideChar);
int WideCharToMultiByteAlternative(UINT CodePage, DWORD dwFlags, LPCWSTR lpWideCharStr, int cchWideChar, LPSTR lpMultiByteStr, int cbMultiByte, LPCSTR lpDefaultChar, LPBOOL lpUsedDefaultChar);
#if defined(__cplusplus)
}
#endif

#endif
