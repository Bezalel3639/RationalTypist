#include "stdafx.h"
#include "Typing.Win32.h"
#include <stdio.h> //fgets
#include <iostream> //cin
#include <fstream> //cin
#include "commctrl.h" //SB_***
#include "commdlg.h" // file/open dialog [comdlg32.lib]
#include "shellapi.h" // ShellExecute (HTTP link)
#include <stdlib.h>   // strtol for Hex to Dec color conversion
#include "wininet.h"  // wininet for online help support
#include <process.h> //_beginththredex (/ML -> /MT!)
//#include "uxtheme.h" // UxTheme.lib (SetWindowTheme)

#include <string>
#include <vector>
#include <algorithm>

using namespace std ; //for vector declaration

// Conversion from 'LRESULT' to 'int' (or 'LPARAM' to 'int', double' to 'float, 
// int' to 'float, 'LRESULT' to 'HRESULT') possible loss of data: minus ~50 
// warnings.
#pragma warning(disable: 4244)

// Conversion from 'size_t' to 'int' (or 'size_t' to 'short', 'size_t' to 
// 'DWORD') possible loss of data: -14 warnings.
#pragma warning(disable: 4267)   

// Define shotcuts for MessageBox(...) used in debug only

// Use as M(L"asdas") or M(szBuffer).
#define M(LPCTSTR) (int)MessageBox(0, (LPCTSTR), L"Message", 0)
// Use as MA("asdas")
#define MA(LPCSTR) (int)MessageBoxA(0, (LPCSTR), "Message", 0)   

// Global variables and functions (default).
#define MAX_LOADSTRING 200 //100

HINSTANCE g_hInst;								
TCHAR szTitle[MAX_LOADSTRING];
TCHAR szWindowClass[MAX_LOADSTRING];

ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				App_InitInstance(HINSTANCE, int);      // InitInstance is 
                                                           // default name     
LRESULT CALLBACK	App_WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	App_About(HWND, UINT, WPARAM, LPARAM); // About is 
                                                           // default name
// Application global variables and functions.

// Relative position of EXE to folders/files are different in install and
// development environments. In install, EXE on the same level with 
// Settings.xml.
// #define DEVELOPMENT_STRUCTURE // comment for product release 

#define APP_DIRECTORY_ANSI "C:\\MyFiles\\Development\\VCC\\TypingPro.Win32"
#define APP_DIRECTORY_UNICODE _T("C:\\MyFiles\\Development\\VCC\\TypingPro.Win32")
#define MAX_SAMPLE_TEXT 2000

//-- 
short g_nScreenWidth;
short g_nScreenHeight;
int g_nClientHeight;

// Mode 3 and Mode4 control positioning for 1280x1024 screen resolution and any 
// different from 1024x768 (notebook). 
int g_nMode34_ClientHeight_Base = 820;

RECT g_rcMode3_Edit_Base = {25, 60, 525, 260}; // v1.0: 25, 60, 500, 200
RECT g_rcMode3_ListView_Base = {25, 280, 525, 715}; // v1.0: 25, 280, 500, 435 
RECT g_rcMode4_Edit1_Base = {25, 60, 525, 380}; // v1.0: 25, 60, 500, 320
RECT g_rcMode4_Edit2_Base = {25, 395, 525, 715}; // v1.0: 25, 395, 500, 320
RECT g_rcMode4_DS_Base = {0, 77, 325, 792};  // rc.right, rc.top + 77, 325, 703
int g_nMode4_DS_ListView_Bottom_Base = 575; 
int g_nMode4_DS_Button_Top_Base = 595; 

// Mode 3 and Mode4 control positioning for 1024x768 screen resolution 
// (notebook). 
int g_nMode34_ClientHeight_1024x768 = 720;

RECT g_rcMode3_Edit_1024x768 = {25, 40, 525, 240}; 
RECT g_rcMode3_ListView_1024x768 = {25, 250, 525, 630}; 
RECT g_rcMode4_Edit1_1024x768 = {25, 40, 525, 330};
RECT g_rcMode4_Edit2_1024x768 = {25, 340, 525, 630};
RECT g_rcMode4_DS_1024x768 = {0, 77, 325, 693}; 
int g_nMode4_DS_ListView_Bottom_1024x768 = 482; 
int g_nMode4_DS_Button_Top_1024x768 = 500; 
//--

HWND g_hWnd;
HWND g_hwndTB;
HWND g_hwndURLLogo; 
WNDPROC g_wpOldURLLogoWndProc;
HWND g_hwndStatus;
HFONT g_hfnt;
BOOL g_bSaveLastUsed = TRUE;
BOOL g_bShowURLLogo = TRUE;
BOOL g_bShowStatusBarHints = TRUE;
short g_nMode = 1;
short g_nInitialMode; // used to determine dirty state of g_nMode
// g_nLangID is not the same as system LanguageID. 
short g_nLangID = 1; // English (1); French (2); Russian (3).
short g_nInitialLangID; // used to determine dirty state of g_nLangID
short g_nBackgroundColor = COLOR_APPWORKSPACE; // not used now
BOOL g_bAlertEnabled = TRUE;
COLORREF g_clrBackgroundColor;
BOOL g_bDebug = TRUE;
HWND g_hPropSheetDlg; // TODO: neet it?
TCHAR g_szBuffer[MAX_LOADSTRING]; // used as buffer for localized strings
//
//// URL logo positioning ("By Synaptex").
//short g_nMode12_URLLogoLeft = 716; 
//short g_nMode3_URLLogoLeft = 420; 
//short g_nMode4_URLLogoLeft = 420; 
//short g_nURLLogoWidth = 150; // all modes
//short g_nMode4_BS_URLLogoShift = 200;

// URL logo positioning ("Help keep RT free").
short g_nMode12_URLLogoLeft = 698; 
short g_nMode3_URLLogoLeft = 400; 
short g_nMode4_URLLogoLeft = 400; 
short g_nURLLogoWidth = 150; // all modes
short g_nMode4_BS_URLLogoShift = 200;

void App_XMLWriteValue(char* pszTagName /* always ANSI*/, 
                                            char* pszTagValue /* always ANSI*/);
void App_ConfigureOptions();
LRESULT CALLBACK App_OptionsSheetProc(HWND hDlg, UINT message, LPARAM lParam);
LRESULT CALLBACK App_OptionsPageProc(HWND hDlg, UINT message, WPARAM wParam, 
                                                                 LPARAM lParam);
void App_XMLReadSettings();
void App_XMLRead_SampleTestFFN();
void App_SetLanguage();
void App_SetFont();
void App_SetMenuState();
void App_CreateToolBar();
void App_UpdateToolbarButtonsStrings(short nLangID);
void App_CreateStatusBar();
void PopulateWithTrainingWords(short nType, short nLangID);
void App_LocalizeStrings(short nLangID);
TCHAR* App_GetStringFromStringTable(UINT uStringID, short nLangID);
void App_OnSaveSettings();
BOOL App_CenterWindow(HWND hwndCentered, HWND hwndPreferredOwner, 
                                                            int iHorizMove = 0);
void App_OnChangeLanguage(short nLangID);
void App_ParseToDecColorValues(char* pszColorValue /* in */, 
           long& iRed /* out */, long& iGreen /* out */, long& iBlue /* out */);
void App_AddLanguageMenu();
void App_Debug_FunctionCallTrace(char* pszMessage);
void App_CreateLogo();
LRESULT APIENTRY App_URLLogo_SubclassedFunc(HWND hWnd, UINT Message, 
                                                    WPARAM wParam, LONG lParam);
WNDPROC g_wpOldToolbarWndProc;
LRESULT APIENTRY App_Toolbar_SubclassedFunc(HWND hWnd, UINT Message, 
                                                     WPARAM wParam, LONG lParam);
BOOL App_MenuHandler_ShowOnlineHelp(LPCTSTR pszHelpFile);
wstring App_GetFileName(wstring* psFFN);
wstring App_GetDirectory(wstring* psFFN);
wstring App_ProofFFN(LPCTSTR pszName);
BOOL App_IsLangInstalled(LPCTSTR pszLangID /* in */);

//--
wstring g_sSampleFFN; 
string g_sLogoText; 
COLORREF g_clrLogoTextColor;
string g_sLogoURL; 
short g_nLogoLeftIndent;
BOOL g_bTestCompleted; // flag to paint backgroung of base stats window

BOOL App_CheckInternetConnection(LPCTSTR pszURL);
BOOL App_IsBaseHelpURLLinkAvailable(LPCTSTR pszURL);

BOOL App_IsCachedLogoUpdatesFileExists();
static UINT __stdcall App_ThreadProc_ShowLogo(LPVOID param);
void App_CacheLogoUpdatesFile(char* pszRemoteFile);
BOOL App_IsValidRemoteLogoUpdatesFile(char* pszRemoteFile);
string App_XMLReadValue(char* pszTagName /* always ANSI*/, 
                                                   char* pszFFN /* XML file */);
void App_ParseLogoUpdatesFile(char* pszFFN);
void Mode4_ClearBaseStats();
//--

// Mode 1 & 2 globals and function declarations.
short g_nLastTimeHand = 1; // left (2 - right)

HWND g_hwndEdit;

//HWND g_hwndMode1_Edit;
HWND g_hwndMode1_ListBox_LeftHandWords;
HWND g_hwndMode1_ListBox_RightHandWords;
HWND g_hwndMode1_ListBox_Slowest;
HWND g_hwndMode1_ListBox_Fastest;
HWND g_hwndMode1_ListBox_MostFrequent;
HWND g_hwndMode1_ListBox_LeastFrequent;
HWND g_hwndMode1_Static_Slowest;
HWND g_hwndMode1_Static_Fastest;
HWND g_hwndMode1_Static_MostFrequent;
HWND g_hwndMode1_Static_LeastFrequent;
HWND g_hwndMode1_Static_TypingSpeed;
HWND g_hwndMode1_Static_WordsTyped;
HWND g_hwndMode1_Static_CharsTyped; 
HWND g_hwndMode1_Static_ErrorsCount; 
HWND g_hwndMode1_Button_Repopulate;
HWND g_hwndMode1_Button_ShowLog;

LRESULT CALLBACK Mode1_OptionsPageProc(HWND hDlg, UINT message, WPARAM wParam, 
                                                                 LPARAM lParam);
BOOL Mode12_SpellCheck(LPTSTR pszBuffer, WPARAM wParam);
void Mode12_GetLastTypedWord(LPTSTR pszLastWord);
BOOL Mode12_IsListBoxMember(LPTSTR pszWord);

// Mode 3 globals and function declarations.
HWND g_hwndMode3_Edit;
HWND g_hwndMode3_ListView;
short g_nDropWord;
short g_nCorrectlyTypedCount;

HWND g_hwndMode3_ListBox_CustomWords;
HWND g_hwndMode3_ListBox_Slowest;

HWND g_hwndMode3_Static_Slowest;
HWND g_hwndMode3_Static_TypingSpeed;
HWND g_hwndMode3_Static_WordsTyped;
HWND g_hwndMode3_Static_CharsTyped; 
HWND g_hwndMode3_Static_ErrorsCount; 

HWND g_hwndMode3_Button_UpdateSorting;
HWND g_hwndMode3_Button;

void OnMenuMode3();
void Mode3_PopulateWithTrainingWords(short nLangID);
BOOL Mode3_IsWordTypedCorrectly(LPTSTR pszBuffer, WPARAM wParam);
short Mode3_GetSampleWordIndex(LPTSTR pszWord);
void Mode3_GetLastTypedWord(LPTSTR pszLastWord);
BOOL Mode3_IsCustomWordsMember(LPTSTR pszWord /* partial or complete word */);
void Mode3_UpdateData(short i);
void Mode3_UpdateBarsStateMenu();
void Mode3_UpdateLanguageStrings();
LRESULT Mode3_NotifyHandler(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam);
int CALLBACK Mode3_CompareProc(LPARAM lParam1, LPARAM lParam2, 
                         LPARAM lSortColumn /* in, application-defined value*/);
int Mode3_GetCompareReturnValue(float fValue1, float fValue2, int iSortColumn);
void Mode3_SetCustomModeItemState();
void Mode3_UpdateCustomList();

struct TEST_RESULTS 
{
    // Base statistics.
    wstring  sTimeSpent;
    wstring  sCPM;
    wstring  sTextLength;
    wstring  sWordsCount;
    wstring  sWPM;
    wstring  sTotalErrorsCount;

    // Misc.
    wstring sTitle;
    float fTimeSpent; // used with alternative metrics (keeps precision)
    int nLinesCount; // actually CRLF count plus one
};

struct CustomWords
{
    wstring sWord;
    int   nLength;           // word length in chars
    float fTimeSpent;        // time spent for typing
    float fTotalTimeSpent;   // total time spent per word 
    short nErrorsCount;      // number of errors during typing this word
    short nTotalErrorsCount; // total errors count per word
    short nTypedCount;       // number of times the word was typed 
};

vector<CustomWords> m_vecCustomWords;

// Mode 4 globals and function declarations.
HWND g_hwndEdit2; //?

HWND g_hwndMode4_Static_TimeSpent;
HWND g_hwndMode4_Static_CPM;
HWND g_hwndMode4_Static_CharsTyped;
HWND g_hwndMode4_Static_WordsTyped;
HWND g_hwndMode4_Static_WPM;
HWND g_hwndMode4_Static_TotalErrors;

HWND g_hwndMode4_Static_TimeSpent_Value;
HWND g_hwndMode4_Static_CPM_Value;
HWND g_hwndMode4_Static_CharsTyped_Value;
HWND g_hwndMode4_Static_WordsTyped_Value;
HWND g_hwndMode4_Static_WPM_Value;
HWND g_hwndMode4_Static_TotalErrors_Value;

HWND g_hWndMode4_DetailedStats;
HWND g_hwndMode4_ListView; 
HWND g_hwndMode4_Button_CreateCustomList;
HWND g_hwndMode4_DS_StaticLine; 
HWND g_hwndMode4_DS_StaticIcon;
HWND g_hwndMode4_DS_Static;
HWND g_hwndMode4_Button_CheckAll;
HWND g_hwndButtonTest, g_hwndButtonTest2;

COLORREF g_clrMode4_BS_BackgroundColor = RGB(255, 0, 0); // base statistics
COLORREF g_clrMode4_DS_BackgroundColor = RGB(255, 255, 255); 
COLORREF g_clrMode4_DS_TableBgColor = RGB(255, 255, 255); // white (not persisted)
COLORREF g_clrMode4_DS_InterlacedColor = RGB(212, 212, 212); // light gray
COLORREF g_clrMode4_DS_LHColor = RGB(206, 206, 255); // light blue (lavender (#E6E6FA)?)
COLORREF g_clrMode4_DS_RHColor = RGB(255, 202, 202); // light red
COLORREF g_clrMode4_DS_L2RColor = RGB(255, 255, 176);
COLORREF g_clrMode4_DS_R2LColor = RGB(187, 255, 187);
COLORREF g_clrMode4_DS_CustomColor = RGB(255, 215, 174); // light orange
BOOL g_bNaturalWordCount = FALSE;
BOOL g_bLogResults = FALSE;
BOOL g_bInitShowHint1Dialog; // stores initial value
BOOL g_bInitShowHint2Dialog; // stores initial value
BOOL g_bShowHint1Dialog = FALSE;
BOOL g_bShowHint2Dialog = FALSE;

TCHAR g_szSampleText[MAX_SAMPLE_TEXT];
short g_iScanCodePrev = -1;
short g_nTotalErrorsCount = 0;
//int g_nCharsTyped; // for error count (all hits during a test)
int g_nCharsTyped = 0; // for error count (all hits during a test)
short g_nErrorsCount = 0;
short g_bWordInProcessOfTyping = FALSE;
DWORD g_dwTickCount1;
short g_aSortOrder[4] = {0,0,0,0}; // initial/previous sort direction

class CTestResults_ByWords
{
public:
    short nIndex;
    wstring sWord;
    //TCHAR szWord[50];
    int   nLength;      // word length in chars
    float fTimeSpent;   // time spent for typing
    short nErrorsCount; // number of errors during typing this word
};
vector<CTestResults_ByWords> m_vecSampleWords;

void OnMenuMode4();              // test mode
BOOL Mode4_LoadFile(LPTSTR& pszFileText, LPCTSTR pszSampleFilePath = NULL);
void Mode4_OnStartTest();
void Mode4_GetLastTypedWord(LPTSTR pszLastWord);
BOOL Mode4_IsWordTypedCorrectly(LPTSTR pszBuffer, WPARAM wParam);
short Mode4_GetSampleWordIndex(LPTSTR pszWord);
void Mode4_ParseSampleText(LPCTSTR pszText);
void Debug_Mode4_WriteTestResults();
void Mode4_CreateCustomList();
void Mode4_CheckAll();
void Mode4_OnResize(BOOL bExpand);
void Mode4_OnDetailedStats();
LRESULT CALLBACK Mode4_Hint1Dialog_WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK Mode4_Hint2Dialog_WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK Mode4_DetailedStats_WndProc(HWND, UINT, WPARAM, LPARAM);
void Mode4_OnFileOpen();
LRESULT Mode4_NotifyHandler(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam);
int CALLBACK Mode4_CompareProc(LPARAM lParam1, LPARAM lParam2, 
                                                            LPARAM iSortColumn);
int Mode4_GetCompareReturnValue(float dValue1, float dValue2, int iSortColumn);
short Mode4_GetWordType(LPTSTR pszWord);
void Mode4_GetCurrentCustomWords();
void Mode4_CreateHtmlReport();
void Mode4_UpdateLog(TEST_RESULTS* ptr);
void Mode4_GetDateTimeStamp(char* pszDate, 
    LPWSTR pszDayOfWeek /* always wide since language-dependent */, 
                                                                 char* pszTime);
LRESULT CALLBACK Mode4_DetailedStatsColors_WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL Mode4_VerifyFileEncoding(LPCTSTR pszText);
BOOL Mode4_IsEndOfTest(LPTSTR pszLastChar);
BOOL Mode4_IsTestCompleted();
int Mode4_GetTextLength(wstring* psBuffer);
int Mode4_GetTextLinesCount(wstring* psBuffer);
wstring Mode4_GetTextTitle(wstring* psBuffer /* in */);

int g_nTickCountStart, g_nTickCountFinish;
BOOL g_bTestRunning = FALSE;

WNDPROC g_wpOldWndProc, g_wpOldWndProc2;

//LONG FAR PASCAL 
LRESULT APIENTRY Edit_SubclassedFunc(HWND hWnd,UINT Message,WPARAM wParam, 
                                                                   LONG lParam);

void OnKeyDown(LPCTSTR pszChar);
void InitVector();
void OnButtonShowLogClicked();
void OnButtonTest(LPCTSTR pszText, int nCharPos);
void OnRepopulateButtonClicked();
void OnMenuMode12(short nMode); // LH/RH and LTOR/RTOL modes
void SelectPairs(LPCTSTR pszText, int nCharPos);
BOOL GetSampleWords(LPTSTR pszPair /* in */,  LPTSTR pszPrevWord /* out */, 
                                                      LPTSTR pszWord /* out */);
class CCharacterPairs
{
public:
    //string sPair;
    wstring sPair;
    int   nTimeLapLast;    // ms
    float fTimeLapAverage; // ms
    int   nHitCount; 
    int   nTimeLapSum;
};

struct TimeLapAvgSortDesc
{
    bool operator() (const CCharacterPairs & x, const CCharacterPairs & y )
    {
        return x.fTimeLapAverage > y.fTimeLapAverage;
    }
};

struct TimeLapAvgSortAsc
{
    bool operator() (const CCharacterPairs & x, const CCharacterPairs & y )
    {
        return x.fTimeLapAverage < y.fTimeLapAverage;
    }
};

struct HitCountSortDesc
{
    bool operator() (const CCharacterPairs & x, const CCharacterPairs & y )
    {
        return x.nHitCount > y.nHitCount;
    }
};

struct HitCountSortAsc
{
    bool operator() (const CCharacterPairs & x, const CCharacterPairs & y )
    {
        return x.nHitCount < y.nHitCount;
    }
};

struct WordLengthSortDesc
{
    bool operator() (const CTestResults_ByWords & x, 
                                                 const CTestResults_ByWords & y)
    {
        return x.nLength > y.nLength;
    }
};

struct ErrorsCountSortDesc
{
    bool operator() (const CTestResults_ByWords & x, 
                                                 const CTestResults_ByWords & y)
    {
        return x.nErrorsCount > y.nErrorsCount;
    }
};

// LL no longer used.
struct CustomWords_TimeSpent_SortDesc
{
    bool operator() (const CustomWords & x, const CustomWords & y)
    {
        return x.fTimeSpent > y.fTimeSpent;
    }
};


DWORD m_TickCount1, m_TickCount2;
wstring s1, s2;
HANDLE m_hLog;
short GetPairIndex(LPTSTR pszPair);
vector<CCharacterPairs> m_vecPairs;
vector<CCharacterPairs>::iterator it;

vector<wstring> vecLeftHandWords, vecRightHandWords; // not used
vector<wstring> vecSampleWords; // container for samole LH/RH words. 

// About dialog declarations.
WNDPROC g_wpOldAboutStaticWndProc;
void About_SetFont(HWND hWnd);
LRESULT APIENTRY About_StaticSubclassedFunc(HWND hWnd, UINT Message, 
                                                    WPARAM wParam, LONG lParam);

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, 
                                                 LPTSTR lpCmdLine, int nCmdShow)
{
    MSG msg;
    HACCEL hAccelTable;

    // Read application settings from Settings.xml.
    App_XMLReadSettings();

    // After reading g_bDebug. 
    if (g_bDebug) App_Debug_FunctionCallTrace("_tWinMain Entered");

    // Set application locale (affects title, menu etc).
    App_SetLanguage();
    
    // Initialize global strings. Note that default LoadString fails to load 
    // correct language string for the application title albeit language 
    // locale is already set with SetThreadLocale). Therefore FindResourceEx, 
    // LoadResource is used.
    _tcscpy(szTitle, App_GetStringFromStringTable(IDS_APP_TITLE, g_nLangID));
    LoadString(hInstance, IDC_TYPINGWIN32, szWindowClass, MAX_LOADSTRING);

    MyRegisterClass(hInstance);

    // Perform application initialization.
    if (!App_InitInstance (hInstance, nCmdShow)) 
    {
        return FALSE;
    }

    hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_TYPINGWIN32);

    // Main message loop.
    while (GetMessage(&msg, NULL, 0, 0)) 
    {
        if (!TranslateAccelerator(g_hWnd, hAccelTable, &msg)) 
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    if (g_bDebug) App_Debug_FunctionCallTrace("_tWinMain Exited");

    return (int) msg.wParam;
}

//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEX wcex;

    wcex.cbSize = sizeof(WNDCLASSEX); 

    wcex.style			= CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc	= (WNDPROC)App_WndProc;
    wcex.cbClsExtra		= 0;
    wcex.cbWndExtra		= 0;
    wcex.hInstance		= hInstance;
    wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_APP_ICON); 
    wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);

    // Backgroung brush is used by DefWindowProc to process WM_ERASEBKND message,
    // we use custom brush to provide custom background. Default is COLOR_WINDOW. 
    // MSDN'03: "value 1 must be added to the chosen color".
    // TODO: [probably] make an XML switch to use system or HTML-type color 
    //wcex.hbrBackground	= (HBRUSH)(COLOR_APPWORKSPACE + 1);
    //wcex.hbrBackground	= (HBRUSH)(g_nBackgroundColor + 1);
    wcex.hbrBackground = (HBRUSH)CreateSolidBrush(g_clrBackgroundColor); 
    
    wcex.lpszMenuName	= (LPCTSTR)IDC_TYPINGWIN32;
    wcex.lpszClassName	= szWindowClass;
    wcex.hIconSm		= NULL; // when NULL, icon from hIcon member is used.
                                // LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL)

    return RegisterClassEx(&wcex);
}

BOOL App_InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    if (g_bDebug) App_Debug_FunctionCallTrace("App_InitInstance Entered");

    HWND hWnd;
    RECT rc;
    HMENU hMenu, hSubMenu, hSubMenu2;
    //short nScreenWidth, nScreenHeight;
    short nClientWidth = 600; // no effect
    short nClientHeight = 820; // no effect
    HANDLE hThread;
    UINT iThreadID;

    // Store instance handle in our global variable.
    g_hInst = hInstance; 

    g_nScreenWidth  = ::GetSystemMetrics(SM_CXSCREEN); 
    g_nScreenHeight = ::GetSystemMetrics(SM_CYSCREEN);

    if (g_nScreenHeight != 768 /* notebook's default */)
    {
        g_nClientHeight = g_nMode34_ClientHeight_Base;
    }
    else
    {
        g_nClientHeight = g_nMode34_ClientHeight_1024x768;
    }

    switch (g_nMode)
    {
        case 1:
            nClientWidth = 900;
            break;
        case 4:
            nClientWidth = 560;
            break;
    }

    rc.left = (g_nScreenWidth - nClientWidth)/2;
    rc.top = (g_nScreenHeight - nClientHeight)/2;
    
    hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        rc.left, rc.top, nClientWidth, nClientHeight, NULL, NULL, hInstance, 
                                                                          NULL);
    
    if (!hWnd)
    {
        return FALSE;
    }

    g_hWnd = hWnd;
   
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    //InitCommonControls();
    // TODO:  Works with InitCommonControls ...
    INITCOMMONCONTROLSEX icex;   
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC  = ICC_BAR_CLASSES;
    InitCommonControlsEx(&icex);

    // Set application font.
    App_SetFont();

    // Add toolbar.
    App_CreateToolBar();

    // Add status bar.
    App_CreateStatusBar();

    // Set menu language and check state.
    App_SetMenuState();

    switch (g_nMode)
    {
    case 1:
        OnMenuMode12(1 /* LH/RH mode */);
        break;
    case 2:
        OnMenuMode12(2 /* LTOR/RTOL mode */);
        break;
    case 3:
        OnMenuMode3();
        break;
    case 4:
        OnMenuMode4();
        break;
    }

    // Add dynamic language menu: its menu items appended based on existing 
    // (EN/FR/RU) folders in Rational Typist root directory.
    App_AddLanguageMenu();

    // Check language menu item (English/French or Russian) depending on the
    // current language.
    hMenu = GetMenu(g_hWnd);
    hSubMenu = GetSubMenu(hMenu, 1 /* View */);
    hSubMenu2 = GetSubMenu(hSubMenu, 2 /* Language */);

    switch (g_nLangID)
    {
    case 1:
        ModifyMenu(hSubMenu, ID_LANGUAGE_ENGLISH, 
            MF_POPUP | MF_BYCOMMAND | MF_CHECKED, (UINT_PTR)hSubMenu2,
             App_GetStringFromStringTable(IDS_APP_MENUITEM_LANG_EN, g_nLangID));
        break;
    case 2:
        ModifyMenu(hSubMenu, ID_LANGUAGE_FRENCH, 
            MF_POPUP | MF_BYCOMMAND | MF_CHECKED, (UINT_PTR)hSubMenu2,
             App_GetStringFromStringTable(IDS_APP_MENUITEM_LANG_FR, g_nLangID));
        break;
    case 3:
        ModifyMenu(hSubMenu, ID_LANGUAGE_RUSSIAN, 
            MF_POPUP | MF_BYCOMMAND | MF_CHECKED, (UINT_PTR)hSubMenu2, 
             App_GetStringFromStringTable(IDS_APP_MENUITEM_LANG_RU, g_nLangID));
        break;
    default:
        ModifyMenu(hSubMenu, ID_LANGUAGE_ENGLISH, 
            MF_POPUP | MF_BYCOMMAND | MF_CHECKED, (UINT_PTR)hSubMenu2,
             App_GetStringFromStringTable(IDS_APP_MENUITEM_LANG_EN, g_nLangID));
    }

    // Place URL logo on toolbar. When internet connection is not available the
    // logo info is shown from cached Logo_Updates.xml if the file is available
    // (nothing is shown if not). When internet connection is available, at 
    // first, logo info is shown from cached file (if avilable) and then
    // RT downloads Logo_Updates.xml from site to cache and shows update. All
    // operations are done separate thread e.g. even if computer is not 
    // connected to the internet the internet connection check is made in 
    // separate thread so that RT startup would not be slowed down.
    if (g_bShowURLLogo)
    {
        // Get g_nLeftIndent value from Logo_Updates.xml (if avilable)
        // before creating the control for logo info. App_CreateLogo() is
        // not placed into separate thread in order to keep all UI creation
        // in one thread.
        if (App_IsCachedLogoUpdatesFileExists())
        {
            // TODO: make FN global (calculate once!).
            char szCurDirectory[MAX_PATH];
            char szFullFileName[MAX_PATH];  // FFN for Logo_Updates.xml
        
            // Make full file name for Logo_Updates.xml. 
            #if !defined(DEVELOPMENT_STRUCTURE)
                // Product release settings. Obtain path to current directory.
                GetCurrentDirectoryA(MAX_PATH, szCurDirectory);
                strcpy(szFullFileName, szCurDirectory);
                strcat(szFullFileName, "\\Logo_Updates.xml");
            #else
                // Construct Settings.xml path using development directory.
                strcpy(szFullFileName, APP_DIRECTORY_ANSI);  
                strcat(szFullFileName, "\\Logo_Updates.xml");
            #endif

            g_nLogoLeftIndent = atoi(App_XMLReadValue("LeftIndent", 
                                                       szFullFileName).c_str());
        }
        App_CreateLogo();
        
        // Thread auto-terminates when thread function returns. 
        hThread = (HANDLE)_beginthreadex(NULL, 0, 
                                 &App_ThreadProc_ShowLogo, NULL, 0, &iThreadID);
        if (hThread != NULL) CloseHandle(hThread);
    }
    
    if (g_bDebug) App_Debug_FunctionCallTrace("App_InitInstance Exited");

    return TRUE;
}

void App_SetFont()
{
    LOGFONT lf;

    // Specify the font to use (stored in a LOGFONT structure).
    lf.lfHeight = 16;
    lf.lfWidth = 0;
    lf.lfEscapement = 0;
    lf.lfOrientation = 0;
    lf.lfWeight = FW_DONTCARE;
    lf.lfItalic = FALSE;
    lf.lfUnderline = FALSE;
    lf.lfStrikeOut = FALSE;
    lf.lfCharSet = DEFAULT_CHARSET;
    lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
    lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
    lf.lfQuality = DEFAULT_QUALITY;
    lf.lfPitchAndFamily = FF_DONTCARE;
    _tcsncpy(lf.lfFaceName, _T("Arial") /*_T("MS Shell Dlg")*/, 32);

    g_hfnt = CreateFontIndirect(&lf); 
}

// Based on MSDN'03 sample code.
void App_CreateToolBar()
{
    TBBUTTON tbb[5];
    //HWND g_hwndTB;
    int iButton0, iButton1, iButton2, iButton3, iButton4;  
    TBADDBITMAP tb;

    // Create toolbar window with tooltips style enabled (tooltips handled in 
    // WM_NOTIFY).
    g_hwndTB = CreateWindowEx(0, TOOLBARCLASSNAME, (LPTSTR) NULL, 
        WS_CHILD | WS_VISIBLE | TBSTYLE_TOOLTIPS, 0, 0, 0, 0, g_hWnd, 
                                              (HMENU)ID_TOOLBAR, g_hInst, NULL); 

    //--
    g_wpOldToolbarWndProc = (WNDPROC)SetWindowLong(g_hwndTB, 
                                GWL_WNDPROC, (DWORD)App_Toolbar_SubclassedFunc);
    //--

    tb.hInst = g_hInst;
    tb.nID =  IDB_TOOLBAR_16x15xN;

    SendMessage(g_hwndTB, TB_ADDBITMAP, 8 /* number of images in the bitmap */, 
                                                                   (LPARAM)&tb);

    SendMessage(g_hwndTB, TB_BUTTONSTRUCTSIZE, (WPARAM) sizeof(TBBUTTON), 0); 

    // I use intermediate step to copy localized string from 
    // App_GetStringFromStringTable to buffer instead of direct use as a
    // SendMessage's 4th parameter since the latter lead to run-time failure in 
    // release mode (OK in Debug mode). Also, MSDN says that "the last string in
    // the array must be terminated with two null characters.", so I add one 
    // more NULL (albeit it works OK without it).
    _tcscpy(g_szBuffer, App_GetStringFromStringTable(IDS_FILE_OPEN, g_nLangID));
    _tcscat(g_szBuffer, _T("\0"));

    iButton0 = SendMessage(g_hwndTB, TB_ADDSTRING, 0, (LPARAM)g_szBuffer);

    _tcscpy(g_szBuffer, App_GetStringFromStringTable(IDS_START_OR_STOP_TEST, 
                                                                    g_nLangID));
    _tcscat(g_szBuffer, _T("\0"));

    iButton1 = SendMessage(g_hwndTB, TB_ADDSTRING, 0, (LPARAM)g_szBuffer);

    _tcscpy(g_szBuffer, 
          App_GetStringFromStringTable(IDS_TYPING_ERROR_ALERT_TYPE, g_nLangID));
    _tcscat(g_szBuffer, _T("\0"));

    iButton2 = SendMessage(g_hwndTB, TB_ADDSTRING, 0, (LPARAM)g_szBuffer);

    _tcscpy(g_szBuffer, App_GetStringFromStringTable(IDS_CLEAR_TEST_AREA, 
                                                                    g_nLangID));
    _tcscat(g_szBuffer, _T("\0"));

    iButton3 = SendMessage(g_hwndTB, TB_ADDSTRING, 0, (LPARAM)g_szBuffer);

    _tcscpy(g_szBuffer, App_GetStringFromStringTable(IDS_DETAILED_STATS, 
                                                                    g_nLangID));
    _tcscat(g_szBuffer, _T("\0"));
    iButton4 = SendMessage(g_hwndTB, TB_ADDSTRING, 0, (LPARAM)g_szBuffer);
    
    // MSDN'03: If an application specifies labels in the iString member of the 
    // associated TBBUTTON structure, the toolbar control automatically uses 
    // that string as the ToolTip.
    tbb[0].iBitmap = 0; //I_IMAGENONE; 
    tbb[0].idCommand = ID_FILE_OPEN; //IDS_CUT; 
    tbb[0].fsState = TBSTATE_ENABLED; 
    tbb[0].fsStyle = BTNS_BUTTON; 
    tbb[0].dwData = 0; 
    tbb[0].iString = iButton0; 

    tbb[1].iBitmap = 1; //I_IMAGENONE; 
    tbb[1].idCommand = ID_START_OR_STOP_TEST; //IDS_COPY; 
    tbb[1].fsState = TBSTATE_ENABLED; 
    tbb[1].fsStyle = BTNS_BUTTON; 
    tbb[1].dwData = 0; 
    tbb[1].iString = iButton1; 

    tbb[2].iBitmap = 2;// I_IMAGENONE; 
    tbb[2].idCommand = ID_TYPING_ERROR_ALERT_TYPE; //IDS_PASTE; 
    tbb[2].fsState = TBSTATE_ENABLED; 
    tbb[2].fsStyle = BTNS_BUTTON; 
    tbb[2].dwData = 0; 
    tbb[2].iString = iButton2; 

    tbb[3].iBitmap = 3;// I_IMAGENONE; 
    tbb[3].idCommand = ID_CLEAR_TEST_AREA; //IDS_PASTE; 
    tbb[3].fsState = TBSTATE_INDETERMINATE; 
    tbb[3].fsStyle = BTNS_BUTTON; 
    tbb[3].dwData = 0; 
    tbb[3].iString = iButton3; 

    tbb[4].iBitmap = 4;// I_IMAGENONE; 
    tbb[4].idCommand = ID_DETAILED_STATS; //IDS_PASTE; 
    tbb[4].fsState = TBSTATE_INDETERMINATE;  
    tbb[4].fsStyle = BTNS_BUTTON; 
    tbb[4].dwData = 0; 
    tbb[4].iString = iButton4; 
    
    SendMessage(g_hwndTB, TB_SETMAXTEXTROWS, 
        0 /* use tooltips as strings from TBBUTTON structure */, 
                                                          0 /* must be zero */);
    SendMessage(g_hwndTB, TB_ADDBUTTONS, (WPARAM) 5 /* buttons number */, 
                                                     (LPARAM)(LPTBBUTTON) &tbb);
    SendMessage(g_hwndTB, TB_AUTOSIZE, 0, 0); 
    ShowWindow(g_hwndTB, SW_SHOW); // when missed, toolbar not shown

    //App_CreateLogo();
}

// Based on MSDN'03 sample code.
void App_CreateStatusBar()
{
    RECT rcClient;
    HLOCAL hloc;
    LPINT lpParts;
    int nParts = 2;

    g_hwndStatus = CreateWindowEx(0 /* no extended styles */,                       
        STATUSCLASSNAME, (LPCTSTR) NULL/* no text */,
        SBARS_SIZEGRIP | WS_CHILD, 0, 0, 0, 0 /* ignore size and position*/,
        g_hWnd /* parent */, (HMENU) 334 /* child window identifier */, g_hInst,      
                                            NULL /* no window creation data */);                   
    
    GetClientRect(g_hWnd, &rcClient);

    // Allocate an array for holding the right edge coordinates.
    hloc = LocalAlloc(LHND, sizeof(int) * nParts);
    lpParts = (LPINT)LocalLock(hloc);

    // The status bar has nParts. Specify the right edges of the corresponding 
    // parts.
    lpParts[0] = 500;
    lpParts[1] = rcClient.right; 

    // Tell the status bar to create the window parts.
    SendMessage(g_hwndStatus, SB_SETPARTS, (WPARAM) nParts, (LPARAM)lpParts);

    // Free the array, and return.
    LocalUnlock(hloc);
    LocalFree(hloc);

    ShowWindow(g_hwndStatus, SW_SHOW);
}

LRESULT CALLBACK App_WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hdc;
    HRESULT lRes; 
    //TCHAR szPair[3]; 
    TCHAR szBuffer[1000];
    wstring sBuffer, sPair, sEdit; 
    int nPos;
    HMENU hMenu, hSubMenu;  
    TCHAR szMenuString[100];
    HWND hwndTB ;
    TBBUTTONINFO tbbi;

    switch (message) 
    {
    case WM_CREATE: // 1st WM_CREATE, then App_InitInstance call

        break;
    case WM_PAINT:
        //hdc = BeginPaint(hWnd, &ps);
        //// TODO: Add any drawing code here...
        //EndPaint(hWnd, &ps);

        //break;

        {
            hdc = BeginPaint(hWnd, &ps);
            //if (g_nMode == 4)
            if (g_nMode == 4 && g_bTestCompleted)
            {
                HBRUSH hBrush = CreateSolidBrush(g_clrMode4_BS_BackgroundColor);
                HBRUSH hBrushTemp =  (HBRUSH)SelectObject(hdc, hBrush);
                RoundRect(hdc, 555 /* left */, 120 /* top */, 770 /* right */, 
                    305 /* bottom */, 40 /* ellipse width */, 
                                                        40 /* ellipse height */);
                SelectObject(hdc, hBrushTemp);
                DeleteObject(hBrush); DeleteObject(hBrushTemp);
            }
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_CTLCOLORSTATIC:

        if (g_nMode == 4)
        {
            // Change static controls background color.
            HWND hwndStatic = (HWND)lParam;

            if (g_hwndMode4_Static_TimeSpent == hwndStatic || 
                g_hwndMode4_Static_TimeSpent_Value == hwndStatic || 
                g_hwndMode4_Static_CPM == hwndStatic ||
                g_hwndMode4_Static_CPM_Value == hwndStatic ||
                g_hwndMode4_Static_CharsTyped == hwndStatic ||
                g_hwndMode4_Static_CharsTyped_Value == hwndStatic ||
                g_hwndMode4_Static_WordsTyped == hwndStatic ||
                g_hwndMode4_Static_WordsTyped_Value == hwndStatic ||
                g_hwndMode4_Static_WPM == hwndStatic ||
                g_hwndMode4_Static_WPM_Value == hwndStatic ||
                g_hwndMode4_Static_TotalErrors == hwndStatic ||
                             g_hwndMode4_Static_TotalErrors_Value == hwndStatic)
            {
                //SetTextColor((HDC)wParam /* handle to display context */, 
                //	                                              RGB(0,0,255));
                // Background is not changed when drawing a text.
                SetBkMode((HDC)wParam /* handle to display context */, 
                                                                   TRANSPARENT); 
                // If the dialog box procedure returns FALSE, then 
                // default message handling is performed.
                return (BOOL)GetStockObject(HOLLOW_BRUSH); 
            }
        }

        break;
    case WM_SIZE:
        // For some reason resizing toolbar with g_hwndTB does not work, so I
        // obtain toolbar's handle from its control ID (as in Win32 book).
        hwndTB = GetDlgItem (hWnd, ID_TOOLBAR) ;
        SendMessage(hwndTB, TB_AUTOSIZE, 0, 0);

        //ShowWindow(g_hwndURLLogo, SW_HIDE);

        // This adjusts status bar to the bottom (from MConsole). TODO: figure 
        // out the meaning of 500, 100.
        SendMessage(g_hwndStatus, WM_SIZE, (WPARAM) 500, (LPARAM) 100); 
        break;
    case WM_MOVE:

        // Re-position detailed stats windows as main window moves.
        if (g_hWndMode4_DetailedStats != NULL)
        {
            RECT rc; int nDSHeight;
            GetWindowRect(g_hWnd, &rc);
            if (g_nScreenHeight != 768)
            {
                nDSHeight = g_rcMode4_DS_Base.bottom - g_rcMode4_DS_Base.top;
            }
            else
            {
                nDSHeight = g_rcMode4_DS_1024x768.bottom - 
                                                      g_rcMode4_DS_1024x768.top;
            }

            SetWindowPos(g_hWndMode4_DetailedStats, NULL, 
                //rc.right, rc.top + 77, 325, 703,
                           rc.right, rc.top + 77, 325, nDSHeight, SWP_NOZORDER);
        }

        break;
    case WM_CONTEXTMENU:
        //if(wParam == g_hwndListBox2) MessageBox(0, "WM_CONTEXTMENU", "wParam", 0);
        break;
    case WM_NOTIFY:
        if (g_nMode == 3)
                      return Mode3_NotifyHandler(hWnd, message, wParam, lParam);
        break;
    case WM_CHAR:
        //MessageBox(0, _T("WM_CHAR"), _T(""), 0); // not sent if focus on edit
        break;
    case WM_COMMAND:
        // Parse the menu selections:
        switch (LOWORD(wParam))
        {
        case IDC_STATIC_URL_LOGO:
            //MessageBox(0, L"asdsadsa", L"asdasdas", 0);
            ShellExecute(NULL, _T("open"), 
                         _T("http://www.carexplorer.org"), 0, 0, SW_SHOWNORMAL);
            break;
        case IDM_FILE_OPEN:
            Mode4_OnFileOpen();
            break;
        case ID_FILE_OPEN:
            Mode4_OnFileOpen();
            break;
        case IDM_VIEW_TOOLBAR:
            hMenu = GetMenu(g_hWnd);
            hSubMenu = GetSubMenu(hMenu, 1);
            // TODO: g_hwndTB does not work, g_hwndStatus - OK.
            hwndTB = GetDlgItem(hWnd, ID_TOOLBAR);
            if (IsWindowVisible(hwndTB))
            {
                ShowWindow(hwndTB, SW_HIDE); 
                CheckMenuItem(hSubMenu, IDM_VIEW_TOOLBAR, 
                                                   MF_BYCOMMAND | MF_UNCHECKED);
            }
            else
            {
                ShowWindow(hwndTB, SW_SHOW);
                CheckMenuItem(hSubMenu, IDM_VIEW_TOOLBAR, 
                                                     MF_BYCOMMAND | MF_CHECKED); 
            }

            break;
        case IDM_VIEW_STATUSBAR:
            hMenu = GetMenu(g_hWnd);
            hSubMenu = GetSubMenu(hMenu, 1);
            
            if (IsWindowVisible(g_hwndStatus))
            {
                ShowWindow(g_hwndStatus, SW_HIDE);
                CheckMenuItem(hSubMenu, IDM_VIEW_STATUSBAR, 
                                                   MF_BYCOMMAND | MF_UNCHECKED);
            }
            else
            {
                ShowWindow(g_hwndStatus, SW_SHOW);
                CheckMenuItem(hSubMenu, IDM_VIEW_STATUSBAR, 
                                                     MF_BYCOMMAND | MF_CHECKED); 
            }

            break;
        case ID_START_OR_STOP_TEST:

            // For some reason SendMessage() with g_hwndTB does not work, so I
            // obtain toolbar's handle from its control ID (as in Win32 book).
            hwndTB = GetDlgItem(hWnd, ID_TOOLBAR) ;
            hMenu = GetMenu(g_hWnd);
            hSubMenu = GetSubMenu(hMenu, 3);

            // cbSize & dwMask must be set before sending TB_GETBUTTONINFO 
            // message.
            tbbi.cbSize = sizeof(tbbi);
            tbbi.dwMask = TBIF_IMAGE;
            
            SendMessage(hwndTB, TB_GETBUTTONINFO, ID_START_OR_STOP_TEST, 
                                                                 (LPARAM)&tbbi);
            
            if (tbbi.iImage == 1 /* start test image */) // ready for test
            {
                SendMessage(hwndTB, TB_CHANGEBITMAP, 
                                       ID_START_OR_STOP_TEST, MAKELPARAM(5, 0));
                SendMessage(hwndTB, TB_SETSTATE, ID_FILE_OPEN, 
                                                         TBSTATE_INDETERMINATE);
                Mode4_OnStartTest();
                g_bTestRunning = TRUE;

                // Show the menu item as Stop Test.
                _tcscpy(g_szBuffer, 
                    App_GetStringFromStringTable(IDS_TEST_MODE_MENU_STOP_TEST, 
                                                                    g_nLangID));
                ModifyMenu(hSubMenu, IDM_COMMANDS_START_OR_STOP_TEST, 
                     MF_BYCOMMAND, IDM_COMMANDS_START_OR_STOP_TEST, g_szBuffer);
            }
            else if (tbbi.iImage == 5 /* stop test image */) // test is running
            {
                SendMessage(hwndTB, TB_CHANGEBITMAP, ID_START_OR_STOP_TEST,
                                                              MAKELPARAM(1, 0));
                SendMessage(hwndTB, TB_SETSTATE, ID_FILE_OPEN, 
                                                              TBSTATE_ENABLED );
                if (Mode4_IsTestCompleted())
                    SendMessage(hwndTB, TB_SETSTATE, ID_DETAILED_STATS, 
                                                              TBSTATE_ENABLED );
                Mode4_OnStartTest();
                g_bTestRunning = FALSE;

                // Show the menu item as Start Test.
                _tcscpy(g_szBuffer, 
                    App_GetStringFromStringTable(IDS_TEST_MODE_MENU_START_TEST, 
                                                                    g_nLangID));
                ModifyMenu(hSubMenu, IDM_COMMANDS_START_OR_STOP_TEST, 
                     MF_BYCOMMAND, IDM_COMMANDS_START_OR_STOP_TEST, g_szBuffer);
            }
                                                 
            break;
        case ID_TYPING_ERROR_ALERT_TYPE:

            // For some reason SendMessage() with g_hwndTB does not work, so I
            // obtain toolbar's handle from its control ID (as in Win32 book).
            hwndTB = GetDlgItem(hWnd, ID_TOOLBAR) ;
            hMenu = GetMenu(g_hWnd);
            hSubMenu = GetSubMenu(hMenu, 3);

            // cbSize & dwMask must be set before sending TB_GETBUTTONINFO 
            // message.
            tbbi.cbSize = sizeof(tbbi);
            tbbi.dwMask = TBIF_IMAGE;
            
            SendMessage(hwndTB, TB_GETBUTTONINFO, ID_TYPING_ERROR_ALERT_TYPE, 
                                                                 (LPARAM)&tbbi);

            if (tbbi.iImage == 2 /* zero-based */) // sounds enabled image
            {
                SendMessage(hwndTB, TB_CHANGEBITMAP, 
                                  ID_TYPING_ERROR_ALERT_TYPE, MAKELPARAM(7, 0));
                g_bAlertEnabled = FALSE;

                _tcscpy(g_szBuffer, 
                    App_GetStringFromStringTable(
                                   IDS_TEST_MODE_MENU_ENABLE_ALERT, g_nLangID));			
                ModifyMenu(hSubMenu, IDM_COMMANDS_ALERT_STATE, 
                            MF_BYCOMMAND, IDM_COMMANDS_ALERT_STATE, g_szBuffer);
                                                    //_T("Enable Sound Alert"));
            }
            else if (tbbi.iImage == 7) // sounds disabled image 
            {
                SendMessage(hwndTB, TB_CHANGEBITMAP, ID_TYPING_ERROR_ALERT_TYPE,
                                                              MAKELPARAM(2, 0));
                g_bAlertEnabled = TRUE;
                
                _tcscpy(g_szBuffer, 
                    App_GetStringFromStringTable(
                                  IDS_TEST_MODE_MENU_DISABLE_ALERT, g_nLangID));
                ModifyMenu(hSubMenu, IDM_COMMANDS_ALERT_STATE, 
                            MF_BYCOMMAND, IDM_COMMANDS_ALERT_STATE, g_szBuffer);
                                                   //_T("Disable Sound Alert"));
            }

            break;
        case ID_CLEAR_TEST_AREA: 
            hwndTB = GetDlgItem(g_hWnd, ID_TOOLBAR);
            SendMessage(hwndTB, TB_SETSTATE, ID_CLEAR_TEST_AREA, 
                                                         TBSTATE_INDETERMINATE);
            hMenu = GetMenu(g_hWnd);
            hSubMenu = GetSubMenu(hMenu, 3);
            EnableMenuItem(hSubMenu, IDM_COMMANDS_CLEAR_TEST_AREA, 
                                                      MF_BYCOMMAND | MF_GRAYED);
            // Send empty string (NULL value) to clear the edit control.
            if (g_nMode == 1  || g_nMode == 2)
                SendMessage(g_hwndEdit, WM_SETTEXT, 0 /* not used */, NULL);
            else if (g_nMode == 3)
                SendMessage(g_hwndMode3_Edit, WM_SETTEXT, 0 /* not used */, 
                                                                          NULL);
            else if (g_nMode == 4)
            {
                g_nCharsTyped = 0; 
                SendMessage(g_hwndEdit, WM_SETTEXT, 0 /* not used */, NULL);
                // Disable also, for consistency, statistics toolbar icon and 
                // menu item (makes sense with after closing of DS window).
                SendMessage(hwndTB, TB_SETSTATE, ID_DETAILED_STATS, 
                                                         TBSTATE_INDETERMINATE);
                EnableMenuItem(hSubMenu, IDM_COMMANDS_DETAILED_STATS, 
                                                      MF_BYCOMMAND | MF_GRAYED);
            }
            
            break;
        case ID_DETAILED_STATS:
            {
                // Show/close detailed statistics window based on previous 
                // state.
                hwndTB = GetDlgItem(g_hWnd, ID_TOOLBAR) ;

                // Update detailed stats toolbar's image based on previous 
                // image. cbSize & dwMask must be set before sending 
                // TB_GETBUTTONINFO message.
                tbbi.cbSize = sizeof(tbbi);
                tbbi.dwMask = TBIF_IMAGE;

                SendMessage(hwndTB, TB_GETBUTTONINFO, ID_DETAILED_STATS, 
                                                                 (LPARAM)&tbbi);
                // Get handle to commands submenu.
                hMenu = GetMenu(g_hWnd);
                hSubMenu = GetSubMenu(hMenu, 3 /* Commands menu*/);

                if (tbbi.iImage == 4) // show detailed stats image ("expand")
                {
                    SendMessage(hwndTB, TB_CHANGEBITMAP, ID_DETAILED_STATS,
                                                              MAKELPARAM(6, 0));

                    EnableMenuItem(hSubMenu, IDM_COMMANDS_DETAILED_STATS, 
                                                      MF_BYCOMMAND | MF_GRAYED);
                    Mode4_OnDetailedStats();
                }
                else if (tbbi.iImage == 6) // close ("contract") image 
                {
                    SendMessage(hwndTB, TB_CHANGEBITMAP, ID_DETAILED_STATS,
                                                              MAKELPARAM(4, 0));
                    DestroyWindow(g_hWndMode4_DetailedStats);

                    EnableMenuItem(hSubMenu, IDM_COMMANDS_DETAILED_STATS, 
                                                     MF_BYCOMMAND | MF_ENABLED);
                }
            }
            
            break;
        case ID_LANGUAGE_ENGLISH:
            App_OnChangeLanguage(1);
            break;
        case ID_LANGUAGE_FRENCH:
            App_OnChangeLanguage(2);
            break;
        case ID_LANGUAGE_RUSSIAN:
            App_OnChangeLanguage(3);
            break;
        case IDC_EDIT_MAIN:
            //MessageBox(0, "IDC_EDIT_MAIN", "", 0);
            //switch (wmEvent)
            //{
                //case BN_CLICKED:
                    //MessageBox(0, "IDC_EDIT_MAIN", "", 0);
                    //break;
                
            //}
            break;
        case IDC_LISTBOX_FASTEST:
            switch(HIWORD(wParam))
            {
            case LBN_SELCHANGE: 
                HRESULT lRes = SendMessage(g_hwndMode1_ListBox_Fastest, 
                              LB_GETCURSEL, 0 /* not used */, 0 /* not used */);
                SendMessage(g_hwndMode1_ListBox_Fastest, LB_GETTEXT, lRes, 
                                                      (LPARAM)(LPTSTR)szBuffer);
                MessageBox(0, _T("IDC_LISTBOX_FASTEST"), szBuffer, 0);
                break;
            }
            break;
        case IDC_LISTBOX_SLOWEST:
            switch(HIWORD(wParam))
            {
            case LBN_SELCHANGE: 
                lRes = SendMessage(g_hwndMode1_ListBox_Slowest, LB_GETCURSEL, 
                                            0 /* not used */, 0 /* not used */);
                SendMessage(g_hwndMode1_ListBox_Slowest, LB_GETTEXT, lRes, 
                                                              (LPARAM)szBuffer); 
                sBuffer = szBuffer; 
                sPair = sBuffer.substr(0, 2);

                SendMessage(g_hwndEdit, WM_GETTEXT, 1000, (LPARAM)szBuffer);
                sEdit = szBuffer; 

                // Colorize all found character pairs in the edit control.
                nPos = sEdit.find(sPair);
                if(nPos != -1) 
                {
                    SelectPairs((LPTSTR)sPair.c_str(), nPos);

                    nPos = sEdit.find(sPair, nPos + 2);
                    while(nPos != -1)
                    {						
                        wsprintf(szBuffer, _T("%d"), nPos);
                        SelectPairs((LPTSTR)sPair.c_str(), nPos);
                        nPos = sEdit.find(sPair, nPos + 2);
                    }
                }

                break;
            }
            break;
        case IDC_BUTTON_REPOPULATE:
            switch (HIWORD(wParam))
            {
                case BN_CLICKED:
                    OnRepopulateButtonClicked();
                    break;
            }
            break;
        case IDC_BUTTON_TEST:
            Mode4_OnResize(FALSE);
            break;
        case IDC_BUTTON_TEST2:
            Mode4_OnStartTest();
            break;
        //case IDM_VIEW_OPTIONS:
        //	App_ConfigureOptions();
        //	break;
        case IDM_COMMANDS_START_OR_STOP_TEST:
            // Delegate to the toolbar's handler. Note: when handled by  
            // App_WndProc, ID obtained as low-word of wParam (works, 
            // nevertheless, without MAKEWPARAM also).
            SendMessage(g_hWnd, WM_COMMAND, 
                MAKEWPARAM(ID_START_OR_STOP_TEST /* low word */, 
                                                         0 /* high word */), 0);
            break;
        case IDM_COMMANDS_ALERT_STATE:
            // Delegate to the toolbar's handler. Note: when handled by  
            // App_WndProc, ID obtained as low-word of wParam (works, 
            // nevertheless, without MAKEWPARAM also).
            SendMessage(g_hWnd, WM_COMMAND, 
                MAKEWPARAM(ID_TYPING_ERROR_ALERT_TYPE /* low word */, 
                                                         0 /* high word */), 0);
            break;
        case IDM_COMMANDS_CLEAR_TEST_AREA:
            // Delegate to the toolbar's handler. Note: when handled by  
            // App_WndProc, ID obtained as low-word of wParam (works, 
            // nevertheless, without MAKEWPARAM also).
            SendMessage(g_hWnd, WM_COMMAND, 
                MAKEWPARAM(ID_CLEAR_TEST_AREA /* low word */, 
                                                         0 /* high word */), 0);
            break;
        case IDM_COMMANDS_DETAILED_STATS:
            // Delegate to the toolbar's handler. Note: when handled by  
            // App_WndProc, ID obtained as low-word of wParam (works, 
            // nevertheless, without MAKEWPARAM also).
            SendMessage(g_hWnd, WM_COMMAND, 
                MAKEWPARAM(ID_DETAILED_STATS /* low word */, 
                                                         0 /* high word */), 0);
            break;
        case IDM_COMMANDS_SHOW_REPORT:
            Mode4_CreateHtmlReport();
            break;
        case IDM_RESIZE: // TODO: probably remove (likely ot used now).

            hMenu = GetMenu(g_hWnd);
            hSubMenu = GetSubMenu(hMenu, 3);

            GetMenuString(hSubMenu, IDM_RESIZE, szMenuString, 100, 
                                                                  MF_BYCOMMAND);

            if (_tcscmp(szMenuString, _T("Expand")) == 0)
            {
                Mode4_OnResize(TRUE /* expand */);
                //AppendMenu(hSubMenu, MF_STRING, IDM_DETAILED_STATS, 
                //	                                      _T("Detailed Stats"));

            }
            else
            {
                Mode4_OnResize(FALSE /* contract */);
                //DeleteMenu(hSubMenu, IDM_DETAILED_STATS, MF_BYCOMMAND);
            }

            break;
        //case IDC_TEST_MODE_BUTTON_CUSTOM_LIST:
        //	Mode4_CreateCustomList();
  //          break;
  //      case IDC_TEST_MODE_BUTTON_CHECK_ALL:
        //	Mode4_CheckAll();
  //          break;
        case IDC_BUTTON_SHOW_LOG:
            OnButtonShowLogClicked();
               break;
        case IDM_MODES_LH_RH_WORDS:
            // Removes dynamic menyu item created in test mode. If the item does
            // not exit the call harlessly fails.
            OnMenuMode12(1 /* LH/RH mode */);
            break;
        case IDM_MODES_LH_RH_WORDS_BOTH_HANDS:
            OnMenuMode12(2 /* LTOR/RTOL mode */);
            break;
        case IDM_MODES_CUSTOM:
            OnMenuMode3();
            break;
        case IDM_MODES_TEST:
            OnMenuMode4();
            break;
        case IDM_HELP_KEYBOARD_LAYOUT:
            App_MenuHandler_ShowOnlineHelp(L"Hands_Positioning.htm");
           break;
        case IDM_HELP_ONLINE:
            App_MenuHandler_ShowOnlineHelp(L"RT_Help.htm");
            break;
        case IDM_HELP_MSB:
        {
            TCHAR szMsg[100];

            // Make a warning message when internet connection is not available. 
            lstrcpy(szMsg, L"There was a problem while connecting to the ");
            lstrcat(szMsg, L"internet! Please check connection settings and ");
            lstrcat(szMsg, L"try again ...");

            // Very slow (~40 sec), but works.
            if (!App_CheckInternetConnection(
                                 L"http://www.google.com" /* any major site */))  	
            {
                MessageBox(0, szMsg, L"Rational Typist", MB_ICONINFORMATION);
                return FALSE;
            }

            // Check the availability of base site URL.
            if (App_IsBaseHelpURLLinkAvailable(L"http://www.carexplorer.org"))
            {
                ShellExecute(NULL, L"open", 
                            L"http://www.carexplorer.org", 0, 0, SW_SHOWNORMAL);
            }
            else // use always alternative site URL if the MSB site is down
            {
                ShellExecute(NULL, L"open", 
                             L"http://netston.tripod.com", 0, 0, SW_SHOWNORMAL);
            }
            break;
        }
        case IDM_ABOUT:
            DialogBox(g_hInst, (LPCTSTR)IDD_ABOUTBOX, hWnd, (DLGPROC)App_About);
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
        break;
    case WM_DESTROY:

        //MessageBox(0, L"1", L"WM_DESTROY", 0);
        App_OnSaveSettings();

        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0; // actually, should never be here to provide default processing
              // with DefWindowProc (zero is default, provided by application 
              // template) 
}

void App_OnSaveSettings()
{
    char szTagValue[MAX_PATH];
    BOOL bWriteFFN = FALSE;
    TCHAR szCurDir[MAX_PATH];
    TCHAR szSamplesDir[MAX_PATH];

    // Persist to XML file.
    //App_XMLWriteValue("SaveLastUsed", g_bSaveLastUsed ? "1" : "0");

    // Save values if persistence flag is set.
    if (g_bSaveLastUsed)
    {
        // Save a value only if it were changed.
        if (g_nMode != g_nInitialMode)
        {
            sprintf(szTagValue, "%d", g_nMode); 
            App_XMLWriteValue("Mode", szTagValue);
        }

        if (g_nLangID != g_nInitialLangID)
        {
            sprintf(szTagValue, "%d", g_nLangID);
            App_XMLWriteValue("Language", szTagValue);
        }

        //if (g_nLangID != g_nInitialLangID)
        if (g_sSampleFFN.size() != 0)
        {
            // In the most cases (99%) the test is used from the default folder(s) 
            // for tests depending on language used. This mean it is redundant, 
            // in most cases, to store FFN in Settings.xml and storing a file name
            // would be enough. The price for that is the following code that 
            // enables to write to XML file name or FFN depending on the location 
            // of test file. The g_sSampleFFN is always FFN, so 1) get directory 
            // (and file name) from FFN and figure if it coincides with install 
            // directory plus path for test file; 2) if coincides, store only 
            // file name, if not - FFN.

            GetCurrentDirectory(MAX_PATH, szCurDir);
            lstrcpy(szSamplesDir, szCurDir);
            switch (g_nLangID)
            {
            case 1:	
                _tcscat(szSamplesDir, L"\\EN\\Tests");
                break;
            case 2:	
                _tcscat(szSamplesDir, L"\\FR\\Tests");
                break;
            case 3:	
                _tcscat(szSamplesDir, L"\\RU\\Tests");
                break;
            default:
                _tcscat(szSamplesDir, L"\\EN\\Tests");
            }

            if (lstrcmp(szSamplesDir, 
               App_GetDirectory(&g_sSampleFFN).c_str()) != 0) bWriteFFN = TRUE;

            if (bWriteFFN)
            {
                WideCharToMultiByte(CP_UTF8, 0 /* must be zero */, 
                    g_sSampleFFN.c_str(), -1, szTagValue, sizeof(szTagValue), 
                              NULL /* must be NULL */, NULL /* must be NULL */);
            }
            else
            {
                WideCharToMultiByte(CP_UTF8, 0 /* must be zero */, 
                    App_GetFileName(&g_sSampleFFN).c_str(), -1, szTagValue, 
                    sizeof(szTagValue), NULL /* must be NULL */, 
                                                       NULL /* must be NULL */);
            }

            switch (g_nLangID)
            {
            case 1:
                App_XMLWriteValue("Sample_Test_EN", szTagValue);
                break;
            case 2:
                App_XMLWriteValue("Sample_Test_FR", szTagValue);
                break;
            case 3:
                App_XMLWriteValue("Sample_Test_RU", szTagValue);
                break;
            }

            if (g_bShowHint1Dialog != g_bInitShowHint1Dialog)
            {
                sprintf(szTagValue, "%d", g_bShowHint1Dialog);
                App_XMLWriteValue("H1D_Show", szTagValue);
            }

            if (g_bShowHint2Dialog != g_bInitShowHint2Dialog)
            {
                sprintf(szTagValue, "%d", g_bShowHint2Dialog);
                App_XMLWriteValue("H2D_Show", szTagValue);
            }
        }
    }
}

// Message handler for about box.
LRESULT CALLBACK App_About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    HFONT hfnt;
    LOGFONT lf;

    // Specify the font to use (stored in a LOGFONT structure).
    lf.lfHeight = 16;
    lf.lfWidth = 0;
    lf.lfEscapement = 0;
    lf.lfOrientation = 0;
    lf.lfWeight = FW_DONTCARE;
    lf.lfItalic = FALSE;
    lf.lfUnderline = FALSE;
    lf.lfStrikeOut = FALSE;
    lf.lfCharSet = DEFAULT_CHARSET;
    lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
    lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
    lf.lfQuality = DEFAULT_QUALITY;
    lf.lfPitchAndFamily = FF_DONTCARE;
    _tcsncpy(lf.lfFaceName, _T("Arial"), 32);

    hfnt = CreateFontIndirect(&lf);

    switch (message)
    {
    case WM_INITDIALOG:

        // Setup cursor for URL.
        g_wpOldAboutStaticWndProc = (WNDPROC)SetWindowLong(GetDlgItem (hDlg, 
              IDC_STATIC_LINK), GWL_WNDPROC, (DWORD)About_StaticSubclassedFunc);
        // Setup font for URL.
        About_SetFont(hDlg);

        // Set font and language-dependent strings for dialo's static controls.
        SendMessage(GetDlgItem(hDlg, IDC_STATIC_PRODUCT), WM_SETFONT, 
                                                            (WPARAM)hfnt, TRUE);
        SendMessage(GetDlgItem(hDlg, IDC_STATIC_COPYRIGHT), WM_SETFONT, 
                                                            (WPARAM)hfnt, TRUE);
        SendMessage(GetDlgItem(hDlg, IDC_STATIC_COPYRIGHT2), WM_SETFONT, 
                                                            (WPARAM)hfnt, TRUE);

        SendMessage(GetDlgItem(hDlg, IDC_STATIC_PRODUCT), WM_SETTEXT, 
            0 /* not used */, 
            (LPARAM)App_GetStringFromStringTable(IDS_ABOUT_STATIC_PRODUCT, 
                                                                    g_nLangID));
        //--
        SendMessage(GetDlgItem(hDlg, IDC_STATIC_COPYRIGHT), WM_SETTEXT, 
            0 /* not used */, 
            (LPARAM)App_GetStringFromStringTable(IDS_ABOUT_STATIC_COPYRIGHT, 
                                                                    g_nLangID));
        SendMessage(GetDlgItem(hDlg, IDC_STATIC_COPYRIGHT2), WM_SETTEXT, 
            0 /* not used */, 
            (LPARAM)App_GetStringFromStringTable(IDS_ABOUT_STATIC_COPYRIGHT2, 
                                                                    g_nLangID));
        //--

        return TRUE;

    case WM_CTLCOLORSTATIC:
        // Change color for URL and make it underlined.
        if (GetDlgItem (hDlg, IDC_STATIC_LINK) == 
                                    (HWND)lParam /* handle to static control */)
        {
            SetTextColor((HDC)wParam /* handle to display context */, 
                                                                  RGB(0,0,255));
            // Background is not changed when drawing a text.
            SetBkMode((HDC)wParam /* handle to display context */, TRANSPARENT); 
            // If the dialog box procedure returns FALSE, then default message 
            // handling is performed.
            return (BOOL)GetStockObject(HOLLOW_BRUSH); 
        }
        break;

    case WM_COMMAND:
        // Handle URL click.
        if (LOWORD(wParam) == IDC_STATIC_LINK)
        {
            ShellExecute(NULL, _T("open"), 
                _T("http://order.kagi.com/?6FBHN&lang=en"), 0, 0, 
                                                                 SW_SHOWNORMAL);
        }

        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) 
        {
            EndDialog(hDlg, LOWORD(wParam));
            return TRUE;
        }
        break;
    }
    return FALSE;
}

// When enered character is space, the function exctracts last entered words, 
// and checks LH/RH lists to see if there is match. If match found, the function
// selects the match in the list box. 
void OnKeyDown(LPCTSTR pszChar)
{
    TCHAR szBuffer[100];
    TCHAR szLastWord[100];
    int nPos, nLen, nRes;

    if (lstrcmp(pszChar, _T(" ")) == 0 /* entered character is space */)
    {
        // MSDN: Before sending the message, set the first word of this 
        // buffer to  the size, in TCHARs, of the buffer. For ANSI text,  
        // this is the number of bytes; for Unicode text, this is the  
        // number of characters. The size in the first word is overwritten  
        // by the copied line.
        *((LPWORD)szBuffer) = (WORD)100;
        nPos = SendMessage(g_hwndEdit, EM_LINEFROMCHAR, 
                                -1 /* use current line */, 0 /* not used */);
        nLen = SendMessage(g_hwndEdit, EM_GETLINE, nPos, (LPARAM)szBuffer);
        // MSDN: The copied line does not contain a terminating null 
        // character.
        szBuffer[nLen] = _T('\0');

        //LPSTR pLW = _tcsrchr(szBuffer, _T(' ')); 
        LPTSTR pLW = _tcsrchr(szBuffer, _T(' ')); 
        
        if (pLW  == NULL)
        {
            lstrcpy(szLastWord, szBuffer);
        }
        else
        {
            // We need a pointer to the next character after space.
            pLW++;
            lstrcpy(szLastWord, pLW);
        }

        // Check if typed word has a match in the LH/RH list boxes. If yes 
        // select matching item in corresponding list box. We use global 
        // g_nLastTimeHand here to store what training word was used last 
        // (left or right). This discards redundant search (i.e. when RH 
        // list used it is not necessary to search in LH words).
        switch (g_nLastTimeHand)
        {
        case 1: // left hand
            nRes = SendMessage(g_hwndMode1_ListBox_LeftHandWords, 
                LB_FINDSTRINGEXACT, -1 /* search from the top */, 
                                                        (LPARAM)szLastWord);

            if (nRes != LB_ERR)
            {
                SendMessage(g_hwndMode1_ListBox_LeftHandWords, LB_SETCURSEL, 
                                                    nRes, 0 /* not used */);
                SendMessage(g_hwndMode1_ListBox_LeftHandWords, 
                                    LB_SETTOPINDEX, nRes, 0 /* not used */);
            }
            else
            {
                nRes = SendMessage(g_hwndMode1_ListBox_RightHandWords, 
                    LB_FINDSTRINGEXACT, -1 /* search from the top */, 
                                                        (LPARAM)szLastWord);

                if (nRes != LB_ERR)
                {
                    g_nLastTimeHand = 2;
                    SendMessage(g_hwndMode1_ListBox_RightHandWords, 
                                        LB_SETCURSEL, nRes, 0 /* not used */);
                    SendMessage(g_hwndMode1_ListBox_RightHandWords, 
                                    LB_SETTOPINDEX, nRes, 0 /* not used */);
                }
            }
            break;
        case 2: // right hand
            nRes = SendMessage(g_hwndMode1_ListBox_RightHandWords, 
                LB_FINDSTRINGEXACT, -1 /* search from the top */, 
                                                        (LPARAM)szLastWord);

            if (nRes != LB_ERR)
            {
                SendMessage(g_hwndMode1_ListBox_RightHandWords, 
                                        LB_SETCURSEL, nRes, 0 /* not used */);
                SendMessage(g_hwndMode1_ListBox_RightHandWords, 
                                    LB_SETTOPINDEX, nRes, 0 /* not used */);
            }
            else
            {
                nRes = SendMessage(g_hwndMode1_ListBox_LeftHandWords, 
                    LB_FINDSTRINGEXACT, -1 /* search from the top */, 
                                                        (LPARAM)szLastWord);

                if (nRes != LB_ERR)
                {
                    g_nLastTimeHand = 1;
                    SendMessage(g_hwndMode1_ListBox_LeftHandWords, 
                                        LB_SETCURSEL, nRes, 0 /* not used */);
                    SendMessage(g_hwndMode1_ListBox_LeftHandWords, 
                                    LB_SETTOPINDEX, nRes, 0 /* not used */);
                }
            }
            break;
        }
    }
}

// Purpose: The same function is used in Mode 1 & 2, Mode 3, Mode 4.
LRESULT APIENTRY Edit_SubclassedFunc(HWND hWnd, UINT Message, WPARAM wParam, 
                                                                    LONG lParam)
{
    TCHAR c;
    TCHAR buf[] = _T(" ");
    short iScanCodeCurrent;
    DWORD dwTickCount2;
    short iWordIndex;
    HWND hwndTB; 
    HMENU hMenu, hSubMenu;
    float fTotalTimeSpent;
    short nTotalErrorsCount;
    short nTypedCount;
    BOOL bEndOfTestChar;
    int iCWIndex;
    LV_FINDINFO lvf;
    int iLVItem;
    TCHAR szLastWord[100];

    //  When the focus is in an edit control inside a dialog box, the
    //  default ENTER key action will not occur.         
    if (Message == WM_CHAR)
    {
        c = (TCHAR)WM_CHAR;
        buf[0] = (TCHAR)*&wParam; // wParam holds character code.

        // Enable clear work area menu item clear work area toolbar button 
        // when 1st character typed  regarless of mode.
        hMenu = GetMenu(g_hWnd);
        hSubMenu = GetSubMenu(hMenu, 3);
        EnableMenuItem(hSubMenu, IDM_COMMANDS_CLEAR_TEST_AREA, 
                                                     MF_BYCOMMAND | MF_ENABLED);
        hwndTB = GetDlgItem(g_hWnd, ID_TOOLBAR);
        SendMessage(hwndTB, TB_SETSTATE, ID_CLEAR_TEST_AREA, 
                                                               TBSTATE_ENABLED);
        
        if (g_nMode == 1 || g_nMode == 2)
        {
            // When finished with a word (last symbol is space), select the word 
            // in left or right list box. TODO: rename function.
            OnKeyDown(buf);

            //-- 
            // Mode 1&2 Spell check. TODO: I used this logic from Mode3 
            // section, not always checking for meaning to provise Mo1 & 2 spell 
            // check. Check meaning, avoid redundancy.
            iScanCodeCurrent = HIWORD(lParam);

            // Entry 1.
            if (g_iScanCodePrev == -1 /* 1st word */ || 
                                              g_iScanCodePrev == 57 /* space */)
            {
                g_bWordInProcessOfTyping = TRUE;
                Mode12_SpellCheck(buf, wParam);
            }

            // Entry 2.
            if (g_iScanCodePrev != -1 /* 1st word */ && 
                g_iScanCodePrev != 57 /* space */ && iScanCodeCurrent != 57 /* space */)
            {	
                Mode12_SpellCheck(buf, wParam);
            }

            // Entry 3.
            if (g_iScanCodePrev != 57 /* space */ && iScanCodeCurrent == 57)
            {
                g_bWordInProcessOfTyping = FALSE;
                Mode12_SpellCheck(buf, wParam);
            }

            g_iScanCodePrev = iScanCodeCurrent;
         }
        else if (g_nMode == 3) // mode 3
        {
            iScanCodeCurrent = HIWORD(lParam);

            // Start timer event (user begins typing a new word).
            if (g_iScanCodePrev == -1 /* 1st word */ || 
                                              g_iScanCodePrev == 57 /* space */)
            {
                g_dwTickCount1 = GetTickCount();
                g_bWordInProcessOfTyping = TRUE;
                Mode3_IsWordTypedCorrectly(buf, wParam);
            }

            // Entry: we are in process of typing a word, and this is not the 
            // 1st letter, and not the space after the word typed.
            if (g_iScanCodePrev != -1 /* 1st word */ && 
                g_iScanCodePrev != 57 /* space */ && 
                                             iScanCodeCurrent != 57 /* space */)
            {	
                Mode3_IsWordTypedCorrectly(buf, wParam);
            }

            // Stop timer event (user finishes up with a word and hits space).
            if (g_iScanCodePrev != 57 /* space */ && iScanCodeCurrent == 57)
            {
                dwTickCount2 = GetTickCount();
                g_bWordInProcessOfTyping = FALSE;

                if (Mode3_IsWordTypedCorrectly(buf, wParam))
                {
                    Mode3_GetLastTypedWord(szLastWord);

                    // Update statistics for a typed word.
                    iWordIndex = Mode3_GetSampleWordIndex(szLastWord);

                    if (iWordIndex != -1 /* if found */)
                    {
                        if (g_nErrorsCount == 0) g_nCorrectlyTypedCount++;

                        fTotalTimeSpent = 
                                m_vecCustomWords.at(iWordIndex).fTotalTimeSpent;
                        m_vecCustomWords.at(iWordIndex).fTotalTimeSpent = 
                            fTotalTimeSpent + ((float)dwTickCount2 - 
                                                   (float)g_dwTickCount1)/1000.;
                        
                        m_vecCustomWords.at(iWordIndex).fTimeSpent = 
                            ((float)dwTickCount2 - (float)g_dwTickCount1)/1000.;

                        nTotalErrorsCount = 
                              m_vecCustomWords.at(iWordIndex).nTotalErrorsCount;
                        m_vecCustomWords.at(iWordIndex).nTotalErrorsCount = 
                                             nTotalErrorsCount + g_nErrorsCount;
                        
                        m_vecCustomWords.at(iWordIndex).nErrorsCount = 
                                                                 g_nErrorsCount;
                        nTypedCount = 
                                    m_vecCustomWords.at(iWordIndex).nTypedCount;
                        nTypedCount++;
                        m_vecCustomWords.at(iWordIndex).nTypedCount = 
                                                                    nTypedCount;

                        // Update calculated data in the list view for the 
                        // current completed word. Supposedly, iWordIndex of a 
                        // word stored in vector coincide with its index when 
                        // list view was populated from text file (OK for now).
                        Mode3_UpdateData(iWordIndex);
                        if (g_nDropWord != 0 
                                    /* when zero we don't want drop sequence */)
                        {
                            if (g_nCorrectlyTypedCount >= g_nDropWord)
                            {
                                iCWIndex = Mode3_GetSampleWordIndex(szLastWord);
                                lvf.flags = LVFI_STRING;
                                lvf.psz = szLastWord;
                                iLVItem = ListView_FindItem(
                                                g_hwndMode3_ListView, -1, &lvf);

                                // Remove selected item from the list view and 
                                // from m_vecCustomWords.
                                ListView_DeleteItem(g_hwndMode3_ListView, 
                                                                       iLVItem);
                                // Single "iCWIndex" as argument of erase() does 
                                // not work.
                                m_vecCustomWords.erase(m_vecCustomWords.begin()
                                                                    + iCWIndex);

                                // Update Custom_Words.dat (rewritten actually).
                                Mode3_UpdateCustomList();
                                g_nCorrectlyTypedCount = 0;
                                ListView_RedrawItems(g_hwndMode3_ListView, 0, 
                                   ListView_GetItemCount(g_hwndMode3_ListView));
                            }
                        }
                    } // if (iWordIndex != -1)

                    g_nErrorsCount = 0;
                }
            }
            g_iScanCodePrev = iScanCodeCurrent; 

            //// Start test with any character first typed.
            //if (g_nCharsTyped == 0) Mode4_OnStartTest();

            //g_nCharsTyped++;
            // We are here when 1st character typed. TODO: move into Start timer section. 
            //Mode3_IsWordTypedCorrectly(buf, wParam);
        }
        else if (g_nMode == 4)
        {			
            // Entry A: start test with any character first typed. This segment 
            // merely initializes statistical variables, and sets correct state 
            // of menu, and toolbar.
            if (g_nCharsTyped == 0) 
            {
                if (!g_bTestRunning)
                {
                    Mode4_OnStartTest(); 
                    hwndTB = GetDlgItem(g_hWnd, ID_TOOLBAR);
                    SendMessage(hwndTB, TB_SETSTATE, ID_FILE_OPEN, 
                                                         TBSTATE_INDETERMINATE);
                    SendMessage(hwndTB, TB_CHANGEBITMAP, 
                                       ID_START_OR_STOP_TEST, MAKELPARAM(5, 0));

                    hMenu = GetMenu(g_hWnd);
                    hSubMenu = GetSubMenu(hMenu, 3);

                    _tcscpy(g_szBuffer, 
                    App_GetStringFromStringTable(IDS_TEST_MODE_MENU_STOP_TEST, 
                                                                    g_nLangID));
                    ModifyMenu(hSubMenu, IDM_COMMANDS_START_OR_STOP_TEST, 
                        MF_BYCOMMAND, IDM_COMMANDS_START_OR_STOP_TEST, 
                                                                    g_szBuffer);
                                                             //_T("Stop Test"));
                }
            }

            // Sections B, C, B must be mutually exclusive to keep things not
            // overly complicated. Note that Mode4_IsWordTypedCorrectly makes 
            // a beep when the char entered is incorrect.
            iScanCodeCurrent = HIWORD(lParam);       // used in sections C, D 
            bEndOfTestChar = Mode4_IsEndOfTest(buf); // used in sections C, D 

            // Entry B: first character of a new word; start timer-per-word, 
            // spell-check.
            if ((g_iScanCodePrev == -1 /* 1st word */ || 
                g_iScanCodePrev == 28 /* next line (enter) */ || 
                          g_iScanCodePrev == 57 /* space */) && !bEndOfTestChar)
            {
                g_dwTickCount1 = GetTickCount();
                g_bWordInProcessOfTyping = TRUE;
                Mode4_IsWordTypedCorrectly(buf, wParam);
            }
    
            // Entry C: we are in process of typing a word; just spell-check. 
            if (g_iScanCodePrev != -1 /* 1st word */ && 
                g_iScanCodePrev != 28 /* next line (enter) */ && 
                g_iScanCodePrev != 57 /* space */ && 
                          iScanCodeCurrent != 57 /* space */ && !bEndOfTestChar)
            {	
                Mode4_IsWordTypedCorrectly(buf, wParam);
            }

            // Entry D: user finishes up with a word and hits space or press 
            // enter at the line's end or enters last char of sample test 
            // (must be in this section since we need statistics for the last 
            // word also); stop timer-per-word, spell-check.
            if (((g_iScanCodePrev != -1 /* 1st word */ && 
                g_iScanCodePrev != 57 /* space */ && 
                g_iScanCodePrev != 28 /* next line (enter) */) && 
                (iScanCodeCurrent == 57 || iScanCodeCurrent == 28)) || 
                                                                 bEndOfTestChar)
            {
                dwTickCount2 = GetTickCount();

                if (Mode4_IsWordTypedCorrectly(buf, wParam))
                {
                    TCHAR szLastWord[100];
                    Mode4_GetLastTypedWord(szLastWord);

                    // Update statistics for a typed word.
                    iWordIndex = Mode4_GetSampleWordIndex(szLastWord);

                    if (iWordIndex != -1 /* if found */)
                    {
                        m_vecSampleWords.at(iWordIndex).fTimeSpent = 
                            ((float)dwTickCount2 - (float)g_dwTickCount1)/1000.;
                        m_vecSampleWords.at(iWordIndex).nErrorsCount = 
                                                                 g_nErrorsCount;
                    }

                    // Reset flag/counter for the next word.
                    g_bWordInProcessOfTyping = FALSE;
                    g_nErrorsCount = 0;
                }
            }
            g_iScanCodePrev = iScanCodeCurrent; 

            // Update statistics of errors per entire test.
            g_nCharsTyped++;
        }
   }

   if (Message == WM_KEYUP)
   {
        if (g_nMode == 4)
        {
            //MessageBox(0, L"a", L"a", 0);
        }
   }

   // When edit control has a focus and is subclassed I route processing of menu 
   // commands, which don't work in normal way, to the procedure of subclassed 
   // control i.e. currect procedure. 
   if (Message == WM_COMMAND)
   {
           switch(LOWORD(wParam))
        {
        case IDM_FILE_OPEN:
            Mode4_OnFileOpen();
            break;
        case IDM_MODES_LH_RH_WORDS:
            OnMenuMode12(1 /* LH/RH mode */);
            break;
        case IDM_MODES_LH_RH_WORDS_BOTH_HANDS:
            OnMenuMode12(2 /* LTOR/RTOL mode */);
            break;
        case IDM_MODES_CUSTOM:
            OnMenuMode3();
            break;
        case IDM_MODES_TEST:
            OnMenuMode4();
            break;
        case ID_LANGUAGE_ENGLISH:
            if (App_IsLangInstalled(L"EN")) App_OnChangeLanguage(1);
            break;
        case ID_LANGUAGE_FRENCH:
            if (App_IsLangInstalled(L"FR")) App_OnChangeLanguage(2);
            break;
        case ID_LANGUAGE_RUSSIAN:
            if (App_IsLangInstalled(L"RU")) App_OnChangeLanguage(3);
            break;
        case IDM_COMMANDS_START_OR_STOP_TEST:
            // Delegate to the toolbar's handler. 
            SendMessage(g_hWnd, WM_COMMAND, 
                MAKEWPARAM(ID_START_OR_STOP_TEST /* low word */, 
                                                         0 /* high word */), 0);
             break;
        case IDM_COMMANDS_CLEAR_TEST_AREA:
            // Delegate to the toolbar's handler. 
            SendMessage(g_hWnd, WM_COMMAND, 
                MAKEWPARAM(ID_CLEAR_TEST_AREA /* low word */, 
                                                         0 /* high word */), 0);
            break;
        case IDM_COMMANDS_DETAILED_STATS:
            // Delegate to the toolbar's handler. 
            SendMessage(g_hWnd, WM_COMMAND, 
                MAKEWPARAM(ID_DETAILED_STATS /* low word */, 
                                                         0 /* high word */), 0);
            break;
        case IDM_COMMANDS_SHOW_REPORT:
            Mode4_CreateHtmlReport();
            break;
        case IDM_HELP_KEYBOARD_LAYOUT:
            App_MenuHandler_ShowOnlineHelp(L"Hands_Positioning.htm");
            break;
        case IDM_HELP_ONLINE:
            App_MenuHandler_ShowOnlineHelp(L"RT_Help.htm");
            break;
        default:
            break;
        }
   }

   return CallWindowProc((WNDPROC)g_wpOldWndProc, hWnd, Message, wParam, 
                                                                        lParam);
}

short GetPairIndex(LPTSTR pszPair)
{
    //string s;
    wstring s;
    UINT i;
    
    for (i = 0; i < m_vecPairs.size() /* 676 */; i++)
    {
        s = m_vecPairs.at(i).sPair;
        if(lstrcmp(s.c_str(), CharLower(pszPair))==0)
        {
            //CString ss;
            //ss.Format("%d", i);
            //AfxMessageBox(ss);
            return i;
        }    
    }

    //AfxMessageBox(0, pszPair, "not found", 0);
    return -1;
};

void InitVector()
{
    //HANDLE hLog;
    //DWORD dwType, dwLength, dwResult;

    TCHAR szResultsFileName[] = _T("c:\\ResultsFileName.txt");
    
    //--------------------------------------------------------------------------
    // CreateFile uses such parameters so that if the log file exits, the file 
    // is just opened, if the file does not not exist it is created
    //--------------------------------------------------------------------------
    
    m_hLog = CreateFile(szResultsFileName,
                        GENERIC_READ|GENERIC_WRITE, 
                        FILE_SHARE_READ|FILE_SHARE_WRITE, 
                        NULL,                  //security - not used          
                        OPEN_ALWAYS,           //if exists - just open;if file does not exist, create it
                        FILE_ATTRIBUTE_NORMAL, //normal use
                        NULL );         

    // Tell the writer we need to write at the end of file.
    DWORD dwOffset = SetFilePointer(m_hLog, 0, NULL, FILE_END); 

    //string s, a, b;
    wstring s, a, b;
    s = _T("abcdefghijklmnopqrstuvwxyz");

    //DWORD dWritten;
    //TCHAR szLog[256];
    CCharacterPairs cp;

    for (int i = 0; i < 26; i++)
    {
          for (int k = 0; k < 26; k++)
          {
              a = s.at(i); b = s.at(k);
              cp.sPair = a + b;
              // We need to zero-initialize because it does not set 
              // automatically to zero.
              cp.nHitCount = 0; 
              cp.nTimeLapSum = 0; 
              cp.fTimeLapAverage = 0.; 
              m_vecPairs.push_back(cp);
          }
    }
}

void OnButtonShowLogClicked()
{
    //TCHAR szFileLocation[200];
    //TCHAR szCommand[200];

    char szFileLocation[200];
    char szCommand[200];

    sprintf(szFileLocation, "%s", "C:\\ResultsFileName.txt");
    sprintf(szCommand, "%s %s", "notepad.exe", szFileLocation); 

    WinExec(szCommand, TRUE); // takes LPCSTR (not LPCTSTR !)
}

void OnRepopulateButtonClicked()
{
    TCHAR szText[10];
    
    // Clear list boxes before populating.
    ::SendMessage(g_hwndMode1_ListBox_Fastest, LB_RESETCONTENT, 0 /* not used*/, 
                                                               0 /* not used*/);
    ::SendMessage(g_hwndMode1_ListBox_Slowest, LB_RESETCONTENT, 0 /* not used*/, 
                                                               0 /* not used*/);

    ::SendMessage(g_hwndMode1_ListBox_MostFrequent, LB_RESETCONTENT, 
                                              0 /* not used*/, 0 /* not used*/);

    ::SendMessage(g_hwndMode1_ListBox_LeastFrequent, LB_RESETCONTENT, 
                                              0 /* not used*/, 0 /* not used*/);
    
    // Populate fastest list box.
    std::sort(m_vecPairs.begin(), m_vecPairs.end(), TimeLapAvgSortAsc());
    short nCount = 0;
    for(it = m_vecPairs.begin(); 
                              it != m_vecPairs.end(); it++)
    {
        if(it->fTimeLapAverage != 0)
        {
            //just f (%.2f) if wsprintf is used.
            wsprintf(szText, _T("%s%.2f"), it->sPair.c_str(), 
                                                           it->fTimeLapAverage); 

            //ListBox_AddString?
            ::SendMessage(g_hwndMode1_ListBox_Fastest, LB_ADDSTRING, 
                                            NULL /* not used*/, (LPARAM)szText);

            nCount++; if( nCount > 10) break;
        }
    }
    
    // Populate slowest list box.
    std::sort(m_vecPairs.begin(), m_vecPairs.end(), TimeLapAvgSortDesc());
    nCount = 0;
    for(it = m_vecPairs.begin(); it != m_vecPairs.end(); it++)
    {
        nCount++; if( nCount > 10) break;

        //just f (%.2f) if wsprintf is used.
        //sprintf(szText, _T("%s%.2f"), it->sPair.c_str(), it->fTimeLapAverage);
        wsprintf(szText, _T("%s%.2f"), it->sPair.c_str(), it->fTimeLapAverage); 

        ::SendMessage(g_hwndMode1_ListBox_Slowest, LB_ADDSTRING, 
                                            NULL /* not used*/, (LPARAM)szText);
    }

    // Populate most hits list box.
    std::sort(m_vecPairs.begin(), m_vecPairs.end(), HitCountSortDesc());
    nCount = 0;
    for (it = m_vecPairs.begin(); it != m_vecPairs.end(); it++)
    {
        nCount++; if (nCount > 10) break;

        //just f (%.2f) if wsprintf is used.
        //sprintf(szText, _T("%s%d"), it->sPair.c_str(), it->nHitCount); 
        wsprintf(szText, _T("%s%d"), it->sPair.c_str(), it->nHitCount); 

        ::SendMessage(g_hwndMode1_ListBox_MostFrequent, LB_ADDSTRING, 
                                            NULL /* not used*/, (LPARAM)szText);
    }

    // Populate least hits list box.
    std::sort(m_vecPairs.begin(), m_vecPairs.end(), HitCountSortAsc());
    nCount = 0;
    for (it = m_vecPairs.begin(); it != m_vecPairs.end(); it++)
    {
        nCount++; if (nCount > 10) break;

        //just f (%.2f) if wsprintf is used.
        wsprintf(szText, _T("%s%d"), it->sPair.c_str(), it->nHitCount); 

        ::SendMessage(g_hwndMode1_ListBox_LeastFrequent, LB_ADDSTRING, 
                                            NULL /* not used*/, (LPARAM)szText);
    }
}

void PopulateWithTrainingWords(short nType, short nLangID)
{
    char szLine[100];
    TCHAR szBuffer[100];
    short nCounter = 0;
    ifstream left_hand_file, right_hand_file, test;
    char szCurDirectory[255]; // ANSI because argument of open is ANSI string
    char szFilePath[255];     // ANSI because argument of open is ANSI string
    char szDevDirectory[255]; // ANSI because argument of open is ANSI string

    // Clear list boxes contents before all.
    switch (nType)
    {
    case 1:
        ::SendMessage(g_hwndMode1_ListBox_LeftHandWords, LB_RESETCONTENT, 
                                    0 /* must be zero */, 0 /* must be zero */);
        ::SendMessage(g_hwndMode1_ListBox_RightHandWords, LB_RESETCONTENT, 
                                    0 /* must be zero */, 0 /* must be zero */);
        break;
    case 2:
        ::SendMessage(g_hwndMode1_ListBox_LeftHandWords, LB_RESETCONTENT, 
                                    0 /* must be zero */, 0 /* must be zero */);
        ::SendMessage(g_hwndMode1_ListBox_RightHandWords, LB_RESETCONTENT, 
                                    0 /* must be zero */, 0 /* must be zero */);
        break;
    case 3:
        //::SendMessage(g_hwndMode3_ListBox_CustomWords, 
        //	       LB_RESETCONTENT, 0 /* must be zero */, 0 /* must be zero */);
        Mode3_PopulateWithTrainingWords(g_nLangID);
        break;
    }

       // Obtain ANSI path to current directory.
    GetCurrentDirectoryA(255, szCurDirectory);
    // Set development directory.
    strcpy(szDevDirectory, APP_DIRECTORY_ANSI); 
    
    switch(nLangID)
    {
    case 1: // English

        // We use ANSI file as input file for English training words, and read 
        // it as a stream with usual ifstream (not wifstream). Then we convert 
        // each line (with MultiByteToWideChar) to Unicode stream. 

        // TODO: open takes ANSI string only, find function that can take 
        // Unicode.
        // TODO: avoid "test" stream, it is used now because if open fails, 
        // and stream closes I could not use the same name of string with 
        // different path.
        strcpy(szFilePath, szCurDirectory);

        switch (nType)
        {
            case 1:
                strcat(szFilePath, "\\EN\\Training Words\\LH_Words.dat");
                break;
            case 2:
                strcat(szFilePath, "\\EN\\Training Words\\LTOR_Both.dat");
                break;
            case 3:
                strcat(szFilePath, "\\EN\\Training Words\\Custom_Words.dat");
                break;
        }

        test.open(szFilePath);

        if (test == NULL)
        {
            test.close();

            // Switch to development directory during development.
            strcpy(szFilePath, szDevDirectory);

            switch (nType)
            {
            case 1:
                strcat(szFilePath, "\\EN\\Training Words\\LH_Words.dat");
                break;
            case 2:
                strcat(szFilePath, "\\EN\\Training Words\\LTOR_Both.dat");
                break;
            case 3:
                strcat(szFilePath, "\\EN\\Training Words\\Custom_Words.dat");
                break;
            }

            left_hand_file.open(szFilePath);
        }
        else
        {
            test.close();
            left_hand_file.open(szFilePath);
        }

        if (left_hand_file) 
        {
            while(left_hand_file.good())
            {
                left_hand_file.getline(szLine, 50);

                // MultiByteToWideChar use is OK for Windows 98 etc (however 
                // function does not exist on 95).
                MultiByteToWideChar(CP_ACP, 
                    0 /* must be zero [TODO: clarify] */, szLine, 
                    -1 /* string NULL-terminated [TODO: clarify] */, 
                                                    szBuffer, sizeof(szBuffer)); 
                if(_tcslen(szBuffer)!=0)
                {
                    if ( nType == 1 || nType == 2)
                    {
                        ::SendMessage(g_hwndMode1_ListBox_LeftHandWords, 
                            LB_ADDSTRING, NULL /* not used*/, (LPARAM)szBuffer);
                    }

                    if ( nType == 3)
                    {
                        ::SendMessage(g_hwndMode3_ListBox_CustomWords, 
                            LB_ADDSTRING, NULL /* not used*/, (LPARAM)szBuffer);
                    }
                }
            }
        }

        left_hand_file.close();

        // TODO: open takes ANSI string only, find function that can take 
        // Unicode.
        // TODO: avoid "test" stream, it is used now because if open fails, 
        // and stream closes I could not use the same name of string with 
        // different path.
        strcpy(szFilePath, szCurDirectory);

        if (nType == 1)
        {
            strcat(szFilePath, "\\EN\\Training Words\\RH_Words.dat");
        }
        else
        {
            strcat(szFilePath, "\\EN\\Training Words\\RTOL_Both.dat");
        }

        test.open(szFilePath);

        if (test == NULL)
        {
            test.close();

            // Switch to development directory during development.
            strcpy(szFilePath, szDevDirectory);

            if (nType == 1)
            {
                strcat(szFilePath, "\\EN\\Training Words\\RH_Words.dat");
            }
            else
            {
                strcat(szFilePath, "\\EN\\Training Words\\RTOL_Both.dat");
            }
            right_hand_file.open(szFilePath);
        }
        else
        {
            test.close();
            right_hand_file.open(szFilePath);
        }

        if (right_hand_file) 
        {
            while(right_hand_file.good())
            {
                right_hand_file.getline(szLine, 50);

                // MultiByteToWideChar use is OK for Windows 98 etc (however 
                // function does not exist on 95).
                MultiByteToWideChar(CP_ACP, 
                    0 /* must be zero [TODO: clarify] */, szLine, 
                    -1 /* string NULL-terminated [TODO: clarify] */, 
                                                   szBuffer, sizeof(szBuffer)); 
                if(_tcslen(szBuffer)!=0)
                {
                    ::SendMessage(g_hwndMode1_ListBox_RightHandWords, 
                            LB_ADDSTRING, NULL /* not used*/, (LPARAM)szBuffer);
                }
            }
        }

        right_hand_file.close();
        break;

    case 2: // French
        
        // TODO: Code copied from EN version. Merge, probably.
        // We use ANSI file as input file for English training words, and read 
        // it as a stream with usual ifstream (not wifstream). Then we convert 
        // each line (with MultiByteToWideChar) to Unicode stream. 

        // TODO: open takes ANSI string only, find function that can take 
        // Unicode.
        // TODO: avoid "test" stream, it is used now because if open fails, 
        // and stream closes I could not use the same name of string with 
        // different path.
        strcpy(szFilePath, szCurDirectory);

        switch (nType)
        {
            case 1:
                strcat(szFilePath, "\\FR\\Training Words\\LH_Words.dat");
                break;
            case 2:
                strcat(szFilePath, "\\FR\\Training Words\\LTOR_Both.dat");
                break;
            case 3:
                strcat(szFilePath, "\\FR\\Training Words\\Custom_Words.dat");
                break;
        }

        test.open(szFilePath);

        if (test == NULL)
        {
            test.close();

            // Switch to development directory during development.
            strcpy(szFilePath, szDevDirectory);

            switch (nType)
            {
            case 1:
                strcat(szFilePath, "\\FR\\Training Words\\LH_Words.dat");
                break;
            case 2:
                strcat(szFilePath, "\\FR\\Training Words\\LTOR_Both.dat");
                break;
            case 3:
                strcat(szFilePath, "\\FR\\Training Words\\Custom_Words.dat");
                break;
            }

            left_hand_file.open(szFilePath);
        }
        else
        {
            test.close();
            left_hand_file.open(szFilePath);
        }

        if (left_hand_file) 
        {
            while(left_hand_file.good())
            {
                left_hand_file.getline(szLine, 50);

                // MultiByteToWideChar use is OK for Windows 98 etc (however 
                // function does not exist on 95).
                MultiByteToWideChar(CP_ACP, 
                    0 /* must be zero [TODO: clarify] */, szLine, 
                    -1 /* string NULL-terminated [TODO: clarify] */, 
                                                    szBuffer, sizeof(szBuffer)); 
                if(_tcslen(szBuffer)!=0)
                {
                    if ( nType == 1 || nType == 2)
                    {
                        ::SendMessage(g_hwndMode1_ListBox_LeftHandWords, 
                            LB_ADDSTRING, NULL /* not used*/, (LPARAM)szBuffer);
                    }

                    if ( nType == 3)
                    {
                        ::SendMessage(g_hwndMode3_ListBox_CustomWords, 
                            LB_ADDSTRING, NULL /* not used*/, (LPARAM)szBuffer);
                    }
                }
            }
        }

        left_hand_file.close();

        // TODO: open takes ANSI string only, find function that can take 
        // Unicode.
        // TODO: avoid "test" stream, it is used now because if open fails, 
        // and stream closes I could not use the same name of string with 
        // different path.
        strcpy(szFilePath, szCurDirectory);

        if (nType == 1)
        {
            strcat(szFilePath, "\\FR\\Training Words\\RH_Words.dat");
        }
        else
        {
            strcat(szFilePath, "\\FR\\Training Words\\RTOL_Both.dat");
        }

        test.open(szFilePath);

        if (test == NULL)
        {
            test.close();

            // Switch to development directory during development.
            strcpy(szFilePath, szDevDirectory);

            if (nType == 1)
            {
                strcat(szFilePath, "\\FR\\Training Words\\RH_Words.dat");
            }
            else
            {
                strcat(szFilePath, "\\FR\\Training Words\\RTOL_Both.dat");
            }
            right_hand_file.open(szFilePath);
        }
        else
        {
            test.close();
            right_hand_file.open(szFilePath);
        }

        if (right_hand_file) 
        {
            while(right_hand_file.good())
            {
                right_hand_file.getline(szLine, 50);

                // MultiByteToWideChar use is OK for Windows 98 etc (however 
                // function does not exist on 95).
                MultiByteToWideChar(CP_ACP, 
                    0 /* must be zero [TODO: clarify] */, szLine, 
                    -1 /* string NULL-terminated [TODO: clarify] */, 
                                                   szBuffer, sizeof(szBuffer)); 
                if(_tcslen(szBuffer)!=0)
                {
                    ::SendMessage(g_hwndMode1_ListBox_RightHandWords, 
                            LB_ADDSTRING, NULL /* not used*/, (LPARAM)szBuffer);
                }
            }
        }

        right_hand_file.close();
        break;

    case 3: // Russian
        // We use UTF-8 file as input file for Russian training words, and read 
        // it as a stream with usual ifstream (not wifstream). Then we convert 
        // each line (with MultiByteToWideChar) to Unicode stream. Because
        // UTF-8 file is prepended with 3-byte UTF-8 preamble (EF BB BF in hex), 
        // I left-trim 3 bytes from the first line. Note that other way is to save
        // UTF-8 without BOM (but notepad will always add BOM!).

        //left_hand_file.open("C:\\MyFiles\\Development\\VCC\\TypingPro.Win32\\LH_Words_RU_UTF8.txt");

        // TODO: open takes ANSI string only, find function that can take 
        // Unicode.
        // TODO: avoid "test" stream, it is used now because if open fails, 
        // and stream closes I could not use the same name of string with 
        // different path.
        strcpy(szFilePath, szCurDirectory);
        //strcat(szFilePath, "\\RU\\Training Words\\LH_Words_RU_UTF8.txt");
        switch (nType)
        {
            case 1:
                strcat(szFilePath, 
                                  "\\RU\\Training Words\\LH_Words_RU_UTF8.txt");
                break;
            case 2:
                strcat(szFilePath, 
                                "\\RU\\Training Words\\LTOR_Words_RU_UTF8.txt");
                break;
        }

        test.open(szFilePath);

        if (test == NULL)
        {
            test.close();

            // Switch to development directory during development.
            strcpy(szFilePath, szDevDirectory);
            //strcat(szFilePath, "\\RU\\Training Words\\LH_Words_RU_UTF8.txt");
            switch (nType)
            {
            case 1:
                strcat(szFilePath, 
                                  "\\RU\\Training Words\\LH_Words_RU_UTF8.txt");
                break;
            case 2:
                strcat(szFilePath, 
                                "\\RU\\Training Words\\LTOR_Words_RU_UTF8.txt");
                break;
            }
            left_hand_file.open(szFilePath);
        }
        else
        {
            test.close();
            left_hand_file.open(szFilePath);
        }
        
        if (left_hand_file) 
        {
            while(left_hand_file.good())
            {
                nCounter++;
                left_hand_file.getline(szLine, 50);

                // MultiByteToWideChar use is OK for Windows 98 etc (however 
                // function does not exist on 95).
                MultiByteToWideChar(CP_UTF8, 0 /* must be zero */, 
                    szLine, -1 /* string NULL-terminated [TODO: clarify] */,  
                                                    szBuffer, sizeof(szBuffer)); 

                // Remove 3-byte UTF-8 preamble ("BOM") from the first line. 
                // CharNext moves correctly to 1st TCHAR. 
                if (nCounter == 1)		    
                {
                    ::SendMessage(g_hwndMode1_ListBox_LeftHandWords, 
                        LB_ADDSTRING, NULL /* not used*/, 
                                                    (LPARAM)CharNext(szBuffer));
                }
                else
                {
                    if(_tcslen(szBuffer)!=0)
                    {
                        ::SendMessage(g_hwndMode1_ListBox_LeftHandWords, 
                            LB_ADDSTRING, NULL /* not used*/, (LPARAM)szBuffer);
                    }
                }
            }
        }

        left_hand_file.close(); // close actually deletes handle

        nCounter = 0;
        // TODO: open takes ANSI string only, find function that can take 
        // Unicode.
        // TODO: avoid "test" stream, it is used now because if open fails, 
        // and stream closes I could not use the same name of string with 
        // different path.
        strcpy(szFilePath, szCurDirectory);
        //strcat(szFilePath, "\\RU\\Training Words\\RH_Words_RU_UTF8.txt");
        switch (nType)
        {
            case 1:
                strcat(szFilePath, 
                                  "\\RU\\Training Words\\RH_Words_RU_UTF8.txt");
                break;
            case 2:
                strcat(szFilePath,
                                "\\RU\\Training Words\\RTOL_Words_RU_UTF8.txt");
                break;
        }

        test.open(szFilePath);

        if (test == NULL)
        {
            test.close();

            // Switch to development directory during development.
            strcpy(szFilePath, szDevDirectory);
            //strcat(szFilePath, "\\RU\\Training Words\\RH_Words_RU_UTF8.txt");
            switch (nType)
            {
            case 1:
                strcat(szFilePath, 
                                  "\\RU\\Training Words\\RH_Words_RU_UTF8.txt");
                break;
            case 2:
                strcat(szFilePath,
                                "\\RU\\Training Words\\RTOL_Words_RU_UTF8.txt");
                break;
            }
            right_hand_file.open(szFilePath);
        }
        else
        {
            test.close();
            right_hand_file.open(szFilePath);
        }
    
        if (right_hand_file) 
        {
            while(right_hand_file.good())
            {
                nCounter++;
                right_hand_file.getline(szLine, 50);

                // MultiByteToWideChar use is OK for Windows 98 etc (however 
                // function does not exist on 95).
                MultiByteToWideChar(CP_UTF8, 0 /* must be zero */, 
                    szLine, -1 /* string NULL-terminated [TODO: clarify] */,  
                                                    szBuffer, sizeof(szBuffer)); 

                // Remove 3-byte UTF-8 preamble ("BOM") from the first line. 
                // CharNext moves correctly to 1st TCHAR. 
                if (nCounter == 1)		    
                {
                    ::SendMessage(g_hwndMode1_ListBox_RightHandWords, 
                        LB_ADDSTRING, NULL /* not used*/, 
                                                    (LPARAM)CharNext(szBuffer));
                }
                else
                {
                    if (_tcslen(szBuffer)!=0)
                    {
                        ::SendMessage(g_hwndMode1_ListBox_RightHandWords, 
                            LB_ADDSTRING, NULL /* not used*/, (LPARAM)szBuffer);
                    }
                }
            }
        }

        right_hand_file.close();
        break;
    };
}

// Based on PopulateWithTrainingWords.
void Mode3_PopulateWithTrainingWords(short nLangID)
{
    char szLine[100];
    TCHAR szBuffer[100];
    TCHAR szBuffer2[5];
    wstring sBuffer;
    short nCounter = 0;
    ifstream custom_words_file, test;
    char szCurDirectory[255]; // argument of open is ANSI string
    char szFilePath[255];     // argument of open is ANSI string
    char szDevDirectory[255]; // argument of open is ANSI string
    CustomWords sw;

    // Clear the list of custom words (used when language switched).
    m_vecCustomWords.clear();
    // Clear list view contents before all.
    ListView_DeleteAllItems(g_hwndMode3_ListView);
    //::SendMessage(g_hwndMode3_ListBox_CustomWords, 
    //		       LB_RESETCONTENT, 0 /* must be zero */, 0 /* must be zero */);

    LVITEM lvi;
    lvi.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM | LVIF_STATE; 
    lvi.state = 0; 
    lvi.stateMask = 0;

    int iWordCount = 0;

       // Obtain ANSI path to current directory.
    GetCurrentDirectoryA(255, szCurDirectory);
    // Set development directory.
    strcpy(szDevDirectory, APP_DIRECTORY_ANSI); 
    
    switch(nLangID)
    {
    case 1: // English

        // We use ANSI file as input file for English training words, and read 
        // it as a stream with usual ifstream (not wifstream). Then we convert 
        // each line (with MultiByteToWideChar) to Unicode stream. 

        // TODO: open takes ANSI string only, find function that can take 
        // Unicode.
        // TODO: avoid "test" stream, it is used now because if open fails, 
        // and stream closes I could not use the same name of string with 
        // different path.
        strcpy(szFilePath, szCurDirectory);
        strcat(szFilePath, "\\EN\\Training Words\\Custom_Words.dat");

        test.open(szFilePath);

        if (test == NULL)
        {
            test.close();

            // Switch to development directory during development.
            strcpy(szFilePath, szDevDirectory);
            strcat(szFilePath, "\\EN\\Training Words\\Custom_Words.dat");

            custom_words_file.open(szFilePath);
        }
        else
        {
            test.close();
            custom_words_file.open(szFilePath);
        }

        if (custom_words_file) 
        {
            //while (custom_words_file.good())
            while (!custom_words_file.eof())
            {
                custom_words_file.getline(szLine, 50);
                // TODO: eliminate one last pass (empty string)
                //if (strlen(szLine) == 0) break;

                // MultiByteToWideChar use is OK for Windows 98 etc (however 
                // function does not exist on 95).
                MultiByteToWideChar(CP_ACP, 
                    0 /* must be zero [TODO: clarify] */, szLine, 
                    -1 /* string NULL-terminated [TODO: clarify] */, 
                                                  szBuffer, sizeof(szBuffer)/2); 
                sBuffer = szBuffer;
                //if (_tcslen(szBuffer)!=0)
                if (sBuffer.length() != 0)
                {
                    ::SendMessage(g_hwndMode3_ListBox_CustomWords, 
                       LB_ADDSTRING, NULL /* not used*/, (LPARAM)sBuffer.c_str());

                    lvi.iItem = iWordCount;
                    lvi.iSubItem = 0;
                    lvi.lParam = iWordCount; 
                    lvi.pszText = szBuffer;
                    int iInsertedItem = 
                                ListView_InsertItem(g_hwndMode3_ListView, &lvi);

                    wsprintf(szBuffer2, _T("%d"), lstrlen(szBuffer)); 
                    ListView_SetItemText(g_hwndMode3_ListView, iInsertedItem, 1, 
                                                                     szBuffer2);
                    ListView_SetItemText(g_hwndMode3_ListView, iInsertedItem, 2, 
                                                                        L"n/a");
                    ListView_SetItemText(g_hwndMode3_ListView, iInsertedItem, 3, 
                                                                        L"n/a");
                    ListView_SetItemText(g_hwndMode3_ListView, iInsertedItem, 4, 
                                                                        L"n/a");
                    iWordCount++;
                }

                sw.sWord = sBuffer; 
                sw.nLength = lstrlen(szBuffer);
                sw.fTimeSpent = (float)0;
                sw.fTotalTimeSpent = (float)0;
                sw.nErrorsCount = 0; 
                sw.nTotalErrorsCount = 0;
                sw.nTypedCount = 0;
                
                m_vecCustomWords.push_back(sw);
            }
        }
        custom_words_file.close();

        break;

    case 2: // French. Code fragment copied from RU section.
        strcpy(szFilePath, szCurDirectory);
        strcat(szFilePath, "\\FR\\Training Words\\Custom_Words.dat");

        test.open(szFilePath);

        if (test == NULL)
        {
            test.close();

            // Switch to development directory during development.
            strcpy(szFilePath, szDevDirectory);
            strcat(szFilePath, "\\FR\\Training Words\\Custom_Words.dat");
            custom_words_file.open(szFilePath);
        }
        else
        {
            test.close();
            custom_words_file.open(szFilePath);
        }
        
        if (custom_words_file) 
        {
            while (custom_words_file.good())
            {
                //nCounter++;
                custom_words_file.getline(szLine, 50);

                // MultiByteToWideChar use is OK for Windows 98 etc (however 
                // function does not exist on 95).
                MultiByteToWideChar(CP_UTF8, 0 /* must be zero */, 
                    szLine, -1 /* string NULL-terminated [TODO: clarify] */,  
                                                    szBuffer, sizeof(szBuffer)); 

                // Remove 3-byte UTF-8 preamble ("BOM") from the first line. 
                // CharNext moves correctly to 1st TCHAR. 
                if (_tcslen(szBuffer) != 0)
                {
                    ::SendMessage(g_hwndMode3_ListBox_CustomWords, 
                            LB_ADDSTRING, NULL /* not used*/, (LPARAM)szBuffer);

                    lvi.iItem = iWordCount;
                    lvi.iSubItem = 0;
                    lvi.lParam = iWordCount; 
                    lvi.pszText = szBuffer;
                    int iInsertedItem = 
                                ListView_InsertItem(g_hwndMode3_ListView, &lvi);

                    wsprintf(szBuffer2, _T("%d"), lstrlen(szBuffer)); 
                    ListView_SetItemText(g_hwndMode3_ListView, iInsertedItem, 1, 
                                                                     szBuffer2);
                    ListView_SetItemText(g_hwndMode3_ListView, iInsertedItem, 2, 
                                                                        L"n/a");
                    ListView_SetItemText(g_hwndMode3_ListView, iInsertedItem, 3, 
                                                                        L"n/a");
                    ListView_SetItemText(g_hwndMode3_ListView, iInsertedItem, 4, 
                                                                        L"n/a");
                    iWordCount++;

                    sw.sWord = szBuffer;
                    sw.nLength = lstrlen(szBuffer);
                    sw.fTimeSpent = (float)0;
                    sw.fTotalTimeSpent = (float)0;
                    sw.nErrorsCount = 0; 
                    sw.nTotalErrorsCount = 0;
                    sw.nTypedCount = 0;
                    
                    m_vecCustomWords.push_back(sw);
                }
            }
        }

        custom_words_file.close(); // close actually deletes handle

        break;

    case 3: // Russian TODO: unite with English (LL the only difference is 
        // CP_ACP or CP_UTF8 and BOM does not matter, and EN/RU folders

        // We use UTF-8 file as input file for Russian training words, and read 
        // it as a stream with usual ifstream (not wifstream). Then we convert 
        // each line (with MultiByteToWideChar) to Unicode stream. Because
        // UTF-8 file is prepended with 3-byte UTF-8 preamble (EF BB BF in hex), 
        // I left-trim 3 bytes from the first line. Note that other way is to save
        // UTF-8 without BOM (but notepad will always add BOM!).

        //left_hand_file.open("C:\\MyFiles\\Development\\VCC\\TypingPro.Win32\\LH_Words_RU_UTF8.txt");

        // TODO: open takes ANSI string only, find function that can take 
        // Unicode.
        // TODO: avoid "test" stream, it is used now because if open fails, 
        // and stream closes I could not use the same name of string with 
        // different path.
        strcpy(szFilePath, szCurDirectory);
        strcat(szFilePath, "\\RU\\Training Words\\Custom_Words.dat");

        test.open(szFilePath);

        if (test == NULL)
        {
            test.close();

            // Switch to development directory during development.
            strcpy(szFilePath, szDevDirectory);
            strcat(szFilePath, "\\RU\\Training Words\\Custom_Words.dat");
            custom_words_file.open(szFilePath);
        }
        else
        {
            test.close();
            custom_words_file.open(szFilePath);
        }
        
        if (custom_words_file) 
        {
            while (custom_words_file.good())
            {
                //nCounter++;
                custom_words_file.getline(szLine, 50);

                // MultiByteToWideChar use is OK for Windows 98 etc (however 
                // function does not exist on 95).
                MultiByteToWideChar(CP_UTF8, 0 /* must be zero */, 
                    szLine, -1 /* string NULL-terminated [TODO: clarify] */,  
                                                    szBuffer, sizeof(szBuffer)); 

                // Remove 3-byte UTF-8 preamble ("BOM") from the first line. 
                // CharNext moves correctly to 1st TCHAR. 
                if (_tcslen(szBuffer) != 0)
                {
                    //if (nCounter == 1)		    
                    //{
                    //	::SendMessage(g_hwndMode3_ListBox_CustomWords, 
                    //		LB_ADDSTRING, NULL /* not used*/, 
                    //									(LPARAM)CharNext(szBuffer));
                    //}
                    //else
                    //{
                    ::SendMessage(g_hwndMode3_ListBox_CustomWords, 
                                LB_ADDSTRING, NULL /* not used*/, (LPARAM)szBuffer);
                    //}

                    lvi.iItem = iWordCount;
                    lvi.iSubItem = 0;
                    lvi.lParam = iWordCount; 
                    lvi.pszText = szBuffer;
                    int iInsertedItem = 
                                ListView_InsertItem(g_hwndMode3_ListView, &lvi);

                    wsprintf(szBuffer2, _T("%d"), lstrlen(szBuffer)); 
                    ListView_SetItemText(g_hwndMode3_ListView, iInsertedItem, 1, 
                                                                     szBuffer2);
                    ListView_SetItemText(g_hwndMode3_ListView, iInsertedItem, 2, 
                                                                        L"n/a");
                    ListView_SetItemText(g_hwndMode3_ListView, iInsertedItem, 3, 
                                                                        L"n/a");
                    ListView_SetItemText(g_hwndMode3_ListView, iInsertedItem, 4, 
                                                                        L"n/a");
                    iWordCount++;

                    sw.sWord = szBuffer;
                    sw.nLength = lstrlen(szBuffer);
                    sw.fTimeSpent = (float)0;
                    sw.fTotalTimeSpent = (float)0;
                    sw.nErrorsCount = 0; 
                    sw.nTotalErrorsCount = 0;
                    sw.nTypedCount = 0;
                    
                    m_vecCustomWords.push_back(sw);
                }
            }
        }

        custom_words_file.close(); // close actually deletes handle

        break;
    };
}

void OnMenuMode4()
{
    if (g_bDebug) App_Debug_FunctionCallTrace("OnMenuMode4 Entered");

    HWND hwndTB;
    HMENU hMenu, hSubMenu;
    TCHAR szSampleText[MAX_SAMPLE_TEXT];
    LPTSTR pszText = szSampleText; 
    //LPTSTR pszSampleText = NULL;
    HFONT hfnt;
    LOGFONT lf;
    BOOL bFileLoaded = FALSE;

    // URL logo position (there is no yet URL logo when startuped).
    if (g_hwndURLLogo != NULL)
    {
        //MoveWindow(g_hwndURLLogo, g_nMode4_URLLogoLeft, 4, g_nURLLogoWidth, 20, 
        MoveWindow(g_hwndURLLogo, g_nMode4_URLLogoLeft + g_nLogoLeftIndent, 
                                                  4, g_nURLLogoWidth, 20, TRUE);
    }

    // Show status bar hint for the current mode.
    if (g_bShowStatusBarHints)
        SendMessage(g_hwndStatus, WM_SETTEXT, 0 /* not used */, 
               (LPARAM)App_GetStringFromStringTable(IDS_MODE4_HINT, g_nLangID));

    // Update modes menu status.
    hMenu = GetMenu(g_hWnd);

    // Enable File/Open, which is used in this mode (test mode) only.
    hSubMenu = GetSubMenu(hMenu, 0 /* File menu*/);
    EnableMenuItem(hSubMenu, IDM_FILE_OPEN, MF_BYCOMMAND | MF_ENABLED);

    hSubMenu = GetSubMenu(hMenu, 2 /* Modes menu*/);
    EnableMenuItem(hSubMenu, IDM_MODES_LH_RH_WORDS, MF_BYCOMMAND | MF_ENABLED);
    EnableMenuItem(hSubMenu, IDM_MODES_LH_RH_WORDS_BOTH_HANDS, 
                                                     MF_BYCOMMAND | MF_ENABLED);
    //EnableMenuItem(hSubMenu, IDM_MODES_CUSTOM, MF_BYCOMMAND | MF_ENABLED);
    Mode3_SetCustomModeItemState();
    EnableMenuItem(hSubMenu, IDM_MODES_TEST, MF_BYCOMMAND | MF_GRAYED);

    hSubMenu = GetSubMenu(hMenu, 3 /* Commands menu*/);
    EnableMenuItem(hSubMenu, IDM_COMMANDS_START_OR_STOP_TEST, 
                                                     MF_BYCOMMAND | MF_ENABLED);
    EnableMenuItem(hSubMenu, IDM_COMMANDS_CLEAR_TEST_AREA, 
                                                      MF_BYCOMMAND | MF_GRAYED);
    EnableMenuItem(hSubMenu, IDM_COMMANDS_DETAILED_STATS, 
                                                      MF_BYCOMMAND | MF_GRAYED);
    EnableMenuItem(hSubMenu, IDM_COMMANDS_SHOW_REPORT, 
                                                      MF_BYCOMMAND | MF_GRAYED);
    
    // Update toolbar items status.
    hwndTB = GetDlgItem (g_hWnd, ID_TOOLBAR);
    SendMessage(hwndTB, TB_HIDEBUTTON, ID_FILE_OPEN, MAKELONG(FALSE, 0));
    SendMessage(hwndTB, TB_HIDEBUTTON, ID_START_OR_STOP_TEST, 
                                                            MAKELONG(FALSE, 0));
    SendMessage(hwndTB, TB_HIDEBUTTON, ID_DETAILED_STATS, MAKELONG(FALSE, 0));
                                                     
    // Desttoy controls from the previous mode before construction controls for 
    // this mode.
    switch (g_nMode)
    {
        case 1:
            if(g_hwndEdit!=NULL) DestroyWindow(g_hwndEdit);
            //if(g_hwndEdit2!=NULL) DestroyWindow(g_hwndEdit2);

            DestroyWindow(g_hwndEdit);
            DestroyWindow(g_hwndMode1_ListBox_LeftHandWords);
            DestroyWindow(g_hwndMode1_ListBox_RightHandWords);
            DestroyWindow(g_hwndMode1_ListBox_Slowest);
            DestroyWindow(g_hwndMode1_ListBox_Fastest);
            DestroyWindow(g_hwndMode1_ListBox_MostFrequent);
            DestroyWindow(g_hwndMode1_ListBox_LeastFrequent);
            DestroyWindow(g_hwndMode1_Static_Slowest);
            DestroyWindow(g_hwndMode1_Static_Fastest);
            DestroyWindow(g_hwndMode1_Static_MostFrequent);
            DestroyWindow(g_hwndMode1_Static_LeastFrequent);
            DestroyWindow(g_hwndMode1_Static_TypingSpeed);
            DestroyWindow(g_hwndMode1_Static_WordsTyped);
            DestroyWindow(g_hwndMode1_Static_CharsTyped); 
            DestroyWindow(g_hwndMode1_Static_ErrorsCount); 
            DestroyWindow(g_hwndMode1_Button_Repopulate);
            DestroyWindow(g_hwndMode1_Button_ShowLog);
            break;
        case 2: // same code as for case 1 
            if(g_hwndEdit!=NULL) DestroyWindow(g_hwndEdit);
            //if(g_hwndEdit2!=NULL) DestroyWindow(g_hwndEdit2);

            DestroyWindow(g_hwndEdit);
            DestroyWindow(g_hwndMode1_ListBox_LeftHandWords);
            DestroyWindow(g_hwndMode1_ListBox_RightHandWords);
            DestroyWindow(g_hwndMode1_ListBox_Slowest);
            DestroyWindow(g_hwndMode1_ListBox_Fastest);
            DestroyWindow(g_hwndMode1_ListBox_MostFrequent);
            DestroyWindow(g_hwndMode1_ListBox_LeastFrequent);
            DestroyWindow(g_hwndMode1_Static_Slowest);
            DestroyWindow(g_hwndMode1_Static_Fastest);
            DestroyWindow(g_hwndMode1_Static_MostFrequent);
            DestroyWindow(g_hwndMode1_Static_LeastFrequent);
            DestroyWindow(g_hwndMode1_Static_TypingSpeed);
            DestroyWindow(g_hwndMode1_Static_WordsTyped);
            DestroyWindow(g_hwndMode1_Static_CharsTyped); 
            DestroyWindow(g_hwndMode1_Static_ErrorsCount); 
            DestroyWindow(g_hwndMode1_Button_Repopulate);
            DestroyWindow(g_hwndMode1_Button_ShowLog);
            break;
        case 3:
            DestroyWindow(g_hwndMode3_Edit);
            DestroyWindow(g_hwndMode3_ListView);
            DestroyWindow(g_hwndMode3_ListBox_CustomWords);
            DestroyWindow(g_hwndMode3_ListBox_Slowest);
            DestroyWindow(g_hwndMode3_Static_Slowest);
            DestroyWindow(g_hwndMode3_Static_TypingSpeed);
            DestroyWindow(g_hwndMode3_Static_WordsTyped);
            DestroyWindow(g_hwndMode3_Static_CharsTyped);
            DestroyWindow(g_hwndMode3_Static_ErrorsCount);
            DestroyWindow(g_hwndMode3_Button);
            DestroyWindow(g_hwndMode3_Button_UpdateSorting);
        case 4: // test mode to test mode when language changed
            DestroyWindow(g_hwndEdit);
            DestroyWindow(g_hwndEdit2);
            DestroyWindow(g_hwndButtonTest);
            DestroyWindow(g_hwndButtonTest2);
            DestroyWindow(g_hwndMode4_Static_TimeSpent);
            DestroyWindow(g_hwndMode4_Static_TimeSpent_Value);
            DestroyWindow(g_hwndMode4_Static_CPM);
            DestroyWindow(g_hwndMode4_Static_CPM_Value);
            DestroyWindow(g_hwndMode4_Static_CharsTyped);
            DestroyWindow(g_hwndMode4_Static_CharsTyped_Value);
            DestroyWindow(g_hwndMode4_Static_WordsTyped);
            DestroyWindow(g_hwndMode4_Static_WordsTyped_Value);
            DestroyWindow(g_hwndMode4_Static_WPM);
            DestroyWindow(g_hwndMode4_Static_WPM_Value);
            DestroyWindow(g_hwndMode4_Static_TotalErrors);
            DestroyWindow(g_hwndMode4_Static_TotalErrors_Value);
            DestroyWindow(g_hwndMode4_ListView);
            DestroyWindow(g_hwndMode4_Button_CreateCustomList);
            DestroyWindow(g_hwndMode4_Button_CheckAll);
            break;
    }

    // After previous mode cleanup, set new mode indicator.
    g_nMode = 4;

    Mode4_OnResize(FALSE);

    // Specify the font to use (stored in a LOGFONT structure).
    lf.lfHeight = 16;
    lf.lfWidth = 0;
    lf.lfEscapement = 0;
    lf.lfOrientation = 0;
    lf.lfWeight = FW_DONTCARE;
    lf.lfItalic = FALSE;
    lf.lfUnderline = FALSE;
    lf.lfStrikeOut = FALSE;
    lf.lfCharSet = DEFAULT_CHARSET;
    lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
    lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
    lf.lfQuality = DEFAULT_QUALITY;
    lf.lfPitchAndFamily = FF_DONTCARE;
    _tcsncpy(lf.lfFaceName, _T("Arial") /*_T("MS Shell Dlg")*/, 32);

    hfnt = CreateFontIndirect(&lf);

    RECT rcEdit1, rcEdit2; 
    if (g_nScreenHeight != 768 /* notebook's default */)
    {
        rcEdit1.top = g_rcMode4_Edit1_Base.top;
        rcEdit1.bottom = g_rcMode4_Edit1_Base.bottom;

        rcEdit2.top = g_rcMode4_Edit2_Base.top;
        rcEdit2.bottom = g_rcMode4_Edit2_Base.bottom;
    }
    else
    {
        rcEdit1.top = g_rcMode4_Edit1_1024x768.top;
        rcEdit1.bottom = g_rcMode4_Edit1_1024x768.bottom;

        rcEdit2.top = g_rcMode4_Edit2_1024x768.top;
        rcEdit2.bottom = g_rcMode4_Edit2_1024x768.bottom;
    }

    g_hwndEdit = CreateWindow(_T("EDIT"), NULL, 
       WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPSIBLINGS | ES_MULTILINE | 
       //ES_AUTOVSCROLL, 25, 60, 500, 320, 
       ES_AUTOVSCROLL, 25, rcEdit1.top, 500, rcEdit1.bottom - rcEdit1.top, 
                                      g_hWnd, (HMENU)IDC_EDIT_MAIN, g_hInst, NULL);
    SendMessage(g_hwndEdit,  WM_SETFONT, (WPARAM)hfnt, TRUE);

    //--------------------------------------------------------------------------
    // Subclass edit control to catch WM_KEYBOARD, WM_CHAR that are not received
    // by parent's WndProc if edit control has a focus. 
    // NOTE: it works (albeit I don't see why WndProc does not get messages from
    // WM_KEYBOARD, WM_CHARedit). MSDN'03 says that "subclass window procedure 
    // enables the edit control to receive all keyboard input, including the 
    // ENTER and TAB keys, whenever the control has the input focus".
    //--------------------------------------------------------------------------

    g_wpOldWndProc = (WNDPROC)SetWindowLong(g_hwndEdit, GWL_WNDPROC, 
                                                    (DWORD)Edit_SubclassedFunc);
    g_hwndEdit2 = CreateWindow(_T("EDIT"), NULL, 
       WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPSIBLINGS | ES_MULTILINE | 
       //ES_AUTOVSCROLL, 25, 395, 500, 320, 
       ES_AUTOVSCROLL, 25, rcEdit2.top, 500, rcEdit2.bottom - rcEdit2.top,
                                      g_hWnd, (HMENU)IDC_EDIT_DEMO, g_hInst, NULL); 
    SendMessage(g_hwndEdit2,  WM_SETFONT, (WPARAM)hfnt, TRUE);

    // Returned pointer to sample text is 2-bytes (2-byte Unicode's BE byte 
    // order mark: FEFF) right-shifted so that returned pointer points to 
    // text as it should be seen (without BOM). I use lstrcpy(...) so that
    // szSampleText would also indicate not BOM-prepended text (on the other  
    // hand, Mode4_LoadFile(szSampleText) does not makes it, and 
    // szSampleText = pszText is incorrect since szSampleText is const pointer
    // and not l-value). Don't have time to figure out how to make that TCHAR
    // receives not BOM-prepended UNICODE text just in one call. This solution 
    // is one-call solution for pointers only, and required additional line for
    // TCHAR. Note that using reference for pointer is likely overkill for code 
    // safety (heaps/stacks etc), not required.
    if (g_sSampleFFN.length() == 0)
    {
        // Most typically, when default sample test is used, the g_sSampleFFN
        // string is empty.
        if (Mode4_LoadFile(pszText)) bFileLoaded = TRUE;
    }
    else
    {
        // When other than default sample test was selected with File/Open 
        // the g_sSampleFFN is not empty.
        if (Mode4_LoadFile(pszText, g_sSampleFFN.c_str())) bFileLoaded = TRUE;
    }
    
    if (bFileLoaded)
    {
        lstrcpy(szSampleText, pszText);
        lstrcpy(g_szSampleText, szSampleText); //likely avoid 

        SendMessage(g_hwndEdit2, WM_SETTEXT, 0 /* not used */, 
                                                          (LPARAM)szSampleText);
        Mode4_ParseSampleText(szSampleText);
    }

    // Create static controls for base statistics.
    g_hwndMode4_Static_TimeSpent =  CreateWindow(_T("STATIC"), NULL, 
           WS_CHILD|WS_VISIBLE, 575, 150, 200, 20, g_hWnd, NULL, g_hInst, NULL);
    SendMessage(g_hwndMode4_Static_TimeSpent, WM_SETFONT, (WPARAM)hfnt, TRUE);

    g_hwndMode4_Static_TimeSpent_Value =  CreateWindow(_T("STATIC"), NULL, 
         WS_CHILD | WS_VISIBLE, 700, 150, 200, 20, g_hWnd, NULL, g_hInst, NULL);
    SendMessage(g_hwndMode4_Static_TimeSpent_Value, WM_SETFONT, 
                                                            (WPARAM)hfnt, TRUE);

    g_hwndMode4_Static_CPM =  CreateWindow(_T("STATIC"), NULL, 
           WS_CHILD|WS_VISIBLE, 575, 170, 200, 20, g_hWnd, NULL, g_hInst, NULL);
    SendMessage(g_hwndMode4_Static_CPM, WM_SETFONT, (WPARAM)hfnt, TRUE);

    g_hwndMode4_Static_CPM_Value =  CreateWindow(_T("STATIC"), NULL, 
         WS_CHILD | WS_VISIBLE, 700, 170, 200, 20, g_hWnd, NULL, g_hInst, NULL);
    SendMessage(g_hwndMode4_Static_CPM_Value, WM_SETFONT, (WPARAM)hfnt, TRUE);

    g_hwndMode4_Static_CharsTyped =  CreateWindow(_T("STATIC"), NULL, 
           WS_CHILD|WS_VISIBLE, 575, 190, 200, 20, g_hWnd, NULL, g_hInst, NULL);
    SendMessage(g_hwndMode4_Static_CharsTyped, WM_SETFONT, (WPARAM)hfnt, 
                                                                          TRUE);
    
    g_hwndMode4_Static_CharsTyped_Value =  CreateWindow(_T("STATIC"), NULL, 
           WS_CHILD|WS_VISIBLE, 700, 190, 200, 20, g_hWnd, NULL, g_hInst, NULL);
    SendMessage(g_hwndMode4_Static_CharsTyped_Value, WM_SETFONT, (WPARAM)hfnt, 
                                                                          TRUE);

    g_hwndMode4_Static_WordsTyped =  CreateWindow(_T("STATIC"), NULL, 
           WS_CHILD|WS_VISIBLE, 575, 210, 200, 20, g_hWnd, NULL, g_hInst, NULL);
    SendMessage(g_hwndMode4_Static_WordsTyped, WM_SETFONT, (WPARAM)hfnt, 
                                                                          TRUE);

    g_hwndMode4_Static_WordsTyped_Value =  CreateWindow(_T("STATIC"), NULL, 
         WS_CHILD | WS_VISIBLE, 700, 210, 200, 20, g_hWnd, NULL, g_hInst, NULL);
    SendMessage(g_hwndMode4_Static_WordsTyped_Value, WM_SETFONT, (WPARAM)hfnt, 
                                                                          TRUE);

    g_hwndMode4_Static_WPM =  CreateWindow(_T("STATIC"), NULL, 
           WS_CHILD|WS_VISIBLE, 575, 230, 200, 20, g_hWnd, NULL, g_hInst, NULL);
    SendMessage(g_hwndMode4_Static_WPM,  WM_SETFONT, (WPARAM)hfnt, TRUE);

    g_hwndMode4_Static_WPM_Value =  CreateWindow(_T("STATIC"), NULL, 
        WS_CHILD | WS_VISIBLE, 700, 230, 200, 20, g_hWnd, NULL, g_hInst, NULL);
    SendMessage(g_hwndMode4_Static_WPM_Value,  WM_SETFONT, (WPARAM)hfnt, TRUE);

    g_hwndMode4_Static_TotalErrors =  CreateWindow(_T("STATIC"), NULL, 
           WS_CHILD|WS_VISIBLE, 575, 250, 200, 20, g_hWnd, NULL, g_hInst, NULL);
    SendMessage(g_hwndMode4_Static_TotalErrors, WM_SETFONT, (WPARAM)hfnt, 
                                                                          TRUE);

    g_hwndMode4_Static_TotalErrors_Value =  CreateWindow(_T("STATIC"), NULL, 
         WS_CHILD | WS_VISIBLE, 700, 250, 200, 20, g_hWnd, NULL, g_hInst, NULL);
    SendMessage(g_hwndMode4_Static_TotalErrors_Value, WM_SETFONT, (WPARAM)hfnt, 
                                                                          TRUE);
    App_LocalizeStrings(g_nLangID);

    m_vecCustomWords.clear();
    Mode4_GetCurrentCustomWords();

    // Get ready edit area by setting focus to it (cursor should blink).
    SetFocus(g_hwndEdit);

    if (g_bDebug) App_Debug_FunctionCallTrace("OnMenuMode4 Exited");
}

// Not NULL secind parameter is used with File/Open. 
BOOL Mode4_LoadFile(LPTSTR& pszFileText, 
                          LPCTSTR pszSampleFilePath /* default value is NULL */)
{
    if (g_bDebug) App_Debug_FunctionCallTrace("Mode4_LoadFile Entered");

    TCHAR szCurDirectory[MAX_PATH];
    TCHAR szSampleFilePath[200];
    HANDLE hFile;
    DWORD dwFileSize;
    DWORD dwBytesRead;

    #if !defined(DEVELOPMENT_STRUCTURE)
        // Product release settings. Obtain path to current directory.
        GetCurrentDirectory(MAX_PATH, szCurDirectory);
        lstrcpy(szSampleFilePath, szCurDirectory);
    #else
        lstrcpy(szSampleFilePath, APP_DIRECTORY_UNICODE);
    #endif

    if (g_nLangID == 1) 
        lstrcat(szSampleFilePath, _T("\\EN\\Tests\\BOR_Amendment_I.txt"));
    else if (g_nLangID == 2)
        lstrcat(szSampleFilePath, _T("\\FR\\Tests\\Sample.txt"));
    else if (g_nLangID == 3)
        lstrcat(szSampleFilePath, _T("\\RU\\Tests\\Sample.txt"));

    if (pszSampleFilePath == NULL)
    {
        hFile = CreateFile(szSampleFilePath, GENERIC_READ, 0 /* not shared */,            
            NULL /* no security */, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,  
                                                   NULL /* no template file */);
    }
    else
    {
        hFile = CreateFile(pszSampleFilePath, GENERIC_READ, 0 /* not shared */,            
            NULL /* no security */, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,  
                                                   NULL /* no template file */);
    }
    
    if (hFile != INVALID_HANDLE_VALUE)
    {
        dwFileSize = GetFileSize(hFile, NULL); // bytes
        
        // TODO: merge this with Mode4_VerifyFileEncoding. This function 
        // should dynamically allocate memory, check pointer, free memory.
        // I check size before ReadFile to avoid buffer overrun. 
        if (dwFileSize /* bytes */ /2 >= MAX_SAMPLE_TEXT /* characters */) 
        {
            TCHAR szMessage[100];
            wsprintf(szMessage, L"The maximum number of characters is: %d", 
                                                               MAX_SAMPLE_TEXT);
            MessageBox(0, szMessage, L"The file is too big!", 0);
            return FALSE;
        }

        if (ReadFile(hFile, pszFileText, dwFileSize, &dwBytesRead, NULL))
        {
            // Add null terminator (character count).
            pszFileText[dwFileSize/2] = '\0'; 
        }

        CloseHandle(hFile);

        if (!Mode4_VerifyFileEncoding(pszFileText)) return FALSE;

        // This shifts pointer by one wide char, but if I used LoadFile(szText)
        // (TCHAR not pointer) in caller, the zText would still has BOM. 
        // Therefore I am passing pointers and make lstrcpy(...) after  
        // LoadFile(pszText) in caller.
        pszFileText = CharNext(pszFileText);

        return  TRUE; 
    }

    if (g_bDebug) App_Debug_FunctionCallTrace("Mode4_LoadFile Exited");

    return FALSE; 
}

void OnMenuMode12(short nMode)
{
    HWND hwndTB;
    HMENU hMenu, hSubMenu;
    RECT rc;
    HFONT hfnt;
    LOGFONT lf;
    //TEXTMETRIC tm;

    // Update URL logo position (there is no yet URL logo when startuped).
    if (g_hwndURLLogo != NULL)
    {
        MoveWindow(g_hwndURLLogo, g_nMode12_URLLogoLeft + g_nLogoLeftIndent, 4, 
                                                    g_nURLLogoWidth, 20, FALSE);
    }

    // New (alpha 2) layout with edit area in the center.
    RECT rcEdit = {170, 60, 675, 342}; // width: 505, height: 282 
    RECT rcLHLB = {25, 60, 145, 360};  // width: 120, height: 300 
    RECT rcRHLB = {700, 60, 820, 360}; // width: 120, height: 300 

    // Destroy controls from the previous mode before construction controls for 
    // this mode.
    switch (g_nMode)
    {
        case 1:
            DestroyWindow(g_hwndEdit);
            DestroyWindow(g_hwndMode1_ListBox_LeftHandWords);
            DestroyWindow(g_hwndMode1_ListBox_RightHandWords);
            DestroyWindow(g_hwndMode1_ListBox_Slowest);
            DestroyWindow(g_hwndMode1_ListBox_Fastest);
            DestroyWindow(g_hwndMode1_ListBox_MostFrequent);
            DestroyWindow(g_hwndMode1_ListBox_LeastFrequent);
            DestroyWindow(g_hwndMode1_Static_Slowest);
            DestroyWindow(g_hwndMode1_Static_Fastest);
            DestroyWindow(g_hwndMode1_Static_MostFrequent);
            DestroyWindow(g_hwndMode1_Static_LeastFrequent);
            DestroyWindow(g_hwndMode1_Static_TypingSpeed);
            DestroyWindow(g_hwndMode1_Static_WordsTyped);
            DestroyWindow(g_hwndMode1_Static_CharsTyped); 
            DestroyWindow(g_hwndMode1_Static_ErrorsCount); 
            DestroyWindow(g_hwndMode1_Button_Repopulate);
            DestroyWindow(g_hwndMode1_Button_ShowLog);
            break;
        case 2: // same code as for case 1
            DestroyWindow(g_hwndEdit);
            DestroyWindow(g_hwndMode1_ListBox_LeftHandWords);
            DestroyWindow(g_hwndMode1_ListBox_RightHandWords);
            DestroyWindow(g_hwndMode1_ListBox_Slowest);
            DestroyWindow(g_hwndMode1_ListBox_Fastest);
            DestroyWindow(g_hwndMode1_ListBox_MostFrequent);
            DestroyWindow(g_hwndMode1_ListBox_LeastFrequent);
            DestroyWindow(g_hwndMode1_Static_Slowest);
            DestroyWindow(g_hwndMode1_Static_Fastest);
            DestroyWindow(g_hwndMode1_Static_MostFrequent);
            DestroyWindow(g_hwndMode1_Static_LeastFrequent);
            DestroyWindow(g_hwndMode1_Static_TypingSpeed);
            DestroyWindow(g_hwndMode1_Static_WordsTyped);
            DestroyWindow(g_hwndMode1_Static_CharsTyped); 
            DestroyWindow(g_hwndMode1_Static_ErrorsCount); 
            DestroyWindow(g_hwndMode1_Button_Repopulate);
            DestroyWindow(g_hwndMode1_Button_ShowLog);
            break;
        case 3:
            DestroyWindow(g_hwndMode3_Edit);
            DestroyWindow(g_hwndMode3_ListView);
            DestroyWindow(g_hwndMode3_ListBox_CustomWords);
            DestroyWindow(g_hwndMode3_ListBox_Slowest);
            DestroyWindow(g_hwndMode3_Static_Slowest);
            DestroyWindow(g_hwndMode3_Static_TypingSpeed);
            DestroyWindow(g_hwndMode3_Static_WordsTyped);
            DestroyWindow(g_hwndMode3_Static_CharsTyped);
            DestroyWindow(g_hwndMode3_Static_ErrorsCount);
            DestroyWindow(g_hwndMode3_Button);
            break;
        case 4:
            DestroyWindow(g_hwndEdit);
            DestroyWindow(g_hwndEdit2);
            DestroyWindow(g_hwndButtonTest);
            DestroyWindow(g_hwndButtonTest2);
            DestroyWindow(g_hwndMode4_Static_TimeSpent);
            DestroyWindow(g_hwndMode4_Static_TimeSpent_Value);
            DestroyWindow(g_hwndMode4_Static_CPM);
            DestroyWindow(g_hwndMode4_Static_CPM_Value);
            DestroyWindow(g_hwndMode4_Static_CharsTyped);
            DestroyWindow(g_hwndMode4_Static_CharsTyped_Value);
            DestroyWindow(g_hwndMode4_Static_WordsTyped);
            DestroyWindow(g_hwndMode4_Static_WordsTyped_Value);
            DestroyWindow(g_hwndMode4_Static_WPM);
            DestroyWindow(g_hwndMode4_Static_WPM_Value);
            DestroyWindow(g_hwndMode4_Static_TotalErrors);
            DestroyWindow(g_hwndMode4_Static_TotalErrors_Value);
            DestroyWindow(g_hwndMode4_ListView);
            DestroyWindow(g_hwndMode4_Button_CreateCustomList);
            DestroyWindow(g_hwndMode4_Button_CheckAll);
            break;
    }   
    
    g_nMode = nMode;

    // Update modes menu status.
    hMenu = GetMenu(g_hWnd);

    // Disable File/Open, which is used in test Mode only.
    hSubMenu = GetSubMenu(hMenu, 0 /* File menu */);
    EnableMenuItem(hSubMenu, IDM_FILE_OPEN, MF_BYCOMMAND | MF_GRAYED);

    hSubMenu = GetSubMenu(hMenu, 2 /* Modes menu */);
    if (g_nMode == 1)
    {
        EnableMenuItem(hSubMenu, IDM_MODES_LH_RH_WORDS, 
                                                      MF_BYCOMMAND | MF_GRAYED);
        EnableMenuItem(hSubMenu, IDM_MODES_LH_RH_WORDS_BOTH_HANDS, 
                                                     MF_BYCOMMAND | MF_ENABLED);
    }
    else if (g_nMode == 2)
    {
        EnableMenuItem(hSubMenu, IDM_MODES_LH_RH_WORDS, 
                                                     MF_BYCOMMAND | MF_ENABLED);
        EnableMenuItem(hSubMenu, IDM_MODES_LH_RH_WORDS_BOTH_HANDS, 
                                                      MF_BYCOMMAND | MF_GRAYED);
    }
    //EnableMenuItem(hSubMenu, IDM_MODES_CUSTOM, MF_BYCOMMAND | MF_ENABLED);
    Mode3_SetCustomModeItemState();
    EnableMenuItem(hSubMenu, IDM_MODES_TEST, MF_BYCOMMAND | MF_ENABLED);

    hSubMenu = GetSubMenu(hMenu, 3 /* Commands menu */);
    EnableMenuItem(hSubMenu, IDM_COMMANDS_START_OR_STOP_TEST, 
                                                      MF_BYCOMMAND | MF_GRAYED);
    EnableMenuItem(hSubMenu, IDM_COMMANDS_CLEAR_TEST_AREA, 
                                                      MF_BYCOMMAND | MF_GRAYED);
    EnableMenuItem(hSubMenu, IDM_COMMANDS_DETAILED_STATS, 
                                                      MF_BYCOMMAND | MF_GRAYED);
    EnableMenuItem(hSubMenu, IDM_COMMANDS_SHOW_REPORT, 
                                                      MF_BYCOMMAND | MF_GRAYED);

    // Update toolbar items status.
    hwndTB = GetDlgItem (g_hWnd, ID_TOOLBAR);
    SendMessage(hwndTB, TB_HIDEBUTTON, ID_FILE_OPEN, MAKELONG(TRUE, 0));
    SendMessage(hwndTB, TB_HIDEBUTTON, ID_START_OR_STOP_TEST, 
                                                             MAKELONG(TRUE, 0));
    SendMessage(hwndTB, TB_HIDEBUTTON, ID_DETAILED_STATS, MAKELONG(TRUE, 0));

 
    // Set mode 12 window size.
    GetWindowRect(g_hWnd, &rc);
    SetWindowPos(g_hWnd, NULL, 0, 0, 860 /*900*/, /* rc.bottom - rc.top*/ 450, 
                                                       SWP_NOZORDER | SWP_NOMOVE);

    //if(g_hwndEdit!=NULL) DestroyWindow(g_hwndEdit);
    //if(g_hwndEdit2!=NULL) DestroyWindow(g_hwndEdit2);
    //if(g_hwndButtonTest!=NULL) DestroyWindow(g_hwndButtonTest);
    //if(g_hwndButtonTest2!=NULL) DestroyWindow(g_hwndButtonTest2);

 //   if(g_hwndMode4_Static_TimeSpent!=NULL) 
    //	                            DestroyWindow(g_hwndMode4_Static_TimeSpent);
 //   if(g_hwndMode4_Static_CPM!=NULL) DestroyWindow(g_hwndMode4_Static_CPM);
    //if(g_hwndMode4_Static_CharsTyped!=NULL) 
    //	                           DestroyWindow(g_hwndMode4_Static_CharsTyped);
    //if(g_hwndMode4_Static_WordsTyped!=NULL) 
    //	                           DestroyWindow(g_hwndMode4_Static_WordsTyped);
    //if(g_hwndMode4_Static_WPM!=NULL) DestroyWindow(g_hwndMode4_Static_WPM);
    //if(g_hwndMode4_Static_TotalErrors!=NULL) 
    //	                          DestroyWindow(g_hwndMode4_Static_TotalErrors);
    
    // Specify the font to use (stored in a LOGFONT structure).
    lf.lfHeight = 20;
    lf.lfWidth = 0;
    lf.lfEscapement = 0;
    lf.lfOrientation = 0;
    lf.lfWeight = FW_DONTCARE;
    lf.lfItalic = FALSE;
    lf.lfUnderline = FALSE;
    lf.lfStrikeOut = FALSE;
    lf.lfCharSet = DEFAULT_CHARSET;
    lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
    lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
    lf.lfQuality = DEFAULT_QUALITY;
    lf.lfPitchAndFamily = FF_DONTCARE;
    _tcsncpy(lf.lfFaceName, _T("MS Shell Dlg"), 32);

    hfnt = CreateFontIndirect(&lf);

    g_hwndEdit = CreateWindow(_T("EDIT"), NULL, 
        WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL | 
        WS_CLIPSIBLINGS, rcEdit.left, rcEdit.top, rcEdit.right - rcEdit.left, 
        rcEdit.bottom - rcEdit.top, g_hWnd, (HMENU)IDC_EDIT_MAIN, g_hInst, 
                                                                          NULL);

    //SetWindowFont(g_hwndEdit, hfnt, TRUE); // Can't find description in MSDN
    SendMessage(g_hwndEdit,  WM_SETFONT, (WPARAM)hfnt, TRUE);

   //---------------------------------------------------------------------------
   // Subclass edit control to catch WM_KEYBOARD, WM_CHAR that are not received
   // by parent's WndProc if edit control has a focus. 
   // NOTE: it works (albeit I don't see why WndProc does not get messages from
   // WM_KEYBOARD, WM_CHARedit). MSDN'03 says that "subclass window procedure 
   // enables the edit control to receive all keyboard input, including the 
   // ENTER and TAB keys, whenever the control has the input focus".
   //---------------------------------------------------------------------------

   g_wpOldWndProc = (WNDPROC)SetWindowLong(g_hwndEdit, GWL_WNDPROC, 
                                                   (DWORD)Edit_SubclassedFunc );

   g_hwndMode1_ListBox_LeftHandWords = CreateWindow(_T("LISTBOX"), NULL, 
       WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL | WS_CLIPSIBLINGS, 
       rcLHLB.left, rcLHLB.top, rcLHLB.right - rcLHLB.left, 
       rcLHLB.bottom - rcLHLB.top, g_hWnd, (HMENU)IDC_LISTBOX_LEFT_HAND_WORDS, 
                                                                 g_hInst, NULL);
   SendMessage(g_hwndMode1_ListBox_LeftHandWords, WM_SETFONT, (WPARAM)hfnt, 
                                                                          TRUE);
   
   g_hwndMode1_ListBox_RightHandWords = CreateWindow(_T("LISTBOX"), NULL, 
       WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL | WS_CLIPSIBLINGS, 
       rcRHLB.left, rcRHLB.top, rcRHLB.right - rcRHLB.left, 
       rcRHLB.bottom - rcRHLB.top, g_hWnd, (HMENU)IDC_LISTBOX_RIGHT_HAND_WORDS, 
                                                                 g_hInst, NULL);
   SendMessage(g_hwndMode1_ListBox_RightHandWords,  WM_SETFONT, (WPARAM)hfnt, 
                                                                          TRUE);
   
   PopulateWithTrainingWords(g_nMode, g_nLangID);
   InitVector();

   App_LocalizeStrings(g_nLangID);
   App_CenterWindow(g_hWnd, GetDesktopWindow());

   // Get ready edit area by setting focus to it (cursor should blink).
   SetFocus(g_hwndEdit);
}

void OnMenuMode3()
{
    HMENU hMenu, hSubMenu;
    HWND hwndTB;
    RECT rc;
    HFONT hfnt;
    LOGFONT lf;
    //TEXTMETRIC tm;
    DWORD dwExStyles;
    LVCOLUMN lvc;

    // Update URL logo position (there is no yet URL logo when startuped)
    if (g_hwndURLLogo != NULL)
    {
        MoveWindow(g_hwndURLLogo, g_nMode3_URLLogoLeft + g_nLogoLeftIndent, 
                                                 4, g_nURLLogoWidth, 20, FALSE);
    }

    // Re-initialization of globals.
    //g_nTotalErrorsCount = 0;
    g_nErrorsCount = 0;

    // Update modes menu status.
    hMenu = GetMenu(g_hWnd);

    // Disable File/Open, which is used in test Mode only.
    hSubMenu = GetSubMenu(hMenu, 0 /* File menu */);
    EnableMenuItem(hSubMenu, IDM_FILE_OPEN, MF_BYCOMMAND | MF_GRAYED);

    hSubMenu = GetSubMenu(hMenu, 2 /* Modes menu */);
    EnableMenuItem(hSubMenu, IDM_MODES_LH_RH_WORDS, MF_BYCOMMAND | MF_ENABLED);
    EnableMenuItem(hSubMenu, IDM_MODES_LH_RH_WORDS_BOTH_HANDS, 
                                                     MF_BYCOMMAND | MF_ENABLED);
    EnableMenuItem(hSubMenu, IDM_MODES_CUSTOM, MF_BYCOMMAND | MF_GRAYED);
    EnableMenuItem(hSubMenu, IDM_MODES_TEST, MF_BYCOMMAND | MF_ENABLED);

    hSubMenu = GetSubMenu(hMenu, 3 /* Commands menu */);
    EnableMenuItem(hSubMenu, IDM_COMMANDS_START_OR_STOP_TEST, 
                                                      MF_BYCOMMAND | MF_GRAYED);
    EnableMenuItem(hSubMenu, IDM_COMMANDS_CLEAR_TEST_AREA, 
                                                      MF_BYCOMMAND | MF_GRAYED);
    EnableMenuItem(hSubMenu, IDM_COMMANDS_DETAILED_STATS, 
                                                      MF_BYCOMMAND | MF_GRAYED);
    EnableMenuItem(hSubMenu, IDM_COMMANDS_SHOW_REPORT, 
                                                      MF_BYCOMMAND | MF_GRAYED);

    //if (g_nMode == 4) RemoveMenu(hMenu, 3, MF_BYPOSITION /* commands menu */);

    // Update toolbar items status.
    hwndTB = GetDlgItem (g_hWnd, ID_TOOLBAR);
    SendMessage(hwndTB, TB_HIDEBUTTON, ID_FILE_OPEN, MAKELONG(TRUE, 0));
    SendMessage(hwndTB, TB_HIDEBUTTON, ID_START_OR_STOP_TEST, 
                                                             MAKELONG(TRUE, 0));
    SendMessage(hwndTB, TB_HIDEBUTTON, ID_DETAILED_STATS, MAKELONG(TRUE, 0));


    // Destroy controls from the previous mode before construction controls for 
    // this mode.
    switch (g_nMode)
    {
        case 1:
            //if (g_hwndEdit != NULL) 
            DestroyWindow(g_hwndEdit);
            //if (g_hwndMode1_ListBox_LeftHandWords != NULL)
            DestroyWindow(g_hwndMode1_ListBox_LeftHandWords);
            //if (g_hwndMode1_ListBox_RightHandWords != NULL)
            DestroyWindow(g_hwndMode1_ListBox_RightHandWords);
            //if (g_hwndMode1_ListBox_Slowest != NULL)
            DestroyWindow(g_hwndMode1_ListBox_Slowest);
            //if (g_hwndMode1_ListBox_Fastest != NULL)
            DestroyWindow(g_hwndMode1_ListBox_Fastest);
            //if (g_hwndMode1_ListBox_MostFrequent != NULL)
            DestroyWindow(g_hwndMode1_ListBox_MostFrequent);
            //if (g_hwndMode1_ListBox_LeastFrequent != NULL)
            DestroyWindow(g_hwndMode1_ListBox_LeastFrequent);
            //if (g_hwndMode1_Static_Slowest != NULL)
            DestroyWindow(g_hwndMode1_Static_Slowest);
            //if (g_hwndMode1_Static_Fastest != NULL)
            DestroyWindow(g_hwndMode1_Static_Fastest);
            //if (g_hwndMode1_Static_MostFrequent != NULL)
            DestroyWindow(g_hwndMode1_Static_MostFrequent);
            //if (g_hwndMode1_Static_LeastFrequent != NULL)
            DestroyWindow(g_hwndMode1_Static_LeastFrequent);
            //if (g_hwndMode1_Static_TypingSpeed!= NULL)
            DestroyWindow(g_hwndMode1_Static_TypingSpeed);
            //if (g_hwndMode1_Static_WordsTyped != NULL)
            DestroyWindow(g_hwndMode1_Static_WordsTyped);
            //if (g_hwndMode1_Static_CharsTyped != NULL)
            DestroyWindow(g_hwndMode1_Static_CharsTyped);
            //if (g_hwndMode1_Static_ErrorsCount != NULL)
            DestroyWindow(g_hwndMode1_Static_ErrorsCount);
            //if (g_hwndMode1_Button_Repopulate != NULL)
            DestroyWindow(g_hwndMode1_Button_Repopulate);
            //if (g_hwndMode1_Button_ShowLog != NULL)
            DestroyWindow(g_hwndMode1_Button_ShowLog);
            break;
        case 2:// same code as for case 1
            //if (g_hwndEdit != NULL) 
            DestroyWindow(g_hwndEdit);
            //if (g_hwndMode1_ListBox_LeftHandWords != NULL)
            DestroyWindow(g_hwndMode1_ListBox_LeftHandWords);
            //if (g_hwndMode1_ListBox_RightHandWords != NULL)
            DestroyWindow(g_hwndMode1_ListBox_RightHandWords);
            //if (g_hwndMode1_ListBox_Slowest != NULL)
            DestroyWindow(g_hwndMode1_ListBox_Slowest);
            //if (g_hwndMode1_ListBox_Fastest != NULL)
            DestroyWindow(g_hwndMode1_ListBox_Fastest);
            //if (g_hwndMode1_ListBox_MostFrequent != NULL)
            DestroyWindow(g_hwndMode1_ListBox_MostFrequent);
            //if (g_hwndMode1_ListBox_LeastFrequent != NULL)
            DestroyWindow(g_hwndMode1_ListBox_LeastFrequent);
            //if (g_hwndMode1_Static_Slowest != NULL)
            DestroyWindow(g_hwndMode1_Static_Slowest);
            //if (g_hwndMode1_Static_Fastest != NULL)
            DestroyWindow(g_hwndMode1_Static_Fastest);
            //if (g_hwndMode1_Static_MostFrequent != NULL)
            DestroyWindow(g_hwndMode1_Static_MostFrequent);
            //if (g_hwndMode1_Static_LeastFrequent != NULL)
            DestroyWindow(g_hwndMode1_Static_LeastFrequent);
            //if (g_hwndMode1_Static_TypingSpeed!= NULL)
            DestroyWindow(g_hwndMode1_Static_TypingSpeed);
            //if (g_hwndMode1_Static_WordsTyped != NULL)
            DestroyWindow(g_hwndMode1_Static_WordsTyped);
            //if (g_hwndMode1_Static_CharsTyped != NULL)
            DestroyWindow(g_hwndMode1_Static_CharsTyped);
            //if (g_hwndMode1_Static_ErrorsCount != NULL)
            DestroyWindow(g_hwndMode1_Static_ErrorsCount);
            //if (g_hwndMode1_Button_Repopulate != NULL)
            DestroyWindow(g_hwndMode1_Button_Repopulate);
            //if (g_hwndMode1_Button_ShowLog != NULL)
            DestroyWindow(g_hwndMode1_Button_ShowLog);
            break;
        case 4:
            //if (g_hwndEdit != NULL) DestroyWindow(g_hwndEdit);
            //if (g_hwndEdit2 != NULL) DestroyWindow(g_hwndEdit2);
            //if (g_hwndButtonTest != NULL) DestroyWindow(g_hwndButtonTest);
            //if (g_hwndButtonTest2 != NULL) DestroyWindow(g_hwndButtonTest2);
            //if (g_hwndMode4_Static_TimeSpent != NULL) 
            //						DestroyWindow(g_hwndMode4_Static_TimeSpent);
            //if (g_hwndMode4_Static_CPM != NULL) 
            //	                          DestroyWindow(g_hwndMode4_Static_CPM);
            //if (g_hwndMode4_Static_CharsTyped != NULL) 
            //					   DestroyWindow(g_hwndMode4_Static_CharsTyped);
            //if (g_hwndMode4_Static_WordsTyped != NULL) 
            //					   DestroyWindow(g_hwndMode4_Static_WordsTyped);
            //if (g_hwndMode4_Static_WPM != NULL) 
            //	                          DestroyWindow(g_hwndMode4_Static_WPM);
            //if (g_hwndMode4_Static_TotalErrors!=NULL) 
            //	                  DestroyWindow(g_hwndMode4_Static_TotalErrors);
            //if (g_hwndMode4_ListView != NULL) 
            //	                            DestroyWindow(g_hwndMode4_ListView);
            //if (g_hwndMode4_Button_CreateCustomList != NULL) 
            //	              DestroyWindow(g_hwndMode4_Button_CreateCustomList);
            //if (g_hwndMode4_Button_CheckAll != NULL) 
            //	                      DestroyWindow(g_hwndMode4_Button_CheckAll);
            DestroyWindow(g_hwndEdit);
            DestroyWindow(g_hwndEdit2);
            DestroyWindow(g_hwndButtonTest);
            DestroyWindow(g_hwndButtonTest2);
            DestroyWindow(g_hwndMode4_Static_TimeSpent);
            DestroyWindow(g_hwndMode4_Static_TimeSpent_Value);
            DestroyWindow(g_hwndMode4_Static_CPM);
            DestroyWindow(g_hwndMode4_Static_CPM_Value);
            DestroyWindow(g_hwndMode4_Static_CharsTyped);
            DestroyWindow(g_hwndMode4_Static_CharsTyped_Value);
            DestroyWindow(g_hwndMode4_Static_WordsTyped);
            DestroyWindow(g_hwndMode4_Static_WordsTyped_Value);
            DestroyWindow(g_hwndMode4_Static_WPM);
            DestroyWindow(g_hwndMode4_Static_WPM_Value);
            DestroyWindow(g_hwndMode4_Static_TotalErrors);
            DestroyWindow(g_hwndMode4_Static_TotalErrors_Value);
            DestroyWindow(g_hwndMode4_ListView);
            DestroyWindow(g_hwndMode4_Button_CreateCustomList);
            DestroyWindow(g_hwndMode4_Button_CheckAll);

            break;
    }

    // After previous mode cleanup, set new mode indicator.
    g_nMode = 3;

    // Set mode 3 window size.
    GetWindowRect(g_hWnd, &rc);
    //SetWindowPos(g_hWnd, NULL, 0, 0, 560, 820,
    SetWindowPos(g_hWnd, NULL, 0, 0, 560, g_nClientHeight,
                                                     SWP_NOZORDER | SWP_NOMOVE);
    // Construct controls.
    RECT rcEdit, rcListView; 
    if (g_nScreenHeight != 768 /* notebook's default */)
    {
        rcEdit.top = g_rcMode3_Edit_Base.top;
        rcEdit.bottom = g_rcMode3_Edit_Base.bottom;

        rcListView.top = g_rcMode3_ListView_Base.top;
        rcListView.bottom = g_rcMode3_ListView_Base.bottom;
    }
    else
    {
        rcEdit.top = g_rcMode3_Edit_1024x768.top;
        rcEdit.bottom = g_rcMode3_Edit_1024x768.bottom;

        rcListView.top = g_rcMode3_ListView_1024x768.top;
        rcListView.bottom = g_rcMode3_ListView_1024x768.bottom;
    }

    g_hwndMode3_Edit = CreateWindow(_T("EDIT"), NULL, 
       WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL | 
       //WS_CLIPSIBLINGS, 25, 60, 500, 200, g_hWnd, (HMENU)IDC_EDIT_MAIN, 
       WS_CLIPSIBLINGS, 25, rcEdit.top, 500, rcEdit.bottom - rcEdit.top, 
                                   g_hWnd, (HMENU)IDC_EDIT_MAIN, g_hInst, NULL); 

    // Specify the font to use (stored in a LOGFONT structure).
    lf.lfHeight = 20;
    lf.lfWidth = 0;
    lf.lfEscapement = 0;
    lf.lfOrientation = 0;
    lf.lfWeight = FW_DONTCARE;
    lf.lfItalic = FALSE;
    lf.lfUnderline = FALSE;
    lf.lfStrikeOut = FALSE;
    lf.lfCharSet = DEFAULT_CHARSET;
    lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
    lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
    lf.lfQuality = DEFAULT_QUALITY;
    lf.lfPitchAndFamily = FF_DONTCARE;
    _tcsncpy(lf.lfFaceName, _T("MS Shell Dlg"), 32);

    hfnt = CreateFontIndirect(&lf); 
   
    SendMessage(g_hwndMode3_Edit,  WM_SETFONT, (WPARAM)hfnt, TRUE);

    //--------------------------------------------------------------------------
    // Subclass edit control to catch WM_KEYBOARD, WM_CHAR that are not received
    // by parent's WndProc if edit control has a focus. 
    // NOTE: it works (albeit I don't see why WndProc does not get messages from
    // WM_KEYBOARD, WM_CHARedit). MSDN'03 says that "subclass window procedure 
    // enables the edit control to receive all keyboard input, including the 
    // ENTER and TAB keys, whenever the control has the input focus".
    //--------------------------------------------------------------------------

    g_wpOldWndProc = (WNDPROC)SetWindowLong(g_hwndMode3_Edit, GWL_WNDPROC, 
                                                    (DWORD)Edit_SubclassedFunc);

    // Create and populate list view with custom words.
    g_hwndMode3_ListView = CreateWindow(WC_LISTVIEW, NULL, 
        WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPSIBLINGS | LVS_REPORT, 
        //25, 280, 500, 435, g_hWnd, (HMENU)IDC_CUSTOM_MODE_LISTVIEW, g_hInst,
        25, rcListView.top, 500, rcListView.bottom - rcListView.top, g_hWnd, 
                                (HMENU)IDC_CUSTOM_MODE_LISTVIEW, g_hInst, NULL);

    // Works with CreateWindow-created (without Ex) window.
    dwExStyles = LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT;
    ListView_SetExtendedListViewStyle(g_hwndMode3_ListView, dwExStyles);

    // Setup list view colums.
    lvc.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_FMT;

    lvc.iSubItem = 0;
    _tcscpy(g_szBuffer, 
        App_GetStringFromStringTable(IDS_CUSTOM_MODE_LV_COLUMN_WORD, 
                                                                    g_nLangID));
    lvc.pszText = g_szBuffer;
    lvc.cx = 100; 
    ListView_InsertColumn(g_hwndMode3_ListView, 0, &lvc);

    lvc.iSubItem = 1;
    _tcscpy(g_szBuffer, 
        App_GetStringFromStringTable(IDS_CUSTOM_MODE_LV_COLUMN_LENGTH, 
                                                                    g_nLangID));
    lvc.pszText = g_szBuffer;
    lvc.fmt = LVCFMT_CENTER;
    lvc.cx = 60;
    ListView_InsertColumn(g_hwndMode3_ListView, 1, &lvc);

    lvc.iSubItem = 2;
    _tcscpy(g_szBuffer, 
        App_GetStringFromStringTable(IDS_CUSTOM_MODE_LV_COLUMN_SPEED, 
                                                                    g_nLangID));
    lvc.pszText = g_szBuffer;	
    lvc.fmt = LVCFMT_CENTER;
    lvc.cx = 95;  
    ListView_InsertColumn(g_hwndMode3_ListView, 2, &lvc);

    lvc.iSubItem = 3;
    _tcscpy(g_szBuffer, 
        App_GetStringFromStringTable(IDS_CUSTOM_MODE_LV_COLUMN_ERRORS, 
                                                                    g_nLangID));
    lvc.pszText = g_szBuffer;
    lvc.fmt = LVCFMT_CENTER;
    lvc.cx = 80;  
    ListView_InsertColumn(g_hwndMode3_ListView, 3, &lvc);

    lvc.iSubItem = 4;
    _tcscpy(g_szBuffer, 
        App_GetStringFromStringTable(IDS_CUSTOM_MODE_LV_COLUMN_COUNT, 
                                                                    g_nLangID));
    lvc.pszText = g_szBuffer;
    lvc.fmt = LVCFMT_CENTER;
    lvc.cx = 80;  
    ListView_InsertColumn(g_hwndMode3_ListView, 4, &lvc);

    Mode3_PopulateWithTrainingWords(g_nLangID);

    // Call after popiulateting list view for proper SB message.
    App_LocalizeStrings(g_nLangID); 

    App_CenterWindow(g_hWnd, GetDesktopWindow());

    // Get ready edit area by setting focus to it (cursor should blink).
    SetFocus(g_hwndMode3_Edit);
}

void OnButtonTest(LPCTSTR pszText, int nCharPos)
{
    HBRUSH hbrush;
    HDC hDC;
    LRESULT lRes;
    //PAINTSTRUCT ps;
    RECT rc;

    rc.left = 0; rc.top = 0; rc.right =  60; rc.bottom = 40;

    //The return value contains the client area coordinates of the character. 
    // The low-order word contains the horizontal coordinate and the high-order 
    // word contains the vertical coordinate. 

    lRes = SendMessage(g_hwndEdit2, EM_POSFROMCHAR, (WPARAM)nCharPos, 
                                                              0 /* not used */);

    TCHAR szTest[100];
    wsprintf(szTest, _T("%d %d"), LOWORD(lRes), HIWORD(lRes));
    //MessageBox(0, pszText, pszText, 0);

    rc.left = 0; rc.top = HIWORD(lRes); rc.right = LOWORD(lRes); rc.bottom = 40; //rc.right =  60;
    //rc.left = 0; rc.top = 2; rc.right = LOWORD(lRes); rc.bottom = 40; //rc.right =  60;
    

    hbrush = CreateSolidBrush(RGB(255,0,0));
    hDC = GetWindowDC(g_hwndEdit2); //GetDC (g_hWnd); //
    //HBRUSH hbrushOld = SelectObject(g_hWnd, hbrush); 
    FillRect(hDC, &rc, hbrush);

    //SetTextColor(hdcChild, RGB(0,0,255));
    SetBkColor(hDC, RGB(0,128,0));
    
    if(lstrlen(pszText)==0)
    {
        DrawText(hDC, _T("szCaption"), -1, &rc, DT_EDITCONTROL); //DT_LEFT, DT_WORDBREAK |DT_VCENTER
    }
    else
    {
        DrawText(hDC, pszText, -1, &rc, DT_EDITCONTROL);
    }

    //BeginPaint();

    DeleteObject(hbrush); 
    ReleaseDC(g_hwndEdit2, hDC);
    //EndPaint();
}

void Mode4_OnStartTest()
{
    TCHAR szBuffer[MAX_SAMPLE_TEXT]; 
    HMENU hMenu, hSubMenu; 
    wstring sBuffer;
    short nPos;
    int nTextLength;
    short nWordsCount;
    float fTimeSpent; // sec
    char szAnsiBuffer[200];
    TCHAR szText[200];
    g_bTestCompleted = FALSE; // flag to paint background of base stats window
    RECT rc, rcMode4_Test = {0, 0, 560, 820};
    TEST_RESULTS tr;

    // There is a possibility that new test is started after previous test while
    // staying in Mode4 (test mode). In this case, resize the window to original
    // test size (this also repaints controls in base stats window hiding them 
    // after Mode4_ClearBaseStats). The snippet is basically from 
    // Mode4_OnResize(FALSE) except it does not center main window.
    GetWindowRect(g_hWnd, &rc);
    if (rc.right - rc.left != rcMode4_Test.right - rcMode4_Test.left)
    {
        Mode4_ClearBaseStats();
        
        if (g_hwndURLLogo != NULL)
        {
            MoveWindow(g_hwndURLLogo, 
                g_nMode4_URLLogoLeft + g_nLogoLeftIndent, 4, g_nURLLogoWidth, 
                                                                     20, FALSE);
        }
        SetWindowPos(g_hWnd, NULL, 0, 0, 560, 820,  SWP_NOZORDER|SWP_NOMOVE);
    }
    
    if (!g_bTestRunning)
    {
        hMenu = GetMenu(g_hWnd);
        hSubMenu = GetSubMenu(hMenu, 3 /* Commands menu */);
        EnableMenuItem(hSubMenu, IDM_COMMANDS_DETAILED_STATS, 
                                                      MF_BYCOMMAND | MF_GRAYED);  
        EnableMenuItem(hSubMenu, IDM_COMMANDS_SHOW_REPORT, 
                                                      MF_BYCOMMAND | MF_GRAYED);   

        g_bTestRunning = TRUE;
        
        SendMessage(g_hwndButtonTest2, WM_SETTEXT, 0 /* not used */, 
                (LPARAM)App_GetStringFromStringTable(IDS_BUTTON_STOP_TIMER, 1));

        g_nTickCountStart = GetTickCount();
        g_nCharsTyped = 0; g_nTotalErrorsCount = 0;
        SendMessage(g_hwndMode4_Static_TimeSpent, WM_SETTEXT, 0 /* not used */, 
                                                                    (LPARAM)"");
        SendMessage(g_hwndMode4_Static_TimeSpent_Value, WM_SETTEXT, 
                                                  0 /* not used */, (LPARAM)"");
    }
    else
    {
        g_bTestRunning = FALSE;

        // Set button text.
        SendMessage(g_hwndButtonTest2, WM_SETTEXT, 0 /* not used */, 
               (LPARAM)App_GetStringFromStringTable(IDS_BUTTON_START_TIMER, 1));
        g_nTickCountFinish = GetTickCount();
        
        // wsprintf makes "f" (overflow) for %.2f, use sprintf instead.
        fTimeSpent = (float)(g_nTickCountFinish - g_nTickCountStart)/1000.;

        // wsprintf does not have format option for double like sprintf's %.2f 
        // (if %.2f used with wsprintf the output is f). Thus, I employ a trick 
        // by using ANSI's sprintf and converting resulting ANSI string to 
        // UNICODE with MultiByteToWideChar.
        sprintf(szAnsiBuffer, "%.2f", fTimeSpent/60.); 
        tr.fTimeSpent = fTimeSpent/60.; // keeps precision

        MultiByteToWideChar(CP_ACP, 0 /* must be zero [TODO: clarify] */, 
            szAnsiBuffer, -1 /* string NULL-terminated [TODO: clarify] */, 
                                                        szText, sizeof(szText)); 

        SendMessage(g_hwndMode4_Static_TimeSpent, WM_SETTEXT, 
            0 /* not used */, 
            (LPARAM)App_GetStringFromStringTable(IDS_TEST_STATIC_BS_TIME_SPENT, 
                                                                    g_nLangID));
        SendMessage(g_hwndMode4_Static_TimeSpent_Value, WM_SETTEXT, 
                                              0 /* not used */, (LPARAM)szText);
        // Store "Time Spent" value for log updates function.
        tr.sTimeSpent = szText;

        SendMessage(g_hwndEdit, WM_GETTEXT, MAX_SAMPLE_TEXT, (LPARAM)szBuffer);
        sBuffer = szBuffer; 

        // Show CPM stats. As in WPM stats, I employ a trick  by using ANSI's 
        // sprintf and converting resulting ANSI string to UNICODE with 
        // MultiByteToWideChar.
        nTextLength = Mode4_GetTextLength(&sBuffer);
        tr.nLinesCount = Mode4_GetTextLinesCount(&sBuffer);
        tr.sTitle = Mode4_GetTextTitle(&sBuffer);

        //wsprintf(szText, _T("%d (%d, %d)"), nTextLength, sBuffer.length(), 
        // g_nCharsTyped);
        //MessageBox(0, szText, szText, 0); 
        //sprintf(szAnsiBuffer, "%.2f", (float)(g_nCharsTyped)/(fTimeSpent/60.)); 
        sprintf(szAnsiBuffer, "%.2f", (float)(nTextLength)/(fTimeSpent/60.));

        MultiByteToWideChar(CP_ACP, 0 /* must be zero [TODO: clarify] */, 
            szAnsiBuffer, -1 /* string NULL-terminated [TODO: clarify] */, 
                                                        szText, sizeof(szText)); 

        SendMessage(g_hwndMode4_Static_CPM, WM_SETTEXT, 0 /* not used */, 
            (LPARAM)App_GetStringFromStringTable(IDS_TEST_STATIC_BS_CPM, 
                                                                    g_nLangID));
        SendMessage(g_hwndMode4_Static_CPM_Value, WM_SETTEXT, 0 /* not used */, 
                                                                (LPARAM)szText);
        // Store CPM value for log updates function.
        tr.sCPM = szText;
        
        // Show text length.
        wsprintf(szText, _T("%d"), nTextLength); 
        SendMessage(g_hwndMode4_Static_CharsTyped, WM_SETTEXT, 0 /* not used */, 
            (LPARAM)App_GetStringFromStringTable(IDS_TEST_STATIC_BS_CHARS, 
                                                                    g_nLangID));
        SendMessage(g_hwndMode4_Static_CharsTyped_Value, WM_SETTEXT, 
                                              0 /* not used */, (LPARAM)szText);
        // Store "Chars Typed" for log updates function.
        tr.sTextLength = szText;

        nPos = sBuffer.find(_T(" "));
        if (nPos != -1) 
        {
            // Show words count.
            if (g_bNaturalWordCount)
            {
                // Coincides with word count obtained with Word 2000's Word 
                // Count.
                nWordsCount = m_vecSampleWords.size();
                //wsprintf(szText, L"%d %d", (float)((float)nTextLength/5.), 
                //	                                               nWordsCount);
            }
            else
            {
                // For the purposes of WPM measurement, used in recruitment of 
                // typists, a word is standardized to five characters or 
                // keystrokes.
                nWordsCount = (float)((float)nTextLength/5.); 
                //wsprintf(szText, L"%d %d", nWordsCount, 
                //	                                   m_vecSampleWords.size());
            }

            wsprintf(szText, L"%d", nWordsCount);
            SendMessage(g_hwndMode4_Static_WordsTyped, WM_SETTEXT, 
                0 /* not used */, 
                (LPARAM)App_GetStringFromStringTable(
                                    IDS_TEST_STATIC_BS_WORDS_TYPED, g_nLangID));
            SendMessage(g_hwndMode4_Static_WordsTyped_Value, WM_SETTEXT, 
                                              0 /* not used */, (LPARAM)szText);
            // Store "Words Typed" for log updates function.
            tr.sWordsCount = szText;

            // Show typing speeds (words/minute). As before, I employ a trick 
            // by using ANSI's sprintf and converting resulting ANSI string to 
            // UNICODE with MultiByteToWideChar.
            sprintf(szAnsiBuffer, "%.2f", 
                                         (float)(nWordsCount)/(fTimeSpent/60.)); 

            MultiByteToWideChar(CP_ACP, 0 /* must be zero [TODO: clarify] */, 
                szAnsiBuffer, -1 /* string NULL-terminated [TODO: clarify] */, 
                                                        szText, sizeof(szText)); 

            SendMessage(g_hwndMode4_Static_WPM, WM_SETTEXT, 0 /* not used */, 
                (LPARAM)App_GetStringFromStringTable(IDS_TEST_STATIC_BS_WPM, 
                                                                    g_nLangID));
            SendMessage(g_hwndMode4_Static_WPM_Value, WM_SETTEXT, 
                                              0 /* not used */, (LPARAM)szText);
            // Store "WPM" for log updates function.
            tr.sWPM = szText;

             // Show total errors count.
            wsprintf(szText, _T("%d"), g_nTotalErrorsCount);
            SendMessage(g_hwndMode4_Static_TotalErrors, WM_SETTEXT, 
                0 /* not used */, 
                (LPARAM)App_GetStringFromStringTable(
                                IDS_TEST_STATIC_BS_TOTAL_ERR_COUNT, g_nLangID));
            SendMessage(g_hwndMode4_Static_TotalErrors_Value, WM_SETTEXT, 
                                              0 /* not used */, (LPARAM)szText);
            // Store "Total Errors Count" for log updates function.
            tr.sTotalErrorsCount = szText;

            //int nPos = sBuffer.find(L"Slides are extremely");
         //   if (nPos != -1)
            //	MessageBox(0, L"Slide are extremely", L"", 0); 
            ////if (sBuffer.find(""))
            ////	MessageBox(0, L"", L"", 0); 
            //nPos = sBuffer.find(L"Slides are extremely");
            //if (nPos != -1)
            //	MessageBox(0, L"Le Pays-Bas sont", L"", 0);

            // Log results into text file, ResultsLog.txt, to control extended 
            // output.
            if (g_bLogResults) Mode4_UpdateLog(&tr);
            Debug_Mode4_WriteTestResults();

            //Mode4_OnDetailedStats();
        }
        else
        {
            //MessageBox(0, _T("No spaces found"), _T(""), 0);
        }
    }
}

void Mode4_OnResize(BOOL bExpand)
{
    RECT rc;
    HMENU hMenu, hSubMenu; 

    GetWindowRect(g_hWnd, &rc);
    hMenu = GetMenu(g_hWnd);
    hSubMenu = GetSubMenu(hMenu, 3/* commans */);

    if (bExpand)
    {
        if (g_hwndURLLogo != NULL)
        {
            MoveWindow(g_hwndURLLogo, 
                g_nMode4_URLLogoLeft + g_nLogoLeftIndent + 
                       g_nMode4_BS_URLLogoShift, 4, g_nURLLogoWidth, 20, FALSE);
        }

        SetWindowPos(g_hWnd, NULL, 0, 0, 807, rc.bottom - rc.top,
                                                       SWP_NOZORDER|SWP_NOMOVE);
        //ModifyMenu(hSubMenu, IDM_RESIZE, MF_BYCOMMAND, IDM_RESIZE, 
        //                                                      _T("Contract"));		                                           
    }
    else
    {
        if (g_hwndURLLogo != NULL)
        {
            MoveWindow(g_hwndURLLogo, g_nMode4_URLLogoLeft + g_nLogoLeftIndent, 
                                                 4, g_nURLLogoWidth, 20, FALSE);
        }

        SetWindowPos(g_hWnd, NULL, 0, 0, 560, g_nClientHeight,
        //SetWindowPos(g_hWnd, NULL, 0, 0, 560, 820, 
                                                         SWP_NOZORDER|SWP_NOMOVE);
        //ModifyMenu(hSubMenu, IDM_RESIZE, MF_BYCOMMAND, IDM_RESIZE, 
        //                                                          _T("Expand"));
    }

    App_CenterWindow(g_hWnd, GetDesktopWindow());
}

void Mode4_OnDetailedStats()
{
    TCHAR szWindowName[100]; // detailed stats window
    WNDCLASSEX wcex;
    DWORD dwExStyles;
    RECT rc;
    LVCOLUMN lvc;
    LVITEM lvi;
    TCHAR szBuffer[100];
    char szAnsiBuffer[100];
    int iInsertedItem;
    UINT i;
    HICON hIcon;
    BOOL bAllUnchecked; 

    wcex.cbSize = sizeof(WNDCLASSEX); 

    wcex.style			= CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc	= (WNDPROC)Mode4_DetailedStats_WndProc;
    wcex.cbClsExtra		= 0;
    wcex.cbWndExtra		= 0;
    wcex.hInstance		= g_hInst;
    // To make iconless window, I use dummy icon, which takes the color of the
    // background (i.e. transparent icon including of 32x32 and 16x16 images).
    // I did not find another solution e.g. if NULL passed, default icon 
    // is used.
    wcex.hIcon			= LoadIcon(g_hInst, (LPCTSTR)IDI_DUMMY_TRANSPARENT);  
    wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);

    // TODO: [probably] make an XML switch to use system or HTML-type color	
    wcex.hbrBackground = 
                        (HBRUSH)CreateSolidBrush(g_clrMode4_DS_BackgroundColor); 
    //wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1); // COLOR_APPWORKSPACE
    wcex.lpszMenuName	= NULL; // menuless window
    wcex.lpszClassName	= _T("DetailedStats");
    wcex.hIconSm		= NULL; // when NULL, the icon from hIcon member is used

    RegisterClassEx(&wcex);

    // Center a bounded couple (test area window and DS window) by providing
    // a value (half of DS window width) for the third argument of 
    // App_CenterWindow, which is left horizontal shift. The third parametr 
    // is a App_CenterWindow's default value and used only here. The centering 
    // accounts also for a situation when test window with its base stats was
    // manually moved.
    App_CenterWindow(g_hWnd, GetDesktopWindow(),    
                                             162 /* half of DS window width */);
    GetWindowRect(g_hWnd, &rc);
    
    // Create detailed statistics window.
    lstrcpy(szWindowName, 
        App_GetStringFromStringTable(IDS_TEST_MODE_DS_WINDOW_NAME, 
                                                                    g_nLangID));
    RECT rcDS;
    int	g_nListView_Bottom, g_nButton_Top;
    if (g_nScreenHeight != 768 /* notebook's default */)
    {
        rcDS.top = g_rcMode4_DS_Base.top;
        rcDS.bottom = g_rcMode4_DS_Base.bottom;
        g_nListView_Bottom = g_nMode4_DS_ListView_Bottom_Base;
        g_nButton_Top = g_nMode4_DS_Button_Top_Base;
    }
    else
    {
        rcDS.top = g_rcMode4_DS_1024x768.top; 
        rcDS.bottom = g_rcMode4_DS_1024x768.bottom;
        g_nListView_Bottom = g_nMode4_DS_ListView_Bottom_1024x768;
        g_nButton_Top = g_nMode4_DS_Button_Top_1024x768;
    }
    
    g_hWndMode4_DetailedStats = CreateWindow(_T("DetailedStats"), 
        szWindowName /* NULL if no title */, 
        WS_POPUP | WS_CAPTION | WS_SYSMENU, 
        //rc.right, rc.top + 77 /*30*/, 325, 703 /*750*/, 
        rc.right, rc.top + rcDS.top, 325, rcDS.bottom - rcDS.top, 
                                                     NULL, NULL, g_hInst, NULL);
                                                                          
    ShowWindow(g_hWndMode4_DetailedStats, TRUE);
    UpdateWindow(g_hWndMode4_DetailedStats);

    // Detailed analytics in list view control. comctl32.lib must be set in 
    // linker, InitCommonControls() called. Otherwise, list view is not created, 
    // and VS.NET does not report any errors.
    g_hwndMode4_ListView = CreateWindow(WC_LISTVIEW, NULL, 
       WS_CHILD | WS_VISIBLE | WS_BORDER | LVS_REPORT, 10, 10, 300, 
       g_nListView_Bottom, g_hWndMode4_DetailedStats, 
                                  (HMENU)IDC_TEST_MODE_LISTVIEW, g_hInst, NULL);
    
    // Works with CreateWindow-created (without Ex) window.

    // TODO: full row select style is used here merely because if not, the 1st 
    // item is selected when clicked, which is not nice. Don't have time to 
    // figure out how to compelely avoid selection. By the way here here is a 
    // nice feature, but useless: LVS_EX_TRACKSELECT.
    dwExStyles = LVS_EX_CHECKBOXES | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT;
    
    ListView_SetExtendedListViewStyle(g_hwndMode4_ListView, dwExStyles);

    // Setup list view colums.
    lvc.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_FMT; 
    
    lvc.iSubItem = 0;
    // See notes about g_szBuffer and the method in Add_ToolBar function. 
    _tcscpy(g_szBuffer, 
        App_GetStringFromStringTable(IDS_TEST_MODE_DS_LV_COLUMN_WORD, 
                                                                    g_nLangID));
    lvc.pszText = g_szBuffer;
    lvc.cx = 100;     
    ListView_InsertColumn(g_hwndMode4_ListView, 0, &lvc);

    lvc.iSubItem = 1;
    _tcscpy(g_szBuffer, 
        App_GetStringFromStringTable(IDS_TEST_MODE_DS_LV_COLUMN_ERRORS, 
                                                                    g_nLangID));
    lvc.pszText = g_szBuffer;
    lvc.fmt = LVCFMT_CENTER;
    lvc.cx = 60;     
    ListView_InsertColumn(g_hwndMode4_ListView, 1, &lvc);

    lvc.iSubItem = 2;
    _tcscpy(g_szBuffer, 
        App_GetStringFromStringTable(IDS_TEST_MODE_DS_LV_COLUMN_SPEED, 
                                                                    g_nLangID));
    lvc.pszText = g_szBuffer;
    lvc.fmt = LVCFMT_CENTER;
    lvc.cx = 80;     
    ListView_InsertColumn(g_hwndMode4_ListView, 2, &lvc);

    lvc.iSubItem = 3;
    _tcscpy(g_szBuffer, 
        App_GetStringFromStringTable(IDS_TEST_MODE_DS_LV_COLUMN_LENGTH, 
                                                                    g_nLangID));
    lvc.pszText = g_szBuffer;
    lvc.fmt = LVCFMT_CENTER;
    lvc.cx = 60;     
    ListView_InsertColumn(g_hwndMode4_ListView, 3, &lvc);

    // Populate list view with detailed test results.    
    std::sort(m_vecSampleWords.begin(), m_vecSampleWords.end(), 
                                                         ErrorsCountSortDesc());
    
    lvi.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM | LVIF_STATE; 
    lvi.state = 0; 
    lvi.stateMask = 0;

    for (i = 0; i < m_vecSampleWords.size(); i++)
    {
        // Discard "words" of single character length (since I use per-pair    
        // mesure). 
        if (m_vecSampleWords.at(i).fTimeSpent != 0 && 
                                             m_vecSampleWords.at(i).nLength > 1)
        //if (m_vecSampleWords.at(i).fTimeSpent != 0)
        {
            lvi.iItem = i;
            lvi.iSubItem = 0;
            lvi.lParam = m_vecSampleWords.at(i).nIndex; // used for ordering

            // Insert new item.
            _tcscpy(szBuffer, m_vecSampleWords.at(i).sWord.c_str());
            //_tcscpy(szBuffer, m_vecSampleWords.at(i).szWord);
            lvi.pszText = szBuffer; // crash if direct copy with lstrcpy 
                                    // (or _tcscpy)
            iInsertedItem = ListView_InsertItem(g_hwndMode4_ListView, &lvi);

            // Show errors count.  
            if (m_vecSampleWords.at(i).nErrorsCount == -1)
            {
                wsprintf(szBuffer, _T("%s"), _T("n/a"));
            }
            else
            {
                wsprintf(szBuffer, _T("%d"), 
                                           m_vecSampleWords.at(i).nErrorsCount);
                if (m_vecSampleWords.at(i).nErrorsCount > 0)
                    ListView_SetCheckState(g_hwndMode4_ListView, iInsertedItem, 
                                                                          TRUE);
            }
            ListView_SetItemText(g_hwndMode4_ListView, iInsertedItem, 1, 
                                                                      szBuffer);

            // Show seconds per character.
            sprintf(szAnsiBuffer, "%.2f", 
                (float)m_vecSampleWords.at(i).fTimeSpent/
                                       (float)(m_vecSampleWords.at(i).nLength));
            MultiByteToWideChar(CP_ACP, 0 /* must be zero [TODO: clarify] */, 
                szAnsiBuffer, -1 /* string NULL-terminated [TODO: clarify] */, 
                                                    szBuffer, sizeof(szBuffer)); 
            ListView_SetItemText(g_hwndMode4_ListView, iInsertedItem, 2, 
                                                                      szBuffer);

            // Show the length of the word.
            wsprintf(szBuffer, _T("%d"), m_vecSampleWords.at(i).nLength);
            ListView_SetItemText(g_hwndMode4_ListView, iInsertedItem, 3, 
                                                                      szBuffer);
        }
    }

    // Add "Custom List" and "Check All" buttons.
    g_hwndMode4_Button_CreateCustomList = CreateWindow(_T("BUTTON"),
        NULL, WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_TEXT,
        20, g_nButton_Top, 135, 30, g_hWndMode4_DetailedStats, 
                        (HMENU)IDC_TEST_MODE_BUTTON_CUSTOM_LIST, g_hInst, NULL); 
                                                                          
    SendMessage(g_hwndMode4_Button_CreateCustomList, WM_SETFONT, 
                                                          (WPARAM)g_hfnt, TRUE);

    g_hwndMode4_Button_CheckAll = CreateWindow(_T("BUTTON"), 
        NULL, WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_TEXT, 
        165, g_nButton_Top, 135, 30, g_hWndMode4_DetailedStats, 
                          (HMENU)IDC_TEST_MODE_BUTTON_CHECK_ALL, g_hInst, NULL);

    SendMessage(g_hwndMode4_Button_CheckAll, WM_SETFONT, (WPARAM)g_hfnt, TRUE);

    // Create static controls for the hint to the colors legend window.
    g_hwndMode4_DS_StaticLine =  CreateWindowEx(WS_EX_STATICEDGE, _T("STATIC"), 
        NULL, WS_CHILD | WS_VISIBLE, 10, g_nButton_Top + 39, 300, 1, 
                                g_hWndMode4_DetailedStats, NULL, g_hInst, NULL);

    // Show information icon (static control).
    g_hwndMode4_DS_StaticIcon =  CreateWindow(_T("STATIC"), NULL, 
           WS_CHILD | WS_VISIBLE | SS_ICON, 16, g_nButton_Top + 49, 20, 20, 
                                g_hWndMode4_DetailedStats, NULL, g_hInst, NULL);
    hIcon = LoadIcon(NULL, IDI_INFORMATION); 
    SendMessage(g_hwndMode4_DS_StaticIcon, STM_SETICON,  (WPARAM)hIcon, 0);

    g_hwndMode4_DS_Static =  CreateWindow(_T("STATIC"), NULL, 
        WS_CHILD | WS_VISIBLE, 58, g_nButton_Top + 54, 250, 20, 
          g_hWndMode4_DetailedStats, (HMENU)IDC_MODE4_DS_STATIC, g_hInst, NULL);
    SendMessage(g_hwndMode4_DS_Static, WM_SETFONT, (WPARAM)g_hfnt, TRUE);
    SendMessage(g_hwndMode4_DS_Static, WM_SETTEXT, 0 /* not used */, 
          (LPARAM)App_GetStringFromStringTable(IDS_MODE4_DS_STATIC, g_nLangID));

    App_LocalizeStrings(g_nLangID);

    // Disable Create Custom List button if nothing is checked.
    bAllUnchecked = TRUE;
    for (i = 0; i < (UINT)ListView_GetItemCount(g_hwndMode4_ListView); i++)
    {
        if (ListView_GetCheckState(g_hwndMode4_ListView, i)) 
        {
            bAllUnchecked = FALSE;
            break;
        }
        break;
    }
    if (bAllUnchecked) EnableWindow(g_hwndMode4_Button_CreateCustomList, FALSE);
}

void SelectPairs(LPCTSTR pszText, int nCharPos)
{
    HBRUSH hbrush;
    HDC hDC;
    LRESULT lRes;
    RECT rc;
    //TCHAR szTest[100];

    rc.left = 0; rc.top = 0; rc.right =  60; rc.bottom = 40;
    lRes = SendMessage(g_hwndEdit, EM_POSFROMCHAR, (WPARAM)nCharPos, 
                                                              0 /* not used */);
    //sprintf(szTest, "nCharPos: %d Hor: %d Vert: %d", nCharPos, 
    //	LOWORD(lRes) /* horizontal coordinate */, 
    //	                                HIWORD(lRes) /* vertical coordinate */);
    //MessageBox(0, szTest, pszText, 0);

    rc.left   = LOWORD(lRes); 
    rc.top    = HIWORD(lRes); 
    rc.right  = LOWORD(lRes) + 16; 
    rc.bottom = HIWORD(lRes) + 16;

    hbrush = CreateSolidBrush(RGB(255,0,0));
    hDC = GetWindowDC(g_hwndEdit); 

    FillRect(hDC, &rc, hbrush);
    SetBkColor(hDC, RGB(0,128,0));

    DrawText(hDC, pszText, -1, &rc, DT_EDITCONTROL);

    DeleteObject(hbrush); 
    ReleaseDC(g_hwndEdit, hDC);
}

BOOL GetSampleWords(LPTSTR pszPair /* in */, LPTSTR pszPrevWord /* in */, 
                                                       LPTSTR pszWord /* out */)
{
    //string sWord, sPair;
    wstring sWord, sPair;
    sPair = pszPair;
    int nPos;
    UINT i;
    BOOL bPrevFound = FALSE;
    
    // for(int i = 0; i < vecRightHandWords.size(); i++)
    for (i = 0; i < vecSampleWords.size(); i++)
    {
        // pszWord returned (effectively) when pszPrevWord matched.
        //sWord = vecRightHandWords.at(i); lstrcpy(pszWord, sWord.c_str());
        sWord = vecSampleWords.at(i); lstrcpy(pszWord, sWord.c_str());
        //if(pszPrevWord != NULL) MessageBox(0, sWord.c_str(), pszPrevWord, 0);

        nPos = sWord.find(sPair);
        if(nPos != -1) 
        {
            // 1st pass (ignored for the *nd passes).
            if(pszPrevWord == NULL) return TRUE;

            if(bPrevFound) return TRUE;
            if(lstrcmp(sWord.c_str(), pszPrevWord) == 0) bPrevFound = TRUE;
        }
    }

    return FALSE;
}

void App_LocalizeStrings(short nLangID)
{
    if (g_bDebug) App_Debug_FunctionCallTrace("App_LocalizeStrings Entered");

    if (g_nMode == 1 || g_nMode == 2)
    {
        // Set application title.
        _tcscpy(g_szBuffer, App_GetStringFromStringTable(IDS_APP_TITLE, 
                                                                    g_nLangID));
        SendMessage(g_hWnd, WM_SETTEXT, 0 /* not used */, (LPARAM)g_szBuffer); 

        // Show status bar hint for the current mode.
        if (g_bShowStatusBarHints)
            SendMessage(g_hwndStatus, WM_SETTEXT, 0 /* not used */, 
              (LPARAM)App_GetStringFromStringTable(IDS_MODE12_HINT, g_nLangID));
    }

    if (g_nMode == 3)
    {
        // Set application title.
        SendMessage(g_hWnd, WM_SETTEXT, 0 /* not used */, 
                  (LPARAM)App_GetStringFromStringTable(IDS_APP_TITLE, nLangID));

        SendMessage(g_hwndMode3_Static_Slowest, WM_SETTEXT, 0 /* not used */, 
             (LPARAM)App_GetStringFromStringTable(IDS_STATIC_SLOWEST, nLangID));

        SendMessage(g_hwndMode3_Static_TypingSpeed, WM_SETTEXT, 0 /* not used */, 
        (LPARAM)App_GetStringFromStringTable(IDS_STATIC_TYPING_SPEED, nLangID));

        //SendMessage(g_hwndMode3_Static_WordsTyped, WM_SETTEXT, 0 /* not used */, 
        //  (LPARAM)App_GetStringFromStringTable(IDS_STATIC_WORDS_TYPED, nLangID));

        SendMessage(g_hwndMode3_Static_CharsTyped, WM_SETTEXT, 0 /* not used */, 
        (LPARAM)App_GetStringFromStringTable(IDS_STATIC_CHARACTERS_TYPED, 
                                                                      nLangID));

        SendMessage(g_hwndMode3_Static_ErrorsCount, WM_SETTEXT, 0 /* not used */, 
            (LPARAM)App_GetStringFromStringTable(IDS_STATIC_ERRORS_COUNT, 
                                                                      nLangID));
        
        SendMessage(g_hwndMode3_Button_UpdateSorting, WM_SETTEXT, 
            0 /* not used */, 
        (LPARAM)App_GetStringFromStringTable(IDS_CUSTOM_MODE_BUTTON_SORT_UPDATE, 
                                                                      nLangID));
        
        SendMessage(g_hwndMode3_Button, WM_SETTEXT, 0 /* not used */, 
            (LPARAM)App_GetStringFromStringTable(IDS_BUTTON_REPOPULATE, 
                                                                      nLangID));

        // Show status bar hint for the current mode.
        if (g_bShowStatusBarHints)
        {
            if (ListView_GetItemCount(g_hwndMode3_ListView))
                // Show normal hint if there are list view items.
                SendMessage(g_hwndStatus, WM_SETTEXT, 0 /* not used */, 
                    (LPARAM)App_GetStringFromStringTable(IDS_MODE3_HINT, 
                                                                    g_nLangID));
            else
                // Show normal hint to use test mofde if list view is empty.
                SendMessage(g_hwndStatus, WM_SETTEXT, 0 /* not used */, 
                    (LPARAM)App_GetStringFromStringTable(IDS_MODE3_HINT2, 
                                                                    g_nLangID));
        }
    }

   if (g_nMode == 4)
   {
       // Set application title.
       //SendMessage(g_hWnd, WM_SETTEXT, 0 /* not used */, 
       //             (LPARAM)GetStringFromStringTable(IDS_APP_TITLE, nLangID));

       SendMessage(g_hwndMode4_Button_CreateCustomList, WM_SETTEXT, 
           0 /* not used */, 
          (LPARAM)App_GetStringFromStringTable(IDS_TEST_MODE_BUTTON_CUSTOM_LIST, 
                                                                      nLangID));

       SendMessage(g_hwndMode4_Button_CheckAll, WM_SETTEXT, 0 /* not used */, 
           (LPARAM)App_GetStringFromStringTable(IDS_TEST_MODE_BUTTON_CHECK_ALL, 
                                                                      nLangID));
   }

   if (g_bDebug) App_Debug_FunctionCallTrace("App_LocalizeStrings Exited");
}

//------------------------------------------------------------------------------
// Function: App_GetStringFromStringTable
// Notes: Since application uses multi-language resource strings we cannot 
//        merely use LoadString to load language-specific strings. Instead, 
//        FindResourceEx, LoadResource is used. Q-article 200893 provides basic
//        code (one modification is not to use new operator, see code comments).
//        This Q-article says that LoadString might fail to load corrent 
//        language-specific string on Windows 95/98, so we need to use 
//        FindResourceEx, which takes LanguageID as the 4th parameter. Notice 
//        that IDS_* ID of string is modified for use with FindResourceEx!
//        g_szBuffer is declared a the application level so that the function
//        does not return a local object.
//------------------------------------------------------------------------------

TCHAR* App_GetStringFromStringTable(UINT uStringID, short nLangID)
{
    WORD      wLanguageID;
    TCHAR*    pwchMem; 
    TCHAR*	  pwchCur;
    // One-based idRsrcBlk indicates the string segment or block (each block 
    // consists of 16 strings). On ealier systems partitioning string resources 
    // into blocks saved memory since string resources are loaded by blocks.
    UINT      idRsrcBlk = uStringID/16 + 1; 
    // Zero-based iIndex shows position within a string segment (block). The 
    // index here is a reminder found with modulus operator (%).
    int       iIndex  = uStringID%16; // 
    HINSTANCE hModule = NULL;
    HRSRC     hResource = NULL;
    int i;
    int cchString;

    switch (nLangID)
    {
        case 1: // English
            wLanguageID = MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US);
            break;
        case 2: // French
            wLanguageID = MAKELANGID(LANG_FRENCH, SUBLANG_FRENCH);
            break;
        case 3: // Russian
            wLanguageID = MAKELANGID(LANG_RUSSIAN, SUBLANG_DEFAULT);
            break;
        default:
            // In fact, SUBLANG_ENGLISH_US == SUBLANG_DEAFULAT (0x01).
            wLanguageID = MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US);
            break;
    }

    // LoadResource loads the one block from string table, indicated before by 
    // idRsrcBlk, which normally corresponds to 16 string resources, and is  
    // merely a block of memory that should be parsed.
    hResource = FindResourceEx(g_hInst, RT_STRING, MAKEINTRESOURCE(idRsrcBlk),
                                                                   wLanguageID);

    if (hResource != NULL)
    {
        pwchMem = (TCHAR*)LoadResource(g_hInst, hResource); // returns HGLOBAL 

        if (pwchMem != NULL)
        {
            pwchCur = pwchMem;
            for (i = 0; i < 16; i++)
            {
                if (*pwchCur)
                {
                    cchString = *pwchCur;  // string size in characters
                    pwchCur++;
                    if (i == iIndex )
                    {
                        // The string has been found in the string table.
                        _tcsncpy(g_szBuffer, pwchCur, cchString);
                        // I avoid allocating memoty dynamically with new, as
                        // in original Q200893, since it might cause a crash 
                        // at run-time (HeapAlloc, "new" returns NULL). For 
                        // example, "new" caused a crash in Release mode 
                        // (Test, EN/FR, or when switching to another mode).
                        g_szBuffer[cchString] = '\0';
                        return g_szBuffer;
                    }
                    pwchCur += cchString;
                }
                else
                    pwchCur++;
            }
        }
    }

    return NULL;
}

void Mode4_ParseSampleText(LPCTSTR pszText)
{
    if (g_bDebug) App_Debug_FunctionCallTrace("Mode4_ParseSampleText Entered");

    TCHAR szBuffer[100];
    TCHAR *pToken = NULL;
    CTestResults_ByWords m_TestResults;
    short nIndex = 0; // unique index of a word used for ordering

    m_vecSampleWords.clear();
    
    // With _tcstok we are able to get words (tokens) in selected sentence 
    // regardless of number of spaces, CR/LF characters, punctuation characters
    // between words. This is because the second argument can be not only a 
    // single delimiter (separator), but also a list of possible delimiters.

    // MSDN: "On the first call to strtok, the function skips leading delimiters 
    // and returns a pointer to the first token in strToken, terminating the 
    // token with a null character". Notice that token is a word, not a 
    // delimiter.
    pToken = _tcstok((wchar_t*)pszText, L" ");
    lstrcpy(szBuffer, pToken); 

    m_TestResults.nIndex = nIndex;
    m_TestResults.sWord = szBuffer;
    m_TestResults.nLength = lstrlen(szBuffer);
    // Assign initial values to avoid meaningless output.
    m_TestResults.fTimeSpent = (float)0;
    m_TestResults.nErrorsCount = -1;
    m_vecSampleWords.push_back(m_TestResults);

    while (pToken != NULL)
    {
        nIndex++;
        // Return a pointer to the next token found in the string.
        pToken = _tcstok(NULL, L" .,;:!?\r\n");
        lstrcpy(szBuffer, pToken);

        // This condition helps to avoid counting a zero-length string such as 
        // between "." and "\r\n" delimiters at the end of sample (this adds 
        // one wrong count to a count of words for any sample test).
        if (lstrlen(pToken) != 0)
        {
            m_TestResults.nIndex = nIndex;
            m_TestResults.sWord = szBuffer;
            m_TestResults.nLength = lstrlen(szBuffer);
            // Assign initial values to avoid meaningless output.
            m_TestResults.fTimeSpent = (float)0;
            m_TestResults.nErrorsCount = -1;
            m_vecSampleWords.push_back(m_TestResults);
        }
    }

    // Debug_Mode4_WriteTestResults(); // output sample words into text file

    if (g_bDebug) App_Debug_FunctionCallTrace("Mode4_ParseSampleText Exited");
}

void Mode4_CreateHtmlReport()
{
    if (g_bDebug) App_Debug_FunctionCallTrace("Mode4_CreateHtmlReport Entered");

    HANDLE hReport;
    DWORD dwOffset;
    DWORD dWritten;

    char szHTMLDocHeader[200];
    char szTitle[100];
    // Date/time are always have the form - 03/14/07, 10:16 - neutral to 
    // language. Here, char type is used since date/time need not to be 
    // translated to MB characters because UTF-8 always treat such 
    // date/time characters as single-byted.
    char szDateTimeStamp[100];
    char szDate[10];
    WCHAR szDayOfWeek[20];
    char szTime[10];

    char szTablePrefix[]   = "<table align='center' border = '1'>";
    char szTableHeader[100];
    //char szOddRowPrefix[]  = "<tr bgcolor='rgb(212, 212, 212)'>";
    //char szEvenRowPrefix[] = "<tr bgcolor='rgb(255, 255, 255)'>";
    char szTableFooter[]   = "</table><br>";
    char szHTMLDocFooter[] = "</body></html>";
    char szRow[256];
    char szCopyright[200];

    TCHAR szText[100];
    char szMultiByteName[100];
    char szMultiByteValue[100];
    char szMultiByteColumn1[50];
    char szMultiByteColumn2[50];
    
    DeleteFile(_T("Report.htm"));

    // Create HTML report in RT directory.
    hReport = CreateFile(_T("Report.htm"), GENERIC_READ | GENERIC_WRITE, 
    //hReport = CreateFile(szReportFilePath, GENERIC_READ | GENERIC_WRITE, 
        FILE_SHARE_READ | FILE_SHARE_WRITE, NULL /* security - not used*/,          
        OPEN_ALWAYS /* if exists - just open; if does not exist, create it */,
                                  FILE_ATTRIBUTE_NORMAL /* normal use */, NULL);         

    // Tell the writer we need to write at the end of file.
    //dwOffset = SetFilePointer(m_hLog, 0, NULL, FILE_END);
    dwOffset = SetFilePointer(hReport, 0, NULL, FILE_END);

    // Write HTML document header.
    strcpy(szHTMLDocHeader, "<html>\r\n<head>\r\n"); 
    strcat(szHTMLDocHeader, "<meta http-equiv='content-type' ");  
    strcat(szHTMLDocHeader, "content='text/html; charset=utf-8'>\r\n"); 
    strcat(szHTMLDocHeader, "</head>\r\n<body>\r\n\0");
    WriteFile(hReport, szHTMLDocHeader, strlen(szHTMLDocHeader), &dWritten, 
                                                                          NULL);
    // Write report header.
    // Get report header (language-dependent name).
    lstrcpy(szText, 
        App_GetStringFromStringTable(IDS_TEST_MODE_HTML_REPORT_TITLE, 
                                                                    g_nLangID));
    WideCharToMultiByte(CP_UTF8, 0 /* must be zero */, szText, -1,        
        szMultiByteName, sizeof(szMultiByteName), NULL /* must be NULL */, 
                                                       NULL /* must be NULL */);
    sprintf(szTitle, "<h3 align='center'>%s</h3>\r\n\0", szMultiByteName);
    WriteFile(hReport, szTitle, strlen(szTitle), &dWritten, NULL);

    // Write horizontal line.
    WriteFile(hReport, "<hr>", strlen("<hr>"), &dWritten, NULL);
    // Get and write date/time stamp data.
    Mode4_GetDateTimeStamp(szDate /* out */, szDayOfWeek /* out */, 
                                                              szTime /* out */);
    // Translate language-dependent day of week into multi-byte chars.
    WideCharToMultiByte(CP_UTF8, 0 /* must be zero */, szDayOfWeek, -1,        
        szMultiByteValue, sizeof(szMultiByteValue), NULL /* must be NULL */, 
                                                       NULL /* must be NULL */);

    sprintf(szDateTimeStamp, "<p align='center'>%s, %s, %s</p>\r\n\0", szDate, 
                                                      szMultiByteValue, szTime);
    WriteFile(hReport, szDateTimeStamp, strlen(szDateTimeStamp),
                                                               &dWritten, NULL);

    // Write table header.
    lstrcpy(szText, 
        App_GetStringFromStringTable(IDS_TEST_MODE_HTML_REPORT_COL1, 
                                                                    g_nLangID));
    WideCharToMultiByte(CP_UTF8, 0 /* must be zero */, szText, -1,        
        szMultiByteColumn1, sizeof(szMultiByteColumn1), NULL /* must be NULL */, 
                                                       NULL /* must be NULL */);
    lstrcpy(szText, 
        App_GetStringFromStringTable(IDS_TEST_MODE_HTML_REPORT_COL2, 
                                                                    g_nLangID));
    WideCharToMultiByte(CP_UTF8, 0 /* must be zero */, szText, -1,        
        szMultiByteColumn2, sizeof(szMultiByteColumn2), NULL /* must be NULL */, 
                                                       NULL /* must be NULL */);
    sprintf(szTableHeader, "%s<tr><td>%s</td><td>%s</td></tr>\r\n\0", 
                         szTablePrefix, szMultiByteColumn1, szMultiByteColumn2);
    WriteFile(hReport, szTableHeader, strlen(szTableHeader), &dWritten, NULL);

    // Time spent (language-depandent name).
    lstrcpy(szText, App_GetStringFromStringTable(IDS_TEST_STATIC_BS_TIME_SPENT, 
                                                                    g_nLangID));
    WideCharToMultiByte(CP_UTF8, 0 /* must be zero */, szText, -1,        
        szMultiByteName, sizeof(szMultiByteName), NULL /* must be NULL */, 
                                                       NULL /* must be NULL */);

    // Time spent value.
    SendMessage(g_hwndMode4_Static_TimeSpent_Value, WM_GETTEXT, 100, 
                                                                (LPARAM)szText);
    WideCharToMultiByte(CP_UTF8, 0 /* must be zero */, szText, -1,        
        szMultiByteValue, sizeof(szMultiByteValue), NULL /* must be NULL */, 
                                                       NULL /* must be NULL */);
    sprintf(szRow, "<tr><td>%s</td><td>%s</td></tr>\r\n\0", szMultiByteName,
                                                              szMultiByteValue);
    WriteFile(hReport, szRow, strlen(szRow), &dWritten, NULL);

    // CPM (language-depandent name).
    lstrcpy(szText, App_GetStringFromStringTable(IDS_TEST_STATIC_BS_CPM, 
                                                                    g_nLangID));
    WideCharToMultiByte(CP_UTF8, 0 /* must be zero */, szText, -1,        
        szMultiByteName, sizeof(szMultiByteName), NULL /* must be NULL */, 
                                                       NULL /* must be NULL */);
    
    // CPM value.
    SendMessage(g_hwndMode4_Static_CPM_Value, WM_GETTEXT, 100, (LPARAM)szText);
    WideCharToMultiByte(CP_UTF8, 0 /* must be zero */, szText, -1,        
        szMultiByteValue, sizeof(szMultiByteValue), NULL /* must be NULL */, 
                                                       NULL /* must be NULL */);
    sprintf(szRow, "<tr><td>%s</td><td>%s</td></tr>\r\n\0", szMultiByteName,
                                                              szMultiByteValue);
    WriteFile(hReport, szRow, strlen(szRow), &dWritten, NULL);

    // Chars typed (language-depandent name).
    lstrcpy(szText, App_GetStringFromStringTable(IDS_TEST_STATIC_BS_CHARS, 
                                                                    g_nLangID));
    WideCharToMultiByte(CP_UTF8, 0 /* must be zero */, szText, -1,        
        szMultiByteName, sizeof(szMultiByteName), NULL /* must be NULL */, 
                                                       NULL /* must be NULL */);
    // Chars typed value.
    SendMessage(g_hwndMode4_Static_CharsTyped_Value, WM_GETTEXT, 100, 
                                                                (LPARAM)szText);
    WideCharToMultiByte(CP_UTF8, 0 /* must be zero */, szText, -1,        
        szMultiByteValue, sizeof(szMultiByteValue), NULL /* must be NULL */, 
                                                       NULL /* must be NULL */);
    sprintf(szRow, "<tr><td>%s</td><td>%s</td></tr>\r\n\0", szMultiByteName,
                                                              szMultiByteValue);
    WriteFile(hReport, szRow, strlen(szRow), &dWritten, NULL);

    // Words typed (language-depandent name).
    lstrcpy(szText, App_GetStringFromStringTable(IDS_TEST_STATIC_BS_WORDS_TYPED, 
                                                                    g_nLangID));
    WideCharToMultiByte(CP_UTF8, 0 /* must be zero */, szText, -1,        
        szMultiByteName, sizeof(szMultiByteName), NULL /* must be NULL */, 
                                                       NULL /* must be NULL */);
    // Words typed value.
    SendMessage(g_hwndMode4_Static_WordsTyped_Value, WM_GETTEXT, 100, 
                                                                (LPARAM)szText);
    WideCharToMultiByte(CP_UTF8, 0 /* must be zero */, szText, -1,        
        szMultiByteValue, sizeof(szMultiByteValue), NULL /* must be NULL */, 
                                                       NULL /* must be NULL */);
    sprintf(szRow, "<tr><td>%s</td><td>%s</td></tr>\r\n\0", szMultiByteName,
                                                              szMultiByteValue);
    WriteFile(hReport, szRow, strlen(szRow), &dWritten, NULL);

    // WPM typed (language-depandent name).
    lstrcpy(szText, App_GetStringFromStringTable(IDS_TEST_STATIC_BS_WPM, 
                                                                    g_nLangID));
    WideCharToMultiByte(CP_UTF8, 0 /* must be zero */, szText, -1,        
        szMultiByteName, sizeof(szMultiByteName), NULL /* must be NULL */, 
                                                       NULL /* must be NULL */);
    // WPM data.
    SendMessage(g_hwndMode4_Static_WPM_Value, WM_GETTEXT, 100, (LPARAM)szText);
    WideCharToMultiByte(CP_UTF8, 0 /* must be zero */, szText, -1,        
        szMultiByteValue, sizeof(szMultiByteValue), NULL /* must be NULL */, 
                                                       NULL /* must be NULL */);
    sprintf(szRow, "<tr><td>%s</td><td>%s</td></tr>\r\n\0", szMultiByteName,
                                                              szMultiByteValue);
    WriteFile(hReport, szRow, strlen(szRow), &dWritten, NULL);

    // Total errors (language-depandent name).
    lstrcpy(szText, 
        App_GetStringFromStringTable(IDS_TEST_STATIC_BS_TOTAL_ERR_COUNT, 
                                                                    g_nLangID));
    WideCharToMultiByte(CP_UTF8, 0 /* must be zero */, szText, -1,        
        szMultiByteName, sizeof(szMultiByteName), NULL /* must be NULL */, 
                                                       NULL /* must be NULL */);
    // Total errors value.
    SendMessage(g_hwndMode4_Static_TotalErrors_Value, WM_GETTEXT, 100, 
                                                                (LPARAM)szText);
    WideCharToMultiByte(CP_UTF8, 0 /* must be zero */, szText, -1,        
        szMultiByteValue, sizeof(szMultiByteValue), NULL /* must be NULL */, 
                                                       NULL /* must be NULL */);
    sprintf(szRow, "<tr><td>%s</td><td>%s</td></tr>\r\n\0", szMultiByteName,
                                                              szMultiByteValue);
    WriteFile(hReport, szRow, strlen(szRow), &dWritten, NULL);

    //// Use interlaced colors for odd/even rows.
    ////if ((float)i/(float)2 -i/2) //value is "1" or "0" 
    ////{
    //	sprintf(szRow, 
    //		"%s<td>%s</td><td>%s</td><td>%s</td><td>%s</td></tr>\r\n\0", 
    //			szOddRowPrefix, szWord, szLength, szSpeed, szTypingErrors);
    ////}
    ////else
    ////{
    //	sprintf(szRow, 
    //		"%s<td>%s</td><td>%s</td><td>%s</td><td>%s</td></tr>\r\n\0", 
    //		szEvenRowPrefix, szWord, szLength, szSpeed, szTypingErrors);
    ////}
    
    WriteFile(hReport, szTableFooter, strlen(szTableFooter), &dWritten, NULL);

    //...
    // Write horizontal line.
    WriteFile(hReport, "<hr>", strlen("<hr>"), &dWritten, NULL);

    strcpy(szCopyright, 
                     "<p align='center'><a href='http://www.carexplorer.org'>"); 
    strcat(szCopyright, "MSB LLC</a> &#169; Rational Typist");  
    strcat(szCopyright, "</p>\r\n\0"); 
    WriteFile(hReport, szCopyright, strlen(szCopyright), &dWritten, NULL);

    WriteFile(hReport, szHTMLDocFooter, strlen(szHTMLDocFooter), &dWritten, 
                                                                          NULL);

    CloseHandle(hReport);

    // Show report in IE.
    WinExec("explorer.exe Report.htm", TRUE);

    if (g_bDebug) App_Debug_FunctionCallTrace("Mode4_CreateHtmlReport Exited");
}

// Based on Mode4_CreateHtmlReport() that reads results from the UI of Test Mode
// (BS controls).
void Mode4_UpdateLog(TEST_RESULTS* ptr)
{
    if (g_bDebug) App_Debug_FunctionCallTrace("Mode4_UpdateLog()");

    HANDLE hLog;
    DWORD dwOffset;
    DWORD dWritten;

    // Date/time are always have the form - 03/14/07, 10:16 - neutral to 
    // language. Here, char type is used since date/time need not to be 
    // translated to MB characters because UTF-8 always treat such 
    // date/time characters as single-byted.
    char szDate[10];
    WCHAR szDayOfWeek[20];
    char szTime[10];
    char szLine[256];
    TCHAR szText[100];
    char szTestTitle[100];
    char szSampleTitle[100];
    char szMultiByteName[100];
    char szMultiByteValue[100];
    int nTextLengh_WithENTER;
    float fCPM_WithENTER; 
    float fWPM_Alternative;
    
    // Create HTML report in RT directory.
    hLog = CreateFile(_T("ResultsLog.txt"), GENERIC_READ | GENERIC_WRITE, 
    //hReport = CreateFile(szReportFilePath, GENERIC_READ | GENERIC_WRITE, 
        FILE_SHARE_READ | FILE_SHARE_WRITE, NULL /* security - not used*/,          
        OPEN_ALWAYS /* if exists - just open; if does not exist, create it */,
                                  FILE_ATTRIBUTE_NORMAL /* normal use */, NULL);         

    // Tell the writer we need to write at the end of file.
    dwOffset = SetFilePointer(hLog, 0, NULL, FILE_END);

    //--------------------------------------------------------------------------
    // Log basic metrics.
    //--------------------------------------------------------------------------
    
    // Get and write date/time stamp data.
    Mode4_GetDateTimeStamp(szDate /* out */, szDayOfWeek /* out */, 
                                                              szTime /* out */);
    // Translate language-dependent day of week into multi-byte chars.
    WideCharToMultiByte(CP_UTF8, 0 /* must be zero */, szDayOfWeek, -1,        
        szMultiByteValue, sizeof(szMultiByteValue), NULL /* must be NULL */, 
                                                       NULL /* must be NULL */);
    WideCharToMultiByte(CP_UTF8, 0 /* must be zero */, ptr->sTitle.c_str(), -1,        
        szSampleTitle, sizeof(szSampleTitle), NULL /* must be NULL */, 
                                                       NULL /* must be NULL */);
    //sprintf(szDateTimeStamp, "%s, %s, %s\r\n\r\n\0", szDate, 
    sprintf(szTestTitle, "%s, %s, %s, (\"%s\")\r\n\r\n\0", szDate, 
                                     szMultiByteValue, szTime, szSampleTitle);
    WriteFile(hLog, szTestTitle, strlen(szTestTitle),
    //WriteFile(hLog, szDateTimeStamp, strlen(szDateTimeStamp),
                                                               &dWritten, NULL);
    // Time spent (language-dependent name).
    lstrcpy(szText, App_GetStringFromStringTable(IDS_TEST_STATIC_BS_TIME_SPENT, 
                                                                    g_nLangID));
    WideCharToMultiByte(CP_UTF8, 0 /* must be zero */, szText, -1,        
        szMultiByteName, sizeof(szMultiByteName), NULL /* must be NULL */, 
                                                       NULL /* must be NULL */);
    WideCharToMultiByte(CP_UTF8, 0 /* must be zero */, 
        ptr->sTimeSpent.c_str(), -1, szMultiByteValue, 
        sizeof(szMultiByteValue), NULL /* must be NULL */, 
                                                       NULL /* must be NULL */);
    // Time spent value.
    sprintf(szLine, "%s %s\r\n\0", szMultiByteName, szMultiByteValue);
    WriteFile(hLog, szLine, strlen(szLine), &dWritten, NULL);

    // CPM (language-dependent name).
    lstrcpy(szText, App_GetStringFromStringTable(IDS_TEST_STATIC_BS_CPM, 
                                                                    g_nLangID));
    WideCharToMultiByte(CP_UTF8, 0 /* must be zero */, szText, -1,        
        szMultiByteName, sizeof(szMultiByteName), NULL /* must be NULL */, 
                                                       NULL /* must be NULL */);
    WideCharToMultiByte(CP_UTF8, 0 /* must be zero */, ptr->sCPM.c_str(), -1,        
        szMultiByteValue, sizeof(szMultiByteValue), NULL /* must be NULL */, 
                                                       NULL /* must be NULL */);
    // CPM value.
    sprintf(szLine, "%s %s\r\n\0", szMultiByteName, szMultiByteValue); 
    WriteFile(hLog, szLine, strlen(szLine), &dWritten, NULL);

    // Chars typed (language-dependent name).
    lstrcpy(szText, App_GetStringFromStringTable(IDS_TEST_STATIC_BS_CHARS, 
                                                                    g_nLangID));
    WideCharToMultiByte(CP_UTF8, 0 /* must be zero */, szText, -1,        
        szMultiByteName, sizeof(szMultiByteName), NULL /* must be NULL */, 
                                                       NULL /* must be NULL */);
    WideCharToMultiByte(CP_UTF8, 0 /* must be zero */, 
        ptr->sTextLength.c_str(), -1, szMultiByteValue, 
        sizeof(szMultiByteValue), NULL /* must be NULL */, 
                                                       NULL /* must be NULL */);
    // Chars typed value.
    sprintf(szLine, "%s %s\r\n\0", szMultiByteName, szMultiByteValue);
    WriteFile(hLog, szLine, strlen(szLine), &dWritten, NULL);

    // Words typed (language-dependent name).
    lstrcpy(szText, App_GetStringFromStringTable(IDS_TEST_STATIC_BS_WORDS_TYPED, 
                                                                    g_nLangID));
    WideCharToMultiByte(CP_UTF8, 0 /* must be zero */, szText, -1,        
        szMultiByteName, sizeof(szMultiByteName), NULL /* must be NULL */, 
                                                       NULL /* must be NULL */);
    WideCharToMultiByte(CP_UTF8, 0 /* must be zero */, 
        ptr->sWordsCount.c_str(), -1, szMultiByteValue, 
        sizeof(szMultiByteValue), NULL /* must be NULL */, 
                                                       NULL /* must be NULL */);
    // Words typed value.
    sprintf(szLine, "%s %s\r\n\0", szMultiByteName, szMultiByteValue);
    WriteFile(hLog, szLine, strlen(szLine), &dWritten, NULL);

    // WPM typed (language-dependent name).
    lstrcpy(szText, App_GetStringFromStringTable(IDS_TEST_STATIC_BS_WPM, 
                                                                    g_nLangID));
    WideCharToMultiByte(CP_UTF8, 0 /* must be zero */, szText, -1,        
        szMultiByteName, sizeof(szMultiByteName), NULL /* must be NULL */, 
                                                       NULL /* must be NULL */);
    WideCharToMultiByte(CP_UTF8, 0 /* must be zero */, 
        ptr->sWPM.c_str(), -1, szMultiByteValue, sizeof(szMultiByteValue), 
                              NULL /* must be NULL */, NULL /* must be NULL */);
    // WPM data.
    sprintf(szLine, "%s %s\r\n\0", szMultiByteName, szMultiByteValue);
    WriteFile(hLog, szLine, strlen(szLine), &dWritten, NULL);

    // Total errors (language-dependent name).
    lstrcpy(szText, 
        App_GetStringFromStringTable(IDS_TEST_STATIC_BS_TOTAL_ERR_COUNT, 
                                                                    g_nLangID));
    WideCharToMultiByte(CP_UTF8, 0 /* must be zero */, szText, -1,        
        szMultiByteName, sizeof(szMultiByteName), NULL /* must be NULL */, 
                                                       NULL /* must be NULL */);
    WideCharToMultiByte(CP_UTF8, 0 /* must be zero */, 
        ptr->sTotalErrorsCount.c_str(), -1, szMultiByteValue, 
        sizeof(szMultiByteValue), NULL /* must be NULL */, 
                                                       NULL /* must be NULL */);
    // Total errors value.
    sprintf(szLine, "%s %s\r\n\r\n\0", szMultiByteName, szMultiByteValue);
    WriteFile(hLog, szLine, strlen(szLine), &dWritten, NULL);

    //--------------------------------------------------------------------------
    // Log alternative metrics.
    //--------------------------------------------------------------------------
    
    // Log title for alternative metrics.
    lstrcpy(szText, 
               App_GetStringFromStringTable(IDS_TEST_LOG_ALT_TITLE, g_nLangID));
    WideCharToMultiByte(CP_UTF8, 0 /* must be zero */, szText, -1,        
        szMultiByteName, sizeof(szMultiByteName), NULL /* must be NULL */, 
                                                       NULL /* must be NULL */);
    sprintf(szLine, "%s\r\n\r\n\0", szMultiByteName);
    WriteFile(hLog, szLine, strlen(szLine), &dWritten, NULL);

    // Log chars with ENTER.
    lstrcpy(szText, App_GetStringFromStringTable(
                                 IDS_TEST_LOG_ALT_CHARS_WITH_ENTER, g_nLangID));
    WideCharToMultiByte(CP_UTF8, 0 /* must be zero */, szText, -1,        
        szMultiByteName, sizeof(szMultiByteName), NULL /* must be NULL */, 
                                                       NULL /* must be NULL */);
    nTextLengh_WithENTER = _tstoi(ptr->sTextLength.c_str()) + 
                                                         (ptr->nLinesCount - 1);
    sprintf(szLine, "%s %d\r\n\0", szMultiByteName, nTextLengh_WithENTER);
    WriteFile(hLog, szLine, strlen(szLine), &dWritten, NULL);

    // Log CPM with ENTER.
    lstrcpy(szText, App_GetStringFromStringTable(
                                 IDS_TEST_LOG_ALT_CPM_WITH_ENTER, g_nLangID));
    WideCharToMultiByte(CP_UTF8, 0 /* must be zero */, szText, -1,        
        szMultiByteName, sizeof(szMultiByteName), NULL /* must be NULL */, 
                                                       NULL /* must be NULL */);
    // Notice that the use of _tstof(ptr->sTimeSpent.c_str()) is wrong since the
    // precision is lost.
    fCPM_WithENTER = (float)(nTextLengh_WithENTER)/ptr->fTimeSpent;
    sprintf(szLine, "%s %.2f\r\n\0", szMultiByteName, fCPM_WithENTER); 
    WriteFile(hLog, szLine, strlen(szLine), &dWritten, NULL);

    // Log Words Typed.
    if (g_bNaturalWordCount)
    {
        lstrcpy(szText, App_GetStringFromStringTable(
                               IDS_TEST_LOG_ALT_WORDS_TYPED_TYPIST, g_nLangID));
        WideCharToMultiByte(CP_UTF8, 0 /* must be zero */, szText, -1,        
            szMultiByteName, sizeof(szMultiByteName), NULL /* must be NULL */, 
                                                       NULL /* must be NULL */);
        sprintf(szLine, "%s %d\r\n\0", szMultiByteName, 
                                    (int)(_tstoi(ptr->sTextLength.c_str())/5.));
        WriteFile(hLog, szLine, strlen(szLine), &dWritten, NULL);
    }
    else
    {
        lstrcpy(szText, App_GetStringFromStringTable(
                              IDS_TEST_LOG_ALT_WORDS_TYPED_NATURAL, g_nLangID));
        WideCharToMultiByte(CP_UTF8, 0 /* must be zero */, szText, -1,        
            szMultiByteName, sizeof(szMultiByteName), NULL /* must be NULL */, 
                                                       NULL /* must be NULL */);
        sprintf(szLine, "%s %d\r\n\0", szMultiByteName, 
                                                       m_vecSampleWords.size());
        WriteFile(hLog, szLine, strlen(szLine), &dWritten, NULL);
    }

    // Log WPM.
    if (g_bNaturalWordCount)
    {
        lstrcpy(szText, App_GetStringFromStringTable(
                                 IDS_TEST_LOG_ALT_WPM_TYPED_TYPIST, g_nLangID));
        WideCharToMultiByte(CP_UTF8, 0 /* must be zero */, szText, -1,        
            szMultiByteName, sizeof(szMultiByteName), NULL /* must be NULL */, 
                                                       NULL /* must be NULL */);
        fWPM_Alternative = (float)(_tstoi(ptr->sTextLength.c_str())/5.)/
                                                                ptr->fTimeSpent;
        sprintf(szLine, "%s %.2f\r\n\r\n\0", szMultiByteName, fWPM_Alternative);
        WriteFile(hLog, szLine, strlen(szLine), &dWritten, NULL);
    }
    else
    {
        lstrcpy(szText, App_GetStringFromStringTable(
                                IDS_TEST_LOG_ALT_WPM_TYPED_NATURAL, g_nLangID));
        WideCharToMultiByte(CP_UTF8, 0 /* must be zero */, szText, -1,        
            szMultiByteName, sizeof(szMultiByteName), NULL /* must be NULL */, 
                                                       NULL /* must be NULL */);
        
        fWPM_Alternative = (float)(m_vecSampleWords.size())/ptr->fTimeSpent;
        sprintf(szLine, "%s %.2f\r\n\r\n\0", szMultiByteName, fWPM_Alternative);
        WriteFile(hLog, szLine, strlen(szLine), &dWritten, NULL);
    }

    CloseHandle(hLog);

    if (g_bDebug) App_Debug_FunctionCallTrace("Mode4_UpdateLog()");
}

void Debug_Mode4_WriteTestResults()
{
    HANDLE hLog;
    //DWORD dwType, dwLength, dwResult;
    DWORD dWritten;
    TCHAR szLog[256];
    TCHAR szFileName[] = _T("c:\\TestResults.txt");
    //TCHAR szBuffer[100];
    //char szAnsiBuffer[100];
    UINT i;

    DeleteFile(szFileName);
    
    hLog = CreateFile(szFileName, GENERIC_READ|GENERIC_WRITE, 
        FILE_SHARE_READ|FILE_SHARE_WRITE, NULL /* security - not used*/,          
        OPEN_ALWAYS /* if exists - just open; if does not exist, create it */,
                                  FILE_ATTRIBUTE_NORMAL /* normal use */, NULL);         

    // Tell the writer we need to write at the end of file.
    DWORD dwOffset = SetFilePointer(hLog, 0, NULL, FILE_END); 

    std::sort(m_vecSampleWords.begin(), m_vecSampleWords.end(), 
                                                          WordLengthSortDesc());
    
    for (i = 0; i < m_vecSampleWords.size(); i++)
    {
/*	    wsprintf(szLog, _T("%s %d \r\n\0"), 
            m_vecSampleWords.at(i).sWord.c_str(), 
                                                m_vecSampleWords.at(i).nLength);*/ 
        wsprintf(szLog, _T("%s %d \r\n\0"), 
            m_vecSampleWords.at(i).sWord.c_str(), 
                                                m_vecSampleWords.at(i).nErrorsCount);

        WriteFile(hLog, szLog, lstrlen(szLog)*2, &dWritten, NULL);
    }

    CloseHandle(hLog);
}

void App_Debug_FunctionCallTrace(char* pszMessage)
{
    HANDLE hTraceFile;
    DWORD dwOffset;
    DWORD dWritten;
    char szLine[100]; 

    //--

    SYSTEMTIME st;
    GetLocalTime(&st);

    char szMonth[5];
    char szDay[5];
    char szYear[6];
    char szHour[5];
    char szMinute[5];
    char szSecond[5];

    char szDate[12];
    char szTime[10];
    //DeleteFile(_T("RT_TraceFile.txt"));

    // Convert "2:6" time into "02:06" time
    sprintf(szHour, st.wHour < 10 ? "0%d" : "%d",  st.wHour);
    sprintf(szMinute, st.wMinute < 10 ? "0%d" : "%d",  st.wMinute);
    sprintf(szSecond, st.wSecond < 10 ? "0%d" : "%d",  st.wSecond);
    sprintf(szYear, "%d", st.wYear);

    // Convert "1/29/2005" date into "01/29/05" date
    sprintf(szMonth, st.wMonth < 10 ? "0%d" : "%d",  st.wMonth);
    sprintf(szDay, st.wDay < 10 ? "0%d" : "%d",  st.wDay);

    sprintf(szDate, "%s/%s/%s", szMonth, szDay, szYear); 
    sprintf(szTime, "%s:%s:%s", szHour, szMinute, szSecond);

    //hTraceFile = CreateFileA("RT_TraceFile.txt", 
    if (g_bDebug) hTraceFile = CreateFileA("RT_TraceFile.txt", 
        GENERIC_READ | GENERIC_WRITE, 
        FILE_SHARE_READ | FILE_SHARE_WRITE, NULL /* security - not used */,          
        OPEN_ALWAYS /* if exists - just open; if does not exist, create it */,
                                  FILE_ATTRIBUTE_NORMAL /* normal use */, NULL);

    // Tell the writer we need to write at the end of file.
    dwOffset = SetFilePointer(hTraceFile, 0, NULL, FILE_END);

    // sprintf(szLine, "%s0\r\n", pszMessage): no carriage return!
    //Mode4_GetDateTimeStamp(szDate, szDayOfWeek, szTime);

    sprintf(szLine, "%s,%s: %s\r\n", szDate, szTime, pszMessage); 
    WriteFile(hTraceFile, szLine, strlen(szLine), &dWritten, NULL);
                                                                          
    CloseHandle(hTraceFile);
}
// TODO: write to UTF-8 file (French, Russian). Currently the function
// writes to ANSI, which is OK for English.
void Mode4_CreateCustomList()
{
    if (g_bDebug) App_Debug_FunctionCallTrace("Mode4_CreateCustomList Entered");

    HANDLE hFile;
    HMENU hMenu, hSubMenu;
    DWORD dwOffset;
    DWORD dWritten;

    //TCHAR szDevDirectory[MAX_PATH];
    TCHAR szCurDirectory[MAX_PATH]; 
    TCHAR szCustomFilePath[MAX_PATH];
    
    TCHAR szBuffer[100];
    char szLine[100];  // custom word length
    char szLine2[100]; // custom word length plus "\r\n"

    #ifndef DEVELOPMENT_STRUCTURE
        // Construct Custom_Words.dat path using current directory.
        GetCurrentDirectory(MAX_PATH, szCurDirectory);

        lstrcpy(szCustomFilePath, szCurDirectory);
        if (g_nLangID == 1) 
            lstrcat(szCustomFilePath, 
                                  _T("\\EN\\Training Words\\Custom_Words.dat"));
        else if (g_nLangID == 2)
            lstrcat(szCustomFilePath, 
                                  _T("\\FR\\Training Words\\Custom_Words.dat"));
        else if (g_nLangID == 3)
            lstrcat(szCustomFilePath, 
                                  _T("\\RU\\Training Words\\Custom_Words.dat"));
    #else
        // Construct Custom_Words.dat path using development directory.
        lstrcpy(szDevDirectory, APP_DIRECTORY_UNICODE);

        lstrcpy(szCustomFilePath, szDevDirectory);
        if (g_nLangID == 1) 
            lstrcat(szCustomFilePath, 
                                  _T("\\EN\\Training Words\\Custom_Words.dat"));
        else if (g_nLangID == 2)
            lstrcat(szCustomFilePath, 
                                  _T("\\FR\\Training Words\\Custom_Words.dat"));
        else if (g_nLangID == 3)
            lstrcat(szCustomFilePath, 
                                  _T("\\RU\\Training Words\\Custom_Words.dat"));
    #endif

    // Delete previous Custom_Words.dat file anyway (if Custom_Words.dat does 
    // not exist DeleteFile harmlessly fails).
    DeleteFile(szCustomFilePath);

    hFile = CreateFile(szCustomFilePath /*szFileName*/, 
        GENERIC_READ|GENERIC_WRITE, 
        FILE_SHARE_READ|FILE_SHARE_WRITE, NULL /* security - not used*/,          
        OPEN_ALWAYS /* if exists - just open; if does not exist, create it */,
                                  FILE_ATTRIBUTE_NORMAL /* normal use */, NULL);         

    // Tell the writer we need to write at the end of file.
    //dwOffset = SetFilePointer(m_hLog, 0, NULL, FILE_END);
    dwOffset = SetFilePointer(hFile, 0, NULL, FILE_END);

    // Obtain checked words (Unicode) from list view, convert them to UTF-8,
    // write line-by-line to file.
    for (int i = 0; i < ListView_GetItemCount(g_hwndMode4_ListView); i++)
    {
        if (ListView_GetCheckState(g_hwndMode4_ListView, i))
        {
            ListView_GetItemText(g_hwndMode4_ListView, i, 0, szBuffer, 100);

            WideCharToMultiByte(CP_UTF8, 0 /* must be zero */, szBuffer, -1,        
            szLine, sizeof(szLine), NULL /* must be NULL */, 
                                                    NULL /* must be NULL */);
    
            strcpy(szLine2, szLine);
            strcat(szLine2, "\r\n");

            WriteFile(hFile, szLine2, strlen(szLine2), &dWritten, NULL);
        }
    }

    // Get Modes menu handle.
    hMenu = GetMenu(g_hWnd);
    hSubMenu = GetSubMenu(hMenu, 2 /* Modes */);
        
    if (hFile == INVALID_HANDLE_VALUE)
    {
        EnableMenuItem(hSubMenu, IDM_MODES_CUSTOM, MF_BYCOMMAND | MF_GRAYED);
    }
    else
    {
        EnableMenuItem(hSubMenu, IDM_MODES_CUSTOM, MF_BYCOMMAND | MF_ENABLED);
        CloseHandle(hFile);
    }

    if (g_bDebug) App_Debug_FunctionCallTrace("Mode4_CreateCustomList Exited");
}

void Mode3_SetCustomModeItemState()
{
    HMENU hMenu, hSubMenu;
    HANDLE hFile;
    //TCHAR szDevDirectory[MAX_PATH];
    TCHAR szCurDirectory[MAX_PATH]; 
    TCHAR szCustomFilePath[MAX_PATH];
    
    #ifndef DEVELOPMENT_STRUCTURE
        // Construct Custom_Words.dat path using current directory.
        GetCurrentDirectory(MAX_PATH, szCurDirectory);

        lstrcpy(szCustomFilePath, szCurDirectory);
        
        if (g_nLangID == 1) 
            lstrcat(szCustomFilePath, 
                                  _T("\\EN\\Training Words\\Custom_Words.dat"));
        else if (g_nLangID == 2)
            lstrcat(szCustomFilePath, 
                                  _T("\\FR\\Training Words\\Custom_Words.dat"));
        else if (g_nLangID == 3)
            lstrcat(szCustomFilePath, 
                                  _T("\\RU\\Training Words\\Custom_Words.dat"));
    #else
        // Construct Custom_Words.dat path using development directory.
        lstrcpy(szDevDirectory, APP_DIRECTORY_UNICODE);
        lstrcpy(szCustomFilePath, szDevDirectory);

        if (g_nLangID == 1) 
            lstrcat(szCustomFilePath, 
                                  _T("\\EN\\Training Words\\Custom_Words.dat"));
        if (g_nLangID == 2) 
            lstrcat(szCustomFilePath, 
                                  _T("\\FR\\Training Words\\Custom_Words.dat"));
        else if (g_nLangID == 3)
            lstrcat(szCustomFilePath, 
                                  _T("\\RU\\Training Words\\Custom_Words.dat"));
    #endif
    
    // Check if custom words file exists and activate/gray custom mode item
    // accordingly.
    hFile = CreateFile(szCustomFilePath, GENERIC_READ, FILE_SHARE_READ, 
        NULL /* security - not used*/,          
        OPEN_EXISTING /* open when exists, fail if not exists */,
                                  FILE_ATTRIBUTE_NORMAL /* normal use */, NULL);

    // Get Modes menu handle.
    hMenu = GetMenu(g_hWnd);
    hSubMenu = GetSubMenu(hMenu, 2 /* Modes */);
        
    if (hFile == INVALID_HANDLE_VALUE)
    {
        EnableMenuItem(hSubMenu, IDM_MODES_CUSTOM, MF_BYCOMMAND | MF_GRAYED);
    }
    else
    {
        EnableMenuItem(hSubMenu, IDM_MODES_CUSTOM, MF_BYCOMMAND | MF_ENABLED);
        CloseHandle(hFile);
    }
}


// PURPOSE: Mode4_CheckAll() checks or unchecks all checkboxes in the list view
//          according to the three options: (1) if partially checked, checks 
//          all (typical scenario), (2) if nothing is checked, checks all, (3)
//          (3) if all checked, clears all.

void Mode4_CheckAll()
{
    BOOL bState;
    int i;

    BOOL bAllChecked;

    // Notice that if there is only one item in the LV (zero item) the "for" 
    // cycle is skipped by its logic. If no items, Mode4_CheckAll() is not called 
    // by the external logic (see call).
    bState = ListView_GetCheckState(g_hwndMode4_ListView, 0);
    for (i = 1; i < ListView_GetItemCount(g_hwndMode4_ListView); i++)
              bState = bState * ListView_GetCheckState(g_hwndMode4_ListView, i);

    bAllChecked = bState;
    
    if (!bAllChecked) // if nothing is checked or partially checked, check all
    {
        for (i = 0; i < ListView_GetItemCount(g_hwndMode4_ListView); i++)
        {
            ListView_SetCheckState(g_hwndMode4_ListView, i, TRUE);
        }

        SendMessage(g_hwndMode4_Button_CheckAll, WM_SETTEXT, 0 /* not used */, 
           (LPARAM)App_GetStringFromStringTable(
                                  IDS_TEST_MODE_BUTTON_UNCHECK_ALL, g_nLangID));

        // Enable custom list button if disabled.
        if (!IsWindowEnabled(g_hwndMode4_Button_CreateCustomList))
                        EnableWindow(g_hwndMode4_Button_CreateCustomList, TRUE);
    }
    else // all checked, clear all
    {
        for (int i = 0; i < ListView_GetItemCount(g_hwndMode4_ListView); i++)
        {
            ListView_SetCheckState(g_hwndMode4_ListView, i, FALSE);
        }

        SendMessage(g_hwndMode4_Button_CheckAll, WM_SETTEXT, 0 /* not used */, 
           (LPARAM)App_GetStringFromStringTable(IDS_TEST_MODE_BUTTON_CHECK_ALL, 
                                                                    g_nLangID));
        // Disable button for making a custom list.
        EnableWindow(g_hwndMode4_Button_CreateCustomList, FALSE);
    }
}

// Note: initially copied from OnKeyDown function.
void Mode4_GetLastTypedWord(LPTSTR pszLastWord)
{
    TCHAR szBuffer[100];
    int nPos, nLen; 
    //int nRes;
    TCHAR szRawLastWord[100]; // may still have ".", ",", "!" at the ending.
    LPTSTR pRLW = NULL;
    TCHAR szExcludedLastCharsList[] = _T(",.?!:;)}\"]"); 
    LPTSTR pszELCL = szExcludedLastCharsList;
    TCHAR szLastCharacter[2];
    int i;
    BOOL bPunctuationSuffixFound = FALSE;

    // Get current line from the edit control. MSDN: Before sending the message,  
    // set the first word of this buffer to the size, in TCHARs, of the buffer.  
    // For ANSI text, this is the number of bytes; for Unicode text, this is the 
    // number of characters. The size in the first word is overwritten by the 
    // copied line.
    *((LPWORD)szBuffer) = (WORD)100;
    nPos = SendMessage(g_hwndEdit, EM_LINEFROMCHAR, 
                                   -1 /* use current line */, 0 /* not used */);
    nLen = SendMessage(g_hwndEdit, EM_GETLINE, nPos, (LPARAM)szBuffer);

    // Add termination NULL. MSDN: The copied line does not contain a terminating 
    // null character.
    szBuffer[nLen] = _T('\0');

    // Obtain last typed word from the last line.
    pRLW = _tcsrchr(szBuffer, _T(' ')); 

    if (pRLW  == NULL) // there are no spaces, the whole line is the last word
    {
        lstrcpy(szRawLastWord, szBuffer);
    }
    else // space(s) found, we need a pointer to the next character after space
    {
        pRLW++;
        lstrcpy(szRawLastWord, pRLW);
    }

    // Check if the word suffixed with punctuation character like ",.?!:;", and 
    // trim it if exists. Note that lstrlen returns the length in characters 
    // (not including NULL).
    szLastCharacter[0] = szRawLastWord[lstrlen(szRawLastWord) - 1];
    szLastCharacter[1] = _T('\0'); // _T("\0"); - error

    for (i = 0; i < lstrlen(szExcludedLastCharsList); i++)
    {
       if (*szLastCharacter == *pszELCL)
       {
           lstrcpy(pszLastWord, szRawLastWord);
           // Right-trim [one] puctuation character with NULL.
           pszLastWord[lstrlen(szRawLastWord) - 1] = _T('\0');
           bPunctuationSuffixFound = TRUE;
           break;
       }
       pszELCL++;
    }

    if (!bPunctuationSuffixFound) lstrcpy(pszLastWord, szRawLastWord);
}

BOOL Mode4_IsWordTypedCorrectly(LPTSTR pszBuffer, WPARAM wParam)
{
    TCHAR szText[MAX_SAMPLE_TEXT];
    HWND hwndTB;
    HMENU hMenu, hSubMenu;

    SetFocus(g_hwndEdit);
    SendMessage(g_hwndEdit, WM_GETTEXT, MAX_SAMPLE_TEXT, (LPARAM)szText);

    if(lstrlen(szText) == 0)
    {
        lstrcpy(szText, pszBuffer);
    }
    else
    {
        lstrcat(szText, pszBuffer); 
    }

    // Spell check while typing, invoke a beep when typing error occurs.
    // Beep urges user to correct mistyped word. Make no beep while 
    // correcting (this is symbolizes by Backspace).
    if (_tcsnccmp(g_szSampleText, szText, lstrlen(szText)) == 0)  
    {
        // No errors now, test additionally if test is completed.
        if (_tcscmp(g_szSampleText, szText) == 0) 
        {
            // Auto-terminate test.
            if (g_bTestRunning)
            {
                Mode4_OnStartTest();

                hwndTB = GetDlgItem(g_hWnd, ID_TOOLBAR) ;
                SendMessage(hwndTB, TB_CHANGEBITMAP, ID_START_OR_STOP_TEST,
                                                              MAKELPARAM(1, 0));
                SendMessage(hwndTB, TB_SETSTATE, ID_FILE_OPEN, 
                                                               TBSTATE_ENABLED);
                SendMessage(hwndTB, TB_SETSTATE, ID_DETAILED_STATS, 
                                                               TBSTATE_ENABLED);
                hMenu = GetMenu(g_hWnd);
                hSubMenu = GetSubMenu(hMenu, 3);
                
                _tcscpy(g_szBuffer, 
                    App_GetStringFromStringTable(IDS_TEST_MODE_MENU_START_TEST, 
                                                                    g_nLangID));
                ModifyMenu(hSubMenu, 
                  IDM_COMMANDS_START_OR_STOP_TEST /* menu item to be changed */, 
                     MF_BYCOMMAND, IDM_COMMANDS_START_OR_STOP_TEST, g_szBuffer); 
                                                            //_T("Start Test"));
                EnableMenuItem(hSubMenu, IDM_COMMANDS_DETAILED_STATS, 
                                                     MF_BYCOMMAND | MF_ENABLED);
                EnableMenuItem(hSubMenu, IDM_COMMANDS_SHOW_REPORT, 
                                                     MF_BYCOMMAND | MF_ENABLED);
            }
            // Set flag to allow draw basic statistics background.
            g_bTestCompleted = TRUE;
            // Auto-expand partially.
            Mode4_OnResize(TRUE);

             // Show hint to continue with detailed stats.
            if (g_bShowHint1Dialog)
                DialogBox(g_hInst, (LPCTSTR)IDD_MODE4_HINT1, g_hWnd, 
                                            (DLGPROC)Mode4_Hint1Dialog_WndProc);
            //--
            // Provide stats for list view control.
            //Mode4_WriteTestResults(); 
        }

        return TRUE;
    }
    else
    {
        // WM_CHAR message: wParam is the ASCII code, lParam is a 
        // bit-encoded state vector.
        if (wParam != VK_BACK /* VK_BACK is 0x08 */) 
        {
            g_nErrorsCount++; g_nTotalErrorsCount++; 
            if (g_bAlertEnabled) Beep(2000, 200);
        }

        return FALSE;
    }
}

short Mode4_GetSampleWordIndex(LPTSTR pszWord)
{
    UINT i;

    for (i = 0; i < m_vecSampleWords.size(); i++)
    {
        if (_tcscmp(m_vecSampleWords.at(i).sWord.c_str(), pszWord) == 0)
        {
            return i;
        }
    }

    return -1; // not found
}


// Based on Mode3_IsWordTypedCorrectly.
BOOL Mode12_SpellCheck(LPTSTR pszBuffer, WPARAM wParam)
{
    TCHAR szLastWord[100];

    Mode12_GetLastTypedWord(szLastWord);

    // In fact, Mode12_GetLastTypedWord returns a word except last character 
    // typed, therefore we add it manually.
    if (g_bWordInProcessOfTyping)
    {
        if (lstrlen(szLastWord) == 0)
        {
            lstrcpy(szLastWord, pszBuffer);
        }
        else
        {
            lstrcat(szLastWord, pszBuffer); 
        }
    }

    //MessageBox(0, szLastWord, szLastWord, 0);

    // Lookup left/right list boxes to check if last entered word (actually 
    // last entered symbols after space and before new space) matches any list 
    // box string that begins with the specified string).
    if (!Mode12_IsListBoxMember(szLastWord))
    {
        //MessageBox(0, L"not found", szLastWord, 0);
        
        if (wParam != VK_BACK /* VK_BACK is 0x08 */)
        {
            if (g_bAlertEnabled) Beep(2000, 200);
        }
    }
    //else
    //{
 //       MessageBox(0, L"Found", szLastWord, 0);
    //}
    
    return TRUE;
}

// Modification of OnKeyDown, which makes lookup for full match with last 
// entered word, so that the function makes looked for partial match (any list 
// box string that begins with the specified string). In fact, merely 
// LB_FINDSTRING used instead of LB_FINDSTRINGEXACT to find a match.
BOOL Mode12_IsListBoxMember(LPTSTR pszWord)
{
    TCHAR szLastWord[100];
    int nRes;

    lstrcpy(szLastWord, pszWord);

    // MSDN'03: LB_FINDSTRING message to find the first string in a 
    // list box that begins with the specified string.

    // Check if typed word has a match in the LH/RH list boxes. If yes 
    // select matching item in corresponding list box. We use global 
    // g_nLastTimeHand here to store what training word was used last 
    // (left or right). This discards redundant search (i.e. when RH 
    // list used it is not necessary to search in LH words).
    switch (g_nLastTimeHand)
    {
    case 1: // left hand
        nRes = SendMessage(g_hwndMode1_ListBox_LeftHandWords, 
            LB_FINDSTRING, -1 /* search from the top */, 
                                                    (LPARAM)szLastWord);

        if (nRes != LB_ERR)
        {
            // Partial match in the left list box is found.
            return TRUE;
        }
        else
        {
            nRes = SendMessage(g_hwndMode1_ListBox_RightHandWords, 
                LB_FINDSTRING, -1 /* search from the top */, 
                                                    (LPARAM)szLastWord);

            if (nRes != LB_ERR)
            {
                g_nLastTimeHand = 2;
                // Partial match in the right list box is found.
                return TRUE;
            }
        }
        break;
    case 2: // right hand
        nRes = SendMessage(g_hwndMode1_ListBox_RightHandWords, 
            LB_FINDSTRING, -1 /* search from the top */, 
                                                    (LPARAM)szLastWord);

        if (nRes != LB_ERR)
        {
            // Partial match in the right list box is found.
            return TRUE;
        }
        else
        {
            nRes = SendMessage(g_hwndMode1_ListBox_LeftHandWords, 
                LB_FINDSTRING, -1 /* search from the top */, 
                                                    (LPARAM)szLastWord);

            if (nRes != LB_ERR)
            {
                g_nLastTimeHand = 1;
                // Partial match in the left list box is found.
                return TRUE;
            }
        }
        break;
    }

    return FALSE;
}

// wParam is used to prevent beep (and error counting) when BACKSPACE is 
// pressed.
BOOL Mode3_IsWordTypedCorrectly(LPTSTR pszBuffer, WPARAM wParam)
{
    TCHAR szLastWord[100];

    Mode3_GetLastTypedWord(szLastWord); // last word obtained as out-parameter

    // In fact, Mode3_GetLastTypedWord returns a word except last character 
    // typed, therefore we add it manually.
    if (g_bWordInProcessOfTyping)
    {
        if(lstrlen(szLastWord) == 0)
        {
            lstrcpy(szLastWord, pszBuffer);
        }
        else
        {
            lstrcat(szLastWord, pszBuffer); 
        }
    }

    
    if (!Mode3_IsCustomWordsMember(szLastWord))
    {
        if (wParam != VK_BACK /* VK_BACK is 0x08 */)
        {
            g_nErrorsCount++; 
            g_nCorrectlyTypedCount = 0; // reset to zero

            if (g_bAlertEnabled) Beep(2000, 200);
        }
    }
    
    return TRUE;
}

// In fact, Mode12_GetLastTypedWord returns a word except last character typed. 
// Mode12_GetLastTypedWord is a copy (just edit window hangle renamed) of void 
// Mode3_GetLastTypedWord(LPTSTR pszLastWord). TODO: probably merge.
void Mode12_GetLastTypedWord(LPTSTR pszLastWord)
{
    TCHAR szBuffer[100];
    int nPos, nLen; 
    LPTSTR pLW;

    // Get current line number by sending EM_LINEFROMCHAR message.
    // MSDN: Before sending the  message, set the first word of this buffer to 
    // the size, in TCHARs, of the buffer. For ANSI text, this is the number of
    // bytes; for Unicode text, this is the number of characters. The size in 
    // the first word is overwritten by the copied line.
    *((LPWORD)szBuffer) = (WORD)100;
    nPos = SendMessage(g_hwndEdit, EM_LINEFROMCHAR, 
                                   -1 /* use current line */, 0 /* not used */);
    // Get current line. Because (MSDN), the copied line does not contain a 
    // terminating null character, we add it manually.
    nLen = SendMessage(g_hwndEdit, EM_GETLINE, nPos, (LPARAM)szBuffer);
    // This is the last (or current) line of characters with terminating NULL.
    szBuffer[nLen] = _T('\0'); 

    // Extract last word by trimming characters on the left (with ending space).
    // If there is no space character, return line as the last word.
    pLW = _tcsrchr(szBuffer, _T(' ')); 

    if (pLW  == NULL)
    {
        lstrcpy(pszLastWord, szBuffer);
    }
    else
    {
        // We need a pointer to the next character after space.
        pLW++;
        lstrcpy(pszLastWord, pLW);
    }
}

// In fact, Mode3_GetLastTypedWord returns a word except last character 
// typed.
// TODO: This is a copy of GetLastTypedWord with g_hwndEdit -> g_hwndMode3_Edit.
// Probably merge
void Mode3_GetLastTypedWord(LPTSTR pszLastWord)
{
    TCHAR szBuffer[100];
    int nPos, nLen; 
    //int nRes;
    LPTSTR pLW;

    // Get current line number by sending EM_LINEFROMCHAR message.
    // MSDN: Before sending the  message, set the first word of this buffer to 
    // the size, in TCHARs, of the buffer. For ANSI text, this is the number of
    // bytes; for Unicode text, this is the number of characters. The size in 
    // the first word is overwritten by the copied line.
    *((LPWORD)szBuffer) = (WORD)100;
    nPos = SendMessage(g_hwndMode3_Edit, EM_LINEFROMCHAR, 
                                   -1 /* use current line */, 0 /* not used */);
    // Get current line. Because (MSDN), the copied line does not contain a 
    // terminating null character, we add it manually.
    nLen = SendMessage(g_hwndMode3_Edit, EM_GETLINE, nPos, (LPARAM)szBuffer);
    // This is the last (or current) line of characters with terminating NULL.
    szBuffer[nLen] = _T('\0'); 

    // Extract last word by trimming characters on the left (with ending space).
    // If there is no space character, return line as the last word.
    pLW = _tcsrchr(szBuffer, _T(' ')); 

    if (pLW  == NULL)
    {
        lstrcpy(pszLastWord, szBuffer);
    }
    else
    {
        // We need a pointer to the next character after space.
        pLW++;
        lstrcpy(pszLastWord, pLW);
    }
}

BOOL Mode3_IsCustomWordsMember(LPTSTR pszWord /* partial or complete word */)
{
    UINT i;

    for (i = 0; i < m_vecCustomWords.size(); i++)
    {
        //MessageBox(0, m_vecCustomWords.at(i).sWord.c_str(), pszWord, 0);
        // See if the last [partial] word is among custom words: 
        // Compare left n ([partial] word length) symbols of custom word with
        // [partial] word .
        if (_tcsnccmp(m_vecCustomWords.at(i).sWord.c_str(), 
                                                pszWord, lstrlen(pszWord)) == 0)
        {
            return TRUE;
        }
    }

    return FALSE; // not found
}

short Mode3_GetSampleWordIndex(LPTSTR pszWord)
{
    UINT i;

    for (i = 0; i < m_vecCustomWords.size(); i++)
    {
        if (_tcscmp(m_vecCustomWords.at(i).sWord.c_str(), pszWord) == 0)
        {
            return i;
        }
    }

    return -1; // not found
}

void Mode3_UpdateData(short i /* item number in vector */)
{
    LV_FINDINFO lvf;
    int iLVItem;
    TCHAR szBuffer[100];
    char szAnsiBuffer[100];
    float fAverageCharTime;
    short nTypedCount;

    lvf.flags = LVFI_STRING;
    lvf.psz = m_vecCustomWords.at(i).sWord.c_str();
    iLVItem = ListView_FindItem(g_hwndMode3_ListView, -1, &lvf);

    nTypedCount = m_vecCustomWords.at(i).nTypedCount;

    // Skip list view update if item is not found or nTypedCount is equal 
    // (erroneuosly) to zero.
    if (iLVItem != -1 /* item is not found */ && 
                                     nTypedCount != 0 /* used as denominator */) 
    {
        // Show average time per character.
        fAverageCharTime = m_vecCustomWords.at(i).fTotalTimeSpent/
                     (float)(m_vecCustomWords.at(i).nLength)/(float)nTypedCount;
        sprintf(szAnsiBuffer, "%.2f", fAverageCharTime);
                               
        // TODO: check if 2nd parameter OK for French chars like �. 
        MultiByteToWideChar(CP_ACP, 0 /* no handling of composite etc chars  */, 
            szAnsiBuffer, -1 /* input string is NULL-terminated */, 
            szBuffer, 
                   sizeof(szBuffer)/sizeof(szBuffer[0]) /* wide chars count */); 

        ListView_SetItemText(g_hwndMode3_ListView, iLVItem, 2, szBuffer);

        // Show error count for the word.
        sprintf(szAnsiBuffer, "%d", m_vecCustomWords.at(i).nTotalErrorsCount);

        // TODO: check if 2nd parameter OK for French chars like �.
        MultiByteToWideChar(CP_ACP, 0 /* no handling of composite etc chars  */, 
            szAnsiBuffer, -1 /* input string is NULL-terminated */, 
            szBuffer, 
                   sizeof(szBuffer)/sizeof(szBuffer[0]) /* wide chars count */);

        ListView_SetItemText(g_hwndMode3_ListView, iLVItem, 3, szBuffer);

        // Set number of time the word was typed.
        wsprintf(szBuffer, _T("%d"), nTypedCount);
        ListView_SetItemText(g_hwndMode3_ListView, iLVItem, 4, szBuffer);
    }
}

string App_XMLReadValue(char* pszTagName /* always ANSI*/, 
                                                    char* pszFFN /* XML file */)
{
    // When queried value is Debug, the Debug's initial value TRUE (hardcoded), 
    // but I do not trace to debug reading to avoid unconditional creation of
    // the trace file.
    if (g_bDebug == TRUE && 
        strcmp("Debug", pszTagName) != 0 /* queried value is not Debug */) 
                        App_Debug_FunctionCallTrace("App_XMLReadValue Entered");

    char szLine[200];
    //char szCurDirectory[MAX_PATH]; // argument of open is ANSI string
    //char szFilePath[MAX_PATH];     // argument of open is ANSI string
    string sLine, sLeftTrimmedLine, sParameter;
    string sOpenTag;
    string sTagName, sTagValue;
    ifstream settings_file;

    sTagName = pszTagName;
    sOpenTag = "<" + sTagName + ">";
    settings_file.open(pszFFN);

    if (settings_file) 
    {
        while (settings_file.good())
        {
            settings_file.getline(szLine, MAX_PATH);
            sLine = szLine;

            if (sLine.find(sOpenTag) !=  -1)
            {
                // Left-trim line found out of spaces if they exist, leave 
                // original line as it is if spaces are not found.
                sLeftTrimmedLine = 
                             sLine.substr(sLine.find_first_not_of(" ")).c_str();

                // Extract tag value.
                sTagValue =  sLeftTrimmedLine.substr(sOpenTag.length(), 
                           sLeftTrimmedLine.length() - 2*sOpenTag.length() - 1);
            }
        }
        settings_file.close();
    }

    if (g_bDebug == TRUE && 
        strcmp("Debug", pszTagName) != 0 /* queried value is not Debug */) 
                         App_Debug_FunctionCallTrace("App_XMLReadValue Exited");
    
    return sTagValue; 
}

void App_XMLWriteValue(char* pszTagName /* always ANSI*/, 
                                             char* pszTagValue /* always ANSI*/)
{
    if (g_bDebug) App_Debug_FunctionCallTrace("App_XMLWriteValue Entered");

    char szLine[MAX_PATH];
    char szCurDirectory[MAX_PATH]; // ANSI because argument of open is ANSI string
    char szFilePath[MAX_PATH];     // ANSI because argument of open is ANSI string
    //string sString, sParameter;
    string sLine, sSpacePrefix;
    string sTopPart, sBottomPart, sUpdatedLine;
    string sResult;
    BOOL bTaggedLineFound = FALSE;
    string sOpenTag, sEndTag;
    string sTagName, sTagValue;
    HANDLE hLog;
    DWORD dWritten;

    sTagName = pszTagName;
    sTagValue = pszTagValue;

    sOpenTag = "<" + sTagName + ">";
    sEndTag  = "</" + sTagName + ">";

    ifstream settings_file;

    #ifndef DEVELOPMENT_STRUCTURE
        // Construct Settings.xml path using current directory.
        GetCurrentDirectoryA(MAX_PATH, szCurDirectory);
        strcpy(szFilePath, szCurDirectory);
        strcat(szFilePath, "\\Settings.xml");
    #else
        // Construct Settings.xml path using development directory.
        strcpy(szFilePath, APP_DIRECTORY_ANSI);
        strcat(szFilePath, "\\Settings.xml");
    #endif

    settings_file.open(szFilePath);

    if (settings_file) 
    {
        while (settings_file.good())
        {
            settings_file.getline(szLine, MAX_PATH);
            sLine = szLine;

            // TODO: works, can improve by removing all (or some) spaces
            //in a line to parse to (fool-proof).
            //sString.
            if (sLine.find(sOpenTag) !=  -1)
            {
                bTaggedLineFound = TRUE;

                // Figure out the number of spaces in the prefix of sLine, and
                // insert this number of spaces starting from zero position into
                // updated line so that the file format is preserved. Note that
                // insert does not work with with " " (' ' are OK).
                sSpacePrefix.insert(0, sLine.find_first_not_of(" "), ' '); 

                sUpdatedLine =  sSpacePrefix + sOpenTag + sTagValue + sEndTag + 
                                                                         "\r\n"; 
            }
            else
            {
                if (!bTaggedLineFound)
                {
                    if (sTopPart.length() == 0)
                        sTopPart = sLine + "\r\n"; // 1st pass
                    else
                        sTopPart = sTopPart + sLine + "\r\n";
                }
                else
                {
                    if (sBottomPart.length() == 0)
                        sBottomPart = sLine + "\r\n"; // 1st pass
                    else
                        sBottomPart = sBottomPart + sLine + "\r\n";
                }
            }
        }
        settings_file.close();

        sResult = sTopPart + sUpdatedLine + sBottomPart;

        // Remove (right-trim) last CRLF ("\r\n") from the output. This prevents 
        // adding CRLF to the end of the file on every write pass (otherwise 
        // output file grows indefinitely).
        sResult.resize(sResult.size() - 2);
    }

    // Re-write XML file completely (delete, then write again).
    DeleteFileA(szFilePath);
    hLog = CreateFileA(szFilePath, GENERIC_READ|GENERIC_WRITE, 
        FILE_SHARE_READ|FILE_SHARE_WRITE, NULL /* security - not used*/,          
        OPEN_ALWAYS /* if exists - just open; if does not exist, create it */,
                                  FILE_ATTRIBUTE_NORMAL /* normal use */, NULL);         

    // No SetFilePointerA, WriteFileA functions when we need to write ANSI
    // text in UNICODE build. LL SetFilePointer, WriteFile auto-determine what
    // type of writing to use based on CreateFileW or CreateFileA used.
    
    // Tell the writer we need to write at the end of file.
    DWORD dwOffset = SetFilePointer(hLog, 0, NULL, FILE_BEGIN);

    //WriteFile(hLog, szXMLString, strlen(szXMLString), &dWritten, NULL);
    // This helps to avoid a *buffer overrun* when actual size of XML string is 
    // bigger then declared in szXMLString.
    WriteFile(hLog, sResult.c_str(), strlen(sResult.c_str()), &dWritten, NULL);
    CloseHandle(hLog);

    if (g_bDebug) App_Debug_FunctionCallTrace("App_XMLWriteValue Exited");
}

void App_ConfigureOptions()
{
    PROPSHEETHEADER	m_PropSheet;
    PROPSHEETPAGE m_psp[2];
    
    memset(m_psp, 0, sizeof(m_psp));
    memset(&m_PropSheet, 0, sizeof(m_PropSheet));

    m_psp[0].dwSize = sizeof(PROPSHEETPAGE);
    m_psp[0].dwFlags = PSP_DEFAULT|PSP_USETITLE;
    m_psp[0].hInstance = g_hInst;
    m_psp[0].pszTemplate = (LPCTSTR)IDD_PP_GENERAL;
    m_psp[0].pszTitle = _T("Application");
    m_psp[0].pfnDlgProc = (DLGPROC)App_OptionsPageProc;

    m_psp[1].dwSize = sizeof(PROPSHEETPAGE);
    m_psp[1].dwFlags = PSP_USETITLE;
    m_psp[1].hInstance = g_hInst;
    m_psp[1].pszTemplate = (LPCTSTR)IDD_PP_MODE1;
    m_psp[1].pszTitle = _T("Mode1");
    m_psp[1].pfnDlgProc = (DLGPROC)Mode1_OptionsPageProc;

    m_PropSheet.dwSize = sizeof(PROPSHEETHEADER);
    m_PropSheet.dwFlags = PSH_PROPSHEETPAGE|PSH_USECALLBACK;
    m_PropSheet.hInstance = g_hInst;
    m_PropSheet.pszCaption = _T("Options"); 
    m_PropSheet.nPages = 2;
    m_PropSheet.nStartPage = 0;
    m_PropSheet.ppsp = (LPCPROPSHEETPAGE)m_psp;
    m_PropSheet.pfnCallback = (PFNPROPSHEETCALLBACK)App_OptionsSheetProc;

    PropertySheet(&m_PropSheet);
}

LRESULT CALLBACK App_OptionsSheetProc(HWND hDlg, UINT message, LPARAM lParam)
{
    g_hPropSheetDlg = hDlg;

    switch( message )
    {
    case WM_INITDIALOG:
        return TRUE;
    }

    return FALSE;
}

LRESULT CALLBACK App_OptionsPageProc(HWND hDlg, UINT message, WPARAM wParam, 
                                                                  LPARAM lParam)
{
    //TCHAR szUpdateTime[10];
    //int iUpdateTime;

    switch (message)
    {
        case WM_INITDIALOG:
            {
                // Set initial state of save last used check box. 
                if(g_bSaveLastUsed)
                {
                    CheckDlgButton(hDlg, IDC_OPTIONS_GENERAL_AUTOSAVE, 
                                                                   BST_CHECKED);
                }
                else
                {
                    CheckDlgButton(hDlg, IDC_OPTIONS_GENERAL_AUTOSAVE, 
                                                                 BST_UNCHECKED);
                }

                return TRUE;
            }
        case WM_COMMAND:
    //		switch(LOWORD(wParam))
    //		{
    //		case  :
    //			break;
    //		default:
    //			break;
    //		}
            break;
        case WM_NOTIFY:
            switch (((NMHDR FAR *) lParam)->code) 
            {
            case PSN_APPLY:

                if (IsDlgButtonChecked(hDlg, IDC_OPTIONS_GENERAL_AUTOSAVE))
                 {
                    g_bSaveLastUsed = TRUE;
                }
                else
                {
                    g_bSaveLastUsed = FALSE;
                }

                  break;
            case PSN_RESET: 
                // User clicked Cancel or system upper-right "x".
                //PostQuitMessage(0);
                break;				
            }
    }
    return FALSE;
}

LRESULT CALLBACK Mode1_OptionsPageProc(HWND hDlg, UINT message, WPARAM wParam, 
                                                                  LPARAM lParam)
{
    //TCHAR szUpdateTime[10];

    switch (message)
    {
        case WM_INITDIALOG:
            {
                //Page1_OnInitDialog(hDlg);
            }
            break;
        case WM_COMMAND:
    //		switch(LOWORD(wParam))
    //		{
    //			case IDC_PP1_COMBO:
    //				switch(HIWORD(wParam))
    //				{
    //						break;
    //						}
    //				}
    //              break;
    //		    case IDC_IPP_BUTTON1:
    //              break;
    //		}
            break;
        case WM_NOTIFY:
            switch (((NMHDR FAR *) lParam)->code) 
            {
                case PSN_APPLY:
                    PostQuitMessage(0);
                    break;
                case PSN_RESET: 
                    // User clicked Cancel or system upper-right "x".
                    PostQuitMessage(0);
                    break;	
            }
            break;
    }
    return FALSE;
};

// From MConsole.
BOOL App_CenterWindow(HWND hwndCentered, HWND hwndPreferredOwner, 
                      int iHorizMove /* default value is 0 (see declaration) */)
{
    BOOL  Result ;
    POINT OwnerCenter , CenteredUL ;
    RECT  WindowRect, OwnerRect, WorkArea ;
    SIZE  CenteredWindow ;

    //ASSERT (NULL != hwndCentered) ;
    //ASSERT (IsWindow (hwndCentered)) ;

    // If a preferred owner isn't specified...
    //if (NULL == hwndPreferredOwner)
        // Use the owner of the window to be centered
    //    hwndPreferredOwner = GetWindowOwner (hwndCentered) ;

    // Get the rectangle for the workarea
    Result = SystemParametersInfo (SPI_GETWORKAREA, sizeof (RECT), &WorkArea, 0) ;

    // If the above call failed, the new shell probably isn't running
    // therefore there is no tray and no workarea.

    // Use the screen size as the workarea size.
    if (!Result) {
        SetRect (&WorkArea,
                 0, 0, 
                 GetSystemMetrics (SM_CXSCREEN),
                 GetSystemMetrics (SM_CYSCREEN)) ;
    }

    // Center around the owner window, if one,
    // otherwise center in the work area
    //if (NULL != hwndPreferredOwner) {
        //ASSERT (IsWindow (hwndPreferredOwner)) ;
        GetWindowRect (hwndPreferredOwner, &OwnerRect) ;
    //}
    //else
    //    OwnerRect = WorkArea ;

    // Preferred center point
    OwnerCenter.x = (OwnerRect.left + OwnerRect.right) / 2 ;
    OwnerCenter.y = (OwnerRect.top + OwnerRect.bottom) / 2 ;

    // Get the centered window's rectangle and compute height/width
    GetWindowRect (hwndCentered, &WindowRect) ;
    CenteredWindow.cx = WindowRect.right - WindowRect.left ;
    CenteredWindow.cy = WindowRect.bottom - WindowRect.top ;

    // Center window in owner horizontally
    // Calculates the left side coordinate
    CenteredUL.x = OwnerCenter.x - CenteredWindow.cx / 2 ;

    // Center window in owner vertically
    // Calculates the top side coordinate
    CenteredUL.y = OwnerCenter.y - CenteredWindow.cy / 2 ;

    // If the left edge of the centered window is clipped by the workarea
    // move the window horizontally to the right until left edge is exposed.
    if (CenteredUL.x < WorkArea.left)
        CenteredUL.x = WorkArea.left ;

    // If the right edge of the centered window is clipped by the workarea
    // move the window horizontally to the left until right edge is exposed.
    else if (CenteredUL.x + CenteredWindow.cx > WorkArea.right)
        CenteredUL.x = WorkArea.right - CenteredWindow.cx ;

    // If the top edge of the centered window is clipped by the workarea
    // move the window vertically down until top edge is exposed.
    if (CenteredUL.y < WorkArea.top)
        CenteredUL.y = WorkArea.top ;

    // If the bottom edge of the centered window is clipped by the workarea
    // move the window vertically up until bottom edge is exposed.
    else if (CenteredUL.y + CenteredWindow.cy > WorkArea.bottom)
        CenteredUL.y = WorkArea.bottom - CenteredWindow.cy ;

    // Reposition the window centered (within visibility constraints)
    return SetWindowPos (hwndCentered, NULL, 
        CenteredUL.x - iHorizMove, CenteredUL.y, 0, 0, 
                                    SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOZORDER);
}

LRESULT CALLBACK Mode4_Hint1Dialog_WndProc(HWND hDlg, UINT message, 
                                                   WPARAM wParam, LPARAM lParam)
{
    RECT rcMainWnd;
    RECT rc;
    int nRes;
    HMENU hMenu, hSubMenu;
    HICON hIcon;

    switch (message)
    {
    case WM_INITDIALOG:
        // Center dialog relative to the application window.
        GetWindowRect(g_hWnd, &rcMainWnd);
        GetWindowRect(hDlg, &rc);
        SetWindowPos(hDlg, NULL, rcMainWnd.left + ((rcMainWnd.right - 
            rcMainWnd.left) - (rc.right - rc.left)) / 2, rcMainWnd.top + 
            ((rcMainWnd.bottom - rcMainWnd.top) - (rc.bottom - rc.top)) / 2,
                                             0, 0, SWP_NOSIZE | SWP_NOACTIVATE);

        // Provide proper language strings.
        SendMessage(GetDlgItem(hDlg, IDC_MODE4_H1D_STATIC1), 
            WM_SETTEXT, 0 /* not used */, 
           (LPARAM)App_GetStringFromStringTable(IDS_MODE4_H1D_STATIC1, 
                                                                    g_nLangID));
        SendMessage(GetDlgItem(hDlg, IDC_MODE4_H1D_CHECKBOX), 
            WM_SETTEXT, 0 /* not used */, 
           (LPARAM)App_GetStringFromStringTable(IDS_MODE4_H1D_CHECKBOX, 
                                                                    g_nLangID));
        SendMessage(GetDlgItem(hDlg, IDC_MODE4_H1D_STATIC2), 
            WM_SETTEXT, 0 /* not used */, 
           (LPARAM)App_GetStringFromStringTable(IDS_MODE4_H1D_STATIC2, 
                                                                    g_nLangID));
        // Set initial state of the checkbox. 
        SendMessage(GetDlgItem(hDlg, IDC_MODE4_H1D_CHECKBOX), 
                BM_SETCHECK, g_bShowHint1Dialog ? 
                BST_UNCHECKED /* inversed in UI */ : 
                                             BST_CHECKED, 0 /* must be zero */);

        hIcon=LoadIcon(NULL, IDI_INFORMATION); 
        return TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK)
        {
            nRes = SendMessage(GetDlgItem(hDlg, 
                    IDC_MODE4_H1D_CHECKBOX), BM_GETCHECK, 
                                            0 /* not used */, 0 /* not used */);
            if (nRes == BST_CHECKED)
                    g_bShowHint1Dialog = FALSE;
            else if (nRes == BST_UNCHECKED)
            {
                    g_bShowHint1Dialog = TRUE;
            }
            
            EndDialog(hDlg, LOWORD(wParam));

            // Disable "Detailed Statistics" menu item.
            hMenu = GetMenu(g_hWnd);
            hSubMenu = GetSubMenu(hMenu, 3 /* Commands menu*/);
            SendMessage(GetDlgItem(g_hWnd, ID_TOOLBAR), TB_CHANGEBITMAP, 
                                           ID_DETAILED_STATS, MAKELPARAM(6, 0));
            EnableMenuItem(hSubMenu, IDM_COMMANDS_DETAILED_STATS, 
                                                      MF_BYCOMMAND | MF_GRAYED);
            Mode4_OnDetailedStats();
            return TRUE;
        }

        if (LOWORD(wParam) == IDCANCEL) 
        {
            EndDialog(hDlg, LOWORD(wParam));
            return TRUE;
        }

        break;
    }
    return FALSE;
}

// TODO: probably merge with Mode4_Hint1Dialog_WndPro
LRESULT CALLBACK Mode4_Hint2Dialog_WndProc(HWND hDlg, UINT message, 
                                                   WPARAM wParam, LPARAM lParam)
{
    RECT rcMainWnd;
    RECT rc;
    int nRes;
    HICON hIcon;

    switch (message)
    {
    case WM_INITDIALOG:
        // Center dialog relative to the DS window.
        GetWindowRect(g_hWndMode4_DetailedStats, &rcMainWnd);
        GetWindowRect(hDlg, &rc);
        SetWindowPos(hDlg, NULL, rcMainWnd.left + ((rcMainWnd.right - 
            rcMainWnd.left) - (rc.right - rc.left)) / 2, rcMainWnd.top + 
            ((rcMainWnd.bottom - rcMainWnd.top) - (rc.bottom - rc.top)) / 2,
                                             0, 0, SWP_NOSIZE | SWP_NOACTIVATE);
        // Provide proper language strings.
        SendMessage(GetDlgItem(hDlg, IDC_MODE4_H2D_STATIC1), 
            WM_SETTEXT, 0 /* not used */, 
            (LPARAM)App_GetStringFromStringTable(IDS_MODE4_H2D_STATIC1, 
                                                                    g_nLangID));
        SendMessage(GetDlgItem(hDlg, IDC_MODE4_H2D_CHECKBOX), 
            WM_SETTEXT, 0 /* not used */, 
            (LPARAM)App_GetStringFromStringTable(IDS_MODE4_H2D_CHECKBOX, 
                                                                    g_nLangID));
        SendMessage(GetDlgItem(hDlg, IDC_MODE4_H2D_STATIC2), 
            WM_SETTEXT, 0 /* not used */, 
            (LPARAM)App_GetStringFromStringTable(IDS_MODE4_H2D_STATIC2, 
                                                                    g_nLangID));
        // Set initial state of the checkbox. 
        SendMessage(GetDlgItem(hDlg, IDC_MODE4_H2D_CHECKBOX), 
                BM_SETCHECK, g_bShowHint2Dialog ? 
                BST_UNCHECKED /* inversed in UI */ : 
                                             BST_CHECKED, 0 /* must be zero */);

        hIcon=LoadIcon(NULL, IDI_INFORMATION); 
        return TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK)
        {
            nRes = SendMessage(GetDlgItem(hDlg, 
                    IDC_MODE4_H2D_CHECKBOX), BM_GETCHECK, 
                                            0 /* not used */, 0 /* not used */);
            if (nRes == BST_CHECKED)
                    g_bShowHint2Dialog = FALSE;
            else if (nRes == BST_UNCHECKED)
            {
                    g_bShowHint2Dialog = TRUE;
            }
            
            EndDialog(hDlg, LOWORD(wParam));

            // Update toolbar icon and disable it.
            SendMessage(GetDlgItem(g_hWnd, ID_TOOLBAR), TB_CHANGEBITMAP, 
                     ID_DETAILED_STATS, MAKELPARAM(4 /* active pie icon */, 0));
            SendMessage(GetDlgItem(g_hWnd, ID_TOOLBAR), TB_SETSTATE, 
                                      ID_DETAILED_STATS, TBSTATE_INDETERMINATE);
            DestroyWindow(g_hWndMode4_DetailedStats);

            // Reinitialize g_iScanCodePrev to the deafult value to properly 
            // handle first word of Custom Mode.
            g_iScanCodePrev = -1;
            OnMenuMode3();
            
            return TRUE;
        }

        if (LOWORD(wParam) == IDCANCEL) 
        {
            EndDialog(hDlg, LOWORD(wParam));
            return TRUE;
        }

        break;
    }
    return FALSE;
}

LRESULT CALLBACK Mode4_DetailedStats_WndProc(HWND hWnd, UINT message, 
                                                   WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hdc;
    HMENU hMenu, hSubMenu;
    HWND hwndTB;
    LVHITTESTINFO ht = {0};
    BOOL bAllUnchecked;
    int i;

    switch (message) 
    {
    case WM_COMMAND:
        // Parse the menu selections:
        switch (LOWORD(wParam) /* control ID*/)
        {
        case IDC_TEST_MODE_BUTTON_CUSTOM_LIST:
            Mode4_CreateCustomList();
            if (g_bShowHint2Dialog)
                DialogBox(g_hInst, (LPCTSTR)IDD_MODE4_HINT2, hWnd, 
                                            (DLGPROC)Mode4_Hint2Dialog_WndProc);
            break;
        case IDC_TEST_MODE_BUTTON_CHECK_ALL:
            if (ListView_GetItemCount(g_hwndMode4_ListView)) Mode4_CheckAll();
            break;
        case IDM_COMMANDS_SHOW_REPORT:
            Mode4_CreateHtmlReport();
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
        break;
    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);
        // TODO: Add any drawing code here...
        EndPaint(hWnd, &ps);
        break;
    case WM_NOTIFY:
        // I don't use list view's control ID (NMHDR's idFrom) because double
        // click handled in windows procedure of list view, and it is redundant.
        switch (((LPNMHDR)lParam)->code) 
        {
            case NM_DBLCLK:
                DialogBox(g_hInst, (LPCTSTR)IDD_DS_COLORS, hWnd, 
                                    (DLGPROC)Mode4_DetailedStatsColors_WndProc);
                break;
            case LVN_ITEMCHANGED: 
                // Skip this handler if message comes from clear all button.
                if (((LPNMLISTVIEW)lParam)->hdr.idFrom == 
                                                 IDC_TEST_MODE_BUTTON_CHECK_ALL) 
                    break;
                // Enable Create Custom List button if it is in disabled state 
                // and user checks a checkbox. Since there is no notification 
                // for checkbox check, I use standard walkaround by using 
                // hittest.
                GetCursorPos(&ht.pt); 
                ClientToScreen(HWND_DESKTOP, &ht.pt);
                ScreenToClient(g_hwndMode4_ListView, &ht.pt); 

                ListView_HitTest(g_hwndMode4_ListView, &ht);

                // Check if all items are unchecked (LVN_ITEMCHANGED must be 
                // used since NM_CLICK is fired BEFORE clicked checkbox is
                // actually changed).
                bAllUnchecked = TRUE;
                for (i = 0; i < ListView_GetItemCount(g_hwndMode4_ListView); 
                                                                            i++)
                {
                    if (ListView_GetCheckState(g_hwndMode4_ListView, i)) 
                    {
                        bAllUnchecked = FALSE;
                        break;
                    }
                }

                if (ht.flags == LVHT_ONITEMSTATEICON && 
                    !IsWindowEnabled(g_hwndMode4_Button_CreateCustomList))
                        EnableWindow(g_hwndMode4_Button_CreateCustomList, TRUE);
                else if (ht.flags == LVHT_ONITEMSTATEICON && bAllUnchecked)
                       EnableWindow(g_hwndMode4_Button_CreateCustomList, FALSE);

                break;
        }

        // Hangle list view custom draw.
        return Mode4_NotifyHandler(hWnd, message, wParam, lParam);
    case WM_CLOSE:
        // Get handle to commands submenu and enable detailed statistics menu 
        // item (again).
        hMenu = GetMenu(g_hWnd);
        hSubMenu = GetSubMenu(hMenu, 3 /* Commands menu*/);
        hwndTB = GetDlgItem(g_hWnd, ID_TOOLBAR) ;

        SendMessage(hwndTB, TB_CHANGEBITMAP, ID_DETAILED_STATS,  
                                            MAKELPARAM(4 /* expand icon */, 0));
        EnableMenuItem(hSubMenu, IDM_COMMANDS_DETAILED_STATS, 
                                                     MF_BYCOMMAND | MF_ENABLED);
        DestroyWindow(hWnd); 
        break;
    case WM_DESTROY:
        //PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

void Mode4_OnFileOpen()
{
    TCHAR szSampleText[MAX_SAMPLE_TEXT];
    OPENFILENAME ofn;
    // in (initialize value of File Name edit control), out (selected file).
    TCHAR szFile[MAX_PATH]; 
    TCHAR szInitialDir[MAX_PATH];
    TCHAR szCurDirectory[MAX_PATH];
    TCHAR szInitialCurDirectory[MAX_PATH]; 

    // MSDN'03: the last string in the filter's fuffer must be terminated by two 
    // NULL characters. 
    TCHAR szFilter[] = _T("All\0*.*\0Text\0*.TXT\0\0");
    // MSDN'03: NULL if initialization is not necessary.
    szFile[0] = _T('\0'); 
    LPTSTR pszSampleText = NULL;

    // Obtain path to current directory.
    GetCurrentDirectory(MAX_PATH, szCurDirectory);
    lstrcpy(szInitialDir, szCurDirectory);

    // Store application-wide used current directory, which must be intact 
    // (File/Open dialog changes current directory), and restore it on function 
    // end.
    lstrcpy(szInitialCurDirectory, szCurDirectory);

    switch (g_nLangID)
    {
    case 1:	
        _tcscat(szInitialDir, _T("\\EN\\Tests"));
        break;
    case 2:	
        _tcscat(szInitialDir, _T("\\FR\\Tests"));
        break;
    case 3:	
        _tcscat(szInitialDir, _T("\\RU\\Tests"));
        break;
    default:
        _tcscat(szInitialDir, _T("\\EN\\Tests"));
    }

    // Initialize OPENFILENAME structure.
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = g_hWnd;
    ofn.lpstrFile = szFile; // in (no initial file name in open dialog)/out 
    ofn.nMaxFile = sizeof(szFile); 
    ofn.lpstrFilter = szFilter;
    ofn.nFilterIndex = 1;      // one-based index of default filter 
    ofn.lpstrFileTitle = NULL; // selected file name (i.e. without path), not used
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = szInitialDir;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    // Display the Open dialog box. 
    if (GetOpenFileName(&ofn) == TRUE) 
    {
        LPTSTR pszText = szSampleText;
        if (Mode4_LoadFile(pszText /* out */, ofn.lpstrFile /* in */))
        {
            lstrcpy(szSampleText, pszText);
            lstrcpy(g_szSampleText, szSampleText); //likely avoid 

            SendMessage(g_hwndEdit2, WM_SETTEXT, 0 /* not used */, 
                                                          (LPARAM)szSampleText);
            Mode4_ParseSampleText(szSampleText);

            // Store test file selection in the global variable.
            g_sSampleFFN = ofn.lpstrFile;
            //MessageBox(0, g_sSampleFFN_EN.c_str(), ofn.lpstrFile, 0);
        }
    }
    // Restore application-used current directory on function end.
    SetCurrentDirectory(szInitialCurDirectory);
}

// App_SetLanguage: not valid on Windows 95 (SetThreadLocale).
void App_SetLanguage()
{
    WORD wLanguageID; 
    HKL hKL; 
    DWORD lcid; 
    WCHAR szKLID_EN[] = L"00000409"; // 0x0409 is English (US) language ID
    WCHAR szKLID_FR[] = L"0000040c"; // 0x040c is French (standard) language ID
    WCHAR szKLID_RU[] = L"00000419"; // 0x0419 is Russian language ID

    switch (g_nLangID)
    {
    case 1:
        wLanguageID = MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US);
        // Make EN keyboard. The szKLID is a string composed of the "hexadecimal 
        // value of the Language Identifier (low word) and a device identifier 
        // (high word)" (MSDN). I do not make this string programmatically from 
        // the MAKELANGID's wLanguageID (the meaning of "device identifier" is 
        // not clear), albeit prefixing with 5 zeros would make it right for 
        // v1.2's three languages: e.g. with Russian's wLanguageID 0x0419 it 
        // gives "00000419". 
        hKL = LoadKeyboardLayout(szKLID_EN, 0);
        if (hKL != NULL) ActivateKeyboardLayout(hKL, 0); 
        break;
    case 2:
        wLanguageID = MAKELANGID(LANG_FRENCH, SUBLANG_FRENCH);
        // Make FR keyboard.
        hKL = LoadKeyboardLayout(szKLID_FR, 0);  
        if (hKL != NULL) ActivateKeyboardLayout(hKL, 0);
        break;
    case 3:
        wLanguageID = MAKELANGID(LANG_RUSSIAN, SUBLANG_DEFAULT);
        // Make RU keyboard.
        hKL = LoadKeyboardLayout(szKLID_RU, 0); 
        if (hKL != NULL) ActivateKeyboardLayout(hKL, 0); 
        break;
    default:
        wLanguageID = MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US);
        // Make EN keyboard.
        hKL = LoadKeyboardLayout(L"00000409", 0);
        if (hKL != NULL) ActivateKeyboardLayout(hKL, 0); 
        break;
    }

    lcid = MAKELCID(wLanguageID, SORT_DEFAULT);
    SetThreadLocale(lcid);
}

void App_SetMenuState()
{
    HMENU hMenu, hSubMenu;

    if (g_nLangID == 1)
        hMenu = LoadMenu(g_hInst, MAKEINTRESOURCE(IDC_TYPINGWIN32_EN));
    else if (g_nLangID == 2)
        hMenu = LoadMenu(g_hInst, MAKEINTRESOURCE(IDC_TYPINGWIN32_FR));
    else if (g_nLangID == 3)
        hMenu = LoadMenu(g_hInst, MAKEINTRESOURCE(IDC_TYPINGWIN32_RU));

    SetMenu(g_hWnd, hMenu);

    // Check toolbar and status bar menu items to indicate that they are
    // visible on application startup.
    hMenu = GetMenu(g_hWnd);
    hSubMenu = GetSubMenu(hMenu, 1);

    CheckMenuItem(hSubMenu, IDM_VIEW_TOOLBAR, MF_BYCOMMAND | MF_CHECKED);
    CheckMenuItem(hSubMenu, IDM_VIEW_STATUSBAR, MF_BYCOMMAND | MF_CHECKED); 
}

// Mode3_NotifyHandler, Mode3_CompareProc, Mode3_GetCompareReturnValue are initially 
// copied from Mode4 functions.

LRESULT Mode3_NotifyHandler(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
    NMLISTVIEW  *pNM_LISTVIEW = (NMLISTVIEW  *)lParam;
    LV_DISPINFO *pLV_DISPINFO = (LV_DISPINFO *)lParam;
    NMLVCUSTOMDRAW  *plvcd = (NMLVCUSTOMDRAW *)lParam;

    switch (pLV_DISPINFO->hdr.code)
    {
        case NM_CUSTOMDRAW:
            switch (plvcd->nmcd.dwDrawStage)
            {
                case CDDS_PREPAINT:
                    return CDRF_NOTIFYITEMDRAW;
                case CDDS_ITEMPREPAINT:
                    if (!(plvcd->nmcd.dwItemSpec % 2))
                    // Set color for odd row (none). 
                    ; 
                    else
                    // Set color for even row (use the same as with DS window).
                    plvcd->clrTextBk = g_clrMode4_DS_InterlacedColor;
                    //return ( CDRF_DODEFAULT );
                break;
            }
            break;
        case LVN_COLUMNCLICK:
            #define pnm (( LPNMLISTVIEW) lParam )

            // g_aSortOrder (initial/previous sort direction values) array 
            // provides that the sorting goes in the opposite direction 
            // when we click any column.
            if (g_aSortOrder[(int)(pnm->iSubItem)] == 0)
            {
                g_aSortOrder[(pnm->iSubItem)] = 1;
            }
            else
            {
                g_aSortOrder[(pnm->iSubItem)] = 0;
            }

            ListView_SortItems(g_hwndMode3_ListView, Mode3_CompareProc, 
                                                      (LPARAM)(pnm->iSubItem ));

            #undef pnm
            break;
        case LVN_KEYDOWN:

            NMLVKEYDOWN* pNM_LVKEYDOWN = (NMLVKEYDOWN*)lParam;
            int iLVItem;
            int iCWIndex;
            TCHAR szItemText[100];
            BOOL bControl = (GetKeyState(VK_CONTROL) < 0);

            if (pNM_LVKEYDOWN->wVKey == VK_DELETE)
            {
                // List view does not have a specific function to get selected item. The
                // workaround is to use GetNextItem.
                iLVItem = ListView_GetNextItem(g_hwndMode3_ListView, -1, LVNI_SELECTED);
                ListView_GetItemText(g_hwndMode3_ListView, iLVItem, 0, szItemText, 100);
                iCWIndex = Mode3_GetSampleWordIndex(szItemText);

                // Remove selected item from the list view and from m_vecCustomWords.
                ListView_DeleteItem(g_hwndMode3_ListView, iLVItem);
                // Single "iCWIndex" as argument of erase() does not work.
                m_vecCustomWords.erase(m_vecCustomWords.begin()+ iCWIndex);

                // Get ready edit area by setting focus to it (cursor should blink).
                SetFocus(g_hwndMode3_Edit);
            }

            // TODO: add body to handle saving of list view items on Ctrl+S.
            if (pNM_LVKEYDOWN->wVKey == 83 /* S */ && bControl)
            {
                //MessageBox(0, L"Ctrl+S", L"Ctrl+S", 0);
                Mode3_UpdateCustomList();
            }
            break;
    }

    return 0;
}

// Based on Mode4_CreateCustomList().
void Mode3_UpdateCustomList()
{
    if (g_bDebug) App_Debug_FunctionCallTrace("Mode3_UpdateCustomList Entered");

    HANDLE hFile;
    DWORD dwOffset;
    DWORD dWritten;

    //TCHAR szDevDirectory[MAX_PATH];
    TCHAR szCurDirectory[MAX_PATH]; 
    TCHAR szCustomFilePath[MAX_PATH];
    
    TCHAR szBuffer[100];
    char szLine[100];  // custom word length
    char szLine2[100]; // custom word length plus "\r\n"

    #ifndef DEVELOPMENT_STRUCTURE
        // Construct Custom_Words.dat path using current directory.
        GetCurrentDirectory(MAX_PATH, szCurDirectory);

        lstrcpy(szCustomFilePath, szCurDirectory);
        if (g_nLangID == 1) 
            lstrcat(szCustomFilePath, 
                                  _T("\\EN\\Training Words\\Custom_Words.dat"));
        else if (g_nLangID == 2)
            lstrcat(szCustomFilePath, 
                                  _T("\\FR\\Training Words\\Custom_Words.dat"));
        else if (g_nLangID == 3)
            lstrcat(szCustomFilePath, 
                                  _T("\\RU\\Training Words\\Custom_Words.dat"));
    #else
        // Construct Custom_Words.dat path using development directory.
        lstrcpy(szDevDirectory, APP_DIRECTORY_UNICODE);

        lstrcpy(szCustomFilePath, szDevDirectory);
        if (g_nLangID == 1) 
            lstrcat(szCustomFilePath, 
                                  _T("\\EN\\Training Words\\Custom_Words.dat"));
        else if (g_nLangID == 2)
            lstrcat(szCustomFilePath, 
                                  _T("\\FR\\Training Words\\Custom_Words.dat"));
        else if (g_nLangID == 3)
            lstrcat(szCustomFilePath, 
                                  _T("\\RU\\Training Words\\Custom_Words.dat"));
    #endif

    // Delete previous Custom_Words.dat file anyway (if Custom_Words.dat does 
    // not exist DeleteFile harmlessly fails).
    DeleteFile(szCustomFilePath);

    hFile = CreateFile(szCustomFilePath /*szFileName*/, 
        GENERIC_READ|GENERIC_WRITE, 
        FILE_SHARE_READ|FILE_SHARE_WRITE, NULL /* security - not used*/,          
        OPEN_ALWAYS /* if exists - just open; if does not exist, create it */,
                                  FILE_ATTRIBUTE_NORMAL /* normal use */, NULL);         

    // Tell the writer we need to write at the end of file.
    //dwOffset = SetFilePointer(m_hLog, 0, NULL, FILE_END);
    dwOffset = SetFilePointer(hFile, 0, NULL, FILE_END);

    // Obtain checked words (Unicode) from list view, convert them to UTF-8,
    // write line-by-line to file.
    for (int i = 0; i < (int)m_vecCustomWords.size(); i++)
    {
            lstrcpy(szBuffer, m_vecCustomWords.at(i).sWord.c_str());
            WideCharToMultiByte(CP_UTF8, 0 /* must be zero */, szBuffer, -1,        
                szLine, sizeof(szLine), NULL /* must be NULL */, 
                                                       NULL /* must be NULL */);
            strcpy(szLine2, szLine);
            strcat(szLine2, "\r\n");

            WriteFile(hFile, szLine2, strlen(szLine2), &dWritten, NULL);
    }

    CloseHandle(hFile);

    if (g_bDebug) App_Debug_FunctionCallTrace("Mode3_UpdateCustomList Exited");
}

int CALLBACK Mode3_CompareProc(LPARAM lParam1, LPARAM lParam2, 
                          LPARAM lSortColumn /* in, application-defined value*/)
{
    LV_FINDINFO lvf;
    int nItem1, nItem2;
    TCHAR szBuffer1[30], szBuffer2[30];
    float fValue1, fValue2;
    int iValue1, iValue2;
    LPTSTR pszBegin1 = NULL;
    LPTSTR pszBegin2 = NULL;
    int ch = '(';

    // Find item based on its lParam value and obtain item's column text
    // based on supplied lSortColumn value. Notice that we divide 
    // sizeof(...) by 2 (Unicode's character length in bytes) because sizeof 
    // returns number of bytes, but we need number of chars. 
    lvf.flags = LVFI_PARAM;
    lvf.lParam = lParam1;
    nItem1 = ListView_FindItem(g_hwndMode3_ListView, -1, &lvf);

    lvf.lParam = lParam2;
    nItem2 = ListView_FindItem(g_hwndMode3_ListView, -1, &lvf);

    ListView_GetItemText(g_hwndMode3_ListView, nItem1, (int)lSortColumn, 
                                                szBuffer1, sizeof(szBuffer1)/2);
    ListView_GetItemText(g_hwndMode3_ListView, nItem2, (int)lSortColumn, 
                                                szBuffer2, sizeof(szBuffer2)/2);

    
    // We assign manually numerical values to "n/a" items so that their relative
    // order remains untouched. Values assigned to "n/a" items might differ for
    // every column: 1st value must be big enough, 2nd  value is -1 always (zero 
    // don't work). Sort order is used to keep "n/a" items at the bottom always.
    switch( (int)lSortColumn)
    {
        case 0:  // word itself, no sort
            break;
        case 1:  // word's length column 
            fValue1 = _tstof(szBuffer1);
            fValue2 = _tstof(szBuffer2);
            return Mode3_GetCompareReturnValue(fValue1, fValue2, lSortColumn);
            break;
        case 2: // speed column
            if (_tcscmp(szBuffer1, _T("n/a")) == 0) 
            {
                fValue1 = (g_aSortOrder[lSortColumn] == 0) ? 
                         (float)100 /* arbitrary, but big enough */ : (float)-1;
            }
            else
            {
                fValue1 = _tstof(szBuffer1);
            }
            
            if (_tcscmp(szBuffer2, _T("n/a")) == 0)
            {
                fValue2 = (g_aSortOrder[lSortColumn] == 0) ? 
                         (float)100 /* arbitrary, but big enough */ : (float)-1;
            }
            else
            {
                fValue2 = _tstof(szBuffer2);
            }

            return Mode3_GetCompareReturnValue(fValue1, fValue2, lSortColumn);;
            break;
        case 3:	// typing errors column	
            if (_tcscmp(szBuffer1, _T("n/a")) == 0) 
            {
                fValue1 = (g_aSortOrder[lSortColumn] == 0) ? 
                         (float)100 /* arbitrary, but big enough */ : (float)-1;
            }
            else
            {
                fValue1 = _tstof(szBuffer1);
            }

            if (_tcscmp(szBuffer2, _T("n/a")) == 0)
            {
                fValue2 = (g_aSortOrder[lSortColumn] == 0) ? 
                         (float)100 /* arbitrary, but big enough */ : (float)-1;
            }
            else
            {
                fValue2 = _tstof(szBuffer2);
            }

            return Mode3_GetCompareReturnValue(fValue1, fValue2, lSortColumn);
            break;
        case 4:	// typed count column	
            if (_tcscmp(szBuffer1, _T("n/a")) == 0) 
            {
                iValue1 = (g_aSortOrder[lSortColumn] == 0) ? 
                                      1000 /* arbitrary, but big enough */ : -1;
            }
            else
            {
                iValue1 = _tstoi(szBuffer1);
            }

            if (_tcscmp(szBuffer2, _T("n/a")) == 0)
            {
                iValue2 = (g_aSortOrder[lSortColumn] == 0) ? 
                                      1000 /* arbitrary, but big enough */ : -1;
            }
            else
            {
                iValue2 = _tstoi(szBuffer2);
            }

            return Mode3_GetCompareReturnValue(iValue1, iValue2, lSortColumn);
            break;
        default:
            break;
    }

    return 0;
}

int Mode3_GetCompareReturnValue(float fValue1, float fValue2, int iSortColumn)
{
    // Sort next time in the opposite direction (initial/previous sort direction
    // is stored in g_aSortOrder array).
    if (g_aSortOrder[iSortColumn] == 0)
    {
        if (fValue1 > fValue2)
        {
            return 1;
        }

        if (fValue1 < fValue2)
        {
            return -1;
        }

        if (fValue1 == fValue2)
        {
            return 0;
        }
    }
    else
    {
        if (fValue1 > fValue2)
        {
            return -1;
        }

        if (fValue1 < fValue2)
        {
            return 1;
        }

        if (fValue1 == fValue2)
        {
            return 0;
        }
    }
    return 0; // never here
}

LRESULT Mode4_NotifyHandler(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
    NMLISTVIEW  *pNM_LISTVIEW = (NMLISTVIEW  *)lParam;
    LV_DISPINFO *pLV_DISPINFO = (LV_DISPINFO *)lParam;

    HWND     hwndListView = GetDlgItem(hwnd, IDC_TEST_MODE_LISTVIEW);
    TCHAR szWord[30];
    LV_FINDINFO lvf;
    int iFoundItem;

    switch (pNM_LISTVIEW->hdr.code)
    {
        case NM_CUSTOMDRAW:

            NMLVCUSTOMDRAW  *plvcd = (NMLVCUSTOMDRAW *)lParam;

            // Request prepaint notifications for each item by returning 
            // CDRF_NOTIFYITEMDRAW.
            switch (plvcd->nmcd.dwDrawStage)
            {
                case CDDS_PREPAINT:
                    return CDRF_NOTIFYITEMDRAW;
                case CDDS_ITEMPREPAINT:
                    return CDRF_NOTIFYSUBITEMDRAW;
                case CDDS_ITEMPREPAINT|CDDS_SUBITEM:

            //		if (bShowInterlacedColors)
                    {
                        if (!(plvcd->nmcd.dwItemSpec % 2))
                            // White is default. 
                            plvcd->clrTextBk = g_clrMode4_DS_TableBgColor; 
                        else
                            // Light gray is default. 
                            plvcd->clrTextBk = g_clrMode4_DS_InterlacedColor; 
                    }
            
                    switch( plvcd->iSubItem )
                    {
                        case 0:
                            lvf.flags = LVFI_PARAM;
                            lvf.lParam = plvcd->nmcd.lItemlParam;
                            iFoundItem = 
                              ListView_FindItem(g_hwndMode4_ListView, -1, &lvf);

                            ListView_GetItemText(g_hwndMode4_ListView, 
                                iFoundItem, 0 /* word's column */, szWord, 
                                              sizeof(szWord)/sizeof(szWord[0]));

                            switch (Mode4_GetWordType(szWord))
                            {
                                case 0:
                                    break;
                                case 1: // LH words (light blue is default) 
                                    plvcd->clrTextBk = g_clrMode4_DS_LHColor; 
                                    break;
                                case 2: // RH words (light red is default)
                                    plvcd->clrTextBk = g_clrMode4_DS_RHColor; 
                                    break;
                                case 3: // L2R words
                                    plvcd->clrTextBk = g_clrMode4_DS_L2RColor; 
                                    break;
                                case 4: // R2L words
                                    plvcd->clrTextBk = g_clrMode4_DS_R2LColor; 
                                    break;
                                case 5: // custom words (light orange is default)
                                    plvcd->clrTextBk = g_clrMode4_DS_CustomColor; 
                                    break;
                                default:
                                    plvcd->clrTextBk = RGB(255, 255, 255);
                                    break;
                            }
                            break;
                    
                        //default:
                            //plvcd->clrTextBk = clrBkColor;
                    }

                //return ( CDRF_DODEFAULT );
                }
            break;
    }

    switch( pLV_DISPINFO->hdr.code )
    {
        case LVN_COLUMNCLICK:

            #define pnm (( LPNMLISTVIEW) lParam )

            // g_aSortOrder (initial/previous sort direction values) array 
            // provides that the sorting goes in the opposite direction 
            // when we click any column.
            if (g_aSortOrder[(int)(pnm->iSubItem)] == 0)
            {
                g_aSortOrder[(pnm->iSubItem)] = 1;
            }
            else
            {
                g_aSortOrder[(pnm->iSubItem)] = 0;
            }

            ListView_SortItems(hwndListView, Mode4_CompareProc, 
                                                      (LPARAM)(pnm->iSubItem ));

            #undef pnm
            break;
    }

    return 0;
}

//int CALLBACK Mode4_CompareProc(HWND hwnd, LPARAM lParam1, LPARAM lParam2, 
//							                                  LPARAM lParamSort)

// Based on MConsole version. Fixed parameter list. We use Unicode string, but
// could use ANSI charactes actually (length, speed, typing errors) actually
// ( at least for now there are no column that need sort of Unicode data).
int CALLBACK Mode4_CompareProc(LPARAM lParam1, LPARAM lParam2, 
                          LPARAM lSortColumn /* in, application-defined value*/)
{
    LV_FINDINFO lvf;
    int nItem1, nItem2;
    TCHAR szBuffer1[30], szBuffer2[30];
    float fValue1, fValue2;
    int iValue1, iValue2;
    LPTSTR pszBegin1 = NULL;
    LPTSTR pszBegin2 = NULL;
    int ch = '(';

    // Find item based on its lParam value and obtain item's column text
    // based on supplied lSortColumn value. Notice that we divide 
    // sizeof(...) by 2 (Unicode's character length in bytes) because sizeof 
    // returns number of bytes, but we need number of chars. 
    lvf.flags = LVFI_PARAM;
    lvf.lParam = lParam1;
    nItem1 = ListView_FindItem(g_hwndMode4_ListView, -1, &lvf);

    lvf.lParam = lParam2;
    nItem2 = ListView_FindItem(g_hwndMode4_ListView, -1, &lvf);

    ListView_GetItemText(g_hwndMode4_ListView, nItem1, (int)lSortColumn, 
                                                szBuffer1, sizeof(szBuffer1)/2);
    ListView_GetItemText(g_hwndMode4_ListView, nItem2, (int)lSortColumn, 
                                                szBuffer2, sizeof(szBuffer2)/2);

    switch( (int)lSortColumn)
    {
        case 0:  // word itself, no sort
            break;
        case 1:	// typing errors column		
            iValue1 = _tstoi(szBuffer1);
            iValue2 = _tstoi(szBuffer2);

            return Mode4_GetCompareReturnValue(iValue1, iValue2, lSortColumn);
            break;
        case 2: // speed column
            fValue1 = _tstof(szBuffer1);
            fValue2 = _tstof(szBuffer2);

            return Mode4_GetCompareReturnValue(fValue1, fValue2, lSortColumn);;
            break;
        case 3:  // word's length colum 
            fValue1 = _tstof(szBuffer1);
            fValue2 = _tstof(szBuffer2);
            return Mode4_GetCompareReturnValue(fValue1, fValue2, lSortColumn);
            break;
        default:
            break;
    }

    return 0;
}

int Mode4_GetCompareReturnValue(float fValue1, float fValue2, int iSortColumn)
{
    // Sort next time in the opposite direction (initial/previous sort direction
    // is stored in g_aSortOrder array).
    if (g_aSortOrder[iSortColumn] == 0)
    {
        if (fValue1 > fValue2)
        {
            return 1;
        }

        if (fValue1 < fValue2)
        {
            return -1;
        }

        if (fValue1 == fValue2)
        {
            return 0;
        }
    }
    else
    {
        if (fValue1 > fValue2)
        {
            return -1;
        }

        if (fValue1 < fValue2)
        {
            return 1;
        }

        if (fValue1 == fValue2)
        {
            return 0;
        }
    }
    return 0; // never here
}

short Mode4_GetWordType(LPTSTR pszWord)
{
    TCHAR szLeftHandChars[MAX_LOADSTRING];
    TCHAR szRightHandChars[MAX_LOADSTRING];

    // Load left and right hand letters e.g. "qwertasdfgzxcvbQWERTASDFDGZXCVB"
    // and "yuiophjklnmYUIOPHJKLNM" for EN.
    _tcscpy(szLeftHandChars, 
        App_GetStringFromStringTable(IDS_TEST_MODE_LEFT_HAND_CHARS, g_nLangID));
    _tcscpy(szRightHandChars, 
        App_GetStringFromStringTable(IDS_TEST_MODE_RIGHT_HAND_CHARS,
                                                                    g_nLangID));
    LPTSTR pszWordChar = NULL;
    LPTSTR pszLHChar = szLeftHandChars;
    LPTSTR pszRHChar = szRightHandChars;
    BOOL bLHWord;
    BOOL bLeftChar;
    BOOL bRHWord;
    BOOL bRightChar;
    BOOL bLTORWord;
    BOOL bLeftPartChecked;
    BOOL bValidChar; 
    BOOL bRTOLWord;
    BOOL bRightPartChecked;
    //int i;

    // Check if word is LH word.
    bLHWord = TRUE;
    pszWordChar = pszWord;

    while (*pszWordChar != NULL /* or '\0'*/)
    {
        if (_tcschr(szLeftHandChars, *pszWordChar) != NULL)
            bLeftChar = TRUE;
        else
            bLeftChar = FALSE;

        bLHWord *= bLeftChar; 
        pszWordChar = CharNext(pszWordChar);
    }

    if (bLHWord) return 1;

    // Check if word is RH word.
    bRHWord = TRUE;
    pszWordChar = pszWord;

    while (*pszWordChar != '\0')
    {
        if (_tcschr(szRightHandChars, *pszWordChar) != NULL)
            bRightChar = TRUE;
        else
            bRightChar = FALSE;

        bRHWord *= bRightChar; 
        pszWordChar = CharNext(pszWordChar);
    }

    if (bRHWord) return 2;

    // Check if word is L2R word.
    bLTORWord = TRUE;
    pszWordChar = pszWord;
    bLeftPartChecked = FALSE;

    while (*pszWordChar != NULL)
    {
        if (_tcschr(szLeftHandChars, *pszWordChar) != NULL && 
                                                       bLeftPartChecked != TRUE)
            bValidChar = TRUE;
        else
        {
            bLeftPartChecked = TRUE;
            if (_tcschr(szRightHandChars, *pszWordChar) != NULL)
                bValidChar = TRUE;
            else
                bValidChar = FALSE;
        }

        bLTORWord *= bValidChar; 
        pszWordChar = CharNext(pszWordChar);
    }

    if (bLTORWord) return 3;

    // Check if word is R2L word.
    bRTOLWord = TRUE;
    pszWordChar = pszWord;
    bRightPartChecked = FALSE;

    while (*pszWordChar != NULL)
    {
        if (_tcschr(szRightHandChars, *pszWordChar) != NULL && 
                                                      bRightPartChecked != TRUE)
            bValidChar = TRUE;
        else
        {
            bRightPartChecked = TRUE;
            if (_tcschr(szLeftHandChars, *pszWordChar) != NULL)
                bValidChar = TRUE;
            else
                bValidChar = FALSE;
        }

        bRTOLWord *= bValidChar; 
        pszWordChar = CharNext(pszWordChar);
    }

    if (bRTOLWord) return 4;

    // Check if word is a custom word.
    if (Mode3_IsCustomWordsMember(pszWord)) return 5;

    return 0; // regular word
}

// Based on Mode3_PopulateWithCustomWords
// TODO: Merge 3 language case in one step (only \\EN\\Training Words
// and MultiByteToWideChar(CP_ACP/CP_UTF8 ..) should be modified).
void Mode4_GetCurrentCustomWords()
{
    char szLine[100];
    TCHAR szBuffer[100];
    wstring sBuffer;
    short nCounter = 0;
    ifstream custom_words_file, test;
    char szCurDirectory[255]; // argument of open is ANSI string
    char szFilePath[255];     // argument of open is ANSI string
    char szDevDirectory[255]; // argument of open is ANSI string
    CustomWords sw;
    
       // Obtain ANSI path to current directory.
    GetCurrentDirectoryA(255, szCurDirectory);
    // Set development directory.
    strcpy(szDevDirectory, APP_DIRECTORY_ANSI); 
    
    switch(g_nLangID)
    {
    case 1: // English

        // We use ANSI file as input file for English training words, and read 
        // it as a stream with usual ifstream (not wifstream). Then we convert 
        // each line (with MultiByteToWideChar) to Unicode stream. 

        // TODO: open takes ANSI string only, find function that can take 
        // Unicode.
        // TODO: avoid "test" stream, it is used now because if open fails, 
        // and stream closes I could not use the same name of string with 
        // different path.
        strcpy(szFilePath, szCurDirectory);
        strcat(szFilePath, "\\EN\\Training Words\\Custom_Words.dat");

        test.open(szFilePath);

        if (test == NULL)
        {
            test.close();

            // Switch to development directory during development.
            strcpy(szFilePath, szDevDirectory);
            strcat(szFilePath, "\\EN\\Training Words\\Custom_Words.dat");

            custom_words_file.open(szFilePath);
        }
        else
        {
            test.close();
            custom_words_file.open(szFilePath);
        }

        if (custom_words_file) 
        {
            //while (custom_words_file.good())
            while (!custom_words_file.eof())
            {
                custom_words_file.getline(szLine, 50);
                // TODO: eliminate one last pass (empty string)
                //if (strlen(szLine) == 0) break;

                // MultiByteToWideChar use is OK for Windows 98 etc (however 
                // function does not exist on 95).
                MultiByteToWideChar(CP_ACP, 
                    0 /* must be zero [TODO: clarify] */, szLine, 
                    -1 /* string NULL-terminated [TODO: clarify] */, 
                                szBuffer, sizeof(szBuffer)/sizeof(szBuffer[0])); 
                sBuffer = szBuffer;
                if (sBuffer.length() != 0)
                {
                    sw.sWord = sBuffer; 
                    sw.fTimeSpent = (float)0;
                    sw.nErrorsCount = 0; 
                    
                    m_vecCustomWords.push_back(sw);

                }
            }
        }
        custom_words_file.close();

        break;

    case 2: // French

        strcpy(szFilePath, szCurDirectory);
        strcat(szFilePath, "\\FR\\Training Words\\Custom_Words.dat");

        test.open(szFilePath);

        if (test == NULL)
        {
            test.close();

            // Switch to development directory during development.
            strcpy(szFilePath, szDevDirectory);
            strcat(szFilePath, "\\FR\\Training Words\\Custom_Words.dat");

            custom_words_file.open(szFilePath);
        }
        else
        {
            test.close();
            custom_words_file.open(szFilePath);
        }

        if (custom_words_file) 
        {
            //while (custom_words_file.good())
            while (!custom_words_file.eof())
            {
                custom_words_file.getline(szLine, 50);
                // TODO: eliminate one last pass (empty string)
                //if (strlen(szLine) == 0) break;

                MultiByteToWideChar(CP_ACP, 
                    0 /* must be zero [TODO: clarify] */, szLine, 
                    -1 /* string NULL-terminated [TODO: clarify] */, 
                                szBuffer, sizeof(szBuffer)/sizeof(szBuffer[0])); 
                sBuffer = szBuffer;
                if (sBuffer.length() != 0)
                {
                    sw.sWord = sBuffer; 
                    sw.fTimeSpent = (float)0;
                    sw.nErrorsCount = 0; 
                    
                    m_vecCustomWords.push_back(sw);

                }
            }
        }
        custom_words_file.close();

        break;
    case 3: // Russian
        strcpy(szFilePath, szCurDirectory);
        strcat(szFilePath, "\\RU\\Training Words\\Custom_Words.dat");

        test.open(szFilePath);

        if (test == NULL)
        {
            test.close();

            // Switch to development directory during development.
            strcpy(szFilePath, szDevDirectory);
            strcat(szFilePath, "\\RU\\Training Words\\Custom_Words.dat");

            custom_words_file.open(szFilePath);
        }
        else
        {
            test.close();
            custom_words_file.open(szFilePath);
        }

        if (custom_words_file) 
        {
            //while (custom_words_file.good())
            while (!custom_words_file.eof())
            {
                custom_words_file.getline(szLine, 50);
                // TODO: eliminate one last pass (empty string)
                //if (strlen(szLine) == 0) break;

                MultiByteToWideChar(CP_UTF8, 
                    0 /* must be zero [TODO: clarify] */, szLine, 
                    -1 /* string NULL-terminated [TODO: clarify] */, 
                                szBuffer, sizeof(szBuffer)/sizeof(szBuffer[0])); 
                sBuffer = szBuffer;
                if (sBuffer.length() != 0)
                {
                    sw.sWord = sBuffer; 
                    sw.fTimeSpent = (float)0;
                    sw.nErrorsCount = 0; 
                    
                    m_vecCustomWords.push_back(sw);

                }
            }
        }
        custom_words_file.close();

        break;
    };
}

void Mode3_UpdateBarsStateMenu()
{
    HWND hwndTB;
    HMENU hMenu, hSubMenu;
    hMenu = GetMenu(g_hWnd);
    hSubMenu = GetSubMenu(hMenu, 1);

    // TODO: g_hwndTB does not work, g_hwndStatus - OK.
    hwndTB = GetDlgItem(g_hWnd, ID_TOOLBAR);
    if (IsWindowVisible(hwndTB))
    {
        CheckMenuItem(hSubMenu, IDM_VIEW_TOOLBAR, 
                                                MF_BYCOMMAND | MF_CHECKED);
    }
    else
    {
        CheckMenuItem(hSubMenu, IDM_VIEW_TOOLBAR, 
                                            MF_BYCOMMAND | MF_UNCHECKED); 
    }

    if (IsWindowVisible(g_hwndStatus))
    {
        CheckMenuItem(hSubMenu, IDM_VIEW_STATUSBAR, 
                                                MF_BYCOMMAND | MF_CHECKED);
    }
    else
    {
        CheckMenuItem(hSubMenu, IDM_VIEW_STATUSBAR, 
                                            MF_BYCOMMAND | MF_UNCHECKED); 
    }
}

void Mode3_UpdateLanguageStrings()
{
    LVCOLUMN lvc;
    TCHAR szBuffer[MAX_LOADSTRING];
    lvc.mask = LVCF_TEXT; 

    lvc.iSubItem = 0;
    _tcscpy(szBuffer, 
        App_GetStringFromStringTable(IDS_CUSTOM_MODE_LV_COLUMN_WORD, 
                                                                    g_nLangID));
    lvc.pszText = szBuffer;
    ListView_SetColumn(g_hwndMode3_ListView, 0, &lvc);

    lvc.iSubItem = 1;
    _tcscpy(szBuffer, 
        App_GetStringFromStringTable(IDS_CUSTOM_MODE_LV_COLUMN_LENGTH, 
                                                                    g_nLangID));
    lvc.pszText = szBuffer;	
    ListView_SetColumn(g_hwndMode3_ListView, 1, &lvc);

    lvc.iSubItem = 2;
    _tcscpy(szBuffer, 
        App_GetStringFromStringTable(IDS_CUSTOM_MODE_LV_COLUMN_SPEED, 
                                                                    g_nLangID));
    lvc.pszText = szBuffer;	
    ListView_SetColumn(g_hwndMode3_ListView, 2, &lvc);

    lvc.iSubItem = 3;
    _tcscpy(szBuffer, 
        App_GetStringFromStringTable(IDS_CUSTOM_MODE_LV_COLUMN_ERRORS, 
                                                                    g_nLangID));
    lvc.pszText = szBuffer;
    ListView_SetColumn(g_hwndMode3_ListView, 3, &lvc);

    lvc.iSubItem = 4;
    _tcscpy(szBuffer, 
        App_GetStringFromStringTable(IDS_CUSTOM_MODE_LV_COLUMN_COUNT, 
                                                                    g_nLangID));
    lvc.pszText = szBuffer;
    ListView_SetColumn(g_hwndMode3_ListView, 4, &lvc);
}

void App_OnChangeLanguage(short nLangID)
{
    WORD wLanguageID; 
    HKL hKL; 
    DWORD lcid; 
    HMENU hMenu, hSubMenu, hSubMenu2;
    // String versions of language ID.
    WCHAR szKLID_EN[] = L"00000409"; // English (US) language ID: 0x0409
    WCHAR szKLID_FR[] = L"0000040c"; // French (standard) language ID: 0x040c
    WCHAR szKLID_RU[] = L"00000419"; // Russian language ID: 0x0419  

    switch (nLangID)
    {
    case 1:
        g_nLangID = 1;

        // Update g_sSampleFFN from Settings.xml that is one variable for all 
        // languages.
        App_XMLRead_SampleTestFFN();

        // Update application title. Use special (long) English title when 
        // switching language to English.
        SetWindowText(g_hWnd, _T("Rational Typist (English Version)")); 

        // Update menu language to English (not valid on Windows 95).
        wLanguageID = MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US);
        lcid = MAKELCID(wLanguageID, SORT_DEFAULT);
        SetThreadLocale(lcid); // TODO: LL redundant if IDC_XXX_EN/FR/RU uesd
        hMenu = LoadMenu(g_hInst, MAKEINTRESOURCE(IDC_TYPINGWIN32_EN));
        SetMenu(g_hWnd, hMenu);

        // Append dynamic language menu.
        App_AddLanguageMenu();

        // Check menu item.
        hSubMenu = GetSubMenu(hMenu, 1 /* View */);
        hSubMenu2 = GetSubMenu(hSubMenu, 2 /* Language */);
        ModifyMenu(hSubMenu, ID_LANGUAGE_ENGLISH, 
            MF_POPUP | MF_BYCOMMAND | MF_CHECKED, (UINT_PTR)hSubMenu2, 
            App_GetStringFromStringTable(IDS_APP_MENUITEM_LANG_EN, 
                                                                g_nLangID));
        if (g_nMode == 4) OnMenuMode4();
        
        if (g_nMode == 3)
        {
            Mode3_UpdateBarsStateMenu();
            Mode3_UpdateLanguageStrings();
        }

        if (g_nMode == 1 || g_nMode == 2 || g_nMode == 3)
        {
            PopulateWithTrainingWords(g_nMode, 1);
        }

        // This function localizes strings for all modes.
        App_LocalizeStrings(1);
        App_UpdateToolbarButtonsStrings(1); // g_nLangID can be used

        // Update active keaboard to English (install one if absent).
        hKL = LoadKeyboardLayout(szKLID_EN, 0);
        if (hKL != NULL) ActivateKeyboardLayout(hKL, 0);

        break;
    case 2:
        g_nLangID = 2;

        // Update g_sSampleFFN from Settings.xml that is one variable for all 
        // languages.
        App_XMLRead_SampleTestFFN();

        // Update application title.
        _tcscpy(g_szBuffer, 
                        App_GetStringFromStringTable(IDS_APP_TITLE, g_nLangID));
        SetWindowText(g_hWnd, g_szBuffer); 

        // Update menu language to French (not valid on Windows 95).
        wLanguageID = MAKELANGID(LANG_FRENCH, SUBLANG_FRENCH);
        lcid = MAKELCID(wLanguageID, SORT_DEFAULT);
        SetThreadLocale(lcid); // TODO: LL redundant if IDC_XXX_EN/FR/RU uesd
        hMenu = LoadMenu(g_hInst, MAKEINTRESOURCE(IDC_TYPINGWIN32_FR));
        SetMenu(g_hWnd, hMenu);

        // Append dynamic language menu.
        App_AddLanguageMenu();

        // Check menu item.
        hSubMenu = GetSubMenu(hMenu, 1 /* View */);
        hSubMenu2 = GetSubMenu(hSubMenu, 2 /* Language */);
        ModifyMenu(hSubMenu, ID_LANGUAGE_FRENCH, 
            MF_POPUP | MF_BYCOMMAND | MF_CHECKED, (UINT_PTR)hSubMenu2, 
            App_GetStringFromStringTable(IDS_APP_MENUITEM_LANG_FR, 
                                                                g_nLangID));

        if (g_nMode == 4) OnMenuMode4();

        if (g_nMode == 3)
        {
            Mode3_UpdateBarsStateMenu();
            Mode3_UpdateLanguageStrings();
        }

        if (g_nMode == 1 || g_nMode == 2 || g_nMode == 3)
        {
            PopulateWithTrainingWords(g_nMode, 2);
        }

        // This function localizes strings for all modes.
        App_LocalizeStrings(2);
        App_UpdateToolbarButtonsStrings(2);  // g_nLangID can be used

        // Update active keaboard to French (install one if absent).
        hKL = LoadKeyboardLayout(szKLID_FR, 0);
        if (hKL != NULL) ActivateKeyboardLayout(hKL, 0);

        break;
    case 3:
        g_nLangID = 3;

        // Update g_sSampleFFN from Settings.xml that is one variable for all 
        // languages.
        App_XMLRead_SampleTestFFN();

        // Update application title.
        _tcscpy(g_szBuffer, 
                        App_GetStringFromStringTable(IDS_APP_TITLE, g_nLangID));
        SetWindowText(g_hWnd, g_szBuffer); 

        // Update menu language to Russian (not valid on Windows 95).
        wLanguageID = MAKELANGID(LANG_RUSSIAN, SUBLANG_DEFAULT);
        lcid = MAKELCID(wLanguageID, SORT_DEFAULT); 
        SetThreadLocale(lcid); // TODO: LL redundant if IDC_XXX_EN/FR/RU uesd
        hMenu = LoadMenu(g_hInst, MAKEINTRESOURCE(IDC_TYPINGWIN32_RU));
        SetMenu(g_hWnd, hMenu);

        // Append dynamic language menu.
        App_AddLanguageMenu();

        // Check menu item.
        hSubMenu = GetSubMenu(hMenu, 1 /* View */);
        hSubMenu2 = GetSubMenu(hSubMenu, 2 /* Language */);
        ModifyMenu(hSubMenu, ID_LANGUAGE_RUSSIAN, 
            MF_POPUP | MF_BYCOMMAND | MF_CHECKED, (UINT_PTR)hSubMenu2, 
            App_GetStringFromStringTable(IDS_APP_MENUITEM_LANG_RU, 
                                                                g_nLangID));

        if (g_nMode == 4) OnMenuMode4();
        
        if (g_nMode == 3)
        {
            Mode3_UpdateBarsStateMenu();
            Mode3_UpdateLanguageStrings();
        }
    
        if (g_nMode == 1 || g_nMode == 2 || g_nMode == 3)
        {
            PopulateWithTrainingWords(g_nMode, 3);
        }

        // This function localizes strings for all modes.
        App_LocalizeStrings(3);
        App_UpdateToolbarButtonsStrings(3);  // g_nLangID can be used

        // Update active keaboard to Russian (install one if absent).
        hKL = LoadKeyboardLayout(szKLID_RU, 0);
        if (hKL != NULL) ActivateKeyboardLayout(hKL, 0);

        break;
    }
    
    // At the end, always clear test area and update the state of toolbar icon
    // and menu item by merely using existing code of the the toolbar's handler. 
    SendMessage(g_hWnd, WM_COMMAND, 
                MAKEWPARAM(ID_CLEAR_TEST_AREA /* low word */, 
                                                         0 /* high word */), 0);
}

void About_SetFont(HWND hWnd)
{
    HFONT hfnt;
    LOGFONT lf;

    // Specify the font to use (stored in a LOGFONT structure).
    lf.lfHeight = 16;
    lf.lfWidth = 0;
    lf.lfEscapement = 0;
    lf.lfOrientation = 0;
    lf.lfWeight = FW_DONTCARE;
    lf.lfItalic = FALSE;
    lf.lfUnderline = TRUE;
    lf.lfStrikeOut = FALSE;
    lf.lfCharSet = DEFAULT_CHARSET;
    lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
    lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
    lf.lfQuality = DEFAULT_QUALITY;
    lf.lfPitchAndFamily = FF_DONTCARE;
    _tcsncpy(lf.lfFaceName, _T("Arial") /*_T("MS Shell Dlg")*/, 32);

    hfnt = CreateFontIndirect(&lf);
    SendMessage(GetDlgItem(hWnd, IDC_STATIC_LINK),  WM_SETFONT, (WPARAM)hfnt, TRUE);
}

LRESULT APIENTRY About_StaticSubclassedFunc(HWND hWnd, UINT Message, WPARAM wParam, 
                LONG lParam)
{
    SetCursor(LoadCursor(NULL, IDC_HAND));

    return CallWindowProc((WNDPROC)g_wpOldAboutStaticWndProc, hWnd, Message, 
                                                                wParam, lParam);
}

// Passting parameters by reference is used to have 3 return values in one pass.
void App_ParseToDecColorValues(char* pszColorValue /* in */, 
            long& iRed /* out */, long& iGreen /* out */, long& iBlue /* out */)
{
    string sColorValue;
    string sHexRed, sHexGreen, sHexBlue;

    sColorValue = pszColorValue;

    sHexRed =  sColorValue.substr(1 /* zero-based start */, 
                                                       2 /* number of chars */);
    sHexGreen =  sColorValue.substr(3, 2);
    sHexBlue =  sColorValue.substr(5, 2);

    iRed = strtol(sHexRed.c_str(), '\0', 16 /* hex base */);
    iGreen = strtol(sHexGreen.c_str(), '\0', 16);
    iBlue = strtol(sHexBlue.c_str(), '\0', 16);
}

void App_AddLanguageMenu()
{
    HANDLE hFile;
    HMENU hMenu, hSubMenu; 
    HMENU hSubMenuLanguages;

    hMenu = GetMenu(g_hWnd);
    hSubMenu = GetSubMenu(hMenu, 1 /* View */);

    // Create language popup menu hanging unattached yet till be attached later 
    // with ModifyMenu.
    hSubMenuLanguages = CreatePopupMenu();
    
    // Check if English language files are installed, add menu item if yes.

    // Check if English directory (EN) exists in RT root. Note that CreateFile 
    // fails (flag OPEN_EXISTING) if the file does not exist, and CreateFile 
    // requires the FILE_FLAG_BACKUP_SEMANTICS flag when Opening a directory.
    hFile = CreateFile(_T("EN"), GENERIC_READ, 0 /* not shared */, 
        NULL /* no security */, OPEN_EXISTING, 
        FILE_ATTRIBUTE_NORMAL | FILE_FLAG_BACKUP_SEMANTICS,  
                                                   NULL /* no template file */);
    if (hFile != INVALID_HANDLE_VALUE)
    {
        AppendMenu(hSubMenuLanguages, MF_STRING, ID_LANGUAGE_ENGLISH,
             App_GetStringFromStringTable(IDS_APP_MENUITEM_LANG_EN, g_nLangID));

        CloseHandle(hFile);
    }

    // Check if French language files are installed, add menu item if yes.
    hFile = CreateFile(_T("FR"), GENERIC_READ, 0 /* not shared */,            
        NULL /* no security */, OPEN_EXISTING, 
        FILE_ATTRIBUTE_NORMAL | FILE_FLAG_BACKUP_SEMANTICS,  
                                                   NULL /* no template file */);
    if (hFile != INVALID_HANDLE_VALUE)
    {
        AppendMenu(hSubMenuLanguages, MF_STRING, ID_LANGUAGE_FRENCH, 
             App_GetStringFromStringTable(IDS_APP_MENUITEM_LANG_FR, g_nLangID));

        CloseHandle(hFile);
    }

    // Check if Russian language files are installed, add menu item if yes.
    hFile = CreateFile(_T("RU"), GENERIC_READ, 0 /* not shared */,            
        NULL /* no security */, OPEN_EXISTING, 
        FILE_ATTRIBUTE_NORMAL | FILE_FLAG_BACKUP_SEMANTICS,  
                                                   NULL /* no template file */);
    if (hFile != INVALID_HANDLE_VALUE)
    {
        AppendMenu(hSubMenuLanguages, MF_STRING, ID_LANGUAGE_RUSSIAN,
             App_GetStringFromStringTable(IDS_APP_MENUITEM_LANG_RU, g_nLangID));

        CloseHandle(hFile);
    }

    // LL fifth parameter can be used to modify already existing menu item name 
    // (already set in RC file). This looks redundant, don't have time though to 
    // figure out the meaning of this redundancy.
    ModifyMenu(hSubMenu, IDM_VIEW_LANGUAGE, MF_BYCOMMAND | MF_POPUP, 
                                       (UINT)hSubMenuLanguages, 
                App_GetStringFromStringTable(IDS_APP_LANGUAGE_ROOT, g_nLangID));						   
}

// PURPOSE: Updates toolbar strings.
// NOTES:  g_nLangID can be used instead of nLangID (without parameter), local 
//         buffer can be ued instead of g_szBuffer.

void App_UpdateToolbarButtonsStrings(short nLangID)
{
    TBBUTTONINFO tbi;

    // These are the only fields of the TBBUTTONINFO structure that need to be 
    // changed to modify text.   
    tbi.dwMask = TBIF_TEXT;
    tbi.cbSize = sizeof (TBBUTTONINFO);

    // Update string for the open file button.
    _tcscpy(g_szBuffer, App_GetStringFromStringTable(IDS_FILE_OPEN, nLangID));
    tbi.pszText = g_szBuffer;
    tbi.cchText = sizeof (g_szBuffer); 
    SendMessage(g_hwndTB, TB_SETBUTTONINFO, ID_FILE_OPEN, (LPARAM)&tbi);
                                                                  
    // Update string for the start or stop test button.
    _tcscpy(g_szBuffer, App_GetStringFromStringTable(IDS_START_OR_STOP_TEST, 
                                                                      nLangID));
    tbi.pszText = g_szBuffer;
    tbi.cchText = sizeof (g_szBuffer); 
    SendMessage(g_hwndTB, TB_SETBUTTONINFO, 
                                           ID_START_OR_STOP_TEST, (LPARAM)&tbi);

    // Update string for the error alert type button.
    _tcscpy(g_szBuffer, App_GetStringFromStringTable(
                                         IDS_TYPING_ERROR_ALERT_TYPE, nLangID));
    tbi.pszText = g_szBuffer;
    tbi.cchText = sizeof (g_szBuffer); 
    SendMessage(g_hwndTB, TB_SETBUTTONINFO, 
                                      ID_TYPING_ERROR_ALERT_TYPE, (LPARAM)&tbi);

    // Update string for the clear test area button.
    _tcscpy(g_szBuffer, App_GetStringFromStringTable(IDS_CLEAR_TEST_AREA, 
                                                                      nLangID));
    tbi.pszText = g_szBuffer;
    tbi.cchText = sizeof (g_szBuffer); 
    SendMessage(g_hwndTB, TB_SETBUTTONINFO, ID_CLEAR_TEST_AREA, (LPARAM)&tbi);

    // Update string for the detailed statistics button.
    _tcscpy(g_szBuffer, App_GetStringFromStringTable(IDS_DETAILED_STATS, 
                                                                      nLangID));
    tbi.pszText = g_szBuffer;
    tbi.cchText = sizeof (g_szBuffer); 
    SendMessage(g_hwndTB, TB_SETBUTTONINFO, ID_DETAILED_STATS, (LPARAM)&tbi);
}

// Based on AddLogFileRecord (from 4S Lock v1.09)
void Mode4_GetDateTimeStamp(char* pszDate, LPWSTR pszDayOfWeek, char* pszTime)
{
    SYSTEMTIME st;
    GetLocalTime(&st);

    char szMonth[5];
    char szDay[5];
    char szYear[3];
    char szTemp[10];

    char szHour[5];
    char szMinute[5];

    switch(st.wDayOfWeek)
    {
        case 0: // Sunday
            lstrcpy(pszDayOfWeek, 
                App_GetStringFromStringTable(IDS_TEST_MODE_HTML_REPORT_SUN, 
                                                                    g_nLangID));
            break;
        case 1:
            lstrcpy(pszDayOfWeek, 
                App_GetStringFromStringTable(IDS_TEST_MODE_HTML_REPORT_MON, 
                                                                    g_nLangID));
            break;
        case 2:
            lstrcpy(pszDayOfWeek, 
                App_GetStringFromStringTable(IDS_TEST_MODE_HTML_REPORT_TUE, 
                                                                    g_nLangID));
            break;
        case 3:
            lstrcpy(pszDayOfWeek, 
                App_GetStringFromStringTable(IDS_TEST_MODE_HTML_REPORT_WED, 
                                                                    g_nLangID));
            break;
        case 4:
            lstrcpy(pszDayOfWeek, 
                App_GetStringFromStringTable(IDS_TEST_MODE_HTML_REPORT_THU, 
                                                                    g_nLangID));
            break;
        case 5:
            lstrcpy(pszDayOfWeek, 
                App_GetStringFromStringTable(IDS_TEST_MODE_HTML_REPORT_FRI, 
                                                                    g_nLangID));
            break;
        case 6:
            lstrcpy(pszDayOfWeek, 
                App_GetStringFromStringTable(IDS_TEST_MODE_HTML_REPORT_SAT, 
                                                                    g_nLangID));
            break;
    }

    // Convert "2:6" time into "02:06" time
    sprintf(szHour, st.wHour < 10 ? "0%d" : "%d",  st.wHour);
    sprintf(szMinute, st.wMinute < 10 ? "0%d" : "%d",  st.wMinute);

    // Convert "1/29/2005" date into "01/29/05" date
    sprintf(szMonth, st.wMonth < 10 ? "0%d" : "%d",  st.wMonth);
    sprintf(szDay, st.wDay < 10 ? "0%d" : "%d",  st.wDay);

    // Convert "2005" into "05" (there is no C-string truncation routine, but
    // this trick works).
    sprintf(szTemp, "%d", st.wYear);
    sprintf(szYear, &szTemp[2]);
    
    // Format mm/dd/yy or dd/mm/yy according to language.
    if (g_nLangID == 1 /* English */)
        sprintf(pszDate, "%s/%s/%s", szMonth, szDay, szYear); 
    else if (g_nLangID == 2 /* French */ || g_nLangID == 3 /* Russian */)
        sprintf(pszDate, "%s/%s/%s", szDay, szMonth, szYear); 

    sprintf(pszTime, "%s:%s", szHour, szMinute);
}

LRESULT CALLBACK Mode4_DetailedStatsColors_WndProc(HWND hDlg, UINT message, 
                                                   WPARAM wParam, LPARAM lParam)
{
    HBRUSH hBackgroundBrush;

    switch (message)
    {
    case WM_INITDIALOG:
        // Set dialog title.
        SendMessage(hDlg, WM_SETTEXT, 0 /* not used */, 
            (LPARAM)App_GetStringFromStringTable(IDS_DS_COLORS_WINDOW_NAME, 
                                                                    g_nLangID));
        // Set names for static controls.
        SendMessage(GetDlgItem(hDlg, IDC_DS_COLORS_STATIC_CUSTOM), WM_SETTEXT, 
            0 /* not used */, 
            (LPARAM)App_GetStringFromStringTable(IDS_DS_COLORS_CUSTOM_WORD, 
                                                                    g_nLangID));
        SendMessage(GetDlgItem(hDlg, IDC_DS_COLORS_STATIC_LH), WM_SETTEXT, 
            0 /* not used */, 
            (LPARAM)App_GetStringFromStringTable(IDS_DS_COLORS_LH_WORD, 
                                                                    g_nLangID));

        SendMessage(GetDlgItem(hDlg, IDC_DS_COLORS_STATIC_L2R), WM_SETTEXT, 
            0 /* not used */, 
            (LPARAM)App_GetStringFromStringTable(IDS_DS_COLORS_L2R_WORD, 
                                                                    g_nLangID));

        SendMessage(GetDlgItem(hDlg, IDC_DS_COLORS_STATIC_R2L), WM_SETTEXT, 
            0 /* not used */, 
            (LPARAM)App_GetStringFromStringTable(IDS_DS_COLORS_R2L_WORD, 
                                                                    g_nLangID));

        SendMessage(GetDlgItem(hDlg, IDC_DS_COLORS_STATIC_RH), WM_SETTEXT, 
            0 /* not used */, 
            (LPARAM)App_GetStringFromStringTable(IDS_DS_COLORS_RH_WORD, 
                                                                    g_nLangID));

        //// Setup font for URL.
        ////About_SetFont(hDlg);

        return TRUE;

    case WM_CTLCOLORSTATIC:

        // MSDN'03: The return value is a handle to a brush that the system uses 
        // to paint the background of the static control.
        if (GetDlgItem (hDlg, IDC_DS_COLORS_STATIC_CUSTOM2) == 
                                    (HWND)lParam /* handle to static control */)
        {
            hBackgroundBrush = CreateSolidBrush(g_clrMode4_DS_CustomColor);
            return (LRESULT)hBackgroundBrush;
        }

        if (GetDlgItem (hDlg, IDC_DS_COLORS_STATIC_LH2) == 
                                    (HWND)lParam /* handle to static control */)
        {
            hBackgroundBrush = CreateSolidBrush(g_clrMode4_DS_LHColor);
            return (LRESULT)hBackgroundBrush;
        }

        if (GetDlgItem (hDlg, IDC_DS_COLORS_STATIC_L2R2) == 
                                    (HWND)lParam /* handle to static control */)
        {
            hBackgroundBrush = CreateSolidBrush(g_clrMode4_DS_L2RColor);
            return (LRESULT)hBackgroundBrush;
        }

        if (GetDlgItem (hDlg, IDC_DS_COLORS_STATIC_R2L2) == 
                                    (HWND)lParam /* handle to static control */)
        {
            hBackgroundBrush = CreateSolidBrush(g_clrMode4_DS_R2LColor);
            return (LRESULT)hBackgroundBrush;
        }

        if (GetDlgItem (hDlg, IDC_DS_COLORS_STATIC_RH2) == 
                                    (HWND)lParam /* handle to static control */)
        {
            hBackgroundBrush = CreateSolidBrush(g_clrMode4_DS_RHColor);
            return (LRESULT)hBackgroundBrush;
        }

        break;

    case WM_COMMAND:

        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) 
        {
            EndDialog(hDlg, LOWORD(wParam));
            return TRUE;
        }
        break;
    case WM_DESTROY:
        // When destroying a detailed statistics colors dialog, destroy brush
        // used for painting static background. The program runs OK without it,
        // but returning brush handle with as 
        // "return (LRESULT)CreateSolidBrush(...)" does not sound nice to me.
        // It is possibly better to delete used brush when painting finished, 
        // and delete each brush created explicitly, don't have time to figure 
        // out though.
        // TODO: run-time error in debug: the variable hBackgroundBrush had been
        //used without been defined.
        //DeleteObject(hBackgroundBrush); 
        break;
    }
    
    return FALSE;
}

// do not change in-pointer, and the second argument
BOOL Mode4_VerifyFileEncoding(LPCTSTR pszFileText) 
{
    // If sample file is not a UNICODE file (neither BE nor LE), warn the 
    // user and prevent using the program.
    if (*pszFileText != 0xFEFF /* Unicode Big Endian */ && 
                             *pszFileText != 0xFFFE /* Unicode Little Endian */)
    {
        MessageBox(0, L"The UNICODE files only are compatible with RTP.", 
                                           L"The text is not UNICODE text!", 0);  

        return FALSE;
    }

    return TRUE;
}

void Mode4_ClearBaseStats()
{
    SendMessage(g_hwndMode4_Static_TimeSpent, WM_SETTEXT, 
                                                  0 /* not used */, (LPARAM)"");
    SendMessage(g_hwndMode4_Static_TimeSpent_Value, WM_SETTEXT, 
                                                  0 /* not used */, (LPARAM)"");
    SendMessage(g_hwndMode4_Static_CPM, WM_SETTEXT, 
                                                  0 /* not used */, (LPARAM)"");
    SendMessage(g_hwndMode4_Static_CPM_Value, WM_SETTEXT, 
                                                  0 /* not used */, (LPARAM)"");
    SendMessage(g_hwndMode4_Static_CharsTyped, WM_SETTEXT, 
                                                  0 /* not used */, (LPARAM)"");
    SendMessage(g_hwndMode4_Static_CharsTyped_Value, WM_SETTEXT, 
                                                  0 /* not used */, (LPARAM)"");
    SendMessage(g_hwndMode4_Static_WordsTyped, WM_SETTEXT, 
                                                  0 /* not used */, (LPARAM)"");
    SendMessage(g_hwndMode4_Static_WordsTyped_Value, WM_SETTEXT, 
                                                  0 /* not used */, (LPARAM)"");
    SendMessage(g_hwndMode4_Static_WPM, WM_SETTEXT, 
                                                  0 /* not used */, (LPARAM)"");
    SendMessage(g_hwndMode4_Static_WPM_Value, WM_SETTEXT, 
                                                  0 /* not used */, (LPARAM)"");
    SendMessage(g_hwndMode4_Static_TotalErrors, WM_SETTEXT, 
                                                  0 /* not used */, (LPARAM)"");
    SendMessage(g_hwndMode4_Static_TotalErrors_Value, WM_SETTEXT, 
                                                  0 /* not used */, (LPARAM)"");
}

void App_XMLReadSettings()
{
     char szCurDirectory[MAX_PATH]; // argument of open is ANSI string
    char szFFN[MAX_PATH];     // argument of open is ANSI string   
    long iRed, iGreen, iBlue;
    char szHTMLColorValue[10]; // e.g. #A52A2A
    //char szSampleFFN[MAX_PATH];
    //TCHAR szBuffer[MAX_PATH];

    #ifndef DEVELOPMENT_STRUCTURE
        // Construct Settings.xml path using current directory.
        GetCurrentDirectoryA(MAX_PATH, szCurDirectory);
        strcpy(szFFN, szCurDirectory);
        strcat(szFFN, "\\Settings.xml");
    #else
        // Construct Settings.xml path using development directory.
        strcpy(szFFN, APP_DIRECTORY_ANSI);
        strcat(szFFN, "\\Settings.xml");
    #endif

    //--------------------------------------------------------------------------
    // Read Debug value before reading all other values not to create trace file 
    // unconditionally. For the same sake it should be no trace in 
    // App_XMLReadValue for Debug value. Except of debug value all other values 
    // are read in XML order.
    g_bDebug = (BOOL)atoi(App_XMLReadValue("Debug", szFFN).c_str());
    if (g_bDebug) App_Debug_FunctionCallTrace("App_XMLReadSettings Entered");
    //--------------------------------------------------------------------------
    
    // Read settings (except Debug value, which is already read).
    g_bSaveLastUsed = (BOOL)atoi(App_XMLReadValue("SaveLastUsed", 
                                                                szFFN).c_str());

    // Initial globals are used to check dirty state of persisted values to 
    // avoid reduntant Settings.xml re-write when some value was not changed 
    // during program use.
    g_nMode = atoi(App_XMLReadValue("Mode", szFFN).c_str()); 
    g_nInitialMode = g_nMode;
    g_nLangID = (BOOL)atoi(App_XMLReadValue("Language", szFFN).c_str()); 
    g_nInitialLangID = g_nLangID;

    g_bAlertEnabled = (BOOL)atoi(App_XMLReadValue("AlertEnabled", 
                                                                szFFN).c_str());
    g_bShowURLLogo = (BOOL)atoi(App_XMLReadValue("ShowURLLogo", szFFN).c_str());
    g_bShowStatusBarHints = (BOOL)atoi(App_XMLReadValue("ShowSBHints", 
                                                                szFFN).c_str());
    g_nDropWord = (BOOL)atoi(App_XMLReadValue("DropWord", szFFN).c_str());
    g_bShowHint1Dialog = (BOOL)atoi(App_XMLReadValue("H1D_Show", 
                                                                szFFN).c_str());
    g_bInitShowHint1Dialog = g_bShowHint1Dialog;
    g_bShowHint2Dialog = (BOOL)atoi(App_XMLReadValue("H2D_Show", 
                                                                szFFN).c_str());
    g_bInitShowHint2Dialog = g_bShowHint2Dialog;
                                                                
    // TODO: [probably] make an XML switch to use system or HTML-type color 
    // If background color is system color
    //g_nBackgroundColor = (BOOL)atoi(App_XMLReadValue("BackgroundColor"));
    
    // Obtain from XML settings background color, which is encoded as HTML-type 
    // color value.  Background color must be retrieved before application  
    // window class creation i.e. before MyRegisterClass call. I use passing by 
    // reference to have 3 return values. 
    strcpy(szHTMLColorValue, App_XMLReadValue("BackgroundColor", 
                                                                szFFN).c_str());
    App_ParseToDecColorValues(szHTMLColorValue /* in */, iRed /* out */, 
                                             iGreen /* out */, iBlue /* out */);
    g_clrBackgroundColor = RGB(iRed, iGreen, iBlue); 

    strcpy(szHTMLColorValue, App_XMLReadValue("BS_BackgroundColor", 
                                                                szFFN).c_str());
    App_ParseToDecColorValues(szHTMLColorValue /* in */, iRed /* out */, 
                                             iGreen /* out */, iBlue /* out */);
    g_clrMode4_BS_BackgroundColor = RGB(iRed, iGreen, iBlue);	

    strcpy(szHTMLColorValue, App_XMLReadValue("DS_BackgroundColor", 
                                                                szFFN).c_str());
    App_ParseToDecColorValues(szHTMLColorValue /* in */, iRed /* out */, 
                                             iGreen /* out */, iBlue /* out */);
    g_clrMode4_DS_BackgroundColor = RGB(iRed, iGreen, iBlue);	
    
    strcpy(szHTMLColorValue, App_XMLReadValue("DS_InterlacedColor", 
                                                                szFFN).c_str());
    App_ParseToDecColorValues(szHTMLColorValue /* in */, iRed /* out */, 
                                             iGreen /* out */, iBlue /* out */);
    g_clrMode4_DS_InterlacedColor = RGB(iRed, iGreen, iBlue);

    strcpy(szHTMLColorValue, App_XMLReadValue("DS_LHColor", szFFN).c_str());
    App_ParseToDecColorValues(szHTMLColorValue /* in */, iRed /* out */, 
                                             iGreen /* out */, iBlue /* out */);
    g_clrMode4_DS_LHColor = RGB(iRed, iGreen, iBlue); 

    strcpy(szHTMLColorValue, App_XMLReadValue("DS_RHColor", szFFN).c_str());
    App_ParseToDecColorValues(szHTMLColorValue /* in */, iRed /* out */, 
                                             iGreen /* out */, iBlue /* out */);
    g_clrMode4_DS_RHColor = RGB(iRed, iGreen, iBlue);

    strcpy(szHTMLColorValue, App_XMLReadValue("DS_L2RColor", szFFN).c_str());
    App_ParseToDecColorValues(szHTMLColorValue /* in */, iRed /* out */, 
                                             iGreen /* out */, iBlue /* out */);
    g_clrMode4_DS_L2RColor = RGB(iRed, iGreen, iBlue);

    strcpy(szHTMLColorValue, App_XMLReadValue("DS_R2LColor", szFFN).c_str());
    App_ParseToDecColorValues(szHTMLColorValue /* in */, iRed /* out */, 
                                             iGreen /* out */, iBlue /* out */);
    g_clrMode4_DS_R2LColor = RGB(iRed, iGreen, iBlue);

    // Read g_sSampleFFN from Settings.xml that is one variable for all 
    // languages. The separate App_XMLRead_SampleTestFFN() function is a 
    // tradeoff that allows to only one variable g_sSampleFFN instead of
    // one variable for each language.
    App_XMLRead_SampleTestFFN();
    
    g_bNaturalWordCount = (BOOL)atoi(App_XMLReadValue("Natural_Word_Count", 
                                                                szFFN).c_str());

    g_bLogResults = (BOOL)atoi(App_XMLReadValue("Log_Results", szFFN).c_str());

    if (g_bDebug) App_Debug_FunctionCallTrace("App_XMLReadSettings Exited");
}

BOOL Mode4_IsEndOfTest(LPTSTR pszLastChar)
{
    TCHAR szText[MAX_SAMPLE_TEXT];

    SetFocus(g_hwndEdit);
    SendMessage(g_hwndEdit, WM_GETTEXT, MAX_SAMPLE_TEXT, (LPARAM)szText);

    int iLength = lstrlen(szText);
    szText[iLength] = *pszLastChar;
    szText[iLength + 1] = '\0';

    if (_tcscmp(g_szSampleText, szText) == 0) 
    {
        return TRUE;
    }

    return FALSE;
}

BOOL Mode4_IsTestCompleted()
{
    TCHAR szText[MAX_SAMPLE_TEXT];

    SetFocus(g_hwndEdit);
    SendMessage(g_hwndEdit, WM_GETTEXT, MAX_SAMPLE_TEXT, (LPARAM)szText);
    if (_tcscmp(g_szSampleText, szText) == 0) 
        return TRUE;

    return FALSE;
}

void App_CreateLogo()
{
    //TCHAR szURLLogo[50]; 
    HFONT hfnt;
    LOGFONT lf;

    // Set URL logo font.
    lf.lfHeight = 14; 
    lf.lfWidth = 0;
    lf.lfEscapement = 0;
    lf.lfOrientation = 0;
    lf.lfWeight = 1000; //1..1000.
    lf.lfItalic = FALSE; 
    lf.lfUnderline = TRUE; // underlining
    lf.lfStrikeOut = FALSE;
    lf.lfCharSet = DEFAULT_CHARSET;
    lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
    lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
    lf.lfQuality = DEFAULT_QUALITY;
    lf.lfPitchAndFamily = FF_DONTCARE;

    _tcsncpy(lf.lfFaceName, _T("Verdana"), 32);

    hfnt = CreateFontIndirect(&lf);
    
    if (g_nMode == 1 || g_nMode == 2)
    {
        g_hwndURLLogo = CreateWindow(_T("STATIC"), NULL, 
            WS_CHILD | WS_VISIBLE | SS_NOTIFY, 
            g_nMode12_URLLogoLeft + g_nLogoLeftIndent, 4, 
            g_nURLLogoWidth, 20, GetDlgItem(g_hWnd, ID_TOOLBAR), 
                                     (HMENU)IDC_STATIC_URL_LOGO, g_hInst, NULL);
    }
    else if (g_nMode == 3)
    {
        g_hwndURLLogo = CreateWindow(_T("STATIC"), NULL, 
            WS_CHILD | WS_VISIBLE | SS_NOTIFY, 
            g_nMode3_URLLogoLeft + g_nLogoLeftIndent, 4, 
            g_nURLLogoWidth, 20, GetDlgItem(g_hWnd, ID_TOOLBAR), 
                                     (HMENU)IDC_STATIC_URL_LOGO, g_hInst, NULL);
    }
    else if (g_nMode == 4)
    {
        g_hwndURLLogo =  CreateWindow(_T("STATIC"), NULL, 
            //WS_CHILD | WS_VISIBLE | SS_NOTIFY, g_nMode4_URLLogoLeft, 4, 
            WS_CHILD | WS_VISIBLE | SS_NOTIFY, 
            g_nMode4_URLLogoLeft + g_nLogoLeftIndent, 4,
            g_nURLLogoWidth, 20, GetDlgItem (g_hWnd, ID_TOOLBAR),
                                     (HMENU)IDC_STATIC_URL_LOGO, g_hInst, NULL);
    }

    SendMessage(g_hwndURLLogo, WM_SETFONT, (WPARAM)hfnt, TRUE); 

    //SendMessage(g_hwndURLLogo, WM_SETTEXT, 0 /* not used */, (LPARAM)szURLLogo);
    g_wpOldURLLogoWndProc = (WNDPROC)SetWindowLong(g_hwndURLLogo, 
                                GWL_WNDPROC, (DWORD)App_URLLogo_SubclassedFunc);

    // Make dummy hide/unhide so that URL background is restored (see 
    // App_Toolbar_SubclassedFunc description).
    ShowWindow(GetDlgItem(g_hWnd, ID_TOOLBAR), SW_HIDE); 
    ShowWindow(GetDlgItem(g_hWnd, ID_TOOLBAR), SW_SHOW);
}

LRESULT APIENTRY App_URLLogo_SubclassedFunc(HWND hWnd, UINT Message, 
                                                     WPARAM wParam, LONG lParam)
{
    ::SetCursor(LoadCursor(NULL, IDC_HAND));

    return CallWindowProc((WNDPROC)g_wpOldURLLogoWndProc, hWnd, Message, 
                                                                wParam, lParam);
}

// I subclass toolbar to catch WM_CTLCOLORSTATIC sent by URL static control 
// ( toolbar's child). For some (unknown to me) reason this WM_CTLCOLORSTATIC is 
// not sent to application procedure (weird since static's parent is toolbar, and
// toolbar's parent is application window). It is no harm, however to make, 
// intercepting with suclassing. All other messages are handled in a regular way.
LRESULT APIENTRY App_Toolbar_SubclassedFunc(HWND hWnd, UINT Message, 
                                                     WPARAM wParam, LONG lParam)
{
    switch(Message)
    {
    case WM_CTLCOLORSTATIC:

        if (g_hwndURLLogo = (HWND)lParam)
        {
             SetTextColor((HDC)wParam /* handle to display context */, 
                                                            g_clrLogoTextColor);
                                                              //RGB(255, 0, 0)); 

            SetBkMode((HDC)wParam /* handle to display context */, 
                                                                   TRANSPARENT); 
            // There is a downside here since background is not painted OK 
            // (I've seen it only when placing UTL link on toolbar; it works OK
            // for dialogs). In caller I make dummy hide/unhide so that 
            // background is restored.
            return (UINT)GetStockObject(HOLLOW_BRUSH);
        }
        break;
    default:
        break;
    }

    return CallWindowProc((WNDPROC)g_wpOldToolbarWndProc, hWnd, Message, 
                                                                wParam, lParam);
}

// Called from App_WndProc and (when edit control has focus) from 
// Edit_SubclassedFunc.
BOOL App_MenuHandler_ShowOnlineHelp(LPCTSTR pszHelpFile)
{
    TCHAR szHelpRootURL[MAX_PATH];
    TCHAR szHelpURL[MAX_PATH];
    TCHAR szMsg[100];

    // Make a warning message when internet connection is not available. 
    lstrcpy(szMsg, L"There was a problem while connecting to the internet! ");
    lstrcat(szMsg, L"Please check connection settings and try again ...");

    // Very slow (~40 sec), but works.
    if (!App_CheckInternetConnection(
                                 L"http://www.google.com" /* any major site */))  	
    {
        MessageBox(0, szMsg, L"Rational Typist", MB_ICONINFORMATION);
        return FALSE;
    }

     // Build language-dependant help URL.
    lstrcpy(szHelpRootURL, L"http://www.carexplorer.org/Downloads/RT/Help");
    //lstrcpy(szHelpFile, _T("RT_Help.htm"));
   
    switch (g_nLangID)
    {
    case 1:
        wsprintf(szHelpURL, L"%s%s%s", szHelpRootURL, L"/EN/", pszHelpFile);
        break;
    case 2:
        wsprintf(szHelpURL, L"%s%s%s", szHelpRootURL, L"/FR/", pszHelpFile);
        break;
    case 3:
        wsprintf(szHelpURL, L"%s%s%s", szHelpRootURL, L"/RU/", pszHelpFile);
        break;
    };

    // Check the availability of base Help URL.
    if (App_IsBaseHelpURLLinkAvailable(szHelpURL))
    {
        // The call uses shell32.lib, shellapi.h.
        ShellExecute(NULL, _T("open"), szHelpURL, 0, 0, SW_SHOWNORMAL);
    }
    else // use always alternative Help URL fs the MSB site is down
    {
        lstrcpy(szHelpRootURL, L"http://netston.tripod.com/Software/RT/Help");

        switch (g_nLangID)
        {
        case 1:
            wsprintf(szHelpURL, L"%s%s%s", szHelpRootURL, L"/EN/", pszHelpFile);
            break;
        case 2:
            wsprintf(szHelpURL, L"%s%s%s", szHelpRootURL, L"/FR/", pszHelpFile);
            break;
        case 3:
            wsprintf(szHelpURL, L"%s%s%s", szHelpRootURL, L"/RU/", pszHelpFile);
            break;
        };
    
        // The call uses shell32.lib, shellapi.h.
        ShellExecute(NULL, _T("open"), szHelpURL, 0, 0, SW_SHOWNORMAL);
    }

    return TRUE;
}

BOOL App_IsBaseHelpURLLinkAvailable(LPCTSTR pszURL)
{ 
    if (g_bDebug) App_Debug_FunctionCallTrace(
                                      "App_IsBaseHelpURLLinkAvailable Entered");	
    HINTERNET hInternetSession; 
    HINTERNET hFile = NULL; 
    char szBuffer[100]; 
    DWORD dwSize;
    DWORD dwCode;
    BOOL bRes;
    BOOL bIsBaseHelp = FALSE;

    hInternetSession  = 
        InternetOpen(L"Rational Typist" /* calling agent */, 
        INTERNET_OPEN_TYPE_DIRECT, NULL /* proxy */, NULL /*proxy bypass */, 0);

    if (hInternetSession)
    {
        hFile = InternetOpenUrl(hInternetSession, pszURL, 
            NULL, 0, INTERNET_FLAG_DONT_CACHE | INTERNET_FLAG_RELOAD,
                                                               0 /* context */); 
        if (hFile)
        {
            dwSize = sizeof (DWORD);  
            bRes = HttpQueryInfo(hFile, HTTP_QUERY_STATUS_CODE |
                                HTTP_QUERY_FLAG_NUMBER, &dwCode, &dwSize, NULL);

            if (bRes)
            {
                sprintf(szBuffer, "Status Code: %d", dwCode);
                if (g_bDebug) App_Debug_FunctionCallTrace(szBuffer);

                if (dwCode == 200 /* HTTP_STATUS_OK */)
                {
                    bIsBaseHelp = TRUE; 
                }
                else
                {
                    bIsBaseHelp = FALSE;
                }
            }
            else
            {
                sprintf(szBuffer, "Last Error: %d", GetLastError());
                if (g_bDebug) App_Debug_FunctionCallTrace(szBuffer);

                bIsBaseHelp = FALSE;
            }
        } 
        else // hFile
        {
            if (g_bDebug) App_Debug_FunctionCallTrace(
                                                  "InternetOpenUrl failure...");
            bIsBaseHelp = FALSE;
        }
    } 
    else // hInternetSession
    {
        if (hFile) CloseHandle(hFile);
        if (hInternetSession) CloseHandle(hInternetSession);

        return FALSE;
    }

    if (hFile) CloseHandle(hFile);
    if (hInternetSession) CloseHandle(hInternetSession);

    return bIsBaseHelp;
}

BOOL App_CheckInternetConnection(LPCTSTR pszURL)
{
    DWORD flag = 0;
    
    // Check if wininet.dll is available.
    if (!LoadLibrary(L"wininet.dll"))
    {
        MessageBox(0, L"IE is not installed likely: wininet.dll not found!", 
                                        L"Rational Typist", MB_ICONINFORMATION);
        return FALSE;
    }
    
    // Check for internet connection.
    if (InternetGetConnectedState(&flag, 0))
    {	
        if (InternetCheckConnection(pszURL, FLAG_ICC_FORCE_CONNECTION, 0))
            return TRUE;
        else
            return FALSE;
    }
    else
        return FALSE;
}

// Notes: uses slightly poliched logic from MSB_SCfg.scr's Updates.cpp.
static UINT __stdcall App_ThreadProc_ShowLogo(LPVOID param)
{
    char szCurDirectory[MAX_PATH];
    char szFullFileName[MAX_PATH];  // FFN for Logo_Updates.xml
    HINTERNET hInternetSession; 
    HINTERNET hFile; 
    TCHAR szRemoteFileURL[MAX_PATH];
    char szRemoteFile[500]; // long enough to keep all Logo_Updates.xml entries 
    DWORD dwRead;
    char szMsg[100];
    char szErrorMsg[100];
    LPVOID lpOutBuffer = NULL;
    //DWORD dwSize = 0;
    BOOL bRes;
    short nConnAttemptsCount = 0;
    short nMaxConnAttempts = 10; // some value, even 2 practically OK
    WCHAR szBuffer[100];	
    
    // Make full file name for Logo_Updates.xml. 
    #if !defined(DEVELOPMENT_STRUCTURE)
        // Product release settings. Obtain path to current directory.
        GetCurrentDirectoryA(MAX_PATH, szCurDirectory);
        strcpy(szFullFileName, szCurDirectory);
        strcat(szFullFileName, "\\Logo_Updates.xml");
    #else
        // Construct Settings.xml path using development directory.
        strcpy(szFullFileName, APP_DIRECTORY_ANSI);  
        strcat(szFullFileName, "\\Logo_Updates.xml");
    #endif
    
    // Regardless of internet availability  show logo info from locally cached 
    // (if available) Logo_Updates.xml. This should be done before checking for
    // internet connection since it would have delayed substantially showing 
    // logo info.
    if (App_IsCachedLogoUpdatesFileExists()) 
    {
        App_ParseLogoUpdatesFile(szFullFileName);

        // Show logo info from cached (probably outdated) 
        // Logo_Updates.xml. First, convert ANSI's g_sLogoText. 
        // to WCHAR.
        MultiByteToWideChar(CP_ACP, 
            0 /* must be zero [TODO: clarify] */, 
            g_sLogoText.c_str(), 
            -1 /* string NULL-terminated [TODO: clarify] */, 
                                    szBuffer, sizeof(szBuffer)); 

        SendMessage(g_hwndURLLogo, WM_SETTEXT, 0 /* not used */, 
                                                (LPARAM)szBuffer);
    }
    
    if (!App_CheckInternetConnection(
                                L"http://www.google.com" /* any major site */)) 
    {
        if (g_bDebug) 
            App_Debug_FunctionCallTrace(
                             "App_ThreadProc_ShowLogo: no internet connection");

        // When internet connection is not available logo info from cached 
        // Logo_Updates.xml file is already shown before internet connection 
        // check (show nothing if the file is missing). Do nothing more, return.
        return FALSE;
    }
    else 
    {
        if (g_bDebug) 
            App_Debug_FunctionCallTrace(
                                 "App_ThreadProc_ShowLogo: internet connected");

        // When internet connection is available, update already shown (before 
        // connection check) logo info from cached Logo_Updates.xml (if present) 
        // by downloading MSB site's Logo_Updates.xml and updating logo.


        // To avoid returning erroneous Logo_Updates.xml from server several 
        // attempts to connect are made (the logic is from MSB_SCfg and, 
        // in fact, I did not see this error with RT though expect it without
        // this code, SLite uses a bit different check using 
        // ERROR_HTTP_HEADER_NOT_FOUND). They are limited to nMaxConnAttempts. 
        // Each attempt recreates InternetOpen, InternetOpenUrl, which were 
        // closed by previous unsuccessful attempt. Typically connection error 
        // occurs on reboot or after some period of inactivity. In this case, 
        // the string beginning with "<!DOCTYPE ..." is returned, however the 
        // 2nd connect was always OK. This is why I make additional attempts 
        // programmatically. The issue is known (internet), the workaround 
        // is mine. Each attempt starts for the label:
        retry_session: 

        nConnAttemptsCount++;

        if (nConnAttemptsCount < nMaxConnAttempts)
        {
            hInternetSession  = 
                InternetOpen(L"Rational Typist" /* calling agent */, 
                INTERNET_OPEN_TYPE_DIRECT, NULL /* proxy */, 
                                                     NULL /*proxy bypass */, 0);
            if (hInternetSession)
            {
                // Make the string for the internet location of 
                // Logo_Updates.xml.
                lstrcpy(szRemoteFileURL, L"http://www.carexplorer.org");
                lstrcat(szRemoteFileURL, L"/Downloads/RT/Logo_Updates.xml");
            
                hFile = InternetOpenUrl(hInternetSession, szRemoteFileURL, 
                NULL, 0, INTERNET_FLAG_DONT_CACHE | INTERNET_FLAG_RELOAD, 
                                                               0 /* context */); 
                if (hFile)
                {
                    // Read the target Logo_Updates.xml file. The characters of 
                    // Logo_Updates.xml are supposed to be ANSI characters.
                    bRes = InternetReadFile(hFile, (LPVOID)szRemoteFile, 
                                                500 /* long enough */, &dwRead);
                    szRemoteFile[dwRead] = '\0';

                    // Make sure that this is a valid Logo_Updates.xml file and 
                    // not an error file. The App_IsValidRemoteLogoUpdatesFile 
                    // function checks if the Logo_Updates.xml file has 
                    // "<RationalTypistPro>" string. If the file is not 
                    // "Logo_Updates.xml" make no update for toolbar's logo 
                    // text.
                    if (App_IsValidRemoteLogoUpdatesFile(szRemoteFile))
                    {
                        //MessageBoxA(0, szRemoteFile, "Valid file!!", 0);
                        
                        // Store (cache) Logo_Updates.xml locally overwriting 
                        // existing if present. 
                        App_CacheLogoUpdatesFile(szRemoteFile);

                        // Parse the Logo_Updates.xml file with XML parser and 
                        // show results in [MSB: updates window]. 
                        App_ParseLogoUpdatesFile(szFullFileName);

                        // Update logo text. First, convert ANSI's g_sLogoText 
                        // to WCHAR.
                        MultiByteToWideChar(CP_ACP, 
                            0 /* must be zero [TODO: clarify] */, 
                            g_sLogoText.c_str(), 
                            -1 /* string NULL-terminated [TODO: clarify] */, 
                                                    szBuffer, sizeof(szBuffer)); 
                        SendMessage(g_hwndURLLogo, WM_SETTEXT, 0 /* not used */, 
                                                              (LPARAM)szBuffer);
                        // Ajust positon of logo window according to 
                        // g_nLogoLeftIndent.
                        switch (g_nMode)
                        {
                        case 1:
                            MoveWindow(g_hwndURLLogo, g_nMode12_URLLogoLeft + 
                               g_nLogoLeftIndent, 4, g_nURLLogoWidth, 20, TRUE);
                            break;
                        case 2:
                            MoveWindow(g_hwndURLLogo, g_nMode12_URLLogoLeft + 
                               g_nLogoLeftIndent, 4, g_nURLLogoWidth, 20, TRUE);
                            break;
                        case 3:
                            MoveWindow(g_hwndURLLogo, g_nMode3_URLLogoLeft + 
                               g_nLogoLeftIndent, 4, g_nURLLogoWidth, 20, TRUE);
                            break;
                        case 4:
                            MoveWindow(g_hwndURLLogo, g_nMode4_URLLogoLeft + 
                               g_nLogoLeftIndent, 4, g_nURLLogoWidth, 20, TRUE);
                            break;
                        }
                                                  
                        // Make dummy hide/unhide so that older text is removed.
                        ShowWindow(GetDlgItem(g_hWnd, ID_TOOLBAR), SW_HIDE); 
                        ShowWindow(GetDlgItem(g_hWnd, ID_TOOLBAR), SW_SHOW);
                    }
                    else
                    {
                        // It is not the Logo_Updates.xml since 
                        // "<RationalTypistPro>" string is not found.
                        if (hFile) CloseHandle(hInternetSession);
                        if (hInternetSession) CloseHandle(hInternetSession);

                        // Print out error message in the debug mode.
                        if (g_bDebug) 
                        {
                            strcpy(szMsg, "App_ThreadProc_ShowLogo: ");  
                            strcat(szMsg, "Logo_Updates.xml is not valid!");
                            App_Debug_FunctionCallTrace(szMsg);
                        }

                        goto retry_session;
                    }
                }
                else // hFile
                {
                    // Connection is available, but the MSB site does not exist e.g. 
                    // when registration is expired. DeleteF Logo_Updates.xml in
                    // this case (no harm since will be auto-restored if main site
                    // is restored).
                    DeleteFileA(szFullFileName);

                    if (hInternetSession) InternetCloseHandle(hInternetSession);

                    return FALSE;
                }
            }
            else // hInternetSession
            {
                // Print out last error in the debug mode.
                if (g_bDebug)
                {
                    sprintf(szErrorMsg, 
                        "App_ThreadProc_ShowLogo's Last Error: %d",  
                                                                GetLastError());
                    App_Debug_FunctionCallTrace(szErrorMsg);
                }

                return FALSE;
            }

            if (hFile) InternetCloseHandle(hFile);
            if (hInternetSession) InternetCloseHandle(hInternetSession);
        }
        else // nConnAttemptsCount < nMaxConnAttempts
        {
            // TODO: remove local Logo_Updates.xml if I sut down MSB site 
            // (nevertherless can be here if someone else became the domain 
            // owner).
            //MessageBox(0, 
            //	L"Connection exists but Logo_Updates.xml is not found", 
            //				      L"max number of conn attempts is expired", 0);
        }

    }

    return TRUE;
}

BOOL App_IsCachedLogoUpdatesFileExists()
{
    TCHAR szCurDirectory[MAX_PATH];
    TCHAR szFullFileName[MAX_PATH];  // FFN for Logo_Updates.xml
    HANDLE hUpdatesFile;

    // Make full file name for Logo_Updates.xml. 
    #if !defined(DEVELOPMENT_STRUCTURE)
        // Product release settings. Obtain path to current directory.
        GetCurrentDirectory(MAX_PATH, szCurDirectory);
        lstrcpy(szFullFileName, szCurDirectory);
        lstrcat(szFullFileName, L"\\Logo_Updates.xml");
    #else
        // Construct Settings.xml path using development directory.
        strcpy(szFullFileName, APP_DIRECTORY_ANSI); 
        strcat(szFullFileName, "\\Logo_Updates.xml");
    #endif

    hUpdatesFile = CreateFile(szFullFileName, GENERIC_READ, 
        FILE_SHARE_READ, NULL /* security - not used*/,          
        OPEN_EXISTING /* opens file if exists, fails if does not */,
                                  FILE_ATTRIBUTE_NORMAL /* normal use */, NULL);

    if (hUpdatesFile == INVALID_HANDLE_VALUE) // Updates.xml does not exist
    {
        return FALSE;
    }
    else
    {
        CloseHandle(hUpdatesFile);
        return TRUE;
    }
}

// Notes: The valid Logo_Updates.xml must have <RationalTypistPro> as the first 
//        line. The function returns FALSE if the string is not in the returned
//        file. [MSB: In fact, the server can return "<!DOCTYPE ..." error file 
//        even though correct Updates.xml exists and CFile's GetFileName returns  
//        Updates.xml.]     
BOOL App_IsValidRemoteLogoUpdatesFile(char* pszRemoteFile)
{
    string sRemoteFile = pszRemoteFile;

    if (sRemoteFile.find("<RationalTypistPro>" /* case-sensitive? */) !=  -1 /* not found */)
    {
        return TRUE;
    }

    return FALSE;
}

void App_CacheLogoUpdatesFile(char* pszRemoteFile)
{
    char szCurDirectory[MAX_PATH];
    char szFullFileName[MAX_PATH];  // FFN for Logo_Updates.xml
    string sResult;
    HANDLE hUpdatesFile;
    DWORD dWritten;
    DWORD dwOffset; 

    sResult = pszRemoteFile;

    //// Make full file name for Updates.xml.
    //wsprintf(szFullFileName, "%s%s", theApp.m_strInstallDirectory, 
    //	                                                       "\\Logo_Updates.xml");

    // Make full file name for Logo_Updates.xml. 
    #if !defined(DEVELOPMENT_STRUCTURE)
        // Product release settings. Obtain path to current directory.
        GetCurrentDirectoryA(MAX_PATH, szCurDirectory);
        strcpy(szFullFileName, szCurDirectory);
        strcat(szFullFileName, "\\Logo_Updates.xml");
    #else
        // Construct Settings.xml path using development directory.
        strcpy(szFullFileName, APP_DIRECTORY_ANSI); 
        strcat(szFullFileName, "\\Logo_Updates.xml");
    #endif

 //   //AfxMessageBox(sResult.c_str());

    // Re-write Logo_Updates.xml file completely: delete, then write again.
    DeleteFileA(szFullFileName);
    hUpdatesFile = CreateFileA(szFullFileName, GENERIC_READ | GENERIC_WRITE, 
        FILE_SHARE_READ | FILE_SHARE_WRITE, NULL /* security - not used*/,          
        OPEN_ALWAYS /* if exists - just open; if does not exist, create it */,
                                  FILE_ATTRIBUTE_NORMAL /* normal use */, NULL);

    // Tell the writer we need to write at the beginning of the file.
    dwOffset = SetFilePointer(hUpdatesFile, 0, NULL, FILE_BEGIN);

    // [As in RTP v1.0] using string might help to avoid a *buffer overrun* (in 
    // contrast to using TCHAR) when the size of resulting string is bigger than 
    // declared in TCHAR.
    WriteFile(hUpdatesFile, sResult.c_str(), strlen(sResult.c_str()), &dWritten, 
                                                                          NULL);
    CloseHandle(hUpdatesFile);
}

void App_ParseLogoUpdatesFile(char* pszFFN)
{
    string sTagValue;
    long iRed, iGreen, iBlue;
    char szHTMLColorValue[10]; // e.g. #A52A2A
    
    g_sLogoText = App_XMLReadValue("Text", pszFFN);

    strcpy(szHTMLColorValue, App_XMLReadValue("TextColor", pszFFN).c_str());
    App_ParseToDecColorValues(szHTMLColorValue /* in */, iRed /* out */, 
                                             iGreen /* out */, iBlue /* out */);
    g_clrLogoTextColor = RGB(iRed, iGreen, iBlue); 

    g_sLogoURL = App_XMLReadValue("URL", pszFFN);
    g_nLogoLeftIndent = atoi(App_XMLReadValue("LeftIndent", pszFFN).c_str());
}

// Purpose: calculate the length of test's text without CRLF (\r\n) symbols.
int Mode4_GetTextLength(wstring* psBuffer)
{
    //TCHAR szText[100];
    int nPos;
    int nCRLFCount = 0;
    int nRawSize = psBuffer->size();
    int nSize;

    nPos = psBuffer->find(L"\r\n");
    if (nPos != -1)
    {
        while (nPos != -1)
        {
            nCRLFCount++;
            nPos = psBuffer->find(L"\r\n", nPos + 1);
        }
    }
    else
    {
        nSize = nRawSize;
        return nSize;
    }

    nSize = (nRawSize + 1 /* UI passes sample in here without ending dot */) - 
                                  2*nCRLFCount /* CRLF represents 2 elements */;
    //wsprintf(szText, L"nRawSize: %d, nSize: %d nCRLFCount: %d", nRawSize, 
    // nSize, nCRLFCount); 
    //MessageBox(0, psBuffer->c_str(), szText, 0);

    return nSize;
}

// Purpose: Calculate the number of lines in sample text (the number of lines is 
//          CRLF (\r\n) symbols count plus 1). 
// Notes:   Bsed on Mode4_GetTextLength.
int Mode4_GetTextLinesCount(wstring* psBuffer)
{
    //TCHAR szText[100];
    int nPos;
    int nCRLFCount = 0;
    int nLinesCount;

    nPos = psBuffer->find(L"\r\n");
    if (nPos != -1)
    {
        while (nPos != -1)
        {
            nCRLFCount++;
            nPos = psBuffer->find(L"\r\n", nPos + 1);
        }
    }
    else
    {
        nLinesCount = 0;
        return nLinesCount;
    }

    nLinesCount = nCRLFCount + 1;
    //wsprintf(szText, L"Lines Count: %d", nLinesCount); 
    //MessageBox(0, psBuffer->c_str(), szText, 0);

    return nLinesCount;
}

// Purpose: Returns text from the beginning of the sample based on set number of 
//          spaces (2 by default) and appends "..." at the end. For, example:
//          the string "Slides are extremely..." is returned for for EN sample.
wstring Mode4_GetTextTitle(wstring* psBuffer /* in */)
{
    wstring sTitle;
    int nPos;
    int nCount = 0; // avoid C4700 warning (use without initialization) 
    wstring sSuffix = L"...";

    nPos = psBuffer->find(L" ");
    if (nPos != -1) 
    {
        while (nPos != -1)
        {
            nCount++;
            nPos = psBuffer->find(L" ", nPos + 1);
            if (nCount == 2 /* take text with 2 spaces */)
            {
                // For some reason unknown to me, substr returns wrong data 
                // without first argument.
                sTitle = psBuffer->substr(0, nPos).c_str();
                sTitle = sTitle + sSuffix;
                //MessageBox(0, sTitle.c_str(), L"", 0);
                return sTitle;
            }
        }
    }
    else
    {
        // No spaces found, one never comes here in general.
        return L"";
    }

    // Avoid C4715 warning ("not all control paths return a value") albeit one 
    // never come here.
    return L"";
}

// Returns file name from FFN (Win32 does not have a function to obtain file 
// name from FFN).
wstring App_GetFileName(wstring* psFFN)
{
    int nPos;
    wstring sFileName;

    nPos  = psFFN->find_last_of(L"\\");
    sFileName = psFFN->substr(nPos + 1);

    return sFileName;
}

// Returns directory name from FFN (Win32 does not have a function to obtain directory
// from FFN).
wstring App_GetDirectory(wstring* psFFN)
{
    int nPos;
    wstring sDirectory;

    nPos  = psFFN->find_last_of(L"\\");
    sDirectory = psFFN->substr(0, nPos); 

    return sDirectory;
}

// Checks if input parameter is FFN. If not modifies it to make FFN if the 
// parameter is not an empty string. 
wstring App_ProofFFN(LPCTSTR pszName)
{
    int nPos;
    TCHAR szCurDir[MAX_PATH];
    TCHAR szSamplesDir[MAX_PATH];
    TCHAR szFFN[MAX_PATH];
    wstring sName;
    wstring sFFN;

    sName = pszName; // the input parameter can be file name (typically) or FFN
    nPos  = sName.find_last_of(L"\\");

    // If pszFFN is merely a file name and not an empry string, which is most 
    // typical, prepend it with the default directory for relative sample tests.
    if (nPos == -1 && lstrlen(pszName) != 0) 
    {
        GetCurrentDirectory(MAX_PATH, szCurDir);
        
        lstrcpy(szSamplesDir, szCurDir);
        switch (g_nLangID)
        {
        case 1:	
            _tcscat(szSamplesDir, L"\\EN\\Tests");
            break;
        case 2:	
            _tcscat(szSamplesDir, L"\\FR\\Tests");
            break;
        case 3:	
            _tcscat(szSamplesDir, L"\\RU\\Tests");
            break;
        default:
            _tcscat(szSamplesDir, L"\\EN\\Tests");
        }

        wsprintf(szFFN, L"%s\\%s", szSamplesDir, pszName);

        sFFN = szFFN; 
        return sFFN;
    }

    // The input parameter is FFN or empty string, return it unmodified.
    return sName;
}

void App_XMLRead_SampleTestFFN()
{
     char szCurDirectory[MAX_PATH]; // argument of open is ANSI string
    char szFFN[MAX_PATH];     // argument of open is ANSI string   
    char szSampleFFN[MAX_PATH];
    TCHAR szBuffer[MAX_PATH];

    #ifndef DEVELOPMENT_STRUCTURE
        // Construct Settings.xml path using current directory.
        GetCurrentDirectoryA(MAX_PATH, szCurDirectory);
        strcpy(szFFN, szCurDirectory);
        strcat(szFFN, "\\Settings.xml");
    #else
        // Construct Settings.xml path using development directory.
        strcpy(szFFN, APP_DIRECTORY_ANSI);
        strcat(szFFN, "\\Settings.xml");
    #endif

    // Update g_sSampleFFN from Settings.xml that is one variable for all
    // languages.
    switch (g_nLangID)
    {
    case 1:
        strcpy(szSampleFFN, App_XMLReadValue("Sample_Test_EN", szFFN).c_str());
        break;
    case 2:
        strcpy(szSampleFFN, App_XMLReadValue("Sample_Test_FR", szFFN).c_str());
        break;
    case 3:
        strcpy(szSampleFFN, App_XMLReadValue("Sample_Test_RU", szFFN).c_str());
        break;
    default:
        strcpy(szSampleFFN, App_XMLReadValue("Sample_Test_EN", szFFN).c_str());
    }

    MultiByteToWideChar(CP_ACP, 0 /* no handling of composite etc chars  */, 
            szSampleFFN, -1 /* input string is NULL-terminated */, 
            szBuffer, 
                   sizeof(szBuffer)/sizeof(szBuffer[0]) /* wide chars count */);

    // Most typically, the tag value is an empty string (default sample test is 
    // used) and g_sSampleFFN is also empty. In other cases, szBuffer can be 
    // file name or FFN, but the g_sSampleFFN must be always FFN. The 
    // App_ProofFFN function always returns correct FFN (or empty string).
    g_sSampleFFN = App_ProofFFN(szBuffer);
}

// The input parameter is either EN, or FR, or RU.
BOOL App_IsLangInstalled(LPCTSTR pszLangID /* in */)
{
    HANDLE hFile;	
    
    // Check if language files are installed by trying of open the relative
    // directory.
    hFile = CreateFile(pszLangID, GENERIC_READ, 0 /* not shared */,            
        NULL /* no security */, OPEN_EXISTING, 
        FILE_ATTRIBUTE_NORMAL | FILE_FLAG_BACKUP_SEMANTICS,  
                                                   NULL /* no template file */);
    if (hFile != INVALID_HANDLE_VALUE)
    {
        CloseHandle(hFile);
        return TRUE;
    }

    CloseHandle(hFile);
    return FALSE;
}


