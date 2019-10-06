#include "stdafx.h"
//#include "resource.h"
#include "installer.h"
#include "commctrl.h" // listview
#include "shlobj.h"   // SHGetDesktopFolder
#include "shellapi.h"

#define MAX_LOADSTRING 100

#include <stdio.h> //fopen/fwrite

// Global Variables:
HINSTANCE hInst;
HWND g_hDlg;
TCHAR g_szDestination[MAX_PATH];
WNDPROC g_wpOldWndProc;

struct LANG_SET 
{
    BOOL m_bInstallEnglish;
    BOOL m_bInstallFrench;
    BOOL m_bInstallRussian;
} ls;

BOOL				InitInstance(HINSTANCE, int);
BOOL Install(HWND hDlg);
void ReadLangData();
void CreateDestinationLocale(short nLangID);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK FinishDialogProc(HWND hDlg, UINT message, WPARAM wParam, 
                                                                 LPARAM lParam);
LRESULT APIENTRY StaticSubclassedFunc(HWND hWnd,UINT Message,WPARAM wParam, 
                                                                   LONG lParam);
void PopulateListView(HWND hDlg, short iDefaultLanguage);

BOOL TransferFileToDisk(short nResourceID, char* pszTargetDirectory, 
                                                       char* pszTargetFileName);
int DoBrowse(HWND hDlg, LPTSTR pszDestination /* out */);
void SetFont(HWND hWnd);
void CreateXMLSettingsFile(char* pszTargetDirectory, short nLangID);

class CMemFile
{
public:
// Constructors
    CMemFile(BYTE* lpBuffer, UINT nBufferSize, UINT nGrowBytes = 0);

// Implementation
public:
    UINT m_nGrowBytes;
    DWORD m_nPosition;
    DWORD m_nBufferSize;
    DWORD m_nFileSize;
    BYTE* m_lpBuffer;
    BOOL m_bAutoDelete;

    UINT Read(void* lpBuf, UINT nCount);
    void Close();
};

class CStdioFile 
{
public:
// Constructors
    CStdioFile(LPCTSTR lpszFileName, UINT nOpenFlags);

// Attributes
    FILE* m_pStream;    // stdio FILE

// Implementation
public:
    void Write(const void* lpBuf, UINT nCount);
    void Close();
};

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
     // TODO: Place code here.
    MSG msg;
    HACCEL hAccelTable;

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow)) 
    {
        return FALSE;
    }

    hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_SELFEXTRACTOR_WIN32);

    // Main message loop:
    while (GetMessage(&msg, NULL, 0, 0)) 
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    HWND hWnd;
    TCHAR szFileNamePart[100], szFileName[100];
    TCHAR szCommandLine[200];
        
    hInst = hInstance; // Store instance handle in our global variable

    //InitCommonControls();
    // TODO:  Works with InitCommonControls ...
    INITCOMMONCONTROLSEX icex;   
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC  = ICC_BAR_CLASSES;
    InitCommonControlsEx(&icex);

    // Choose final options.
    DialogBox(hInst, (LPCTSTR)IDD_FINISH, NULL, (DLGPROC)FinishDialogProc);

   return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    int wmId, wmEvent;
    PAINTSTRUCT ps;
    HDC hdc;

    switch (message) 
    {
    case WM_COMMAND:
        wmId    = LOWORD(wParam); 
        wmEvent = HIWORD(wParam); 
        // Parse the menu selections:
        switch (wmId)
        {
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
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

CMemFile::CMemFile(BYTE* lpBuffer, UINT nBufferSize, UINT nGrowBytes)
{
    m_nGrowBytes = nGrowBytes;
    m_nPosition = 0;
    m_nBufferSize = nBufferSize;
    m_nFileSize = nGrowBytes == 0 ? nBufferSize : 0;
    m_lpBuffer = lpBuffer;
    m_bAutoDelete = FALSE;
}

UINT CMemFile::Read(void* lpBuf, UINT nCount)
{
    if (nCount == 0)
        return 0;

    if (m_nPosition > m_nFileSize)
        return 0;

    UINT nRead;
    if (m_nPosition + nCount > m_nFileSize)
        nRead = (UINT)(m_nFileSize - m_nPosition);
    else
        nRead = nCount;

    memcpy((BYTE*)lpBuf, (BYTE*)m_lpBuffer + m_nPosition, nRead);
    m_nPosition += nRead;

    return nRead;
}

void CMemFile::Close()
{
    //Free(m_lpBuffer);
    m_lpBuffer = NULL;
}

CStdioFile::CStdioFile(LPCTSTR lpszFileName, UINT nOpenFlags)
{
    m_pStream = fopen(lpszFileName, "wb" /* modeWrite */);
}

void CStdioFile::Write(const void* lpBuf, UINT nCount)
{
    fwrite(lpBuf, sizeof(BYTE), nCount, m_pStream);
}

void CStdioFile::Close()
{
    if (m_pStream != NULL) fclose(m_pStream);
    m_pStream = NULL;
}

BOOL Install(HWND hDlg)
{
    HINSTANCE hAppInst;
    TCHAR szTargetDir[MAX_PATH];
    TCHAR szMessage[200];
    TCHAR szCommandLine[MAX_PATH];
    short iComboBoxItem;
    HWND hwndLV;
    TCHAR szItemText[100];

    // Read from UI.
    iComboBoxItem = SendMessage(GetDlgItem(hDlg, IDC_COMBO), CB_GETCURSEL, 
                                              0 /* not used*/, 0 /* not used*/);

    // Create target directory, which is the root for language same level 
    // subfolders (EN/FR/RU). 
    lstrcpy(szTargetDir, g_szDestination); 
    CreateDirectory(szTargetDir, NULL);

    // Transfer to destination language-independent files.
    TransferFileToDisk(IDR_CUSTOM1, szTargetDir, "RTP.exe");

    // Create Settings.xml in destination directory (created from scratch).
    CreateXMLSettingsFile(szTargetDir, 
                                      iComboBoxItem + 1 /* default language */);

    // Install primary language.
    switch (iComboBoxItem /* zero-based combo box item index */)
    {
    case 0:
        CreateDestinationLocale(1 /* English */);
        break;
    case 1:
        CreateDestinationLocale(2 /* French */);
        break;
    case 2:
        CreateDestinationLocale(3 /* Russian */);
        break;
    }

    // Install additional languages.
    hwndLV = GetDlgItem(hDlg, IDC_LISTVIEW);

    for (int i = 0; i < 2 /* max 2 additional languages in 1.0 version */; i++)
    {
        ListView_GetItemText(hwndLV, i,  0, szItemText, 100);

        if (lstrcmp(szItemText, _T("English")) == 0 && 
                                              ListView_GetCheckState(hwndLV, i))
        {
            CreateDestinationLocale(1 /* English */);
        }
        else if (lstrcmp(szItemText, _T("French")) == 0 && 
                                              ListView_GetCheckState(hwndLV, i))
        {
            CreateDestinationLocale(2 /* French */);
        }
        else if (lstrcmp(szItemText, _T("Russian")) == 0 && 
                                              ListView_GetCheckState(hwndLV, i))
        {
            CreateDestinationLocale(3 /* Russian */);
        }
    }

    // Finally prompt a user to open destination folder optionally. 
    lstrcpy(szMessage, "Rational Typist has been installed successfully. ");
    lstrcat(szMessage, "Open destination folder?");
    
    if (IDYES == MessageBox(NULL, szMessage, 
          _T("Rational Typist 1.4 Setup"), MB_YESNO | MB_ICONQUESTION))
    {
        lstrcpy(szCommandLine, _T("explorer.exe "));
        lstrcat(szCommandLine, g_szDestination); 

        WinExec(szCommandLine, TRUE); 
    }

    // Destroy installer window.
    PostQuitMessage(0);
    
    return TRUE; // never here
}

BOOL TransferFileToDisk(short nResourceID, char* pszTargetDirectory, 
                                                        char* pszTargetFileName)
{
    HINSTANCE hAppInst;
    TCHAR szTargetPath[MAX_PATH];
    HRSRC hRsrc;
    TCHAR szName[10];
    BYTE* lpRsrc;
    TCHAR szBuf[16];
    UINT iLength = 10;
    UINT nBytesRead = 1;
    DWORD len;

    hAppInst = hInst;

    //--------------------------------------------------------------------------
    // Stream into setup.
    //--------------------------------------------------------------------------

    wsprintf(szName, "#%d", nResourceID); 
    hRsrc = FindResource(hAppInst , szName, "CUSTOM"); 
    if (hRsrc == NULL)
    {
        ::MessageBox(NULL, "FindResource failed!", "", MB_OK);
    }

    // Load resource into memory.
    lpRsrc = (BYTE*)LoadResource(hAppInst, hRsrc);
    if (lpRsrc == NULL)
    {
        ::MessageBox(NULL, "LoadResource failed!", "", MB_OK);
    }

    wsprintf(szTargetPath, "%s%s%s", pszTargetDirectory, _T("\\"), 
                                                             pszTargetFileName); 

    // Create memory file. 
    len = SizeofResource(hAppInst, hRsrc);

    CMemFile memfile_source(lpRsrc, len); 
    CStdioFile std_file_destination(szTargetPath, 
              0 /* CFile::modeCreate | CFile::modeWrite | CFile::typeBinary */);
    
    while(nBytesRead > 0)
    {
        nBytesRead = memfile_source.Read(szBuf, 10);
        std_file_destination.Write(szBuf, nBytesRead);
    }

    memfile_source.Close();
    std_file_destination.Close();
    FreeResource(hRsrc);

    return TRUE; 
}

LRESULT CALLBACK FinishDialogProc(HWND hDlg, UINT message, WPARAM wParam, 
                                                                  LPARAM lParam)
{
    RECT rc;
    TCHAR szImageSetName[100], szName[100];
    TCHAR szEnvVariable[MAX_PATH];
    int iSelected;
    LVCOLUMN lvc;
    TCHAR szBuffer[MAX_PATH];

    switch (message)
    {
    case WM_INITDIALOG:
        g_hDlg = hDlg;
        GetWindowRect(hDlg, &rc);
        SetWindowPos(hDlg, NULL,
            ((GetSystemMetrics(SM_CXSCREEN) - (rc.right - rc.left)) / 2),
            ((GetSystemMetrics(SM_CYSCREEN) - (rc.bottom - rc.top)) / 2),
                                             0, 0, SWP_NOSIZE | SWP_NOACTIVATE);

        // The CB_ADDSTRING message is used instead of CB_INSERTSTRING message 
        // to indicate that new combo box items are auto-sorted (CB_INSERTSTRING
        // does not cuase auto-sorting).
        SendMessage(GetDlgItem(hDlg, IDC_COMBO), CB_ADDSTRING, 
                                         NULL /* not used*/, (LPARAM)"English");
        SendMessage(GetDlgItem(hDlg, IDC_COMBO), CB_ADDSTRING, 
                                          NULL /* not used*/, (LPARAM)"French");
        SendMessage(GetDlgItem(hDlg, IDC_COMBO), CB_ADDSTRING,  
                                         NULL /* not used*/, (LPARAM)"Russian");
        SendMessage(GetDlgItem(hDlg, IDC_COMBO), CB_SETCURSEL, 0, 0);

        
        ListView_SetExtendedListViewStyle(GetDlgItem(hDlg, IDC_LISTVIEW), 
                                                             LVS_EX_CHECKBOXES);
        // Setup list view colums. Albeit column titles (in fact one column) are 
        // not supposed to be shown, the columns are added because if not 
        // inserted items do not show up.
        lvc.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_FMT; 
        
        lvc.iSubItem = 0;
        lvc.pszText = "Language (invisible)";
        lvc.cx = 100;     
        ListView_InsertColumn(GetDlgItem(hDlg, IDC_LISTVIEW), 0, &lvc);

        PopulateListView(hDlg, 1 /* English is default language */);
    
        // Retrive from environment windows directory. 
        ExpandEnvironmentStrings("%userprofile%", szEnvVariable, 
                                                         sizeof(szEnvVariable)); 
        
        wsprintf(g_szDestination, "%s%s", szEnvVariable, 
                                        _T("\\My Documents\\RTP"));
        SendMessage(GetDlgItem(hDlg, IDC_EDIT), 
                         WM_SETTEXT, 0 /* not used */, (LPARAM)g_szDestination);

        // URL
        g_wpOldWndProc = (WNDPROC)SetWindowLong(GetDlgItem (hDlg, 
                    IDC_STATIC_LINK), GWL_WNDPROC, (DWORD)StaticSubclassedFunc);
        
        SetFont(hDlg);

        return TRUE;
    case WM_CLOSE:
        PostQuitMessage(0);
        break;
    case WM_CTLCOLORSTATIC:
        if (GetDlgItem (hDlg,IDC_STATIC_LINK) == 
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

        if (LOWORD(wParam) == IDC_STATIC_LINK)
        {
            ShellExecute(NULL, "open", "http://www.carexplorer.org", 
                                                           0, 0, SW_SHOWNORMAL);
        }

        if (HIWORD(wParam) == CBN_SELCHANGE) 
        {
            iSelected = SendMessage(GetDlgItem(hDlg, IDC_COMBO), CB_GETCURSEL, 
                                      0 /* must be zero*/, 0 /* must be zero*/);
            ListView_DeleteAllItems(GetDlgItem(hDlg, IDC_LISTVIEW));
            if (iSelected != CB_ERR)
            {
                PopulateListView(hDlg, iSelected /* zero-based */ + 1);
            }
        }

        switch (LOWORD(wParam))
        {
        case IDC_BUTTON_DESTINATION:
            DoBrowse(hDlg, g_szDestination /* out */);
            SendMessage(GetDlgItem(hDlg, IDC_EDIT), 
                         WM_SETTEXT, 0 /* not used */, (LPARAM)g_szDestination);
            break;
        case IDC_BUTTON_INSTALL:
            // Read destination path to account for scenario when it was updated 
            // manually.
            SendMessage(GetDlgItem(hDlg, IDC_EDIT), WM_GETTEXT, MAX_PATH, 
                                                              (LPARAM)szBuffer);
            lstrcpy(g_szDestination, szBuffer); 

            // Transfer files to disk.
            Install(hDlg);
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        }

        break;
    }

    return FALSE;
}

int DoBrowse(HWND hDlg, LPTSTR pszDestination /* out */)
{
    TCHAR szDestination[MAX_PATH];
    LPMALLOC pMalloc;
    BROWSEINFO bInfo;
    LPITEMIDLIST pidl;

    if (SHGetMalloc(&pMalloc)!= NOERROR)
    {
        return 0;
    }

    ZeroMemory ((PVOID) &bInfo,sizeof (BROWSEINFO));

    ////if (!m_strInitDir.IsEmpty ())
    //{
    //OLECHAR       olePath[MAX_PATH];
    //ULONG         chEaten;
    //ULONG         dwAttributes;
    //HRESULT       hr;
    //LPSHELLFOLDER pDesktopFolder;
    //// 
    //// Get a pointer to the Desktop's IShellFolder interface. 
    ////
    //	if (SUCCEEDED(SHGetDesktopFolder(&pDesktopFolder)))
    //	{
    //		//
    //		// IShellFolder::ParseDisplayName requires the file name be in Unicode.
    //		//
    //		MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, 
    //        "c:\\winnt" /*m_strInitDir.GetBuffer(MAX_PATH)*/, -1,
    //							olePath, MAX_PATH);

    //		//m_strInitDir.ReleaseBuffer (-1);
    //		//
    //		// Convert the path to an ITEMIDLIST.
    //		//
    //		hr = pDesktopFolder->ParseDisplayName(NULL,
    //											NULL,
    //											olePath,
    //											&chEaten,
    //											&pidl,
    //											&dwAttributes);
    //		if (FAILED(hr))
    //		{
    //		pMalloc ->Free (pidl);
    //		pMalloc ->Release ();
    //		return 0;
    //		}
    //		bInfo.pidlRoot = pidl;

    //	}
    //}
    bInfo.hwndOwner = hDlg; // keeps browse window close to the owner
    bInfo.pszDisplayName = szDestination; 
    bInfo.lpszTitle = "Select install directory"; 
    bInfo.ulFlags =  BIF_RETURNFSANCESTORS | BIF_RETURNONLYFSDIRS; 
    bInfo.lpfn = NULL; // no callback function

    if ((pidl = ::SHBrowseForFolder(&bInfo)) == NULL)
    {
        return 0;
    }

    if (::SHGetPathFromIDList(pidl, szDestination /* out */) == FALSE)
    {
        pMalloc ->Free(pidl);
        pMalloc ->Release();
        return 0;
    }

    pMalloc ->Free(pidl);
    pMalloc ->Release();

    // When drive letter was selected for destination, the path
    // has ending "\" and we don't need to add this symbol,
    // when selected destination is a folder there is no ending
    // "\" and we add it.
    if (lstrlen(szDestination) == 3 /* length of path like "C:\" */)
    {
        wsprintf(pszDestination, "%s%s", szDestination, "RTP");
    }
    else
    {
        wsprintf(pszDestination, "%s%s", szDestination, "\\RTP");
    }

    return 1;
}

void PopulateListView(HWND hDlg, short iDefaultLanguage)
{
    LVITEM lvi;
    lvi.mask = LVIF_TEXT | LVIF_STATE;

    switch (iDefaultLanguage)
    {
    case 1: // English is default language
        lvi.iItem = 0;
        lvi.iSubItem = 0;
        lvi.pszText = "French";
        //iInsertedItem = 
        ListView_InsertItem(GetDlgItem(hDlg, IDC_LISTVIEW), &lvi);

        lvi.iItem = 1;
        lvi.iSubItem = 0;
        lvi.pszText = "Russian";
        //iInsertedItem = 
        ListView_InsertItem(GetDlgItem(hDlg, IDC_LISTVIEW), &lvi);

        break;
    case 2: // French is default language
        lvi.state = 0; 
        lvi.iItem = 0;
        lvi.iSubItem = 0;
        lvi.pszText = "English";
        //int iInsertedItem = 
        ListView_InsertItem(GetDlgItem(hDlg, IDC_LISTVIEW), &lvi);

        lvi.iItem = 1;
        lvi.iSubItem = 0;
        lvi.pszText = "Russian";
        //iInsertedItem = 
        ListView_InsertItem(GetDlgItem(hDlg, IDC_LISTVIEW), &lvi);

        break;
    case 3:// Russian is default language
        lvi.state = 0; 
        lvi.iItem = 0;
        lvi.iSubItem = 0;
        lvi.pszText = "English";
        //int iInsertedItem = 
        ListView_InsertItem(GetDlgItem(hDlg, IDC_LISTVIEW), &lvi);

        lvi.iItem = 1;
        lvi.iSubItem = 0;
        lvi.pszText = "French";
        //iInsertedItem = 
        ListView_InsertItem(GetDlgItem(hDlg, IDC_LISTVIEW), &lvi);

        break;
    }
}

LRESULT APIENTRY StaticSubclassedFunc(HWND hWnd, UINT Message, WPARAM wParam, 
                LONG lParam)
{
    SetCursor(LoadCursor(NULL, IDC_HAND));

    return CallWindowProc((WNDPROC)g_wpOldWndProc, hWnd, Message, wParam, 
                                                                        lParam);
}

void SetFont(HWND hWnd)
{
    HFONT hfnt;
    LOGFONT lf;

    // Specify the font to use (stored in a LOGFONT structure).
    lf.lfHeight = 14;
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
    _tcsncpy(lf.lfFaceName, _T("Arial"), 32);

    hfnt = CreateFontIndirect(&lf);
    SendMessage(GetDlgItem(hWnd, IDC_STATIC_LINK),  WM_SETFONT, (WPARAM)hfnt, TRUE);
}

void CreateDestinationLocale(short nLangID)
{
    TCHAR szTargetDir[MAX_PATH];
    TCHAR szTargetLangRoot[MAX_PATH];
    TCHAR szTrainingWordsDir[MAX_PATH];
    TCHAR szTestsDir[MAX_PATH];

    lstrcpy(szTargetDir, g_szDestination); 

    switch (nLangID)
    {
    case 1: // English
        wsprintf(szTargetLangRoot, "%s%s", szTargetDir, _T("\\EN"));
        wsprintf(szTestsDir, "%s%s", szTargetLangRoot, "\\Tests");
        wsprintf(szTrainingWordsDir, "%s%s", szTargetLangRoot, 
                                                            "\\Training Words");

        CreateDirectory(szTargetLangRoot, NULL);
        CreateDirectory(szTestsDir, NULL);
        CreateDirectory(szTrainingWordsDir, NULL);

        TransferFileToDisk(IDR_CUSTOM_EN_TW_LH_WORDS, szTrainingWordsDir, 
                                                                "LH_Words.dat");
        TransferFileToDisk(IDR_CUSTOM_EN_TW_RH_WORDS, szTrainingWordsDir, 
                                                                "RH_Words.dat");
        TransferFileToDisk(IDR_CUSTOM_EN_TW_LTOR_WORDS, szTrainingWordsDir, 
                                                               "LTOR_Both.dat");
        TransferFileToDisk(IDR_CUSTOM_EN_TW_RTOH_WORDS, szTrainingWordsDir, 
                                                               "RTOL_Both.dat");
        TransferFileToDisk(IDR_CUSTOM_EN_TEST, szTestsDir, "Sample.txt");
        TransferFileToDisk(IDR_CUSTOM_EN_TEST2, szTestsDir, "Sample2.txt");
        TransferFileToDisk(IDR_CUSTOM_EN_TEST3, szTestsDir, "Sample3.txt");
        TransferFileToDisk(IDR_CUSTOM_EN_TEST4, szTestsDir, 
                                                         "BOR_Amendment_I.txt");
        TransferFileToDisk(IDR_CUSTOM_EN_TEST5, szTestsDir, 
                                                        "BOR_Amendment_II.txt");
        TransferFileToDisk(IDR_CUSTOM_EN_TEST6, szTestsDir, 
                                                       "BOR_Amendment_III.txt");
        TransferFileToDisk(IDR_CUSTOM_EN_TEST7, szTestsDir, 
                                                        "BOR_Amendment_IV.txt");
        TransferFileToDisk(IDR_CUSTOM_EN_TEST8, szTestsDir, 
                                                         "BOR_Amendment_V.txt");
        TransferFileToDisk(IDR_CUSTOM_EN_TEST9, szTestsDir, 
                                                        "BOR_Amendment_VI.txt");
        TransferFileToDisk(IDR_CUSTOM_EN_TEST10, szTestsDir, 
                                                       "BOR_Amendment_VII.txt");
        TransferFileToDisk(IDR_CUSTOM_EN_TEST11, szTestsDir, 
                                                      "BOR_Amendment_VIII.txt");
        TransferFileToDisk(IDR_CUSTOM_EN_TEST12, szTestsDir, 
                                                        "BOR_Amendment_IX.txt");
        TransferFileToDisk(IDR_CUSTOM_EN_TEST13, szTestsDir, 
                                                         "BOR_Amendment_X.txt");
        break;
    case 2: // French
        wsprintf(szTargetLangRoot, "%s%s", szTargetDir, _T("\\FR"));
        wsprintf(szTestsDir, "%s%s", szTargetLangRoot, "\\Tests");
        wsprintf(szTrainingWordsDir, "%s%s", szTargetLangRoot, 
                                                            "\\Training Words");

        CreateDirectory(szTargetLangRoot, NULL); 
        CreateDirectory(szTestsDir, NULL); 
        CreateDirectory(szTrainingWordsDir, NULL); 
    
        TransferFileToDisk(IDR_CUSTOM_FR_TW_LH_WORDS, szTrainingWordsDir, 
                                                                "LH_Words.dat");
        TransferFileToDisk(IDR_CUSTOM_FR_TW_RH_WORDS, szTrainingWordsDir, 
                                                                "RH_Words.dat");
        TransferFileToDisk(IDR_CUSTOM_FR_TW_LTOR_WORDS, szTrainingWordsDir, 
                                                               "LTOR_Both.dat");
        TransferFileToDisk(IDR_CUSTOM_FR_TW_RTOH_WORDS, szTrainingWordsDir, 
                                                               "RTOL_Both.dat");
        TransferFileToDisk(IDR_CUSTOM_FR_TEST, szTestsDir, "Sample.txt");
        TransferFileToDisk(IDR_CUSTOM_FR_TEST2, szTestsDir, "Sample2.txt");
        TransferFileToDisk(IDR_CUSTOM_FR_TEST3, szTestsDir, "Sample3.txt");
        break;
    case 3: // Russian
        wsprintf(szTargetLangRoot, "%s%s", szTargetDir, _T("\\RU"));
        wsprintf(szTestsDir, "%s%s", szTargetLangRoot, "\\Tests");
        wsprintf(szTrainingWordsDir, "%s%s", szTargetLangRoot, 
                                                            "\\Training Words");

        CreateDirectory(szTargetLangRoot, NULL);
        CreateDirectory(szTestsDir, NULL);
        CreateDirectory(szTrainingWordsDir, NULL);

        TransferFileToDisk(IDR_CUSTOM_RU_TW_LH_WORDS, szTrainingWordsDir, 
                                                        "LH_Words_RU_UTF8.txt");
        TransferFileToDisk(IDR_CUSTOM_RU_TW_RH_WORDS, szTrainingWordsDir, 
                                                        "RH_Words_RU_UTF8.txt");
        TransferFileToDisk(IDR_CUSTOM_RU_TW_LTOR_WORDS, szTrainingWordsDir, 
                                                      "LTOR_Words_RU_UTF8.txt");
        TransferFileToDisk(IDR_CUSTOM_RU_TW_RTOH_WORDS, szTrainingWordsDir, 
                                                      "RTOL_Words_RU_UTF8.txt");
        TransferFileToDisk(IDR_CUSTOM_RU_TEST, szTestsDir, "Sample.txt");
        TransferFileToDisk(IDR_CUSTOM_RU_TEST2, szTestsDir, "Sample2.txt");
        TransferFileToDisk(IDR_CUSTOM_RU_TEST3, szTestsDir, "Sample3.txt");
        break;
    }
}

// Settings.xml is created from scratch (instead of streaming as in Beta 1) to
// account that default language is not a constant, but variable (i.e. can 
// Russian, not only English).
void CreateXMLSettingsFile(char* pszTargetDirectory, short nLangID)
{
    char szSettings[2048]; //[1024];
    char szFilePath[MAX_PATH]; 
    HANDLE hSettings;
    DWORD dwOffset, dWritten;

    // Read from the dialog languages to install. Used to add proper 
    // <Sample_Test_XX> tags.
    ReadLangData(); 

    strcpy(szSettings, "<RationalTypistPro>"); strcat(szSettings, "\r\n");
    strcat(szSettings, "\r\n");
    strcat(szSettings, "<Application>"); strcat(szSettings, "\r\n");
    strcat(szSettings, "    <SaveLastUsed>1</SaveLastUsed>"); 
                                                     strcat(szSettings, "\r\n");
    strcat(szSettings, "    <Mode>4</Mode>"); strcat(szSettings, "\r\n");

    // This switch is why I make XML instead of streaming original XML.
    switch (nLangID)
    {
    case 1: // English
        strcat(szSettings, "    <Language>1</Language>"); 
                                                     strcat(szSettings, "\r\n");
        break;
    case 2: // French
        strcat(szSettings, "    <Language>2</Language>"); 
                                                     strcat(szSettings, "\r\n");
        break;
    case 3: // Russian
        strcat(szSettings, "    <Language>3</Language>"); 
                                                     strcat(szSettings, "\r\n");
        break;
    default: // English
        strcat(szSettings, "    <Language>1</Language>"); 
                                                     strcat(szSettings, "\r\n");
        break;
    }

    strcat(szSettings, "    <BackgroundColor>#316AC5</BackgroundColor>"); 
                                                     strcat(szSettings, "\r\n");
    strcat(szSettings, "    <AlertEnabled>1</AlertEnabled>"); 
                                                     strcat(szSettings, "\r\n");
    strcat(szSettings, "    <ShowURLLogo>1</ShowURLLogo>"); 
                                                     strcat(szSettings, "\r\n");
    strcat(szSettings, "    <ShowSBHints>1</ShowSBHints>"); 
                                                     strcat(szSettings, "\r\n");
    strcat(szSettings, "    <Debug>0</Debug>"); strcat(szSettings, "\r\n");
    strcat(szSettings, "</Application>"); strcat(szSettings, "\r\n");
    strcat(szSettings, "\r\n");
    strcat(szSettings, "<Mode1></Mode1>"); strcat(szSettings, "\r\n");
    strcat(szSettings, "<Mode2></Mode2>"); strcat(szSettings, "\r\n");
    strcat(szSettings, "<Mode3>"); strcat(szSettings, "\r\n");
    //strcat(szSettings, "</Mode3>"); strcat(szSettings, "\r\n");
    strcat(szSettings, "    <DropWord>3</DropWord>"); 
                                                     strcat(szSettings, "\r\n");
    strcat(szSettings, "</Mode3>"); strcat(szSettings, "\r\n");
    strcat(szSettings, "\r\n");
    strcat(szSettings, "<Mode4>"); strcat(szSettings, "\r\n");
    if (ls.m_bInstallEnglish)
        strcat(szSettings, "    <Sample_Test_EN></Sample_Test_EN>"); 
                                                     strcat(szSettings, "\r\n");
    if (ls.m_bInstallFrench)
        strcat(szSettings, "    <Sample_Test_FR></Sample_Test_FR>"); 
                                                     strcat(szSettings, "\r\n");
    if (ls.m_bInstallRussian)
        strcat(szSettings, "    <Sample_Test_RU></Sample_Test_RU>"); 
                                                     strcat(szSettings, "\r\n");
    strcat(szSettings, "    <Natural_Word_Count>0</Natural_Word_Count>"); 
                                                     strcat(szSettings, "\r\n");
    strcat(szSettings, "    <Log_Results>0</Log_Results>"); 
                                                     strcat(szSettings, "\r\n");
    strcat(szSettings, "    <H1D_Show>1</H1D_Show>"); 
                                                     strcat(szSettings, "\r\n");
    strcat(szSettings, "    <H2D_Show>1</H2D_Show>"); 
                                                     strcat(szSettings, "\r\n");
    strcat(szSettings, "    <BS_BackgroundColor>#FFFF00</BS_BackgroundColor>"); 
                                                     strcat(szSettings, "\r\n");
    strcat(szSettings, "    <DS_BackgroundColor>#ECE9D8</DS_BackgroundColor>"); 
                                                     strcat(szSettings, "\r\n");
    strcat(szSettings, "    <DS_InterlacedColor>#F5F5DC</DS_InterlacedColor>"); 
                                                     strcat(szSettings, "\r\n");
    strcat(szSettings, "    <DS_Custom>#FF8C00</DS_Custom>"); 
                                                     strcat(szSettings, "\r\n");
    strcat(szSettings, "    <DS_LHColor>#FF0000</DS_LHColor>"); 
                                                     strcat(szSettings, "\r\n");
    strcat(szSettings, "    <DS_L2RColor>#FFD700</DS_L2RColor>"); 
                                                     strcat(szSettings, "\r\n");
    strcat(szSettings, "    <DS_R2LColor>#00FA9A</DS_R2LColor>"); 
                                                     strcat(szSettings, "\r\n");
    strcat(szSettings, "    <DS_RHColor>#1E90FF</DS_RHColor>"); 
                                                     strcat(szSettings, "\r\n");
    strcat(szSettings, "</Mode4>"); strcat(szSettings, "\r\n");
    strcat(szSettings, "\r\n");
    // No CRLF (/r/n) at the file's end.
    strcat(szSettings, "</RationalTypistPro>");  

    // Make path for Settings.xml.
    strcpy(szFilePath, pszTargetDirectory);
    strcat(szFilePath, "\\Settings.xml");

    // Re-write XML file completely (delete, then write again).
    DeleteFile(szFilePath);
    hSettings = CreateFile(szFilePath, GENERIC_READ | GENERIC_WRITE, 
        FILE_SHARE_READ|FILE_SHARE_WRITE, NULL /* security - not used*/,          
        OPEN_ALWAYS /* if exists - just open; if does not exist, create it */,
                                  FILE_ATTRIBUTE_NORMAL /* normal use */, NULL);         

    // Tell the writer we need to write at the beginning of the file.
    dwOffset = SetFilePointer(hSettings, 0, NULL, FILE_BEGIN);

    //WriteFile(hSettings, sLine.c_str(), strlen(sLine.c_str()), &dWritten, NULL);
    WriteFile(hSettings, szSettings, strlen(szSettings), &dWritten, NULL);
    
    CloseHandle(hSettings);
}

void ReadLangData()
{
    short iComboBoxItem;
    HWND hwndLV;
    TCHAR szItemText[100];

    // Initialize LANG_SET structure. The data will be read from the dialog.
    ls.m_bInstallEnglish = FALSE;
    ls.m_bInstallFrench = FALSE;
    ls.m_bInstallRussian = FALSE;

    // Read the primary language from the dialog.
    iComboBoxItem = SendMessage(GetDlgItem(g_hDlg, IDC_COMBO), CB_GETCURSEL, 
                                              0 /* not used*/, 0 /* not used*/);

    switch (iComboBoxItem /* zero-based combo box item index */)
    {
    case 0:
        ls.m_bInstallEnglish = TRUE;
        break;
    case 1:
        ls.m_bInstallFrench = TRUE;
        break;
    case 2:
        ls.m_bInstallRussian = TRUE;
        break;
    }

    // Read additional languages from the dialog.
    hwndLV = GetDlgItem(g_hDlg, IDC_LISTVIEW);

    for (int i = 0; 
              i < 2 /* max 2 additional languages in 1.0 - 1.3 versions */; i++)
    {
        ListView_GetItemText(hwndLV, i,  0, szItemText, 100);

        if (lstrcmp(szItemText, _T("English")) == 0 && 
                                              ListView_GetCheckState(hwndLV, i))
        {
            ls.m_bInstallEnglish = TRUE;
        }
        else if (lstrcmp(szItemText, _T("French")) == 0 && 
                                              ListView_GetCheckState(hwndLV, i))
        {
            ls.m_bInstallFrench = TRUE;
        }
        else if (lstrcmp(szItemText, _T("Russian")) == 0 && 
                                              ListView_GetCheckState(hwndLV, i))
        {
            ls.m_bInstallRussian = TRUE;
        }
    }
}


