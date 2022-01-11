#include "pch.h"
#include "resource.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

using namespace winrt;
using namespace winrt::Microsoft::UI::Xaml::Hosting;

using namespace winrt::Microsoft::UI::Xaml;
using namespace winrt::Microsoft::UI::Xaml::Controls;


// Interop Win32 focus with WinUI focus
static guid m_lastFocusRequestId{};
static std::vector<DesktopWindowXamlSource::TakeFocusRequested_revoker> m_takeFocusEventRevokers{};

// List of XAML islands
static std::vector<DesktopWindowXamlSource> m_xamlSources{ };

// Xaml Island desktop XAML Source
static DesktopWindowXamlSource _desktopWindowXamlSource{ nullptr };
// XAML Manager
static WindowsXamlManager _winxamlmanager{ nullptr };

// WinUI Button
Button _xamlButton{ nullptr };
StackPanel _stack{ nullptr };


// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_ISLANDSAMPLEWINUI3, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_ISLANDSAMPLEWINUI3));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int)msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ISLANDSAMPLEWINUI3));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_ISLANDSAMPLEWINUI3);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance; // Store instance handle in our global variable

    // Initialize XAML Manager and DesktopWindowXamlSource

    init_apartment(winrt::apartment_type::single_threaded);

    _winxamlmanager = WindowsXamlManager::InitializeForCurrentThread();
    _desktopWindowXamlSource = DesktopWindowXamlSource{};

    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
    {
        return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
    {
        HWND _hWndXamlIsland;
        com_ptr<IDesktopWindowXamlSourceNative> interop = _desktopWindowXamlSource.as<IDesktopWindowXamlSourceNative>();
        // Get handle to corewindow
        // Parent the DesktopWindowXamlSource object to current window
        check_hresult(interop->AttachToWindow(hWnd));
        // This Hwnd will be the window handler for the Xaml Island: A child window that contains Xaml.

        // Get the new child window's hwnd
        interop->get_WindowHandle(&_hWndXamlIsland);

        RECT windowRect;

        ::GetWindowRect(hWnd, &windowRect);
        ::SetWindowPos(_hWndXamlIsland, NULL, 20, 20, 400, 200, SWP_SHOWWINDOW);

        LONG l = ::GetWindowLong(_hWndXamlIsland, GWL_STYLE);
        ::SetWindowLong(_hWndXamlIsland, GWL_STYLE, l | WS_TABSTOP);

        auto comboBox = ComboBox();
        comboBox.Items().Append(winrt::box_value(L"Item 1"));
        comboBox.Items().Append(winrt::box_value(L"Item 2"));

        _stack = StackPanel();
        auto collection = _stack.Children();
        collection.Append(comboBox);

        _stack.RequestedTheme(ElementTheme::Dark);

        _desktopWindowXamlSource.Content(_stack);
        m_xamlSources.push_back(_desktopWindowXamlSource);
    }
    break;
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // Parse the menu selections:
        switch (wmId)
        {
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        // TODO: Add any drawing code that uses hdc here...
        RECT rc;
        GetClientRect(hWnd, &rc);
        SetDCBrushColor(hdc, RGB(212, 212, 220));
        FillRect(hdc, &rc, (HBRUSH)GetStockObject(DC_BRUSH));
        EndPaint(hWnd, &ps);
    }
    break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
