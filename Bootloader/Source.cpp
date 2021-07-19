#ifndef UNICODE
#define UNICODE
#endif 
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <windows.h>
#include <ShlObj.h>
#include <string.h>
#include <iostream>
#include <comdef.h> 
#include <fstream>
#include <tchar.h>
#include <stdio.h>
#include <strsafe.h> 
#include "Resoure.h"
#include "hex_file.h"
using namespace std;
const  char* startSend = "SSSSSSSS";
const  char* stopSend =  "PPPPPPPP";
/*Window*/
HWND hwnd;

/*Link file and title lable window*/
HWND hWndLink, hWndLabel;

/*button: open, boot, Ok, end*/
HWND opButton, bootButton, okButton,endButton;

/* text box display hext file and data to send boot*/
HWND hexTxtBox, dataTxtBox;

/*Combo box comport and baudrate*/
HWND hwndBaud, hwndCOM, hwndStatic, hwndStatic1;

/*hanlde comport and hex file*/
HANDLE hComm, hFile;

/*string of name comport*/
char* COMx = (char*)malloc(20);

/* Global variable  g_hinst - handle to the application instance */
HINSTANCE g_hinst;

/* Setting for communication */
DCB dcb;            

/*set selection baudrate comport combo-box*/
const wchar_t* baud[] = {L"115200", L"9600",
        L"4800", L"2400", L"1200" };

/*set selection of comport combo-box*/
const wchar_t* comport[] = {
        L"COM3", L"COM4", L"COM5",L"COM6",L"COM7"};

/* Global number of comport*/
int portnum = 0;

/*Global string file path*/
PWSTR pszFilePath;

/*Global process selection of combo box*/
LRESULT sel;

/*Global buffer line is read in file*/
unsigned char buffer_line[45];

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam); // callback function window process event

/*
/   entry-point function
*/
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    // Register the window class.
    const wchar_t CLASS_NAME[] = L"Bootloader";       //class name 
    WNDCLASS wc = { };                         // window classes defines a set of behaviors       

    wc.lpfnWndProc = WindowProc;               // a pointer to an application-defined funtion          
    wc.hInstance = hInstance;                  // the handle to application instance
    wc.lpszClassName = CLASS_NAME;             // s string that identifies the window class
    RegisterClass(&wc);                         // pass the address of the WNDCLASS structure to Register

     //Create the window.
      hwnd = CreateWindowEx(
        0,                              // Optional window styles.
        CLASS_NAME,                     // name of the window class
        L"Bootloader STM32",            // Window text  display in title bar
        WS_OVERLAPPEDWINDOW,            // Window style : https://docs.microsoft.com/en-us/windows/win32/winmsg/window-styles

        // Size and position
        WIN_LEFT, WIN_TOP, WIN_WIDTH, WIN_HEIGHT,
   
        NULL,       // Parent window    
        NULL,       // Menu
        hInstance,  // Instance handle
        NULL        // Additional application data
    );

    if (hwnd == NULL)  
        return 0;

    /* Show window*/
    ShowWindow(hwnd, nCmdShow);
   
    // Run the message loop.

    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0)) // removes the first message from the head of the queue. 
    {
        TranslateMessage(&msg);     // relate key strokes
        DispatchMessage(&msg);      // to call window procedure of the window that is the target of the message
    }
    return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    HDC hdcText = (HDC)wParam;    // handle display control text 
    switch (uMsg)
    {
        case WM_DESTROY :               // message quit 
        {
            PostQuitMessage(0);
            return 0;
        }
            
        case WM_CREATE:                 //message create element of the window
        {

            HINSTANCE hIns = ((LPCREATESTRUCT)lParam)->hInstance;

            /*create label header with font text*/
            hWndLabel = CreateWindowEx(0, L"Static", L"BOOTLOADER", WS_CHILD | WS_VISIBLE,
                LBL_WIN_LEFT, LBL_WIN_TOP, LBL_WIN_WIDTH, LBL_WIN_HEIGHT, hwnd, NULL, hIns, 0);

            HFONT hFont = CreateFont(32, 0, 0, 0, 700, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Arial");
            SendMessage(hWndLabel, WM_SETFONT, WPARAM(hFont), TRUE);

            /*create text box link file*/
            hWndLink = CreateWindowEx(WS_EX_CLIENTEDGE, L"static", NULL, WS_CHILD | WS_VISIBLE,
                LINK_TXT_LEFT, LINK_TXT_TOP, LINK_TXT_WIDTH, LINK_TXT_HEIGHT, hwnd,NULL, hIns, 0);
          
            /*create open button*/
            opButton = CreateWindowEx(WS_EX_CLIENTEDGE, L"Button", L"OPEN", WS_CHILD | WS_VISIBLE,
                OP_BUTTON_LEFT, OP_BUTTON_TOP, OP_BUTTON_WIDTH, OP_BUTTON_HEIGHT, hwnd, (HMENU)ID_OP_BUTTON, NULL, NULL);

            /*create boot button*/
            bootButton = CreateWindowEx(WS_EX_CLIENTEDGE, L"Button", L"BOOT", WS_CHILD | WS_VISIBLE,
                BOOT_BUTTON_LEFT, BOOT_BUTTON_TOP, BOOT_BUTTON_WIDTH, BOOT_BUTTON_HEIGHT, hwnd, (HMENU)ID_BOOT_BUTTON, NULL, NULL);

            /*create connect button*/
            okButton = CreateWindowEx(WS_EX_CLIENTEDGE, L"Button", L"OK", WS_CHILD | WS_VISIBLE,
                OK_BUTTON_LEFT, OK_BUTTON_TOP, OK_BUTTON_WIDTH, OK_BUTTON_HEIGHT, hwnd, (HMENU)ID_OK_BUTTON, NULL, NULL);

            /*create end button*/
            endButton = CreateWindowEx(WS_EX_CLIENTEDGE, L"Button", L"END", WS_CHILD | WS_VISIBLE,
                END_BUTTON_LEFT, END_BUTTON_TOP, END_BUTTON_WIDTH, END_BUTTON_HEIGHT, hwnd, (HMENU)ID_END_BUTTON, NULL, NULL);

            /*creat hex text box file*/
            hexTxtBox = CreateWindowEx(WS_EX_CLIENTEDGE, L"Edit", NULL, WS_CHILD | WS_VISIBLE |ES_MULTILINE,
                HEX_TXT_LEFT, HEX_TXT_TOP, HEX_TXT_WIDTH, HEX_TXT_HEIGHT, hwnd, NULL, hIns, 0);

            /*creat data text box*/
            dataTxtBox = CreateWindowEx(WS_EX_CLIENTEDGE, L"static", NULL, WS_CHILD | WS_VISIBLE,
                DATA_TXT_LEFT, DATA_TXT_TOP, DATA_TXT_WIDTH, DATA_TXT_HEIGHT, hwnd, NULL, hIns, 0);

            hwndBaud = CreateWindowW(L"Combobox", NULL,
                WS_CHILD | WS_VISIBLE | CBS_DROPDOWN,
                BAUD_LEFT, BAUD_TOP, BAUD_WIDTH, BAUD_HIGHT, hwnd, (HMENU)BAUD_ID, g_hinst, NULL);

            hwndCOM = CreateWindowW(L"Combobox", NULL,
                WS_CHILD | WS_VISIBLE | CBS_DROPDOWN,
                COM_LEFT, COM_TOP, COM_WIDTH, COM_HIGHT, hwnd, (HMENU)COM_ID, g_hinst, NULL);

            hwndStatic = CreateWindowW(L"Static", L"Baud",
                WS_CHILD | WS_VISIBLE,
                TEXT_BAUD_LEFT, TEXT_BAUD_TOP, TEXT_BAUD_WIDTH, TEXT_BAUD_HIGHT, hwnd, NULL, g_hinst, NULL);

            hwndStatic1 = CreateWindowW(L"Static", L"Port",
                WS_CHILD | WS_VISIBLE,
                TEXT_COM_LEFT, TEXT_COM_TOP, TEXT_COM_WIDTH, TEXT_COM_HIGHT, hwnd, NULL, g_hinst, NULL);

            for (int i = 0; i < 5; i++) {
                SendMessageW(hwndCOM, CB_ADDSTRING, 0, (LPARAM)comport[i]);
            }
            for (int i = 0; i < 5; i++) {
                SendMessageW(hwndBaud, CB_ADDSTRING, 0, (LPARAM)baud[i]);
            }
            break;

            return 0;
        }

        case WM_CTLCOLORSTATIC:                             // Do not allow DefWindowProc() to process this message!
        {                                                  // When a WM_CTLCOLORSTATIC message comes through, return
            SetTextColor(hdcText, RGB(0, 0, 255));
            SetBkColor(hdcText, RGB(255, 255, 255));
            SetTextAlign(hdcText, GetTextAlign(hdcText) & (~TA_CENTER));
            return  (INT_PTR)CreateSolidBrush(RGB(255, 255, 255));
        }
        
        case WM_COMMAND:

        {
            if (((HWND)lParam) && (HIWORD(wParam) == BN_CLICKED))
            {
                int iMID;
                iMID = LOWORD(wParam);
                switch (iMID)
                {
                    case ID_OP_BUTTON:
                    {

                        HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
                        if (SUCCEEDED(hr))
                        {
                            IFileOpenDialog* pFileOpen;

                            // Create the FileOpenDialog object.
                            hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL,
                                IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

                            if (SUCCEEDED(hr))
                            {
                                // Show the Open dialog box.
                                hr = pFileOpen->Show(NULL);

                                // Get the file name from the dialog box.
                                if (SUCCEEDED(hr))
                                {
                                    IShellItem* pItem;
                                    hr = pFileOpen->GetResult(&pItem);
                                    if (SUCCEEDED(hr))
                                    {

                                        hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

                                        // Display the file name to the user.
                                        if (SUCCEEDED(hr))
                                        {
                                            MessageBoxW(NULL, (LPCWSTR)pszFilePath, L"File Path", MB_OK);
                                            SetWindowText(hWndLink, (LPCWSTR)pszFilePath);

                                            hFile = CreateFile(pszFilePath,
                                                GENERIC_READ,
                                                FILE_SHARE_READ,
                                                NULL,
                                                OPEN_EXISTING,
                                                FILE_ATTRIBUTE_NORMAL,
                                                NULL);
                                            if (hFile == INVALID_HANDLE_VALUE)
                                            {
                                                MessageBoxW(NULL,L"Create file fail", L"File Path", MB_OK);
                                            }
                                            CoTaskMemFree(pszFilePath);
                                        }
                                        pItem->Release();
                                    }
                                }
                                pFileOpen->Release();
                            }
                            CoUninitialize();
                        }
                        return 0; 
                    }

                    case ID_BOOT_BUTTON:
                    {
                       /* unsigned long fileSize = GetFileSize(pszFilePath, &fileSize);
                        static unsigned char* hexdata = (unsigned char*)malloc(sizeof(unsigned char) * fileSize);*/
                       EnableWindow(GetDlgItem(hwnd, ID_BOOT_BUTTON), false); // to disable button or other control
                       WriteFile(hComm,startSend, 8, NULL, NULL);            // send signal starting boott

                       int status = 0;
                       int total = 0;
                       do
                       {
                           int length_line = 0;
                           int length_data = 0;

                           status = getLine(hFile, buffer_line, &length_line);
                          /* memcpy(hexdata + total, buffer_line, length_line);
                           total += length_line;*/
                           SetWindowTextA(hexTxtBox,(LPCSTR)buffer_line);
                           
                           length_data = length_line - NOT_IS_DATA;
                           unsigned char* data = (unsigned char*)malloc(sizeof(unsigned char) * length_data);
                           if (lineFilter(buffer_line, data, &length_data))
                           {
                              /* SetWindowTextA(dataTxtBox, (LPCSTR)data);*/
                               WriteFile(hComm,        // Handle to the Serial port
                                   data,     // Data to be written to the port
                                   length_data,  //No of bytes to write
                                   NULL, //Bytes written
                                   NULL);
                           }
                           
                           free(data);
                       } while (status);
                      
                       WriteFile(hComm,stopSend, 8, NULL, NULL);
                       MessageBox(hwnd, L"Send OK", L"COMPORT", MB_OK);
                       EnableWindow(GetDlgItem(hwnd,ID_BOOT_BUTTON), true);  // to enable button or other control
                        return 0;
                    }

                    case ID_OK_BUTTON:
                    {
                        hComm = CreateFileA((LPCSTR)COMx,           //port name
                            GENERIC_READ | GENERIC_WRITE,   //Read/Write
                            0,              // No Sharing
                            NULL,           // No Security
                            OPEN_EXISTING,  // Open existing port only
                            0,              // Non Overlapped I/O
                            NULL);          // Null for Comm Devices

                        sel = SendMessage(hwndBaud, CB_GETCURSEL, 0, 0);
                        dcb.BaudRate =  9600;    //  baud rate
                        dcb.ByteSize = 8;                   //  data size, xmit and rcv
                        dcb.Parity = NOPARITY;              //  parity bit
                        dcb.StopBits = ONESTOPBIT;          //  stop bit
                        SetCommState(hComm,&dcb);
                        MessageBox(hwnd,(LPCWSTR)dcb.BaudRate, L"COMPORT", MB_OK);

                        if (hComm == INVALID_HANDLE_VALUE)  /* NO COM port */
                            MessageBox(hwnd, L"CONNECTION FAILED", L"COMPORT", MB_OK);
                        else        // Have a COM connection
                            MessageBox(hwnd, L"CONNECTED", L"COMPORT", MB_OK);
                        return 0;
                    }  
                    case ID_END_BUTTON:
                    {
                         CloseHandle(hComm);     //Closing the Serial Port
                         CloseHandle(hFile);    //closing the file handle
                         MessageBox(hwnd, L"DISCONNECTED", L"COMPORT", MB_OK);
                    }
                    default:
                        return 0;
                }
               
            }
            
            /* Check commboBox */
            if (HIWORD(wParam) == CBN_SELCHANGE)
            {
                switch (LOWORD(wParam))
                {
                    /*Check combobox to choose COM_port */
                case COM_ID:
                    sel = SendMessage(hwndCOM, CB_GETCURSEL, 0, 0);
                    portnum = sel + 3;
                    /* Handles COM port connection */
                    for (int i = 0; i < 20; i++)
                    {
                        COMx[i] = '\0';
                    }
                    sprintf(COMx, "\\\\.\\COM%u", portnum);
                }
                /*---------------------------------------------------------------*/
            }
            break;
            return 0;
        }

        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return TRUE;
   // return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

