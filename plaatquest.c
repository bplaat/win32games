// Made by Bastiaan van der Plaat (http://bastiaan.plaatsoft.nl)
#include <windows.h>
#include <math.h>
#define W 800
#define H 600
#define MW 26
#define MH 15

char *map, *turn_str, *ws_str; int seed, turn, world_size = 0, i, j, x, y;
double random () { double x = sin(seed++) * 1e4; return x - floor(x); }
int rand (min, max) { return floor(random() * (max - min + 1)) + min; }

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
  PAINTSTRUCT ps;
  switch (msg) {
    case WM_CREATE:
      seed = getpid(), turn = 1, turn_str = (char*)malloc(13), ws_str = (char*)malloc(15), map = (char*)malloc(MW * MH);
      for (i = 0; i < MW; i++) for (j = 0; j < MH-x%2; j++) *(map+i*MH+j) = 0;
      
      RECT r1, r2; GetWindowRect(hwnd, &r1); GetClientRect(hwnd, &r2);
      SetWindowPos(hwnd, NULL, 0, 0, r1.right - r1.left - r2.right + W, r1.bottom - r1.top - r2.bottom + H, SWP_NOMOVE);
      
      for (i = 0; i < MW/2; i++) { x = rand(1, MW - 2), y = rand(1, MH - 2); for (j = 0; j < MW/2; j++) {
        if (x <= 1 || x >= MW - 2 || y <= 1 || y >= MH-2-x%2) continue;
        *(map+x*MH+y) = *(map+x*MH+y-1) = *(map+x*MH+y+1) = *(map+(x-1)*MH+y-(x%2?0:1)) = 
        *(map+(x-1)*MH+y+x%2) = *(map+(x+1)*MH+y-(x%2?0:1)) = *(map+(x+1)*MH+y+x%2) = 1;
        x += rand(-1, 1); y += rand(-1, 1);
      }}
      for (i = 0; i < MW; i++) for (j = 0; j < MH-x%2; j++) if (*(map+i*MH+j) == 1) world_size++;
    break;
    case WM_PAINT:
      BeginPaint(hwnd, &ps);
      SelectObject(ps.hdc, CreateFont(20, 0, 0, 0, 400, 0, 0, 0, 0, 0, 0, 0, 0, "Arial"));
      SetTextColor(ps.hdc, RGB(255, 255, 255));
      SetBkColor(ps.hdc, TRANSPARENT);
      
      SelectObject(ps.hdc, CreateSolidBrush(RGB(0, 0, 175)));
      Rectangle(ps.hdc, 0, 0, W, H);
      
      for (i = 0; i < MW; i++) for (j = 0; j < MH-i%2; j++) {
        if (*(map+i*MH+j) == 0) SelectObject(ps.hdc, CreateSolidBrush(RGB(0, 0, 175)));
        if (*(map+i*MH+j) == 1) SelectObject(ps.hdc, CreateSolidBrush(RGB(0, 175, 0)));
        if (*(map+i*MH+j) == 2) SelectObject(ps.hdc, CreateSolidBrush(RGB(255, 255, 255)));
        x = i * 30, y = j * 40 + i % 2 * 20;
        POINT points[6] = {{x, y+20},{x+10, y},{x+30, y},{x+40, y+20},{x+30, y+40},{x+10, y+40}};
        Polygon(ps.hdc, points, 6);
      }
      
      sprintf(&turn_str, "Turn: %6d", turn);
      TextOut(ps.hdc, 20, H - 60, (LPCTSTR)&turn_str, 13);
      sprintf(&ws_str, "World size: %3d", world_size);
      TextOut(ps.hdc, 20, H - 40, (LPCTSTR)&ws_str, 15);
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