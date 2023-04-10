#include<windows.h>
#include<math.h>
#include <float.h>
#include <stdio.h>
#include <stdbool.h>

#define n1 2
#define n2 1
#define n3 0
#define n4 8
#define edges (n3 + 10)

#define child next
#define brother prev

typedef struct Deck {
    int value;
    struct Deck *next;
    struct Deck *prev;
} deck;

int currentDot = 0;
bool visited[edges];
bool searchDfs = false;
bool doSearch = false;
bool initial = false;

deck *queue, *tree;
char nn[edges + max(0, edges - 9)];
int nx[edges];
int ny[edges];
int dx = 16, dy = 16, dtx = 5;
int perLayer[edges], increments[edges], values[edges];

float linkageM[edges][edges];
float *matrixPtr[edges];

deck* init(int v) {
    deck *d;
    d = malloc(sizeof(deck));
    d->value = v;
    d->next = NULL;
    d->prev = NULL;

    return d;
}

deck* unshift(deck *d, int v) {
    while (d->prev != NULL) d = d->prev;

    deck *this;
    this = malloc(sizeof(deck));

    d->prev = this;
    this->next = d;
    this->prev = NULL;

    this->value = v;

    return this;
}

deck* push(deck *d, int v) {
    if (d->next != NULL) {
        printf("error");
        return NULL;
    }

    deck *this;
    this = malloc(sizeof(deck));

    d->next = this;
    this->prev = d;
    this->next = NULL;

    this->value = v;

    return this;
}

deck* pop(deck *d) {
    if (d->next != NULL) {
        printf("error");
        return NULL;
    }
    deck *newLast;
    newLast = d->prev;
    newLast->next = NULL;

    free(d);
    return newLast;
}

deck* shift(deck *d) {
    if (d->prev != NULL) exit(EXIT_FAILURE);

    deck *newFirst;

    newFirst = d->next;
    newFirst->prev = NULL;

    free(d);

    return newFirst;
}

void delete(deck *d) {
    if (d->prev != NULL) d->prev->next = d->next;
    if (d->next != NULL) d->next->prev = d->prev;

    free(d);
}

void clear(deck *d) {
    while (d->next != NULL) d = d->next;

    deck *temp;

    while (d != NULL) {
        temp = d;
        d = d->prev;
        free(temp);
    }
}

void clearTree(deck *d) {
    deck *temp;

    if (d->child != NULL) clearTree(d->child);
    if (d->brother != NULL) clearTree(d->brother);

    free(d);
}

deck* pushTree(deck *d, int v) {
    deck *new;
    new = malloc(sizeof(deck));
    new->value = v;
    new->child = NULL;
    new->brother = NULL;

    deck *temp = d;
    d = d->child;

    if (d == NULL) temp->child = new;
    else {
        while (d->brother != NULL) d = d->brother;
        d->brother = new;
    }
    return temp;
}

deck* findPushTree(deck *d, int find, int v) {
    if (d->value == find) {
        printf("%d -> %d\n", find, v);
        return pushTree(d, v);
    }

    if (d->brother != NULL) findPushTree(d->brother, find, v);
    if  (d->child != NULL) findPushTree(d->child, find, v);

}

deck *testShow(deck *d) {
    while (d != NULL)
        d = d->prev;
}



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

void formTree(float **matrix, deck *d, int *x, int *y, int n, int layer) {
    int maxX = 900, minX = 400, curY = 200, incY = 40; //I want to place this in other place, because next method uses same

    deck *copy = d->child;

    if (layer == 0) {
        x[d->value] = (maxX - minX) / 2 + minX;
        y[d->value] = curY;
        for (int i = 0; i < n; i++) {
            perLayer[i] = 0;
            increments[i] = 0;
            values[i] = 0;
            for (int j = 0; j < n; j++)
                matrix[i][j] = 0.0f;
        }
        layer++;
    }

    while (copy != NULL) {
        perLayer[layer]++;

        matrix[d->value][copy->value] = 1.0f;
        if (copy->child != NULL) formTree(matrix, copy, x, y, n, layer + 1);
        copy = copy->brother;
    }

    increments[layer] = (maxX - minX) / (perLayer[layer] + 1);
}

void placeTree(deck *d, int *x, int *y, int n, int layer) {
    int minX = 400,  curY = 200, incY = 50;
    deck *copy = d->child;

    curY += incY * layer;
    while (copy != NULL) {
        values[layer] += increments[layer];

        y[copy->value] = curY;
        x[copy->value] = minX + values[layer];

        copy = copy->brother;
    }

    copy = d->child;

    while (copy != NULL) {
        if (copy->child != NULL) placeTree(copy, x, y, n, layer + 1);
        copy = copy->brother;
    }
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
            matrix[i][j] = (float) (matrix[i][j] * mul > 1.0f);
        }
    }
}

void showM(float **matrix, int n, HDC hdc) {
    int curX = 300, curY = 300, incY = 30, incX = 15;
    char symbol[2];
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            sprintf(symbol, "%i", (int)matrix[i][j]);
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

bool dfsStep(int **matrix, deck **qDeck, deck **tDeck, int n) {

    for (int i = 0; i < n; i++)
        if (matrix[(*qDeck)->value][i] && !visited[i]) {
            findPushTree(*tDeck,(*qDeck)->value, i);
            //printf("%d -> %d\n", (*qDeck)->value, i);
            *qDeck = push(*qDeck, i);
            visited[i] = true;
            return false;
        }
    if ((*qDeck)->prev == NULL) return true;

    *qDeck = pop(*qDeck);

    return false;

}

bool bfsStep(int **matrix, deck **qDeck, deck **tDeck, int n) {
    deck *copy = *qDeck;

    for (int i = 0; i < n; i++)
        if (matrix[(*qDeck)->value][i] && !visited[i]) {
            unshift(copy, i);
            findPushTree(*tDeck, (*qDeck)->value, i);
            visited[i] = true;
            return false;
        }
    if ((*qDeck)->prev == NULL) return true;

    *qDeck = pop(*qDeck);

    return false;

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
    static HWND hBtnNext, hBtnChange, hEdit;


    switch (messg) {
        case WM_CREATE :
            hInst = ((LPCREATESTRUCT)lParam)->hInstance;

            hBtnNext = CreateWindow("button", "Next Step",
                                    WS_CHILD | WS_VISIBLE | WS_BORDER,
                                    100, 100, 120, 30, hWnd, 0, hInst, NULL);
            ShowWindow(hBtnNext, SW_SHOWNORMAL);

            hBtnChange = CreateWindow("button", "DFS/BFS",
                                    WS_CHILD | WS_VISIBLE | WS_BORDER,
                                    100, 130, 120, 30, hWnd, 0, hInst, NULL);
            ShowWindow(hBtnChange, SW_SHOWNORMAL);

            hEdit = CreateWindow("edit", "1",
                                 WS_CHILD | WS_VISIBLE | WS_BORDER,
                                 230, 100, 120, 30, hWnd, 0, hInst, NULL);
            ShowWindow(hEdit, SW_SHOWNORMAL);

            fillNums(nn, edges);
            generateList(nx, ny);

            for (int i = 0; i < edges; i++) {
                matrixPtr[i] = linkageM[i];
            }

            srand(n1 * 1000 + n2 *100 + n3 *10 + n4);
            randM(&matrixPtr[0], edges);
            mulM(&matrixPtr[0], edges,  1.0f - n3 * 0.01f - n4 * 0.005f - 0.25f);

            queue = init(currentDot);
            tree = init(0);
            visited[0] = true;

            break;
        case WM_PAINT :
            hdc = BeginPaint(hWnd, &ps);

            if (initial) {
                clear(queue);
                clearTree(tree);
                queue = init(currentDot);
                tree = init(currentDot);

                for (int i = 0; i < edges; i++) visited[i] = false;
                visited[currentDot] = true;

                initial = false;
            }

            {
                char label[] = "Choose start point";
                TextOut(hdc, 233, 80, label, (int)strlen(label));

                char alg[] = "BFS";
                if (searchDfs) alg[0] = 'D';
                TextOut(hdc, 100, 180, alg, (int)strlen(alg));
            }

            bool finished = false;
            if (doSearch) {
                finished = searchDfs ?
                           dfsStep((int **) &matrixPtr[0], &queue, &tree, edges) :
                           bfsStep((int **) &matrixPtr[0], &queue, &tree, edges);
                doSearch = false;
                currentDot = queue->value;

                TCHAR buffer[2];
                wsprintf(buffer, TEXT("%d"), currentDot + 1);
                SetWindowText(hEdit, buffer);

                if (finished) {
                    formTree(&matrixPtr[0], tree, &nx[0], &ny[0], edges, 0);
                    placeTree(tree, &nx[0], &ny[0], edges, 1);
                }
            }

            HPEN BPen = CreatePen(PS_SOLID, 2, RGB(50, 0, 255));
            HPEN BPenCurrent = CreatePen(PS_SOLID, 2, RGB(255, 0, 50));
            HPEN BPenVisit = CreatePen(PS_SOLID, 2, RGB(0, 255, 0));

            HPEN KPen = CreatePen(PS_SOLID, 1, RGB(20, 20, 5));

            SelectObject(hdc, KPen);

            for (int i = 0; i < edges; i++)
                for (int j = 0; j < edges; j++) {
                    float angle;
                    int destX, destY;
                    int rotation[2];
                    if (linkageM[i][j] == 1.0f) {
                        int arc = 0;
                        if (linkageM[i][j] == linkageM[j][i] && i > j) {
                            arc = -1;
                        }
                        else {
                            for (int el = 0; el < edges; el++) {
                                if (fabsf(getAngle(nx[i], nx[el], ny[i], ny[el])
                                          - getAngle(nx[el], nx[j], ny[el], ny[j])) <= FLT_MIN)
                                    arc = 1;
                            }
                        }

                        MoveToEx(hdc, nx[i], ny[i], NULL);

                        if (i == j) {
                            AngleArc(hdc, nx[i] - dx * 2, ny[i], (int)((float)dx * 1.45f), 0, 359);
                            angle = M_PI / 4;
                        }
                        else {

                            if (arc != 0) {
                                int halfX = (int) ((float) (nx[i] + nx[j]) * 0.5f);
                                int halfY = (int) ((float) (ny[i] + ny[j]) * 0.5f);
                                if (nx[i] == nx[j]) {
                                    halfX += (dx * 2 + (halfY / 100)) * arc;
                                } else {
                                    halfY += (dx * 2 + (halfX / 100)) * arc;
                                }
                                angle = getAngle(halfX, nx[j], halfY, ny[j]);
                                LineTo(hdc, halfX, halfY);
                            }
                            else {
                                angle = getAngle(nx[i], nx[j], ny[i], ny[j]);
                            }
                        }
                        rotate(angle, dx, rotation);
                        destX = nx[j] - rotation[0];
                        destY = ny[j] - rotation[1];

                        LineTo(hdc, destX, destY);
                        arrow(angle, destX, destY, hdc);
                    }
                }

            int count = 0;
            for(int i=0;i<edges;i++){
                if (i == currentDot) SelectObject(hdc, BPenCurrent);
                else if (visited[i]) SelectObject(hdc, BPenVisit);
                else SelectObject(hdc, BPen);

                Ellipse(hdc, nx[i]-dx,ny[i]-dy,nx[i]+dx,ny[i]+dy);
                if (i < 9) TextOut(hdc, nx[i]-dtx,ny[i]-dy/2, &nn[count],1);
                else {
                    TextOut(hdc, nx[i] - dtx * 2, ny[i] - dy / 2, &nn[count], 1);
                    count++;
                    TextOut(hdc, nx[i], ny[i] - dy / 2, &nn[count], 1);
                }
                count++;
            }

            showM(&matrixPtr[0], edges, hdc);

            EndPaint(hWnd, &ps);
            break;
        case WM_COMMAND:
            if (lParam == (LPARAM)hBtnNext) {
                char input[3];
                GetWindowText(hEdit, input, sizeof(input));

                int inputInt = atoi(input);

                if (inputInt <= 0 || inputInt > edges) break;

                if (inputInt - 1 != currentDot) {
                    currentDot = inputInt - 1;
                    initial = true;
                }
                else doSearch = true;

            }
            else if (lParam == (LPARAM)hBtnChange) {
                searchDfs = !searchDfs;
                initial = true;
            }

            InvalidateRect(hWnd, NULL, TRUE);
            break;

        case WM_DESTROY:
            clear(queue);
            clearTree(tree);

            PostQuitMessage(0);
            break;
        default:
            return(DefWindowProc(hWnd, messg, wParam, lParam));
    }
}