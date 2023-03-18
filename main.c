#include<windows.h>
#include<math.h>
#include <stdio.h>

#define n1 2
#define n2 1
#define n3 0
#define n4 8
#define edges (n3 + 10)

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

char caption[] = "Lab 3";

void arrow(float fi, int px, int py, HDC hdc) {
    {
        fi = 3.1416f * (180.0f - fi) / 180.0f;
        int lx, ly, rx, ry;
        lx = (int)(px + 15 * cos(fi + 0.3));
        rx = (int)(px + 15 * cos(fi - 0.3));
        ly = (int)(py + 15 * sin(fi + 0.3));
        ry = (int)(py + 15 * sin(fi - 0.3));

        MoveToEx(hdc, lx, ly, NULL);
        LineTo(hdc, px, py);
        LineTo(hdc, rx, ry);
    }
}

void generateList(int *x, int *y) {
    int noCenter = edges - 1;
    int sideLength = (int)floor(noCenter * 0.25);
    int lastX = 200, lastY  = 200, incX = 200, incY = 0, index = 0, temp;

    for (int i = 0; i < 4; i++) {
        if (i == 3) {
            incY *= sideLength / noCenter;
            sideLength = noCenter;
        }
        for (int j = 0; j < sideLength; j++) {
            x[index] = lastX;
            y[index] = lastY;
            lastX += incX;
            lastY += incY;
            index++;
        }
        temp = -incX;
        incX = incY;
        incY = temp;
    }

    x[index] = (incX + x[sideLength - 1]) / 2;
    y[index] = x[index];
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE
hPrevInstance, LPSTR lpszCmdLine, int nCmdShow) {
    WNDCLASS w;
    w.lpszClassName=caption;
    w.hInstance=hInstance;
    w.lpfnWndProc=WndProc;
    w.hCursor=LoadCursor(NULL, IDC_ARROW);
    w.hIcon=0;
    w.lpszMenuName=0;
    w.hbrBackground = (HBRUSH) BLACK_BRUSH;
    w.style=CS_HREDRAW|CS_VREDRAW;
    w.cbClsExtra=0;
    w.cbWndExtra=0;

    if(!RegisterClass(&w))
        return 0;

    HWND hWnd;
    MSG lpMsg;
    hWnd=CreateWindow(caption,
                      "Lab 3 by Diachok Maxim",
                      WS_OVERLAPPEDWINDOW,
                      100,
                      100,
                      460,
                      240,
                      (HWND)NULL,
                      (HMENU)NULL,
                      (HINSTANCE)hInstance,
                      (HINSTANCE)NULL);

    ShowWindow(hWnd, nCmdShow);

    while(GetMessage(&lpMsg, hWnd, 0, 0)) {
        TranslateMessage(&lpMsg);
        DispatchMessage(&lpMsg);
    }
    return (int)lpMsg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT messg,WPARAM wParam, LPARAM lParam) {
    HDC hdc;
    PAINTSTRUCT ps;


    switch (messg) {
        case WM_PAINT :
            hdc = BeginPaint(hWnd, &ps);
            char *nn[3][2] = {"1", "2", "3"};
            int nx[3] = {100, 200, 300};
            int ny[3] = {100, 100, 100};
            int dx = 16, dy = 16, dtx = 5;
            int i;

            //generateList(nx, ny);

            HPEN BPen = CreatePen(PS_SOLID, 2, RGB(50, 0, 255));
            HPEN KPen = CreatePen(PS_SOLID, 1, RGB(20, 20, 5));
            SelectObject(hdc, KPen);

            MoveToEx(hdc, nx[0], ny[0], NULL);
            LineTo(hdc, nx[1], ny[1]);
            arrow(0.0f, nx[1] - dx, ny[1], hdc);

            Arc(hdc, nx[0], ny[0]-40, nx[2], ny[2]+40, nx[2], ny[2], nx[0], ny[0]);
            arrow(-45.0f,(int)((float)nx[2]-(float)dx*0.5f),(int)((float)ny[2]-(float)dy*0.8f), hdc);

            SelectObject(hdc, BPen);
            for(i=0;i<=2;i++){
                Ellipse(hdc, nx[i]-dx,ny[i]-dy,nx[i]+dx,ny[i]+dy);
                TextOut(hdc, nx[i]-dtx,ny[i]-dy/2, nn[i][0],1);
            }
            EndPaint(hWnd, &ps);
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default:
            return(DefWindowProc(hWnd, messg, wParam, lParam));
    }
}




