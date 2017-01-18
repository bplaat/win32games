// Made by Bastiaan van der Plaat (http://bastiaan.plaatsoft.nl)
#include <windows.h>
int i, w = 640, h = 480, e = 32; char *t[13] = {"Menu", "BUTTON", "Play", "High Score", "Help", "Credits", "Release Notes", "Settings",
"Made by Bastiaan van der Plaat", "Exit", "start http://bastiaan.plaatsoft.nl/", "Arial"}; HFONT font;
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
  switch (msg) {
    case WM_CREATE:
      font = CreateFont(20, 0, 0, 0, FW_NORMAL, 0, 0, 0, 0, 0, 0, 0, 0, t[11]);
      for (i = 0; i < 8; i++) {
        HWND btn = CreateWindow(t[1], t[i+2], WS_CHILD | WS_VISIBLE, i == 6 ? 10 : w - 210, i > 5 ? h - 60 : i * 60 + 10, i == 6 ? 310 : 200, 50, hwnd, (HMENU)(i+2), NULL, NULL);
        SendMessage(btn, WM_SETFONT, (WPARAM)font, TRUE);
      }
      RECT r; GetClientRect(hwnd, &r); int nw = w * 2 - r.right - r.left, nh = h * 2 - r.bottom  - r.top;
      SetWindowPos(hwnd, NULL, (GetSystemMetrics(SM_CXSCREEN) - nw) / 2, (GetSystemMetrics(SM_CYSCREEN) - nh) / 2, nw, nh, 0);
    break;
    case WM_COMMAND:
      if (LOWORD(wParam) == 8) system(t[10]);
      if (LOWORD(wParam) == 9) DestroyWindow(hwnd);
    break;
    case WM_DESTROY: PostQuitMessage(0); return 0;
  }
  return DefWindowProc(hwnd, msg, wParam, lParam);
}
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
  srand(getpid()); WNDCLASS wc = {0}; wc.lpszClassName = t[0], wc.lpfnWndProc = WndProc, wc.hInstance = hInstance,
  wc.hCursor = LoadCursor(NULL, IDC_ARROW), wc.hbrBackground = CreateSolidBrush(RGB(rand() % 200, rand() % 200, rand() % 200)); RegisterClass(&wc);
  CreateWindow(wc.lpszClassName, t[0], WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME ^ WS_MAXIMIZEBOX | WS_VISIBLE, 0, 0, w, h, NULL, NULL, hInstance, NULL);
  MSG msg; while(GetMessage(&msg, NULL, 0, 0) > 0) TranslateMessage(&msg), DispatchMessage(&msg); return msg.wParam;
}