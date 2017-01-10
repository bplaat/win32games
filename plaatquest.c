// Made by Bastiaan van der Plaat (http://bastiaan.plaatsoft.nl)
#include <windows.h>
#include <math.h>

#define W 800
#define H 600
#define MW 26
#define MH 15

typedef struct { int x, y; } Point;
typedef struct { Point*array; int used, size; } PointArray;
void initPointArray (PointArray *a, int n) { a->array = (Point*)malloc(n * sizeof(Point)), a->used = 0, a->size = n; }
void addPointArray (PointArray *a, Point p) {
  if (a->used == a->size) a->array = (Point*)realloc(a->array, ++a->size*sizeof(Point));
  a->array[a->used++] = p;
}
typedef struct { int t, x, y, d, m; } Unit;
typedef struct { Unit *array; int used, size; } UnitArray;
void initUnitArray (UnitArray *a, int n) { a->array = (Unit*)malloc(n * sizeof(Unit)), a->used = 0, a->size = n; }
void addUnitArray (UnitArray *a, Unit u) {
  if (a->used == a->size) a->array = (Unit*)realloc(a->array, ++a->size*sizeof(Unit));
  a->array[a->used++] = u;
}
typedef struct {char name[9]; COLORREF color; UnitArray units; int size; } Player;
Player players[8] = {{"Willem", RGB(0, 100, 255)}, {"Wietske", RGB(255, 0, 0)}, {"Bastiaan", RGB(0, 255, 0)}, {"Sander", RGB(255, 200, 0)},
{"Leonard", RGB(0, 255, 255)}, {"Jiska", RGB(255, 0, 255)}, {"Browny", RGB(255, 125, 0)}, {"Piepert", RGB(150, 150, 150)}};
char map[MW][MH], str[32], font[5] = "Arial"; int seed, turn = 1, world_size = 0;
double random () { double x = sin(seed++) * 1e4; return x - floor(x); }
int rand (min, max) { return floor(random() * (max - min + 1)) + min; }

PointArray n, portals;
void neighbors (int x, int y, int p) {
  n.array[0] = (Point){x, y-1}; n.array[1] = (Point){x, y+1};
  n.array[2] = (Point){x-1, y-(x%2?0:1)}; n.array[3] = (Point){x-1, y+x%2};
  n.array[4] = (Point){x+1, y-(x%2?0:1)}; n.array[5] = (Point){x+1, y+x%2};
  
  for (int i = 0, k; i < 6; i++) { k = FALSE;
    for (int j = 0; j < players[p].units.used; j++)
      if (players[p].units.array[j].x == n.array[i].x && players[p].units.array[j].y == n.array[i].y) k = TRUE;
   if (!map[n.array[i].x][n.array[i].y] || k) n.array[i] = (Point){0,0};
  }
}
void endTurn () {
  for (int i = 0; i < 8; i++) for (int j = 0; j < players[i].units.used; j++) {
    neighbors(players[i].units.array[j].x, players[i].units.array[j].y, i);
    int x = n.array[0].x, y = n.array[0].y; //do { x = n.array[rand(0, 6)].x, y = n.array[rand(0, 6)].y; } while(x == 0 && y == 0);
    players[i].units.array[j].x = x, players[i].units.array[j].y = y, map[x][y] = i + 3;
  }
  turn++;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
  PAINTSTRUCT ps; int i, j, k, x, y;
  switch (msg) {
    case WM_CREATE:
      seed = getpid(); initPointArray(&n, 6); initPointArray(&portals, 5);
      for (i = 0; i < MW; i++) for (j = 0; j < MH-i%2; j++) map[i][j] = 0;
      
      RECT r1, r2; GetWindowRect(hwnd, &r1); GetClientRect(hwnd, &r2);
      SetWindowPos(hwnd, NULL, 0, 0, r1.right - r1.left - r2.right + W, r1.bottom - r1.top - r2.bottom + H, SWP_NOMOVE);
      
      // Create map
      for (i = 0; i < MW/2; i++) {
        x = rand(1, MW - 2), y = rand(1, MH - 2);
        for (j = 0; j < MW/2; j++) {
          if (x <= 1 || x >= MW - 2 || y <= 1 || y >= MH-2-x%2) continue;
          if (j == 0) addPointArray(&portals, (Point){x, y});
          map[x][y] = map[x][y-1] = map[x][y+1] = map[x-1][y-(x%2?0:1)] = 
          map[x-1][y+x%2] = map[x+1][y-(x%2?0:1)] = map[x+1][y+x%2] = 1;
          x += rand(-1, 1); y += rand(-1, 1);
        }
      }
      for (i = 0; i < portals.size; i++) map[portals.array[i].x][portals.array[i].y] = 2;
      for (i = 0; i < MW; i++) for (j = 0; j < MH-i%2; j++) if (map[i][j] == 1) world_size++;
      
      // Create player start units
      for (i = 0; i < 8; i++) {
        initUnitArray(&players[i].units, 3);
        for (j = 0; j < 3; j++) {
          do { x = rand(1, MW-2), y = rand(1, MH-2); } while (map[x][y] != 1);
          map[x][y] = i + 3; addUnitArray(&players[i].units, (Unit){0, x, y});
        }
      }
      SetTimer(hwnd, 1, 100, NULL);
    break;
    case WM_TIMER: endTurn(), InvalidateRect(hwnd, NULL, TRUE); break;
    case WM_PAINT:
      BeginPaint(hwnd, &ps);
      SetBkMode(ps.hdc, TRANSPARENT);
      SetTextColor(ps.hdc, RGB(0, 0, 0));
      
      SelectObject(ps.hdc, CreateFont(32, 0, 0, 0, 700, 0, 0, 0, 0, 0, 0, 0, 0, font));
      for (i = 0; i < MW; i++) for (j = 0; j < MH-i%2; j++) {
        if (map[i][j] == 0) SelectObject(ps.hdc, CreateSolidBrush(RGB(0, 0, 175)));
        if (map[i][j] == 1) SelectObject(ps.hdc, CreateSolidBrush(RGB(0, 175, 0)));
        if (map[i][j] == 2) SelectObject(ps.hdc, CreateSolidBrush(RGB(255, 255, 255)));
        if (map[i][j] > 2) SelectObject(ps.hdc, CreateSolidBrush(players[map[i][j] - 3].color));
        x = i * 30, y = j * 40 + i % 2 * 20;
        POINT points[6] = {{x, y+20},{x+10, y},{x+30, y},{x+40, y+20},{x+30, y+40},{x+10, y+40}};
        Polygon(ps.hdc, points, 6);
        if (map[i][j] == 2) TextOutW(ps.hdc, x + 8, y + 4, L"\u26D2", 1);
      }
      
      for (i = 0; i < 8; i++) {
        SetTextColor(ps.hdc, RGB(255, 255, 255));
        for (j = 0; j < players[i].units.used; j++) {
          k = players[i].units.array[j].t, x = players[i].units.array[j].x, y = players[i].units.array[j].y;
          TextOutW(ps.hdc, x * 30 + 8, y * 40 + x % 2 * 20 + 2, k == 0 ? L"\u265C" : L"\u265F", 1);
        }
      }
      
      SelectObject(ps.hdc, CreateFont(24, 0, 0, 0, 700, 0, 0, 0, 0, 0, 0, 0, 0, font));
      for (j = 0; j < 32; j++) str[j] = 0;
      sprintf(str, "Turn: %d", turn);
      TextOut(ps.hdc, 20, H - 84, (LPCTSTR)&str, 32);
      sprintf(str, "World size: %d", world_size);
      TextOut(ps.hdc, 20, H - 48, (LPCTSTR)&str, 32);
      
      for (i = 0; i < 8; i++) {
        SetTextColor(ps.hdc, players[i].color);
        for (j = 0; j < 32; j++) str[j] = 0;
        for (players[i].size = j = 0; j < MW; j++) for (k = 0; k < MH-j%2; k++) if (map[j][k] == i + 3) players[i].size++;
        sprintf(str, "%d. %s: %d - %d - %.1f%%", i+1, players[i].name, players[i].units.used, players[i].size, (float)players[i].size / (float)world_size * 100.0);
        TextOut(ps.hdc, 12, 12 + 36 * i, (LPCTSTR)&str, 32);
      }
      EndPaint(hwnd, &ps);
    break;
    case WM_DESTROY: PostQuitMessage(0); break;
    default: return DefWindowProc(hwnd, msg, wParam, lParam);
  }
  return 0;
}
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
  WNDCLASS wc = {0}; wc.lpszClassName = "PlaatQuest", wc.lpfnWndProc = WndProc, wc.hInstance = hInstance;
  wc.hCursor = LoadCursor(NULL, IDC_ARROW), wc.hbrBackground = CreateSolidBrush(RGB(0, 0, 175)); RegisterClass(&wc);
  HWND hwnd = CreateWindow(wc.lpszClassName, wc.lpszClassName, WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME ^ WS_MAXIMIZEBOX,
  CW_USEDEFAULT, CW_USEDEFAULT, W, H, NULL, NULL, hInstance, NULL); ShowWindow(hwnd, nCmdShow);
  MSG msg; while(GetMessage(&msg, NULL, 0, 0)) TranslateMessage(&msg), DispatchMessage(&msg);
  return msg.wParam;
}