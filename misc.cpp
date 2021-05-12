﻿/*=============================================================================
*
*							その他の汎用サブルーチン
*
===============================================================================
/ Copyright (C) 1997-2007 Sota. All rights reserved.
/
/ Redistribution and use in source and binary forms, with or without 
/ modification, are permitted provided that the following conditions 
/ are met:
/
/  1. Redistributions of source code must retain the above copyright 
/     notice, this list of conditions and the following disclaimer.
/  2. Redistributions in binary form must reproduce the above copyright 
/     notice, this list of conditions and the following disclaimer in the 
/     documentation and/or other materials provided with the distribution.
/
/ THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR 
/ IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES 
/ OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
/ IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, 
/ INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, 
/ BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF 
/ USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON 
/ ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
/ (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF 
/ THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
/============================================================================*/

#include "common.h"


/*----- 文字列の最後に "\" を付ける -------------------------------------------
*
*	Parameter
*		char *Str : 文字列
*
*	Return Value
*		なし
*
*	Note
*		オリジナルの文字列 char *Str が変更されます。
*----------------------------------------------------------------------------*/

void SetYenTail(char* Str) {
	if (Str[strlen(Str) - 1] != '\\')
		strcat(Str, "\\");
}


/*----- 文字列の最後に "/" を付ける -------------------------------------------
*
*	Parameter
*		char *Str : 文字列
*
*	Return Value
*		なし
*
*	Note
*		オリジナルの文字列 char *Str が変更されます。
*----------------------------------------------------------------------------*/

void SetSlashTail(char* Str) {
#if defined(HAVE_TANDEM)
	/* Tandem では / の代わりに . を追加 */
	if (AskHostType() == HTYPE_TANDEM) {
		if (Str[strlen(Str) - 1] != '.')
			strcat(Str, ".");
	} else
#endif
		if (Str[strlen(Str) - 1] != '/')
			strcat(Str, "/");
}


/*----- 文字列内の特定の文字を全て置き換える ----------------------------------
*
*	Parameter
*		char *Str : 文字列
*		char Src : 検索文字
*		char Dst : 置換文字
*
*	Return Value
*		なし
*----------------------------------------------------------------------------*/

void ReplaceAll(char* Str, char Src, char Dst) {
#if defined(HAVE_TANDEM)
	/* Tandem ではノード名の変換を行わない */
	/* 最初の1文字が \ でもそのままにする */
	if (AskRealHostType() == HTYPE_TANDEM && strlen(Str) > 0)
		Str++;
#endif
	for (char* Pos; (Pos = strchr(Str, Src)) != NULL;)
		*Pos = Dst;
}


/*----- 大文字／小文字を区別しないstrstr --------------------------------------
*
*	Parameter
*		char *s1 : 文字列１
*		char *s2 : 文字列２
*
*	Return Value
*		char *文字列１中で文字列２が見つかった位置
*			NULL=見つからなかった
*----------------------------------------------------------------------------*/

const char* stristr(const char* s1, const char* s2)
{
	const char* Ret;

	Ret = NULL;
	while(*s1 != NUL)
	{
		if((tolower(*s1) == tolower(*s2)) &&
		   (_strnicmp(s1, s2, strlen(s2)) == 0))
		{
			Ret = s1;
			break;
		}
		s1++;
	}
	return(Ret);
}


/*----- 文字列中のスペースで区切られた次のフィールドを返す --------------------
*
*	Parameter
*		char *Str : 文字列
*
*	Return Value
*		char *次のフィールド
*			NULL=見つからなかった
*----------------------------------------------------------------------------*/

const char* GetNextField(const char* Str)
{
	if((Str = strchr(Str, ' ')) != NULL)
	{
		while(*Str == ' ')
		{
			if(*Str == NUL)
			{
				Str = NULL;
				break;
			}
			Str++;
		}
	}
	return(Str);
}


/*----- 現在のフィールドの文字列をコピーする ----------------------------------
*
*	Parameter
*		char *Str : 文字列
*		char *Buf : コピー先
*		int Max : 最大文字数
*
*	Return Value
*		int ステータス
*			FFFTP_SUCCESS/FFFTP_FAIL=長さが長すぎる
*----------------------------------------------------------------------------*/

int GetOneField(const char* Str, char *Buf, int Max)
{
	int Sts;
	const char* Pos;

	Sts = FFFTP_FAIL;
	if((Pos = strchr(Str, ' ')) == NULL)
	{
		if((int)strlen(Str) <= Max)
		{
			strcpy(Buf, Str);
			Sts = FFFTP_SUCCESS;
		}
	}
	else
	{
		if(Pos - Str <= Max)
		{
			strncpy(Buf, Str, Pos - Str);
			*(Buf + (Pos - Str)) = NUL;
			Sts = FFFTP_SUCCESS;
		}
	}
	return(Sts);
}


/*----- パス名の中のファイル名の先頭を返す ------------------------------------
*
*	Parameter
*		char *Path : パス名
*
*	Return Value
*		char *ファイル名の先頭
*
*	Note
*		ディレクトリの区切り記号は "\" と "/" の両方が有効
*----------------------------------------------------------------------------*/

const char* GetFileName(const char* Path)
{
	const char* Pos;

	if((Pos = strchr(Path, ':')) != NULL)
		Path = Pos + 1;

	if((Pos = strrchr(Path, '\\')) != NULL)
		Path = Pos + 1;

	if((Pos = strrchr(Path, '/')) != NULL)
		Path = Pos + 1;

#if defined(HAVE_TANDEM)
	/* Tandem は . がデリミッタとなる */
	if((AskHostType() == HTYPE_TANDEM) && ((Pos = strrchr(Path, '.')) != NULL))
		Path = Pos + 1;
#endif
	return(Path);
}


/*----- パス名の中の拡張子の先頭を返す ----------------------------------------
*
*	Parameter
*		char *Path : パス名
*
*	Return Value
*		char *拡張子の先頭
*----------------------------------------------------------------------------*/

const char* GetFileExt(const char* Path)
{
	const char* Ret;

	Ret = Path + strlen(Path);
	if((strcmp(Path, ".") != 0) &&
	   (strcmp(Path, "..") != 0))
	{
		while((Path = strchr(Path, '.')) != NULL)
		{
			Path++;
			Ret = Path;
		}
	}
	return(Ret);
}


// パス名からファイル名を取り除く
//   ディレクトリの区切り記号は "\" と "/" の両方が有効
static std::string_view RemoveFileName(std::string_view path) {
	if (auto const p = path.rfind('/'); p != std::string_view::npos)
		return path.substr(0, p);
	if (auto const p = path.rfind('\\'); p != std::string_view::npos)
		if (p == 0 || path[p - 1] != ':')
			return path.substr(0, p);
	return path;
}


/*----- 上位ディレクトリのパス名を取得 ----------------------------------------
*
*	Parameter
*		char *Path : パス名
*
*	Return Value
*		なし
*
*	Note
*		ディレクトリの区切り記号は "\" と "/" の両方が有効
*		最初の "\"や"/"は残す
*			例） "/pub"   --> "/"
*			例） "C:\DOS" --> "C:\"
*----------------------------------------------------------------------------*/

void GetUpperDir(char *Path)
{
	char *Top;
	char *Pos;

	if(((Top = strchr(Path, '/')) != NULL) ||
	   ((Top = strchr(Path, '\\')) != NULL))
	{
		Top++;
		if(((Pos = strrchr(Top, '/')) != NULL) ||
		   ((Pos = strrchr(Top, '\\')) != NULL))
			*Pos = NUL;
		else
			*Top = NUL;
	}
	return;
}


/*----- 上位ディレクトリのパス名を取得 ----------------------------------------
*
*	Parameter
*		char *Path : パス名
*
*	Return Value
*		なし
*
*	Note
*		ディレクトリの区切り記号は "\" と "/" の両方が有効
*		最初の "\"や"/"も消す
*			例） "/pub"   --> ""
*			例） "C:\DOS" --> "C:"
*----------------------------------------------------------------------------*/

void GetUpperDirEraseTopSlash(char *Path)
{
	char *Pos;

	if(((Pos = strrchr(Path, '/')) != NULL) ||
	   ((Pos = strrchr(Path, '\\')) != NULL))
		*Pos = NUL;
	else
		*Path = NUL;

	return;
}


/*----- ディレクトリの階層数を返す --------------------------------------------
*
*	Parameter
*		char *Path : パス名
*
*	Return Value
*		なし
*
*	Note
*		単に '\' と '/'の数を返すだけ
*----------------------------------------------------------------------------*/

int AskDirLevel(const char* Path)
{
	const char* Pos;
	int Level;

	Level = 0;
	while(((Pos = strchr(Path, '/')) != NULL) ||
		  ((Pos = strchr(Path, '\\')) != NULL))
	{
		Path = Pos + 1;
		Level++;
	}
	return(Level);
}


// ファイルサイズを文字列に変換する
std::wstring MakeSizeString(double size) {
	if (size < 1024)
		return strprintf(L"%.0lfB", size);
	size /= 1024;
	if (size < 1024)
		return strprintf(L"%.2lfKB", size);
	size /= 1024;
	if (size < 1024)
		return strprintf(L"%.2lfMB", size);
	size /= 1024;
	if (size < 1024)
		return strprintf(L"%.2lfGB", size);
	size /= 1024;
	if (size < 1024)
		return strprintf(L"%.2lfTB", size);
	size /= 1024;
	return strprintf(L"%.2lfPB", size);
}


// StaticTextの領域に収まるようにパス名を整形して表示
void DispStaticText(HWND hWnd, const char* Str) {
	RECT rect;
	GetClientRect(hWnd, &rect);
	auto font = (HFONT)SendMessageW(hWnd, WM_GETFONT, 0, 0);

	auto dc = GetDC(hWnd);
	auto previous = SelectObject(dc, font);
	auto wstr = u8(Str);
	std::wstring_view view = wstr;
	for (;;) {
		if (size(view) <= 4)
			break;
		SIZE size;
		GetTextExtentPoint32W(dc, data(view), size_as<int>(view), &size);
		if (size.cx <= rect.right)
			break;
		if (auto pos = view.find(L'\\', 4); pos != std::wstring_view::npos)
			view.remove_prefix(pos - 3);
		else if (pos = view.find(L'/', 4); pos != std::wstring_view::npos)
			view.remove_prefix(pos - 3);
		else
			view.remove_prefix(1);
		std::fill_n(const_cast<wchar_t*>(data(view)), 3, L'.');		// viewはwstrを指しているため書き換え可能
	}
	SelectObject(dc, previous);
	ReleaseDC(hWnd, dc);
	SetText(hWnd, data(view));
}


/*----- RECTをクライアント座標からスクリーン座標に変換 ------------------------
*
*	Parameter
*		HWND hWnd : ウインドウハンドル
*		RECT *Rect : RECT
*
*	Return Value
*		なし
*----------------------------------------------------------------------------*/

void RectClientToScreen(HWND hWnd, RECT *Rect)
{
	POINT Tmp;

	Tmp.x = Rect->left;
	Tmp.y = Rect->top;
	ClientToScreen(hWnd, &Tmp);
	Rect->left = Tmp.x;
	Rect->top = Tmp.y;

	Tmp.x = Rect->right;
	Tmp.y = Rect->bottom;
	ClientToScreen(hWnd, &Tmp);
	Rect->right = Tmp.x;
	Rect->bottom = Tmp.y;

	return;
}


/*----- ＵＮＣ文字列を分解する ------------------------------------------------
*
*	Parameter
*		char *unc : UNC文字列
*		char *Host : ホスト名をコピーするバッファ (サイズは HOST_ADRS_LEN+1)
*		char *Path : パス名をコピーするバッファ (サイズは FMAX_PATH+1)
*		char *File : ファイル名をコピーするバッファ (サイズは FMAX_PATH+1)
*		char *User : ユーザ名をコピーするバッファ (サイズは USER_NAME_LEN+1)
*		char *Pass : パスワードをコピーするバッファ (サイズは PASSWORD_LEN+1)
*		int *Port : ポート番号をコピーするバッファ
*
*	Return Value
*		int ステータス
*			FFFTP_SUCCESS/FFFTP_FAIL
*
*	"\"は全て"/"に置き換える
*----------------------------------------------------------------------------*/

int SplitUNCpath(char *unc, std::wstring& Host, std::wstring& Path, char *File, std::wstring& User, std::wstring& Pass, int *Port)
{
	char *Pos1;
	char *Pos2;
	char Tmp[FMAX_PATH+1];

	Host.clear();
	Path.clear();
	memset(File, NUL, FMAX_PATH+1);
	User.clear();
	Pass.clear();
	*Port = IPPORT_FTP;

	ReplaceAll(unc, '\\', '/');

	if((Pos1 = strstr(unc, "//")) != NULL)
		Pos1 += 2;
	else
		Pos1 = unc;

	if((Pos2 = strchr(Pos1, '@')) != NULL)
	{
		memset(Tmp, NUL, FMAX_PATH+1);
		memcpy(Tmp, Pos1, Pos2-Pos1);
		Pos1 = Pos2 + 1;

		if ((Pos2 = strchr(Tmp, ':')) != NULL) {
			User = u8({ Tmp, Pos2 });
			Pass = u8(Pos2 + 1);
		} else
			User = u8(Tmp);
	}

	// IPv6対応
	if((Pos2 = strchr(Pos1, '[')) != NULL && Pos2 < strchr(Pos1, ':'))
	{
		Pos1 = Pos2 + 1;
		if((Pos2 = strchr(Pos2, ']')) != NULL)
		{
			Host = u8({ Pos1, Pos2 });
			Pos1 = Pos2 + 1;
		}
	}

	if((Pos2 = strchr(Pos1, ':')) != NULL)
	{
		if (empty(Host))
			Host = u8({ Pos1, Pos2 });
		Pos2++;
		if(IsDigit(*Pos2))
		{
			*Port = atoi(Pos2);
			while(*Pos2 != NUL)
			{
				if(IsDigit(*Pos2) == 0)
					break;
				Pos2++;
			}
		}
		Path = u8(RemoveFileName(Pos2));
		strncpy(File, GetFileName(Pos2), FMAX_PATH);
	}
	else if((Pos2 = strchr(Pos1, '/')) != NULL)
	{
		if (empty(Host))
			Host = u8({ Pos1, Pos2 });
		Path = u8(RemoveFileName(Pos2));
		strncpy(File, GetFileName(Pos2), FMAX_PATH);
	}
	else
	{
		if (empty(Host))
			Host = u8(Pos1);
	}

	return empty(Host) ? FFFTP_FAIL : FFFTP_SUCCESS;
}


/*----- 日付文字列(JST)をFILETIME(UTC)に変換 ----------------------------------
*
*	Parameter
*		char *Time : 日付文字列 ("yyyy/mm/dd hh:mm")
*		FILETIME *Buf : ファイルタイムを返すワーク
*
*	Return Value
*		int ステータス
*			YES/NO=日付情報がなかった
*----------------------------------------------------------------------------*/

int TimeString2FileTime(const char *Time, FILETIME *Buf)
{
	SYSTEMTIME sTime;
	FILETIME fTime;
	int Ret;

	Ret = NO;
	Buf->dwLowDateTime = 0;
	Buf->dwHighDateTime = 0;

	if(strlen(Time) >= 16)
	{
		if(IsDigit(Time[0]) && IsDigit(Time[5]) && IsDigit(Time[8]) && 
		   IsDigit(Time[12]) && IsDigit(Time[14]))
		{
			Ret = YES;
		}

		sTime.wYear = atoi(Time);
		sTime.wMonth = atoi(Time + 5);
		sTime.wDay = atoi(Time + 8);
		if(Time[11] != ' ')
			sTime.wHour = atoi(Time + 11);
		else
			sTime.wHour = atoi(Time + 12);
		sTime.wMinute = atoi(Time + 14);
		// タイムスタンプのバグ修正
//		sTime.wSecond = 0;
		if(strlen(Time) >= 19)
			sTime.wSecond = atoi(Time + 17);
		else
			sTime.wSecond = 0;
		sTime.wMilliseconds = 0;

		SystemTimeToFileTime(&sTime, &fTime);
		LocalFileTimeToFileTime(&fTime, Buf);
	}
	return(Ret);
}


/*----- FILETIME(UTC)を日付文字列(JST)に変換 ----------------------------------
*
*	Parameter
*		FILETIME *Time : ファイルタイム
*		char *Buf : 日付文字列を返すワーク
*		int Mode : モード (DISPFORM_xxx)
*		int InfoExist : 情報があるかどうか (FINFO_xxx)
*
*	Return Value
*		なし
*----------------------------------------------------------------------------*/
void FileTime2TimeString(const FILETIME *Time, char *Buf, int Mode, int InfoExist, int ShowSeconds)
{
	SYSTEMTIME sTime;
	FILETIME fTime;

	if(Mode == DISPFORM_LEGACY)
	{
		if((Time->dwLowDateTime == 0) && (Time->dwHighDateTime == 0))
			InfoExist = 0;

		// タイムスタンプのバグ修正
//		/* "yyyy/mm/dd hh:mm" */
		/* "yyyy/mm/dd hh:mm:ss" */
		FileTimeToLocalFileTime(Time, &fTime);
		// タイムスタンプのバグ修正
//		FileTimeToSystemTime(&fTime, &sTime);
		if(!FileTimeToSystemTime(&fTime, &sTime))
			InfoExist = 0;

		// タイムスタンプのバグ修正
//		if(InfoExist & FINFO_DATE)
//			sprintf(Buf, "%04d/%02d/%02d ", sTime.wYear, sTime.wMonth, sTime.wDay);
//		else
//			sprintf(Buf, "           ");
//
//		if(InfoExist & FINFO_TIME)
//			sprintf(Buf+11, "%2d:%02d", sTime.wHour, sTime.wMinute);
//		else
//			sprintf(Buf+11, "     ");
		if(InfoExist & (FINFO_DATE | FINFO_TIME))
		{
			if(InfoExist & FINFO_DATE)
				sprintf(Buf, "%04d/%02d/%02d ", sTime.wYear, sTime.wMonth, sTime.wDay);
			else
				sprintf(Buf, "           ");
			if(ShowSeconds == YES)
			{
				if(InfoExist & FINFO_TIME)
					sprintf(Buf+11, "%2d:%02d:%02d", sTime.wHour, sTime.wMinute, sTime.wSecond);
				else
					sprintf(Buf+11, "        ");
			}
			else
			{
				if(InfoExist & FINFO_TIME)
					sprintf(Buf+11, "%2d:%02d", sTime.wHour, sTime.wMinute);
				else
					sprintf(Buf+11, "     ");
			}
		}
		else
			Buf[0] = NUL;
	}
}


/*----- 属性文字列を値に変換 --------------------------------------------------
*
*	Parameter
*		char *Str : 属性文字列 ("rwxrwxrwx")
*
*	Return Value
*		int 値
*----------------------------------------------------------------------------*/

int AttrString2Value(const char* Str) {
	int num = 0;
	if (strlen(Str) >= 9) {
		for (auto bit : { 0x400, 0x200, 0x100, 0x40, 0x20, 0x10, 0x4, 0x2, 0x1 })
			if (*Str++ != '-')
				num |= bit;
	} else if (strlen(Str) >= 3)
		std::from_chars(Str, Str + 3, num, 16);
	return num;
}


/*----- 属性の値を文字列に変換 ------------------------------------------------
*
*	Parameter
*		int Attr : 属性の値
*		char *Buf : 属性文字列をセットするバッファ ("rwxrwxrwx")
*
*	Return Value
*		int 値
*----------------------------------------------------------------------------*/

// ファイルの属性を数字で表示
//void AttrValue2String(int Attr, char *Buf)
void AttrValue2String(int Attr, char *Buf, int ShowNumber)
{
	// ファイルの属性を数字で表示
//	strcpy(Buf, "---------");
//
//	if(Attr & 0x400)
//		Buf[0] = 'r';
//	if(Attr & 0x200)
//		Buf[1] = 'w';
//	if(Attr & 0x100)
//		Buf[2] = 'x';
//
//	if(Attr & 0x40)
//		Buf[3] = 'r';
//	if(Attr & 0x20)
//		Buf[4] = 'w';
//	if(Attr & 0x10)
//		Buf[5] = 'x';
//
//	if(Attr & 0x4)
//		Buf[6] = 'r';
//	if(Attr & 0x2)
//		Buf[7] = 'w';
//	if(Attr & 0x1)
//		Buf[8] = 'x';
	if(ShowNumber == YES)
	{
		sprintf(Buf, "%03x", Attr);
	}
	else
	{
		strcpy(Buf, "---------");

		if(Attr & 0x400)
			Buf[0] = 'r';
		if(Attr & 0x200)
			Buf[1] = 'w';
		if(Attr & 0x100)
			Buf[2] = 'x';

		if(Attr & 0x40)
			Buf[3] = 'r';
		if(Attr & 0x20)
			Buf[4] = 'w';
		if(Attr & 0x10)
			Buf[5] = 'x';

		if(Attr & 0x4)
			Buf[6] = 'r';
		if(Attr & 0x2)
			Buf[7] = 'w';
		if(Attr & 0x1)
			Buf[8] = 'x';
	}

	return;
}

static auto GetFilterString(std::initializer_list<FileType> fileTypes) {
	static auto const map = [] {
		std::map<FileType, std::wstring> map;
		for (auto fileType : AllFileTyes) {
			// リソース文字列は末尾 '\0' を扱えないので '\t' を利用し、ここで置換する。
			auto text = GetString(static_cast<UINT>(fileType));
			for (auto& ch : text)
				if (ch == L'\t')
					ch = L'\0';
			map.emplace(fileType, text);
		}
		return map;
	}();
	return std::accumulate(begin(fileTypes), end(fileTypes), L""s, [](auto const& result, auto fileType) { return result + map.at(fileType); });
}

// ファイル選択
fs::path SelectFile(bool open, HWND hWnd, UINT titleId, const wchar_t* initialFileName, const wchar_t* extension, std::initializer_list<FileType> fileTypes) {
	auto const filter = GetFilterString(fileTypes);
	wchar_t buffer[FMAX_PATH + 1];
	wcscpy(buffer, initialFileName);
	auto const title = GetString(titleId);
	DWORD flags = (open ? OFN_FILEMUSTEXIST : OFN_EXTENSIONDIFFERENT | OFN_OVERWRITEPROMPT) | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST;
	OPENFILENAMEW ofn{ sizeof(OPENFILENAMEW), hWnd, 0, filter.c_str(), nullptr, 0, 1, buffer, size_as<DWORD>(buffer), nullptr, 0, nullptr, title.c_str(), flags, 0, 0, extension };
	auto const cwd = fs::current_path();
	int result = (open ? GetOpenFileNameW : GetSaveFileNameW)(&ofn);
	fs::current_path(cwd);
	if (!result)
		return {};
	return buffer;
}


// ディレクトリを選択
fs::path SelectDir(HWND hWnd) {
	fs::path path;
	auto const cwd = fs::current_path();
	auto const title = GetString(IDS_MSGJPN185);
	BROWSEINFOW bi{ hWnd, nullptr, nullptr, title.c_str(), BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE };
	if (auto idlist = SHBrowseForFolderW(&bi)) {
		wchar_t buffer[FMAX_PATH + 1];
		SHGetPathFromIDListEx(idlist, buffer, size_as<DWORD>(buffer), GPFIDL_DEFAULT);
		path = buffer;
		CoTaskMemFree(idlist);
	}
	fs::current_path(cwd);
	return path;
}


std::string MakeNumString(LONGLONG Num) {
	std::stringstream ss;
	ss.imbue(std::locale{ "" });
	ss << std::fixed << Num;
	return ss.str();
}


#if defined(HAVE_TANDEM)
/*----- ファイルサイズからEXTENTサイズの計算を行う ----------------------------
*
*	Parameter
*		LONGLONG Size : ファイルサイズ
*
*	Return Value
*		なし
*----------------------------------------------------------------------------*/
void CalcExtentSize(TRANSPACKET *Pkt, LONGLONG Size)
{
	LONGLONG extent;

	/* EXTENTS(4,28) MAXEXTENTS 978 */
	if(Size < 56025088) {
		Pkt->PriExt = DEF_PRIEXT;
		Pkt->SecExt = DEF_SECEXT;
		Pkt->MaxExt = DEF_MAXEXT;
	} else {
		/* 増加余地を残すため Used 75% 近辺になるように EXTENT サイズを調整) */
		extent = (LONGLONG)(Size / ((DEF_MAXEXT * 0.75) * 2048LL));
		/* 28未満にすると誤差でFile Fullになる可能性がある */
		if(extent < 28)
			extent = 28;

		Pkt->PriExt = (int)extent;
		Pkt->SecExt = (int)extent;
		Pkt->MaxExt = DEF_MAXEXT;
	}
}
#endif

static std::tuple<int, int> dpi{ 0, 0 };
void UpdateDisplayDPI() {
	static auto shcore = LoadLibraryW(L"Shcore.dll");
	static auto getDpiForMonitor = reinterpret_cast<GetDpiForMonitorFunc>(GetProcAddress(shcore, "GetDpiForMonitor"));
	if (getDpiForMonitor) {
		UINT x = 0U, y = 0U;
		auto currentMonitor = MonitorFromWindow(GetMainHwnd(), MONITOR_DEFAULTTONEAREST);
		getDpiForMonitor(currentMonitor, MDT_EFFECTIVE_DPI, &x, &y);
		dpi = std::tuple<int, int>{ static_cast<int>(x), static_cast<int>(y) };
	} else {
		int x = 0, y = 0;
		if (auto dc = GetDC(0)) {
			x = GetDeviceCaps(dc, LOGPIXELSX);
			y = GetDeviceCaps(dc, LOGPIXELSY);
			DeleteDC(dc);
		}
		dpi = std::tuple<int, int>{ x, y };
	}
}

static auto QueryDisplayDPI() {
	return dpi;
}

int CalcPixelX(int x) {
	auto [dpix, _] = QueryDisplayDPI();
	return (x * dpix + 96 / 2) / 96;
}

int CalcPixelY(int y) {
	auto [_, dpiy] = QueryDisplayDPI();
	return (y * dpiy + 96 / 2) / 96;
}

int CalcLogicalX(int pixelX) {
	auto [dpix, _] = QueryDisplayDPI();
	return (pixelX * 96 + 96 / 2) / dpix;
}

int CalcLogicalY(int pixelY) {
	auto [_, dpiy] = QueryDisplayDPI();
	return (pixelY * 96 + 96 / 2) / dpiy;
}
