#include<windows.h>
#include<math.h>
#include <time.h>
#include <stdbool.h>

#define n1 2
#define n2 1
#define n3 0
#define n4 8
#define edges (n3 + 10)

bool drawArrows = false;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

char caption[] = "Lab 3";

void arrow(float fi, int px, int py, HDC hdc) {
    {
        fi = fi - (float)M_PI;
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
    int lastX = 500, lastY  = 500, incX = 100, incY = 0, index = 0, temp;

    for (int i = 0; i < 4; i++) {
        if (i == 3) {
            incY = (int)((float)(incY * sideLength) / (float)(noCenter - sideLength * 3));
            sideLength = sideLength + noCenter - sideLength * 4;
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
    int sl = (int)floor(noCenter * 0.25);
    x[index] = (x[0] + x[sl]) / 2;
    y[index] = (y[0] + y[sl * 2]) / 2;
}

void randM(float **matrix, int n) {
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            matrix[i][j] = (float)rand() / RAND_MAX * 2;
}

void mulM(float **matrix, int n) {

    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++) {
            matrix[i][j] = (float)((matrix[i][j] * (1.0f - n3 * 0.02f - n4 * 0.005f - 0.25f)) > 1.0f);
        }
}

float getAngle(int startX, int endX, int startY, int endY) {
    int vx = endX - startX;
    int vy = endY - startY;
    float len = sqrtf((float)(vx * vx + vy * vy));
    return acosf((float)vx / len) * (float)(vy < 0 ? -1 : 1);
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
            char *nn[edges + max(0, edges - 9)] = {"1", "2", "3", "4", "5", "6", "7", "8", "9", "1", "0"};
            int nx[edges];
            int ny[edges];
            int dx = 16, dy = 16, dtx = 5;

            generateList(nx, ny);

            float matrix[edges][edges];
            float *matrixPtr[edges];
            for (int i = 0; i < edges; i++) {
                matrixPtr[i] = matrix[i];
            }

            srand(n1 * 1000 + n2 *100 + n3 *10 + n4);
            //srand(time(NULL));
            randM(&matrixPtr[0], edges);
            mulM(&matrixPtr[0], edges);

            HPEN BPen = CreatePen(PS_SOLID, 2, RGB(50, 0, 255));
            HPEN KPen = CreatePen(PS_SOLID, 1, RGB(20, 20, 5));

            SelectObject(hdc, KPen);

            for (int i = 0; i < edges; i++)
                for (int j = 0; j < edges; j++) {
                    if (matrix[i][j] == 1.0f) {
                        MoveToEx(hdc, nx[i], ny[i], NULL);
                        if (i == j) {
                            AngleArc(hdc, nx[i] - dx * 2, ny[i], dx * 2, 0, 359);
                            continue;
                        }
                        if (matrix[i][j] == matrix[i][j] && i > j && !drawArrows) continue;
                        bool arc = false;
                        float angle = getAngle(nx[i], nx[j], ny[i], ny[j]);
                        int destX = nx[j];
                        int destY= ny[j] - dx * (ny[i] < ny[j]) + dx * (ny[i] > ny[j]);
                        if (destY == ny[j]) destX = nx[j] - dy * (nx[i] < nx[j]) + dy * (nx[i] > nx[j]);

                        for (int el = 0; el < edges; el++) {
                            if (getAngle(nx[i], nx[el], ny[i], ny [el])
                            == getAngle(nx[el], nx[j], ny[el], ny[j]))
                                arc = true;
                        }
                        if (!arc) {
                            LineTo(hdc, destX, destY);
                        }
                        else {
                            int halfX = (int)((float)(nx[i] + nx[j]) * 0.5f) + dx * (ny[i] != ny[j]) * 2;
                            int halfY = (int)((float)(ny[i] + ny[j]) * 0.5f) + dx * (nx[i] != nx[j]) * 2;
                            angle = getAngle(halfX, destX, halfY, destY);

                            LineTo(hdc, halfX, halfY);
                            LineTo(hdc, destX, destY);
                        }
                        if (drawArrows) arrow(angle, destX, destY, hdc);
                    }
                }

            SelectObject(hdc, BPen);

            int count = 0;
            for(int i=0;i<edges;i++){
                Ellipse(hdc, nx[i]-dx,ny[i]-dy,nx[i]+dx,ny[i]+dy);
                if (i < 9) TextOut(hdc, nx[i]-dtx,ny[i]-dy/2, nn[count],1);
                else {
                    TextOut(hdc, nx[i] - dtx * 2, ny[i] - dy / 2, nn[count], 1);
                    count++;
                    TextOut(hdc, nx[i], ny[i] - dy / 2, nn[count], 1);
                }
                count++;
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




