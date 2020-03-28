// flat_white_gdi.cpp : 定义应用程序的入口点。
//

#include <thread>
#include <vector>
#include <deque>
#include <iostream>
#include <algorithm>

#include "framework.h"
#include "flat_white_gdi.h"
#include "five.h"

#define MAX_LOADSTRING 100

// 全局变量:
HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名

HWND hWnd__;
std::deque<POINT> line__;
POINT thinking__ { -1, -1 };
int counter__ = 0;

// 此代码模块中包含的函数的前向声明:
ATOM             MyRegisterClass(HINSTANCE hInstance);
BOOL             InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow) {
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // 初始化全局字符串
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_FLATWHITEGDI, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 执行应用程序初始化:
    if (!InitInstance (hInstance, nCmdShow)) {
        return FALSE;
    }

    five_prepare([](void*, std::size_t t, st_info* l, std::size_t s) {
        std::cout << ++counter__ << " - " << t << std::endl;
        if ((l != nullptr) && (s > 0)) {
            std::sort(l, l + s, [](st_info const & x, st_info const & y) {
                return x.visits_ > y.visits_;
            });
            thinking__.x = l[0].x_;
            thinking__.y = l[0].y_;
        }
        else thinking__ = { -1, -1 };

        RECT rect;
        GetClientRect(hWnd__, &rect);
        InvalidateRect(hWnd__, &rect, TRUE);
    }, unsigned(-1), 30, nullptr);

    five_start_game();

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_FLATWHITEGDI));

    MSG msg;

    // 主消息循环:
    while (GetMessage(&msg, nullptr, 0, 0)) {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}

int main() {
    return wWinMain(GetModuleHandle(nullptr), nullptr, nullptr, SW_SHOW);
}

//
//  函数: MyRegisterClass()
//
//  目标: 注册窗口类。
//
ATOM MyRegisterClass(HINSTANCE hInstance) {
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_FLATWHITEGDI));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = nullptr;
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目标: 保存实例句柄并创建主窗口
//
//   注释:
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow) {
   hInst = hInstance; // 将实例句柄存储在全局变量中

   HWND hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, 800, 600, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd) {
      return FALSE;
   }
   hWnd__ = hWnd;

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

void StartThinking(unsigned x, unsigned y) {
    counter__ = 0;
    std::thread([x, y]() mutable {
        auto s = five_calc_next(&x, &y);
        std::cout << "score: " << s << std::endl;
        if (s > 0) {
            std::cout << "x: " << x << ", y: " << y << std::endl;
            line__.push_back({ (LONG)x, (LONG)y });
        }
        thinking__ = { -1, -1 };

        RECT rect;
        GetClientRect(hWnd__, &rect);
        InvalidateRect(hWnd__, &rect, TRUE);
    }).detach();
}

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目标: 处理主窗口的消息。
//
//  WM_COMMAND  - 处理应用程序菜单
//  WM_PAINT    - 绘制主窗口
//  WM_DESTROY  - 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_GETMINMAXINFO: {
            MINMAXINFO * mminfo = (PMINMAXINFO)lParam;
            mminfo->ptMinTrackSize.x = 800;
            mminfo->ptMinTrackSize.y = 600;
        }
        break;
    case WM_LBUTTONDBLCLK:
        if ((thinking__.x < 0) || (thinking__.y < 0)) {
            RECT rect;
            GetClientRect(hWnd, &rect);

            LONG w = min(rect.right - rect.left, rect.bottom - rect.top);
            LONG k = w / 15;
            LONG x = LOWORD(lParam);
            LONG y = HIWORD(lParam);

            POINT pt {
                (x - (rect.right - rect.left - w + 1) / 2) / k, 
                (y - (rect.bottom - rect.top - w + 1) / 2) / k
            };
            if ((pt.x < 0) || (pt.x > 14) || (pt.y < 0) || (pt.y > 14)) break;
            int i = 0;
            for (; i < (int)line__.size(); ++i) {
                if (std::memcmp(&line__[i], &pt, sizeof(POINT)) == 0) break;
            }
            if (i == (int)line__.size()) {
                line__.push_back(pt);
                thinking__ = pt;
            }

            InvalidateRect(hWnd, &rect, TRUE);
            StartThinking((unsigned)thinking__.x, (unsigned)thinking__.y);
        }
        break;
    case WM_RBUTTONDBLCLK: {
            RECT rect;
            GetClientRect(hWnd, &rect);

            LONG w = min(rect.right - rect.left, rect.bottom - rect.top);
            LONG k = w / 15;
            LONG x = LOWORD(lParam);
            LONG y = HIWORD(lParam);

            POINT pt {
                (x - (rect.right - rect.left - w + 1) / 2) / k, 
                (y - (rect.bottom - rect.top - w + 1) / 2) / k
            };
            if ((pt.x < 0) || (pt.x > 14) || (pt.y < 0) || (pt.y > 14)) {
                five_start_game();
                thinking__ = { -1, -1 };
                line__.clear();
                InvalidateRect(hWnd, &rect, TRUE);
            }
            else if ((thinking__.x < 0) || (thinking__.y < 0)) {
                thinking__ = {};
                StartThinking((unsigned)-1, (unsigned)-1);
            }
        }

    case WM_ERASEBKGND:
        break;
    case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);

            RECT rect;
            GetClientRect(hWnd, &rect);

            LONG w = min(rect.right - rect.left, rect.bottom - rect.top);
            LONG k = w / 15;

            HDC     memdc = CreateCompatibleDC(hdc);
            HBITMAP membp = CreateCompatibleBitmap(hdc, rect.right - rect.left, rect.bottom - rect.top);
            SelectObject(memdc, membp);
            // FillSolidRect
            SetBkColor(memdc, RGB(255, 255, 255));
            ExtTextOut(memdc, 0, 0, ETO_OPAQUE, &rect, nullptr, 0, nullptr);

            auto coord_x = [&rect, w, k](LONG c) { return (rect.right - rect.left - w + k) / 2 + k * c; };
            auto coord_y = [&rect, w, k](LONG c) { return (rect.bottom - rect.top - w + k) / 2 + k * c; };

            auto circle = [memdc, w, k](LONG x, LONG y, LONG r, COLORREF frame, COLORREF clr) {
                HPEN    hpn     = CreatePen(PS_SOLID, 1, frame);
                HGDIOBJ old_hpn = SelectObject(memdc, hpn);
                HBRUSH  hbr     = CreateSolidBrush(clr);
                HGDIOBJ old_hbr = SelectObject(memdc, hbr);

                Ellipse(memdc, x - r, y - r, x + r, y + r);

                SelectObject(memdc, old_hbr);
                SelectObject(memdc, old_hpn);
                DeleteObject(hbr);
            };

            auto text = [memdc](LONG x, LONG y, LPCTSTR text) {
                SIZE sz {};
                GetTextExtentExPoint(memdc, text, (int)_tcslen(text), 0, nullptr, nullptr, &sz);
                TextOut(memdc, x - sz.cx / 2, y - sz.cy / 2, text, (int)_tcslen(text));
            };

            /* draw lines */

            for (int i = 0; i < 15; ++i) {
                MoveToEx(memdc, coord_x( 0), coord_y( i), nullptr);
                LineTo  (memdc, coord_x(14), coord_y( i));
                MoveToEx(memdc, coord_x( i), coord_y( 0), nullptr);
                LineTo  (memdc, coord_x( i), coord_y(14));
            }

            /* draw stars */

            circle(coord_x( 7), coord_y( 7), 5, RGB(0, 0, 0), RGB(0, 0, 0));
            circle(coord_x( 3), coord_y( 3), 4, RGB(0, 0, 0), RGB(0, 0, 0));
            circle(coord_x(11), coord_y( 3), 4, RGB(0, 0, 0), RGB(0, 0, 0));
            circle(coord_x( 3), coord_y(11), 4, RGB(0, 0, 0), RGB(0, 0, 0));
            circle(coord_x(11), coord_y(11), 4, RGB(0, 0, 0), RGB(0, 0, 0));

            /* draw coordinate texts */

            TCHAR buf[3] {};
            for (int i = 0; i < 15; ++i) {
                buf[0] = _T('A' + i);
                buf[1] = _T('\0');
                text(coord_x(i), coord_y(0) - 10, buf);
                _itot_s(i + 1, buf, 10);
                text(coord_x(0) - 10, coord_y(i), buf);
            }

            /* draw pieces */

            for (int i = 0; i < (int)line__.size(); ++i) {
                POINT const & pt = line__[i];
                circle(coord_x(pt.x), coord_y(pt.y), (k / 2) - 1, RGB(0, 0, 0), 
                       (i & 1) ? RGB(255, 255, 255) : RGB(0, 0, 0));
            }
            if (!line__.empty()) {
                POINT const & pt = line__.back();
                circle(coord_x(pt.x), coord_y(pt.y), 4, RGB(0, 0, 0), 
                       (line__.size() & 1) ? RGB(255, 255, 255) : RGB(0, 0, 0));
            }

            /* thinking... */

            if ((thinking__.x >= 0) && (thinking__.y >= 0)) {
                circle(coord_x(thinking__.x), coord_y(thinking__.y), 3, RGB(0, 0, 0), RGB(255, 0, 0));
            }

            BitBlt(hdc, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, memdc, 0, 0, SRCCOPY);
            DeleteObject(membp);
            DeleteDC(memdc);
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        five_start_game();
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
