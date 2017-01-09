// Made by Bastiaan van der Plaat (http://bastiaan.plaatsoft.nl)
#include <windows.h>
int w = 640, h = 480, e = 32, i, score, time, level, gameover, dragging;
typedef struct { int x, y, w, h, dx, dy; } Block; Block a, b[4];
void startGame(HWND hwnd) {
  score = time = 0, level = 1, gameover = dragging = FALSE, a = (Block){w/2-e, h/2-e, e*2, e*2},
  b[0] = (Block){0, 0, e*4, e*2, 4, 4}, b[1] = (Block){w-e*3, 0, e*3, e*3, -4, 4},
  b[2] = (Block){0, h-e*4, e*2, e*4, 4, -4}, b[3] = (Block){w-e*4, h-e*3, e*4, e*3, -4, -4};
  SetTimer(hwnd, 1, 25, NULL);
}
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
  PAINTSTRUCT ps;
  switch (msg) {
    case WM_CREATE:
      startGame(hwnd); RECT r1, r2; GetWindowRect(hwnd, &r1); GetClientRect(hwnd, &r2);
      SetWindowPos(hwnd, NULL, 0, 0, r1.right - r1.left - r2.right + w, r1.bottom - r1.top - r2.bottom + h, SWP_NOMOVE);
    break;
    case WM_TIMER:
      if (gameover) {
        KillTimer(hwnd, 1);
        switch (MessageBox(hwnd, "You are game over!", "Game over", MB_RETRYCANCEL | MB_ICONINFORMATION)) {
          case IDRETRY: startGame(hwnd); break; case IDCANCEL: DestroyWindow(hwnd); break;
        }
      } else {
        time++, score += level;
        if (time % 400 == 0) for (level++, i = 0; i < 4; i++) {
          if (b[i].dx > 0) { b[i].dx += 2; } else { b[i].dx -= 2; }
          if (b[i].dy > 0) { b[i].dy += 2; } else { b[i].dy -= 2; }
        }
        RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE | RDW_INTERNALPAINT);
      }
    break;
    case WM_LBUTTONDOWN: { int x = LOWORD(lParam), y = HIWORD(lParam);
      if (x >= a.x && x <= a.x + a.w && y >= a.y && y <= a.y + a.h ) a.dx = x - a.x, a.dy = y - a.y, dragging = TRUE;
    } break;
    case WM_LBUTTONUP: dragging = FALSE; break;
    case WM_MOUSEMOVE: if (dragging) a.x = LOWORD(lParam) - a.dx, a.y = HIWORD(lParam) - a.dy; break;
    case WM_PAINT:
      BeginPaint(hwnd, &ps);
      SetBkColor(ps.hdc, RGB(255, 255, 0));
      SelectObject(ps.hdc, CreateFont(20, 0, 0, 0, 400, 0, 0, 0, 0, 0, 0, 0, 0, "Arial"));
      
      SelectObject(ps.hdc, CreateSolidBrush(RGB(255, 255, 0)));
      Rectangle(ps.hdc, 0, 0, w, h);
      SelectObject(ps.hdc, CreateSolidBrush(RGB(255, 255, 255)));
      Rectangle(ps.hdc, 50, 50, w - 50, h - 50);
      
      LPCTSTR str[43];
      sprintf(&str, "Score: %07d  -  Time: %03ds  -  Level: %02d", score, time / 40, level);
      TextOut(ps.hdc, 50, 5, (LPCTSTR)&str, 43);
      TextOut(ps.hdc, 50, 25, "Help: move the red block avoid the white edge and the blue blocks", 65);
      TextOut(ps.hdc, 50, 445, "Made by Bastiaan van der Plaat (http://bastiaan.plaatsoft.nl)", 61);
      
      SelectObject(ps.hdc, CreateSolidBrush(RGB(255, 0, 0)));
      Rectangle(ps.hdc, a.x, a.y, a.x + a.w, a.y + a.h);
      if (a.x < 50 || a.x + a.w > w - 50 || a.y < 50 || a.y + a.h > h - 50) gameover = TRUE;
      
      SelectObject(ps.hdc, CreateSolidBrush(RGB(0, 0, 255)));
      for (i = 0; i < 4; i++) {
        Rectangle(ps.hdc, b[i].x, b[i].y, b[i].x + b[i].w, b[i].y + b[i].h);
        if (b[i].x + b[i].dx < 0 || b[i].x + b[i].dx > w - b[i].w) b[i].dx = -b[i].dx;
        if (b[i].y + b[i].dy < 0 || b[i].y + b[i].dy > h - b[i].h) b[i].dy = -b[i].dy;
        if (a.x < b[i].x + b[i].w && a.x + a.w > b[i].x && a.y < b[i].y + b[i].h && a.y + a.h > b[i].y) gameover = TRUE;
        b[i].x += b[i].dx, b[i].y += b[i].dy;
      }
      EndPaint(hwnd, &ps);
    break;
    case WM_CLOSE: DestroyWindow(hwnd); break;
    case WM_DESTROY: PostQuitMessage(0); break;
    default: return DefWindowProc(hwnd, msg, wParam, lParam);
  }
  return 0;
}
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
  WNDCLASS wc = {0}; wc.lpszClassName = "RedSquare", wc.lpfnWndProc = WndProc, wc.hInstance = hInstance,
  wc.hCursor = LoadCursor(NULL, IDC_ARROW); RegisterClass(&wc);
  HWND hwnd = CreateWindow(wc.lpszClassName, wc.lpszClassName, WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME ^
  WS_MAXIMIZEBOX, CW_USEDEFAULT, CW_USEDEFAULT, w, h, NULL, NULL, hInstance, NULL);
  ShowWindow(hwnd, nCmdShow);
  MSG msg; while(GetMessage(&msg, NULL, 0, 0)) TranslateMessage(&msg), DispatchMessage(&msg);
  return msg.wParam;
}
