#include<windows.h>
#include<math.h>
#include <time.h>
#include <stdbool.h>
#include <float.h>
#include <stdio.h>

#define n1 2
#define n2 1
#define n3 0
#define n4 8
#define edgesCount (n3 + 10)

#define rCol rand() % 360


bool directed = true;
bool modified = false;
bool cond = false;
int outState = 0;

int edges;

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

void generateList(int *x, int *y, int n) {
    int noCenter = n - 1;
    int sideLength = (int)floor(noCenter * 0.25);
    int lastX = 700, lastY  = 500, incX = 100, incY = 0, index = 0, temp;

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

void fillNums(char *arr, int len) {
    char *ptr = arr;
    for (int i = 0; i < len; i++) {
        sprintf(ptr, "%i", i + 1);
        ptr += strlen(ptr);
    }
}

void randM(float **matrix, int n) {
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            matrix[i][j] = (float)rand() / RAND_MAX * 2;
}

void mulM(float **matrix, int n, float mul) {

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (matrix[i][j] == 3.0f) {
                matrix[i][j] = 1.0f;
                continue;
            }
            matrix[i][j] = (float) (matrix[i][j] * mul > 1.0f);
            if (matrix[i][j] == 1.0f && !directed) {
                matrix[j][i] = i < j ? 3.0f : 1.0f;
            }
        }
    }
}

int maxInRow(int **arr, int n) {
    int max = 0;

    for (int i = 0; i < n; i++)
        if (arr[0][i] > max) max = arr[0][i];

    return max;
}

void setM(int **matrix, float **source, int n) {
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            matrix[i][j] = (int)source[i][j];
}

void setMInt(int **matrix, int **source, int n) {
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            matrix[i][j] = source[i][j];
}

void addM(int **matrix, int **source, int n) {
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            matrix[i][j] += source[i][j];
}

void transposeM(int **matrix, int n) {
    int temp;
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++) {
            temp = matrix[i][j];
            matrix[i][j] = matrix[j][i];
            matrix[j][i] = temp;
        }
}

void multiplyM(int **matrix, int **source, int n) {
    int matrixCopy[n][n];
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            matrixCopy[i][j] = matrix[i][j];

    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++) {
            matrix[i][j] = 0;
            for (int k = 0; k < n; k++)
                matrix[i][j] += matrixCopy[i][k] * source[k][j];
        }
}

void findWays(int **matrix, int n, int min, int length) {
    min--;

    int helpM[n][n];
    int *helpPtr[n];
    for (int i = 0; i < n; i++) {
        helpPtr[i] = helpM[i];
    }

    int helpMS[n][n];
    int *helpSPtr[n];
    for (int i = 0; i < n; i++) {
        helpSPtr[i] = helpMS[i];
    }

    setMInt(&helpSPtr[0], matrix, n);
    setMInt(&helpPtr[0], &helpSPtr[0], n);

    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            matrix[i][j] = 0;

    for (int i = 0; i <= length; i++) {
        if (i >= min) addM(matrix, &helpPtr[0], n);
        multiplyM(&helpPtr[0], &helpSPtr[0], n);
    }

    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            matrix[i][j] = matrix[i][j] > 0;
}

void findSame(int **matrix, int n) {
    int helpM[n][n];
    int *helpPtr[n];
    for (int i = 0; i < n; i++) {
        helpPtr[i] = helpM[i];
    }

    setMInt(&helpPtr[0], matrix, n);

    int *m = matrix[0];

    for (int i = 0; i < n; i++)
        m[i] = 0;

    bool same;
    int num = 0;
    for (int row = 0; row < n; row++) {
        if (m[row] != 0) continue;
        m[row] = ++num;
        for (int row2 = row + 1; row2 < n; row2++) {
            if (m[row2] != 0) continue;
            same = true;
            for (int j = 0; j < n; j++)
                if (helpM[row][j] != helpM[row2][j]) {
                    same = false;
                    break;
                }
            if (same) m[row2] = num;
        }
    }
}

void linkM(float **matrix, int **outM, int **helpM, int n) {

    setM(outM, matrix, n);
    findWays(outM, n, 1, n - 1);

    for (int i = 0; i < n; i++)
        outM[i][i] = 1;

    setMInt(helpM, outM, n);

    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            outM[i][j] *= helpM[j][i];

    findSame(outM, n);
}

void showM(int **matrix, int n, int m, HDC hdc) {
    int curX = 350, curY = 350, incY = 20, incX = 15;
    char symbol[2];
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            sprintf(symbol, "%i", matrix[i][j]);
            TextOut(hdc, curX + j * incX, curY, symbol, 1);
        }
        curY += incY;
    }
}

float getAngle(int startX, int endX, int startY, int endY) {
    int vx = endX - startX;
    int vy = endY - startY;
    float len = sqrtf((float)(vx * vx + vy * vy));
    return acosf((float)vx / len) * (float)(vy < 0 ? -1 : 1);
}

void rotate(float angle, int r, int *arr) {
    int x = (int)((float)r * cosf(angle));
    int y = (int)((float)r * sinf(angle));
    arr[0] = x;
    arr[1] = y;

}

HPEN color(int c) {
    srand(c);
    return CreatePen(PS_SOLID, 2, RGB(rCol, rCol, rCol));
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
                      "Lab 4 by Diachok Maxim",
                      WS_OVERLAPPEDWINDOW,
                      100,
                      100,
                      1000,
                      650,
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
    HINSTANCE hInst;

    static HWND hBtnToDir, hBtnToUnDir, hBtnAdjGraph, hBtnPow, hBtnWays, hBtnLinked, hBtnModify, hBtnSL, hBtnSLM,
    hBtnLG, hBtnIso, hBtnHang;

    switch (messg) {
        case WM_CREATE : {
            hInst = ((LPCREATESTRUCT) lParam)->hInstance;

            hBtnToDir = CreateWindow("button", "To directed",
                                     WS_CHILD | WS_VISIBLE | WS_BORDER,
                                     200, 100, 120, 30, hWnd, 0, hInst, NULL);
            ShowWindow(hBtnToUnDir, SW_SHOWNORMAL);

            hBtnToUnDir = CreateWindow("button", "To undirected",
                                       WS_CHILD | WS_VISIBLE | WS_BORDER,
                                       200, 130, 120, 30, hWnd, 0, hInst, NULL);
            ShowWindow(hBtnToDir, SW_SHOWNORMAL);

            hBtnAdjGraph = CreateWindow("button", "Adjustency matrix",
                                        WS_CHILD | WS_VISIBLE | WS_BORDER,
                                        350, 70, 120, 30, hWnd, 0, hInst, NULL);
            ShowWindow(hBtnAdjGraph, SW_SHOWNORMAL);

            hBtnPow = CreateWindow("button", "Powers",
                                   WS_CHILD | WS_VISIBLE | WS_BORDER,
                                   350, 100, 120, 30, hWnd, 0, hInst, NULL);
            ShowWindow(hBtnPow, SW_SHOWNORMAL);

            hBtnWays = CreateWindow("button", "2/3step ways",
                                    WS_CHILD | WS_VISIBLE | WS_BORDER,
                                    350, 130, 120, 30, hWnd, 0, hInst, NULL);
            ShowWindow(hBtnWays, SW_SHOWNORMAL);

            hBtnLinked = CreateWindow("button", "All connections",
                                      WS_CHILD | WS_VISIBLE | WS_BORDER,
                                      350, 160, 120, 30, hWnd, 0, hInst, NULL);
            ShowWindow(hBtnLinked, SW_SHOWNORMAL);

            hBtnSL = CreateWindow("button", "Strong links",
                                  WS_CHILD | WS_VISIBLE | WS_BORDER,
                                  350, 190, 120, 30, hWnd, 0, hInst, NULL);
            ShowWindow(hBtnSL, SW_SHOWNORMAL);

            hBtnSLM = CreateWindow("button", "Links matrix",
                                  WS_CHILD | WS_VISIBLE | WS_BORDER,
                                  350, 220, 120, 30, hWnd, 0, hInst, NULL);
            ShowWindow(hBtnSLM, SW_SHOWNORMAL);

            hBtnIso = CreateWindow("button", "Isolated",
                                   WS_CHILD | WS_VISIBLE | WS_BORDER,
                                   350, 250, 120, 30, hWnd, 0, hInst, NULL);
            ShowWindow(hBtnIso, SW_SHOWNORMAL);

            hBtnHang = CreateWindow("button", "Hanging",
                                   WS_CHILD | WS_VISIBLE | WS_BORDER,
                                   350, 280, 120, 30, hWnd, 0, hInst, NULL);
            ShowWindow(hBtnHang, SW_SHOWNORMAL);

            hBtnModify = CreateWindow("button", "Modify",
                                      WS_CHILD | WS_VISIBLE | WS_BORDER,
                                      50, 100, 120, 30, hWnd, 0, hInst, NULL);
            ShowWindow(hBtnModify, SW_SHOWNORMAL);

            hBtnLG = CreateWindow("button", "Linkage graph",
                                  WS_CHILD | WS_VISIBLE | WS_BORDER,
                                  200, 160, 120, 30, hWnd, 0, hInst, NULL);
            ShowWindow(hBtnLG, SW_SHOWNORMAL);
        }

        case WM_PAINT : {
            hdc = BeginPaint(hWnd, &ps);

            edges = edgesCount;

            char nn[edges + max(0, edges - 9)];
            int nx[edges];
            int ny[edges];
            int dx = 16, dy = 16, dtx = 5;

            fillNums(nn, edges);
            generateList(nx, ny, edges);

            float matrix[edges][edges];
            float *matrixPtr[edges];
            for (int i = 0; i < edges; i++) {
                matrixPtr[i] = matrix[i];
            }

            int outM[edges][edges];
            int *outPtr[edges];
            for (int i = 0; i < edges; i++) {
                outPtr[i] = outM[i];
            }

            srand(n1 * 1000 + n2 * 100 + n3 * 10 + n4);
            randM(&matrixPtr[0], edges);

            mulM(&matrixPtr[0], edges,
                 modified ? 1.0f - n3 * 0.005f - n4 * 0.005f - 0.27f : 1.0f - n3 * 0.01f - n4 * 0.01f - 0.3f);

            HPEN BPen = CreatePen(PS_SOLID, 2, RGB(50, 0, 255));
            HPEN KPen = CreatePen(PS_SOLID, 1, RGB(20, 20, 5));

            if (cond) {
                int helpM[edges][edges];
                int *helpPtr[edges];
                for (int i = 0; i < edges; i++) {
                    helpPtr[i] = helpM[i];
                }

                linkM(&matrixPtr[0], &outPtr[0], &helpPtr[0], edges);

                edges = maxInRow(&outPtr[0], edges);

                for (int i = 0; i < edgesCount; i++)
                    for (int j = 0; j < edgesCount; j++)
                        matrix[outM[0][i] - 1][outM[0][j] - 1] =
                                (float) (helpM[i][j] && outM[0][i] != outM[0][j]);
            }

            switch (outState) {
                case 0 :
                    setM(&outPtr[0], &matrixPtr[0], edges);
                    showM(&outPtr[0], edges, edges, hdc);
                    break;
                case 1 :
                    for (int i = 0; i < edges; i++) {
                        outM[0][i] = 0;
                        for (int j = 0; j < edges; j++)
                            if (matrix[j][i] == 1.0f)
                                outM[0][i]++;
                    }
                    for (int i = 0; i < edges; i++) {
                        outM[1][i] = 0;
                        for (int j = 0; j < edges; j++)
                            if (matrix[i][j] == 1.0f)
                                outM[1][i]++;
                    }

                    if (!directed) {
                        showM(&outPtr[0], 2, edges, hdc);
                        break;
                    }

                    showM(&outPtr[0], 2, edges, hdc);
                    TextOut(hdc, 270, 350, "Entries", 7);
                    TextOut(hdc, 270, 370, "Outs", 4);
                    break;
                case 2 :
                    setM(&outPtr[0], &matrixPtr[0], edges);
                    findWays(&outPtr[0], edges, 2, 3);
                    showM(&outPtr[0], edges, edges, hdc);
                    break;
                case 3 :
                    setM(&outPtr[0], &matrixPtr[0], edges);
                    findWays(&outPtr[0], edges, 1, edges - 1);
                    showM(&outPtr[0], edges, edges, hdc);
                    break;
                case 4 :{
                    int helpM[edges][edges];
                    int *helpPtr[edges];
                    for (int i = 0; i < edges; i++) {
                        helpPtr[i] = helpM[i];
                    }

                    linkM(&matrixPtr[0], &outPtr[0], &helpPtr[0], edges);

                    showM(&outPtr[0], 1, maxInRow(&outPtr[0], edges), hdc);
                }
                    break;
                case 5 : {
                    int helpM[edges][edges];
                    int *helpPtr[edges];
                    for (int i = 0; i < edges; i++) {
                        helpPtr[i] = helpM[i];
                    }

                    linkM(&matrixPtr[0], &outPtr[0], &helpPtr[0], edges);

                    for (int i = 0; i < edges; i++)
                        for (int j = i; j < edges; j++) {
                            bool temp = helpM[i][j] && helpM[j][i];
                            helpM[i][j] = temp;
                            helpM[j][i] = temp;
                        }

                    showM(&helpPtr[0], edges, edges, hdc);
                }
                    break;
                case 6 :{
                    bool isIso;
                    int point = 0;

                    for (int i = 0; i < edges; i++) {
                        isIso = true;
                        for (int j = 0; j < edges; j++) {
                            if ((int) matrix[i][j]) isIso = false;
                            break;
                        }
                        if (isIso) outM[0][point++] = i + 1;
                    }

                    showM(&outPtr[0], 0, point, hdc);
                }
                case 7 : {
                    for (int i = 0; i < edges; i++) {
                        outM[1][i] = 0;
                        for (int j = 0; j < edges; j++)
                            if (matrix[j][i] == 1.0f)
                                outM[1][i]++;
                        if (outM[1][i] != 1) outM[1][i] = 2;
                    }

                    if (directed)
                        for (int i = 0; i < edges; i++) {
                            outM[1][i] = 0;
                            for (int j = 0; j < edges; j++)
                                if (matrix[i][j] == 1.0f)
                                    outM[1][i]++;
                        }

                    int point = 0;
                    for (int i = 0; i < edges; i++)
                        if (outM[1][i] == 1) outM[0][point++] = i;

                    showM(&outPtr[0], 0, point, hdc);
                }
                default:
                    printf("Invalid");
            }

            SelectObject(hdc, KPen);

            for (int i = 0; i < edges; i++)
                for (int j = 0; j < edges; j++) {
                    float angle;
                    int destX, destY;
                    int rotation[2];
                    if (matrix[i][j] == 1.0f) {
                        bool arc = false;
                        bool lines = true;
                        if (matrix[i][j] == matrix[j][i] && i > j) lines = false;

                        for (int el = 0; el < edges; el++) {
                            if (fabsf(getAngle(nx[i], nx[el], ny[i], ny[el])
                                      - getAngle(nx[el], nx[j], ny[el], ny[j])) <= FLT_MIN)
                                arc = true;
                        }
                        if (lines) MoveToEx(hdc, nx[i], ny[i], NULL);

                        if (i == j) {
                            AngleArc(hdc, nx[i] - dx * 2, ny[i], (int) ((float) dx * 1.45f), 0, 359);
                            if (directed) angle = M_PI / 4;
                        } else if (arc) {
                            int halfX = (int) ((float) (nx[i] + nx[j]) * 0.5f);
                            int halfY = (int) ((float) (ny[i] + ny[j]) * 0.5f);
                            if (nx[i] == nx[j]) {
                                halfX += dx * 2;
                            } else {
                                halfY += dx * 2;
                            }

                            angle = getAngle(halfX, nx[j], halfY, ny[j]);

                            if (lines) LineTo(hdc, halfX, halfY);
                        } else {
                            angle = getAngle(nx[i], nx[j], ny[i], ny[j]);
                        }
                        if (lines || directed) {
                            rotate(angle, dx, rotation);
                            destX = nx[j] - rotation[0];
                            destY = ny[j] - rotation[1];
                        }
                        if (lines) LineTo(hdc, destX, destY);
                        if (directed) arrow(angle, destX, destY, hdc);
                    }
                }

            SelectObject(hdc, BPen);

            int count = 0;
            for (int i = 0; i < edges; i++) {
                if (outState == 4 || outState == 5) SelectObject(hdc, color(outM[0][i]));
                Ellipse(hdc, nx[i] - dx, ny[i] - dy, nx[i] + dx, ny[i] + dy);
                if (i < 9) TextOut(hdc, nx[i] - dtx, ny[i] - dy / 2, &nn[count], 1);
                else {
                    TextOut(hdc, nx[i] - dtx * 2, ny[i] - dy / 2, &nn[count], 1);
                    count++;
                    TextOut(hdc, nx[i], ny[i] - dy / 2, &nn[count], 1);
                }
                count++;
            }

            EndPaint(hWnd, &ps);
            break;
        }
        case WM_COMMAND:
            if (lParam == (LPARAM)hBtnToDir) directed = true;
            else if (lParam == (LPARAM)hBtnToUnDir) directed = false;
            else if (lParam == (LPARAM)hBtnModify) modified = !modified;
            else if (lParam == (LPARAM)hBtnAdjGraph) outState = 0;
            else if (lParam == (LPARAM)hBtnPow) outState = 1;
            else if (lParam == (LPARAM)hBtnWays) outState = 2;
            else if (lParam == (LPARAM)hBtnLinked) outState = 3;
            else if (lParam == (LPARAM)hBtnSL) outState = 4;
            else if (lParam == (LPARAM)hBtnSLM) outState = 5;
            else if (lParam == (LPARAM)hBtnIso) outState = 6;
            else if (lParam == (LPARAM)hBtnHang) outState = 7;
            else if (lParam == (LPARAM)hBtnLG) { cond = true; directed = true; }

            if (lParam == (LPARAM)hBtnToDir || lParam == (LPARAM)hBtnToUnDir ||lParam == (LPARAM)hBtnModify)
                cond = false;

            InvalidateRect(hWnd, NULL, TRUE);
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default:
            return(DefWindowProc(hWnd, messg, wParam, lParam));
    }
}