/*
Copyright (C) 1997-2007 ZSNES Team ( zsKnight, _Demo_, pagefault, Nach )

http://www.zsnes.com
http://sourceforge.net/projects/zsnes
https://zsnes.bountysource.com

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
version 2 as published by the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#define DIRECTINPUT_VERSION 0x0800
#define DIRECTSOUND_VERSION 0x0800
#define __STDC_CONSTANT_MACROS

extern "C"
{
#include <windows.h>
#include <stdio.h>
}
#include <stdint.h>
#include <math.h>
#include <ctype.h>
#include <dsound.h>
#include <dinput.h>
#include <winuser.h>
#include "resource.h"

extern "C"
{
#include "gl_draw.h"
#include "../cfg.h"
#include "../input.h"
#include "../zmovie.h"
#include "../asm_call.h"
  void zexit(), zexit_error();
}

#include "winlink.h"

DWORD Moving = 0;
DWORD SoundBufferSize = 1024 * 18;
DWORD FirstSound = 1;

int SoundEnabled = 1;
int UsePrimaryBuffer = 0;

DWORD FirstActivate = 1;

HANDLE debugWindow = 0;

extern "C"
{
  HWND hMainWindow;
  HINSTANCE hInst;
  HDC hDC;
  HGLRC hRC;
  DWORD FullScreen = 0;
  DWORD WindowWidth = 256;
  DWORD WindowHeight = 224;
}

LPDIRECTSOUND8 lpDirectSound = NULL;
LPDIRECTSOUNDBUFFER8 lpSoundBuffer = NULL;
LPDIRECTSOUNDBUFFER lpPrimaryBuffer = NULL;
DSBUFFERDESC dsbd;

LPVOID lpvPtr1;
DWORD dwBytes1;
LPVOID lpvPtr2;
DWORD dwBytes2;

LPDIRECTINPUT8 DInput = NULL;
LPDIRECTINPUTDEVICE8 MouseInput = NULL;
LPDIRECTINPUTDEVICE8 KeyboardInput = NULL;
LPDIRECTINPUTDEVICE8 JoystickInput[5];
DIJOYSTATE js[5];

DWORD X1Disable[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
DWORD X2Disable[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
DWORD Y1Disable[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
DWORD Y2Disable[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
DWORD Z1Disable[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
DWORD Z2Disable[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
DWORD RX1Disable[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
DWORD RX2Disable[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
DWORD RY1Disable[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
DWORD RY2Disable[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
DWORD RZ1Disable[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
DWORD RZ2Disable[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
DWORD S01Disable[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
DWORD S02Disable[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
DWORD S11Disable[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
DWORD S12Disable[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
DWORD POVDisable[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
DWORD NumPOV[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
DWORD NumBTN[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

DWORD CurrentJoy = 0;

BYTE BitDepth;
BYTE BackColor = 0;
DEVMODE mode;
HWND DebugWindowHandle;

float MouseMinX = 0;
float MouseMaxX = 256;
float MouseMinY = 0;
float MouseMaxY = 223;
float MouseX;
float MouseY;
float MouseMoveX;
float MouseMoveY;
BYTE MouseButtonPressed;
BYTE IsActivated = 1;
BYTE PrevRes=0;

extern "C"
{
  DWORD MouseButton;
  DWORD SurfaceX = 0;
  DWORD SurfaceY = 0;
  RECT BlitArea;
  BYTE AltSurface = 0;
  RECT rcWindow;
  DWORD GBitMask;
}

HANDLE hLock, hThread;
DWORD dwThreadId, dwThreadParam, semaphore_run;

extern "C"
{
  int SemaphoreMax = 5;
  void InitSemaphore();
  void ShutdownSemaphore();
  void DisplayWIPDisclaimer();
  void InitDebugger();
  void DockDebugger();
  void Clear2xSaIBuffer();
  void clear_display();
  DWORD CurMode = ~0;
  extern WORD totlines;
}
static char dinput8_dll[] = { "dinput8.dll\0" };
static char dinput8_imp[] = { "DirectInput8Create\0" };

static char ddraw_dll[] = { "ddraw.dll\0" };
static char ddraw_imp[] = { "DirectDrawCreateEx\0" };

static char dsound_dll[] = { "dsound.dll\0" };
static char dsound_imp[] = { "DirectSoundCreate8\0" };

static HMODULE hM_ddraw = NULL, hM_dsound = NULL, hM_dinput8 = NULL;

typedef HRESULT (WINAPI* lpDirectInput8Create)(HINSTANCE hinst, DWORD dwVersion, REFIID riidltf,
                                               LPVOID *ppvOut, LPUNKNOWN punkOuter);
static lpDirectInput8Create pDirectInput8Create;

typedef HRESULT (WINAPI* lpDirectDrawCreateEx)(GUID FAR *lpGuid, LPVOID *lplpDD, REFIID  iid,
                                               IUnknown FAR *pUnkOuter);

extern "C"
{
  lpDirectDrawCreateEx pDirectDrawCreateEx;
}

typedef HRESULT (WINAPI* lpDirectSoundCreate8)(LPCGUID pcGuidDevice, LPDIRECTSOUND8 *ppDS8,
                                               LPUNKNOWN pUnkOuter);
static lpDirectSoundCreate8 pDirectSoundCreate8;

extern "C" void FreeDirectX()
{
  FreeLibrary(hM_dsound);
  FreeLibrary(hM_ddraw);
  FreeLibrary(hM_dinput8);
  zexit();
}

extern "C" void DXLoadError()
{
  if (MessageBox(NULL,
                 "Sorry, you need to install or reinstall DirectX v8.0 or higher\nto use ZSNESW.\nWould you like to go to the DirectX homepage?",
                 "Error", MB_ICONINFORMATION | MB_YESNO) == IDYES)
  {
    ShellExecute(NULL, NULL, "http://www.microsoft.com/directx/", NULL, NULL, 0);
  }
  FreeDirectX();
}

extern "C" void ImportDirectX()
{
  hM_dinput8 = LoadLibrary(dinput8_dll);

  if (hM_dinput8 == NULL)
  {
    DXLoadError();
  }

  pDirectInput8Create = (lpDirectInput8Create) GetProcAddress(hM_dinput8, dinput8_imp);

  if (pDirectInput8Create == NULL)
  {
    char err[256];
    wsprintf(err, "Failed to import %s:%s", dinput8_dll, dinput8_imp);
    MessageBox(NULL, err, "Error", MB_ICONERROR);
    DXLoadError();
  }

  hM_ddraw = LoadLibrary(ddraw_dll);

  if (hM_ddraw == NULL)
  {
    char err[256];
    wsprintf(err, "Failed to import %s", ddraw_dll);
    MessageBox(NULL, err, "Error", MB_ICONERROR);
    DXLoadError();
  }

  pDirectDrawCreateEx = (lpDirectDrawCreateEx) GetProcAddress(hM_ddraw, ddraw_imp);

  if (pDirectDrawCreateEx == NULL)
  {
    char err[256];
    wsprintf(err, "Failed to import %s:%s", ddraw_dll, ddraw_imp);
    MessageBox(NULL, err, "Error", MB_ICONERROR);
    DXLoadError();
  }

  hM_dsound = LoadLibrary(dsound_dll);

  if (hM_dsound == NULL)
  {
    char err[256];
    wsprintf(err, "Failed to import %s", dsound_dll);
    MessageBox(NULL, err, "Error", MB_ICONERROR);
    DXLoadError();
  }

  pDirectSoundCreate8 = (lpDirectSoundCreate8) GetProcAddress(hM_dsound, dsound_imp);

  if (pDirectSoundCreate8 == NULL)
  {
    char err[256];
    wsprintf(err, "Failed to import %s:%s", dsound_dll, dsound_imp);
    MessageBox(NULL, err, "Error", MB_ICONERROR);
    DXLoadError();
  }
}

// milliseconds per world update
#define UPDATE_TICKS_GAME (1000.0/59.948743718592964824120603015060)
#define UPDATE_TICKS_GAMEPAL (20.0)
#define UPDATE_TICKS_GUI (1000.0/36.0)
#define UPDATE_TICKS_UDP (1000.0/60.0)

double start, end, freq, update_ticks_pc, start2, end2, update_ticks_pc2;

void ReleaseDirectDraw();
void ReleaseDirectSound();
void ReleaseDirectInput();
int InitDirectDraw();
int ReInitSound();

extern "C"
{
  void MultiMouseInit();
  void MultiMouseShutdown();
  extern BYTE device1, device2;
  extern BYTE GUIOn;
  extern BYTE GUIOn2;
  DWORD InputEn = 0;
  void reInitSound()
  {
    ReInitSound();
  }
}

BOOL InputAcquire()
{
  for (unsigned int i = 0; i < 5; i++)
  {
    if (JoystickInput[i])
    {
      JoystickInput[i]->Acquire();
    }
  }

  if (device1 && device2 && !GUIOn2)
  {
    MultiMouseInit();
  }
  else if (MouseInput && GUIOn2)
  {
    MouseInput->Acquire();
  }
  if (KeyboardInput)
  {
    KeyboardInput->Acquire();
  }
  InputEn = 1;
  return TRUE;
}

BOOL InputDeAcquire()
{
  if (KeyboardInput)
  {
    KeyboardInput->Unacquire();
  }

  for (unsigned int i = 0; i < 5; i++)
  {
    if (JoystickInput[i])
    {
      JoystickInput[i]->Unacquire();
    }
  }

  if (device1 && device2 && !GUIOn2)
  {
    MultiMouseShutdown();
  }
  else if (MouseInput)
  {
    MouseInput->Unacquire();
  }
  InputEn = 0;
  return TRUE;
}

extern "C"
{
  void initwinvideo();
  void DosExit();
  extern BYTE EMUPause;
  extern int CurKeyPos;
  extern int CurKeyReadPos;
  extern int KeyBuffer[16];
  extern BYTE debugger;
}

extern "C" void CheckPriority()
{
  if (HighPriority == 1)
  {
    if (!SetPriorityClass(GetCurrentProcess(), ABOVE_NORMAL_PRIORITY_CLASS))
    {
      SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);
    }
  }
  else
  {
    SetPriorityClass(GetCurrentProcess(), NORMAL_PRIORITY_CLASS);
  }
}

extern "C" void CheckAlwaysOnTop()
{
  if (AlwaysOnTop == 1)
  {
    SetWindowPos(hMainWindow, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
  }
  else
  {
    SetWindowPos(hMainWindow, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
  }
}

extern "C" void CheckScreenSaver()
{
  if (DisableScreenSaver == 1 && IsActivated == 1)
  {
    SystemParametersInfo(SPI_SETSCREENSAVEACTIVE, FALSE, 0, SPIF_SENDCHANGE);
    SystemParametersInfo(SPI_SETLOWPOWERACTIVE, FALSE, 0, SPIF_SENDCHANGE);
    SystemParametersInfo(SPI_SETPOWEROFFACTIVE, FALSE, 0, SPIF_SENDCHANGE);
  }
  else
  {
    SystemParametersInfo(SPI_SETSCREENSAVEACTIVE, TRUE, 0, SPIF_SENDCHANGE);
    SystemParametersInfo(SPI_SETLOWPOWERACTIVE, TRUE, 0, SPIF_SENDCHANGE);
    SystemParametersInfo(SPI_SETPOWEROFFACTIVE, TRUE, 0, SPIF_SENDCHANGE);
  }
}

extern "C" void MinimizeWindow()
{
  ShowWindow(hMainWindow, SW_MINIMIZE);
  IsActivated = 0;
}

BOOL InputRead()
{
  static int PrevZ = 0;
  MouseMoveX = 0;
  MouseMoveY = 0;
  if (MouseInput && InputEn == 1)
  {
    DIMOUSESTATE dims;
    HRESULT hr;
    hr = MouseInput->GetDeviceState(sizeof(DIMOUSESTATE), &dims);

    if (SUCCEEDED(hr))
    {
      MouseMoveX = (float)dims.lX;
      MouseMoveY = (float)dims.lY;

      if (MouseWheel == 1)
      {
        long zDelta = dims.lZ - PrevZ;
        if (!dims.lZ)
        {
          zDelta = 0;
        }
        while (zDelta > 0)
        {
          zDelta -= 40;
          if (!((CurKeyPos + 1 == CurKeyReadPos) ||
                ((CurKeyPos + 1 == 16) && (CurKeyReadPos == 0))))
          {
            KeyBuffer[CurKeyPos] = 72 + 256;
            CurKeyPos++;
            if (CurKeyPos == 16)
            {
              CurKeyPos = 0;
            }
          }
        }
        while (zDelta < 0)
        {
          zDelta += 40;
          if (!((CurKeyPos + 1 == CurKeyReadPos) ||
                ((CurKeyPos + 1 == 16) && (CurKeyReadPos == 0))))
          {
            KeyBuffer[CurKeyPos] = 80 + 256;
            CurKeyPos++;
            if (CurKeyPos == 16)
            {
              CurKeyPos = 0;
            }
          }
        }
        PrevZ = dims.lZ;
      }

      MouseButton = (dims.rgbButtons[0] >> 7) | (dims.rgbButtons[1] >> 6) |
                    (dims.rgbButtons[2] >> 5) | (dims.rgbButtons[3] >> 4);
    }
    else
    {
      return FALSE;
    }
  }
  return TRUE;
}

void ExitFunction()
{
  // We need to clean up the debug window if it's running

  if (debugWindow)
  {
    FreeConsole();
  }

  IsActivated = 0;
  CheckScreenSaver();
  ReleaseDirectInput();
  ReleaseDirectSound();
  ReleaseDirectDraw();
  FreeLibrary(hM_dsound);
  FreeLibrary(hM_ddraw);
  FreeLibrary(hM_dinput8);
  DestroyWindow(hMainWindow);
}

extern "C"
{
  bool ctrlptr = false;
}

LRESULT CALLBACK Main_Proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  static bool shiftpr;
  bool accept;
  int vkeyval = 0;

  switch (uMsg)
  {
    case WM_KEYDOWN:
      // sent when user presses a key
      if (!((CurKeyPos + 1 == CurKeyReadPos) || ((CurKeyPos + 1 == 16) && (CurKeyReadPos == 0))))
      {
        accept = false;

        if (wParam == 16)
        {
          shiftpr = true;
        }
        else if (wParam == 17)
        {
          ctrlptr = true;
        }
        if (((wParam >= 'A') && (wParam <= 'Z')) || ((wParam >= 'a') && (wParam <= 'z')) ||
            (wParam == 27) || (wParam == 32) || (wParam == 8) || (wParam == 13) || (wParam == 9))
        {
          accept = true; vkeyval = wParam;
        }
        if ((wParam >= '0') && (wParam <= '9'))
        {
          accept = true; vkeyval = wParam;
          if (shiftpr)
          {
            switch (wParam)
            {
              case '1':
                vkeyval = '!'; break;
              case '2':
                vkeyval = '@'; break;
              case '3':
                vkeyval = '#'; break;
              case '4':
                vkeyval = '$'; break;
              case '5':
                vkeyval = '%'; break;
              case '6':
                vkeyval = '^'; break;
              case '7':
                vkeyval = '&'; break;
              case '8':
                vkeyval = '*'; break;
              case '9':
                vkeyval = '('; break;
              case '0':
                vkeyval = ')'; break;
            }
          }
        }
        if ((wParam >= VK_NUMPAD0) && (wParam <= VK_NUMPAD9))
        {
          accept = true; vkeyval = wParam - VK_NUMPAD0 + '0';
        }
        if (!shiftpr)
        {
          switch (wParam)
          {
            case 189:
              vkeyval = '-'; accept = true; break;
            case 187:
              vkeyval = '='; accept = true; break;
            case 219:
              vkeyval = '['; accept = true; break;
            case 221:
              vkeyval = ']'; accept = true; break;
            case 186:
              vkeyval = ';'; accept = true; break;
            case 222:
              vkeyval = 39; accept = true; break;
            case 188:
              vkeyval = ','; accept = true; break;
            case 190:
              vkeyval = '.'; accept = true; break;
            case 191:
              vkeyval = '/'; accept = true; break;
            case 192:
              vkeyval = '`'; accept = true; break;
            case 220:
              vkeyval = 92; accept = true; break;
          }
        }
        else
        {
          switch (wParam)
          {
            case 189:
              vkeyval = '_'; accept = true; break;
            case 187:
              vkeyval = '+'; accept = true; break;
            case 219:
              vkeyval = '{'; accept = true; break;
            case 221:
              vkeyval = '}'; accept = true; break;
            case 186:
              vkeyval = ':'; accept = true; break;
            case 222:
              vkeyval = '"'; accept = true; break;
            case 188:
              vkeyval = '<'; accept = true; break;
            case 190:
              vkeyval = '>'; accept = true; break;
            case 191:
              vkeyval = '?'; accept = true; break;
            case 192:
              vkeyval = '~'; accept = true; break;
            case 220:
              vkeyval = '|'; accept = true; break;
          }
        }
        switch (wParam)
        {
          case 33:
            vkeyval = 256 + 73; accept = true; break;
          case 38:
            vkeyval = 256 + 72; accept = true; break;
          case 36:
            vkeyval = 256 + 71; accept = true; break;
          case 39:
            vkeyval = 256 + 77; accept = true; break;
          case 12:
            vkeyval = 256 + 76; accept = true; break;
          case 37:
            vkeyval = 256 + 75; accept = true; break;
          case 34:
            vkeyval = 256 + 81; accept = true; break;
          case 40:
            vkeyval = 256 + 80; accept = true; break;
          case 35:
            vkeyval = 256 + 79; accept = true; break;
          case 107:
            vkeyval = '+'; accept = true; break;
          case 109:
            vkeyval = '-'; accept = true; break;
          case 106:
            vkeyval = '*'; accept = true; break;
          case 111:
            vkeyval = '/'; accept = true; break;
          case 110:
            vkeyval = '.'; accept = true; break;
        }
        if (accept)
        {
          KeyBuffer[CurKeyPos] = vkeyval;
          CurKeyPos++;
          if (CurKeyPos == 16)
          {
            CurKeyPos = 0;
          }
        }
      }
      break;
    case WM_KEYUP:
      // sent when user releases a key
      if (wParam == 16)
      {
        shiftpr = false;
      }
      else if (wParam == 17)
      {
        ctrlptr = false;
      }
      break;
    case WM_MOUSEMOVE:
      if (MouseInput && GUIOn2)
      {
        MouseInput->Acquire();
      }
      break;
    case WM_MOVE:
      break;
    case WM_PAINT:
      ValidateRect(hWnd, NULL);
      break;
    case WM_ACTIVATE:
      if (LOWORD(wParam) != WA_INACTIVE)
      {
        IsActivated = 1;
		if (debugger) { if (!IsWindowVisible(DebugWindowHandle)) ShowWindow(DebugWindowHandle, SW_SHOW);}

        if (FirstActivate == 0)
        {
          initwinvideo();
        }
        if (PauseFocusChange && !MovieProcessing)
        {
          EMUPause = 0;
        }
        InputAcquire();

		if (FirstActivate == 1)
        {
          FirstActivate = 0;
        }
        if (FullScreen == 1)
        {
          Clear2xSaIBuffer();
        }
        CheckPriority();
        CheckScreenSaver();
		SetForegroundWindow(hMainWindow); 

      }
      if (LOWORD(wParam) == WA_INACTIVE)
      {
        IsActivated = 0;
        if (PauseFocusChange)
        {
          EMUPause = 1;
        }
        InputDeAcquire();
        if (GUIOn || GUIOn2 || EMUPause)
        {
          SetPriorityClass(GetCurrentProcess(), IDLE_PRIORITY_CLASS);
        }
        CheckScreenSaver();
      }
      break;
    case WM_SETFOCUS:
      if (FullScreen == 0)
      {
        ShowWindow(hMainWindow, SW_SHOWNORMAL);
      }
      CheckPriority();
      CheckScreenSaver();
      InputAcquire();
      break;
    case WM_KILLFOCUS:
      InputDeAcquire();
      IsActivated = 0;
      if (GUIOn || GUIOn2 || EMUPause)
      {
        SetPriorityClass(GetCurrentProcess(), IDLE_PRIORITY_CLASS);
      }
      CheckScreenSaver();
      break;
    case WM_DESTROY:
      break;
    case WM_CLOSE:
      break;
  }
  return DefWindowProc(hWnd, uMsg, wParam, lParam);;
}

int RegisterWinClass()
{
  if (AllowMultipleInst == 0)
  {
    HWND hFindWindow;
    hFindWindow = FindWindow("ZSNES", NULL);

    if (hFindWindow != NULL)
    {
      ShowWindow(hFindWindow, SW_SHOWNORMAL);
      SetForegroundWindow(hFindWindow);
      DosExit();
    }
  }

  WNDCLASS wcl;

  wcl.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW | CS_NOCLOSE;
  wcl.cbClsExtra = 0;
  wcl.cbWndExtra = 0;
  wcl.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICON1));
  wcl.hCursor = NULL;
  wcl.hInstance = hInst;
  wcl.lpfnWndProc = (WNDPROC)Main_Proc;
  wcl.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
  wcl.lpszMenuName = NULL;
  wcl.lpszClassName = "ZSNES";

  if (RegisterClass(&wcl) == 0)
  {
    return FALSE;
  }

  return TRUE;
}

BYTE PrevStereoSound;
DWORD PrevSoundQuality;

BOOL InitSound()
{
  WAVEFORMATEX wfx;
  DSBCAPS dsbcaps;

  SoundEnabled = 0;

  if (soundon == 0)
  {
    return FALSE;
  }

  PrevSoundQuality = SoundQuality;
  PrevStereoSound = StereoSound;

  if (DS_OK == pDirectSoundCreate8(NULL, &lpDirectSound, NULL))
  {
    lpDirectSound->Initialize(NULL);

    if (PrimaryBuffer)
    {
      if (DS_OK != lpDirectSound->SetCooperativeLevel(hMainWindow, DSSCL_WRITEPRIMARY))
      {
        if (DS_OK != lpDirectSound->SetCooperativeLevel(hMainWindow, DSSCL_EXCLUSIVE))
        {
          return FALSE;
        }
      }
      else
      {
        UsePrimaryBuffer = 1;
      }
    }
    else
    {
      if (DS_OK != lpDirectSound->SetCooperativeLevel(hMainWindow, DSSCL_NORMAL))
      {
        if (DS_OK != lpDirectSound->SetCooperativeLevel(hMainWindow, DSSCL_EXCLUSIVE))
        {
          return FALSE;
        }
      }
      else
      {
        UsePrimaryBuffer = 0;
      }
    }
  }
  else
  {
    return FALSE;
  }

  wfx.wFormatTag = WAVE_FORMAT_PCM;

  switch (SoundQuality)
  {
    case 0:
      wfx.nSamplesPerSec = 8000;
      SoundBufferSize = 1024 * 2;
      break;
    case 1:
      wfx.nSamplesPerSec = 11025;
      SoundBufferSize = 1024 * 2;
      break;
    case 2:
      wfx.nSamplesPerSec = 22050;
      SoundBufferSize = 1024 * 4;
      break;
    case 3:
      wfx.nSamplesPerSec = 44100;
      SoundBufferSize = 1024 * 8;
      break;
    case 4:
      wfx.nSamplesPerSec = 16000;
      SoundBufferSize = 1024 * 4;
      break;
    case 5:
      wfx.nSamplesPerSec = 32000;
      SoundBufferSize = 1024 * 8;
      break;
    case 6:
      wfx.nSamplesPerSec = 48000;
      SoundBufferSize = 1024 * 8;
      break;
    default:
      wfx.nSamplesPerSec = 11025;
      SoundBufferSize = 1024 * 2;
  }

  if (StereoSound == 1)
  {
    wfx.nChannels = 2;
    wfx.nBlockAlign = 4;
    SoundBufferSize *= 2;
  }
  else
  {
    wfx.nChannels = 1;
    wfx.nBlockAlign = 2;
  }

  wfx.wBitsPerSample = 16;
  wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;
  wfx.cbSize = 0;

  memset(&dsbd, 0, sizeof(DSBUFFERDESC));
  dsbd.dwSize = sizeof(DSBUFFERDESC);
  dsbd.dwFlags = DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_STICKYFOCUS;
  if (UsePrimaryBuffer)
  {
    dsbd.dwFlags |= DSBCAPS_PRIMARYBUFFER;
  }
  dsbd.dwBufferBytes = UsePrimaryBuffer ? 0 : SoundBufferSize;
  dsbd.lpwfxFormat = UsePrimaryBuffer ? NULL : &wfx;

  if (DS_OK == lpDirectSound->CreateSoundBuffer(&dsbd, &lpPrimaryBuffer, NULL))
  {
    if (!UsePrimaryBuffer)
    {
      if (DS_OK ==
          lpPrimaryBuffer->QueryInterface(IID_IDirectSoundBuffer8, (LPVOID *)&lpSoundBuffer))
      {
        if (DS_OK != lpSoundBuffer->Play(0, 0, DSBPLAY_LOOPING))
        {
          return FALSE;
        }
      }
      else
      {
        return FALSE;
      }
    }
    else
    {
      lpPrimaryBuffer->SetFormat(&wfx);
      dsbcaps.dwSize = sizeof(DSBCAPS);
      lpPrimaryBuffer->GetCaps(&dsbcaps);
      SoundBufferSize = dsbcaps.dwBufferBytes;

      if (DS_OK != lpPrimaryBuffer->Play(0, 0, DSBPLAY_LOOPING))
      {
        return FALSE;
      }
    }

    SoundEnabled = 1;
    FirstSound = 0;
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

BOOL ReInitSound()
{
  WAVEFORMATEX wfx;
  DSBCAPS dsbcaps;

  if (lpSoundBuffer)
  {
    lpSoundBuffer->Stop();
    lpSoundBuffer->Release();
    lpSoundBuffer = NULL;
  }

  if (lpPrimaryBuffer)
  {
    lpPrimaryBuffer->Stop();
    lpPrimaryBuffer->Release();
    lpPrimaryBuffer = NULL;
  }

  if (soundon == 0)
  {
    SoundEnabled = 0;
    ReleaseDirectSound();
    return FALSE;
  }
  else if (SoundEnabled == 0)
  {
    return InitSound();
  }

  SoundEnabled = 0;

  PrevSoundQuality = SoundQuality;
  PrevStereoSound = StereoSound;

  wfx.wFormatTag = WAVE_FORMAT_PCM;

  switch (SoundQuality)
  {
    case 0:
      wfx.nSamplesPerSec = 8000;
      SoundBufferSize = 1024 * 2;
      break;
    case 1:
      wfx.nSamplesPerSec = 11025;
      SoundBufferSize = 1024 * 2;
      break;
    case 2:
      wfx.nSamplesPerSec = 22050;
      SoundBufferSize = 1024 * 4;
      break;
    case 3:
      wfx.nSamplesPerSec = 44100;
      SoundBufferSize = 1024 * 8;
      break;
    case 4:
      wfx.nSamplesPerSec = 16000;
      SoundBufferSize = 1024 * 4;
      break;
    case 5:
      wfx.nSamplesPerSec = 32000;
      SoundBufferSize = 1024 * 8;
      break;
    case 6:
      wfx.nSamplesPerSec = 48000;
      SoundBufferSize = 1024 * 8;
      break;
    default:
      wfx.nSamplesPerSec = 11025;
      SoundBufferSize = 1024 * 2;
  }

  if (StereoSound == 1)
  {
    wfx.nChannels = 2;
    wfx.nBlockAlign = 4;
    SoundBufferSize *= 2;
  }
  else
  {
    wfx.nChannels = 1;
    wfx.nBlockAlign = 2;
  }

  wfx.wBitsPerSample = 16;
  wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;
  wfx.cbSize = 0;

  memset(&dsbd, 0, sizeof(DSBUFFERDESC));
  dsbd.dwSize = sizeof(DSBUFFERDESC);
  dsbd.dwFlags = DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_STICKYFOCUS;
  if (UsePrimaryBuffer)
  {
    dsbd.dwFlags |= DSBCAPS_PRIMARYBUFFER;
  }
  dsbd.dwBufferBytes = UsePrimaryBuffer ? 0 : SoundBufferSize;
  dsbd.lpwfxFormat = UsePrimaryBuffer ? NULL : &wfx;

  if (DS_OK == lpDirectSound->CreateSoundBuffer(&dsbd, &lpPrimaryBuffer, NULL))
  {
    if (!UsePrimaryBuffer)
    {
      if (DS_OK ==
          lpPrimaryBuffer->QueryInterface(IID_IDirectSoundBuffer8, (LPVOID *)&lpSoundBuffer))
      {
        if (DS_OK != lpSoundBuffer->Play(0, 0, DSBPLAY_LOOPING))
        {
          return FALSE;
        }
      }
      else
      {
        return FALSE;
      }
    }
    else
    {
      lpPrimaryBuffer->SetFormat(&wfx);
      dsbcaps.dwSize = sizeof(DSBCAPS);
      lpPrimaryBuffer->GetCaps(&dsbcaps);
      SoundBufferSize = dsbcaps.dwBufferBytes;

      if (DS_OK != lpPrimaryBuffer->Play(0, 0, DSBPLAY_LOOPING))
      {
        return FALSE;
      }
    }

    SoundEnabled = 1;
    FirstSound = 0;
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

BOOL FAR PASCAL InitJoystickInput(LPCDIDEVICEINSTANCE pdinst, LPVOID pvRef)
{
  LPDIRECTINPUT8 pdi = (LPDIRECTINPUT8)pvRef;
  GUID DeviceGuid = pdinst->guidInstance;

  if (CurrentJoy > 4)
  {
    return DIENUM_CONTINUE;
  }

  // Create the DirectInput joystick device.
  if (pdi->CreateDevice(DeviceGuid, &JoystickInput[CurrentJoy], NULL) != DI_OK)
  {
    return DIENUM_CONTINUE;
  }

  if (JoystickInput[CurrentJoy]->SetDataFormat(&c_dfDIJoystick) != DI_OK)
  {
    JoystickInput[CurrentJoy]->Release();
    return DIENUM_CONTINUE;
  }

  if (JoystickInput[CurrentJoy]->SetCooperativeLevel(hMainWindow,
                                                     DISCL_EXCLUSIVE | DISCL_BACKGROUND) != DI_OK)
  {
    JoystickInput[CurrentJoy]->Release();
    return DIENUM_CONTINUE;
  }

  DIPROPRANGE diprg;

  diprg.diph.dwSize = sizeof(diprg);
  diprg.diph.dwHeaderSize = sizeof(diprg.diph);
  diprg.diph.dwObj = DIJOFS_X;
  diprg.diph.dwHow = DIPH_BYOFFSET;
  diprg.lMin = joy_sensitivity * -1;
  diprg.lMax = joy_sensitivity;

  if (FAILED(JoystickInput[CurrentJoy]->SetProperty(DIPROP_RANGE, &diprg.diph)))
  {
    X1Disable[CurrentJoy] = 1;
    X2Disable[CurrentJoy] = 1;
  }

  diprg.diph.dwObj = DIJOFS_Y;

  if (FAILED(JoystickInput[CurrentJoy]->SetProperty(DIPROP_RANGE, &diprg.diph)))
  {
    Y1Disable[CurrentJoy] = 1;
    Y2Disable[CurrentJoy] = 1;
  }

  diprg.diph.dwObj = DIJOFS_Z;
  if (FAILED(JoystickInput[CurrentJoy]->SetProperty(DIPROP_RANGE, &diprg.diph)))
  {
    Z1Disable[CurrentJoy] = 1;
    Z2Disable[CurrentJoy] = 1;
  }

  diprg.diph.dwObj = DIJOFS_RX;
  if (FAILED(JoystickInput[CurrentJoy]->SetProperty(DIPROP_RANGE, &diprg.diph)))
  {
    RX1Disable[CurrentJoy] = 1;
    RX2Disable[CurrentJoy] = 1;
  }

  diprg.diph.dwObj = DIJOFS_RY;
  if (FAILED(JoystickInput[CurrentJoy]->SetProperty(DIPROP_RANGE, &diprg.diph)))
  {
    RY1Disable[CurrentJoy] = 1;
    RY2Disable[CurrentJoy] = 1;
  }

  diprg.diph.dwObj = DIJOFS_RZ;
  if (FAILED(JoystickInput[CurrentJoy]->SetProperty(DIPROP_RANGE, &diprg.diph)))
  {
    RZ1Disable[CurrentJoy] = 1;
    RZ2Disable[CurrentJoy] = 1;
  }

  diprg.diph.dwObj = DIJOFS_SLIDER(0);
  if (FAILED(JoystickInput[CurrentJoy]->SetProperty(DIPROP_RANGE, &diprg.diph)))
  {
    S01Disable[CurrentJoy] = 1;
    S02Disable[CurrentJoy] = 1;
  }

  diprg.diph.dwObj = DIJOFS_SLIDER(1);
  if (FAILED(JoystickInput[CurrentJoy]->SetProperty(DIPROP_RANGE, &diprg.diph)))
  {
    S11Disable[CurrentJoy] = 1;
    S12Disable[CurrentJoy] = 1;
  }

  DIDEVCAPS didc;

  didc.dwSize = sizeof(DIDEVCAPS);

  if (JoystickInput[CurrentJoy]->GetCapabilities(&didc) != DI_OK)
  {
    JoystickInput[CurrentJoy]->Release();
    return DIENUM_CONTINUE;
  }

  if (didc.dwButtons <= 16)
  {
    NumBTN[CurrentJoy] = didc.dwButtons;
  }
  else
  {
    NumBTN[CurrentJoy] = 16;
  }

  if (didc.dwPOVs)
  {
    NumPOV[CurrentJoy] = didc.dwPOVs;
  }
  else
  {
    POVDisable[CurrentJoy] = 1;
  }

  DIPROPDWORD dipdw;

  dipdw.diph.dwSize = sizeof(DIPROPDWORD);
  dipdw.diph.dwHeaderSize = sizeof(dipdw.diph);
  dipdw.diph.dwHow = DIPH_BYOFFSET;
  dipdw.dwData = 2500;
  dipdw.diph.dwObj = DIJOFS_X;
  JoystickInput[CurrentJoy]->SetProperty(DIPROP_DEADZONE, &dipdw.diph);

  dipdw.diph.dwObj = DIJOFS_Y;
  JoystickInput[CurrentJoy]->SetProperty(DIPROP_DEADZONE, &dipdw.diph);

  dipdw.diph.dwObj = DIJOFS_Z;
  JoystickInput[CurrentJoy]->SetProperty(DIPROP_DEADZONE, &dipdw.diph);

  dipdw.diph.dwObj = DIJOFS_RX;
  JoystickInput[CurrentJoy]->SetProperty(DIPROP_DEADZONE, &dipdw.diph);

  dipdw.diph.dwObj = DIJOFS_RY;
  JoystickInput[CurrentJoy]->SetProperty(DIPROP_DEADZONE, &dipdw.diph);

  dipdw.diph.dwObj = DIJOFS_RZ;
  JoystickInput[CurrentJoy]->SetProperty(DIPROP_DEADZONE, &dipdw.diph);

  dipdw.diph.dwObj = DIJOFS_SLIDER(0);
  JoystickInput[CurrentJoy]->SetProperty(DIPROP_DEADZONE, &dipdw.diph);

  dipdw.diph.dwObj = DIJOFS_SLIDER(1);
  JoystickInput[CurrentJoy]->SetProperty(DIPROP_DEADZONE, &dipdw.diph);

  dipdw.diph.dwSize = sizeof(DIPROPDWORD);
  dipdw.diph.dwHeaderSize = sizeof(dipdw.diph);
  dipdw.diph.dwHow = DIPH_DEVICE;
  dipdw.dwData = DIPROPAXISMODE_ABS;
  dipdw.diph.dwObj = 0;

  JoystickInput[CurrentJoy]->SetProperty(DIPROP_AXISMODE, &dipdw.diph);

  CurrentJoy += 1;

  return DIENUM_CONTINUE;
}

void ReleaseDirectInput()
{
  if (MouseInput)
  {
    MouseInput->Release();
    MouseInput = NULL;
  }

  if (KeyboardInput)
  {
    KeyboardInput->Release();
    KeyboardInput = NULL;
  }

  for (int i = 0; i < 5; i++)
  {
    if (JoystickInput[i])
    {
      JoystickInput[i]->Release();
      JoystickInput[i] = NULL;
    }
  }

  if (DInput)
  {
    DInput->Release();
    DInput = NULL;
  }
}

void ReleaseDirectSound()
{
  if (lpSoundBuffer)
  {
    lpSoundBuffer->Release();
    lpSoundBuffer = NULL;
  }

  if (lpPrimaryBuffer)
  {
    lpPrimaryBuffer->Release();
    lpPrimaryBuffer = NULL;
  }

  if (lpDirectSound)
  {
    lpDirectSound->Release();
    lpDirectSound = NULL;
  }
}

void DInputError()
{
  char message1[256];

  sprintf(message1,
          "Error initializing DirectInput\nYou may need to install DirectX 8.0a or higher located at www.microsoft.com/directx%c",
          0);
  MessageBox(NULL, message1, "DirectInput Error", MB_ICONERROR);
}

bool InitInput()
{
  char message1[256];
  HRESULT hr;

  if (FAILED(hr = pDirectInput8Create(hInst, DIRECTINPUT_VERSION, IID_IDirectInput8A, (void**)&DInput,
                                      NULL)))
  {
    sprintf(message1,
            "Error initializing DirectInput\nYou may need to install DirectX 8.0a or higher located at www.microsoft.com/directx%c",
            0);
    MessageBox(NULL, message1, "DirectInput Error", MB_ICONERROR);

    switch (hr)
    {
      case DIERR_BETADIRECTINPUTVERSION:
        sprintf(message1, "Beta %X\n", (unsigned int) hr);
        MessageBox(NULL, message1, "Init", MB_ICONERROR);
        break;
      case DIERR_INVALIDPARAM:
        sprintf(message1, "Invalid %X\n", (unsigned int) hr);
        MessageBox(NULL, message1, "Init", MB_ICONERROR);
        break;
      case DIERR_OLDDIRECTINPUTVERSION:
        sprintf(message1, "OLDDIRECTINPUTVERSION %X\n", (unsigned int) hr);
        MessageBox(NULL, message1, "Init", MB_ICONERROR);
        break;
      case DIERR_OUTOFMEMORY:
        sprintf(message1, "OUTOFMEMORY %X\n", (unsigned int) hr);
        MessageBox(NULL, message1, "Init", MB_ICONERROR);
        break;
      default:
        sprintf(message1, "UNKNOWN %X\n", (unsigned int) hr);
        MessageBox(NULL, message1, "Init", MB_ICONERROR);
        break;
    }
    return FALSE;
  }

  hr = DInput->CreateDevice(GUID_SysKeyboard, &KeyboardInput, NULL);
  if (FAILED(hr))
  {
    DInputError();return FALSE;
  }

  hr = KeyboardInput->SetDataFormat(&c_dfDIKeyboard);
  if (FAILED(hr))
  {
    DInputError();return FALSE;
  }

  hr = KeyboardInput->SetCooperativeLevel(hMainWindow, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);

  hr = DInput->CreateDevice(GUID_SysMouse, &MouseInput, NULL);
  if (FAILED(hr))
  {
    DInputError();return FALSE;
  }

  hr = MouseInput->SetDataFormat(&c_dfDIMouse);
  if (FAILED(hr))
  {
    DInputError();return FALSE;
  }

  hr = MouseInput->SetCooperativeLevel(hMainWindow, DISCL_EXCLUSIVE | DISCL_FOREGROUND);
  if (FAILED(hr))
  {
    DInputError();return FALSE;
  }

  JoystickInput[0] = NULL;JoystickInput[1] = NULL;JoystickInput[2] = NULL;JoystickInput[3] = NULL;JoystickInput[4] =
  NULL;

  hr = DInput->EnumDevices(DI8DEVCLASS_GAMECTRL, InitJoystickInput, DInput, DIEDFL_ATTACHEDONLY);

  if (FAILED(hr))
  {
    DInputError(); return FALSE;
  }

  InputAcquire();

  return TRUE;
}

void TestJoy()
{
  int i;

  for (i = 0; i < 5; i++)
  {
    if (JoystickInput[i])
    {
      JoystickInput[i]->Poll();

      if (JoystickInput[i]->GetDeviceState(sizeof(DIJOYSTATE), &js[i]) == DIERR_INPUTLOST)
      {
        if (JoystickInput[i])
        {
          JoystickInput[i]->Acquire();
        }
        if (FAILED(JoystickInput[i]->GetDeviceState(sizeof(DIJOYSTATE), &js[i])))
        {
          return;
        }
      }

      if (!X1Disable[i] && (js[i].lX > 0))
      {
        X1Disable[i] = 1;
      }

      if (!X2Disable[i] && (js[i].lX < 0))
      {
        X2Disable[i] = 1;
      }

      if (!Y1Disable[i] && (js[i].lY > 0))
      {
        Y1Disable[i] = 1;
      }

      if (!Y2Disable[i] && (js[i].lY < 0))
      {
        Y2Disable[i] = 1;
      }

      if (!Z1Disable[i] && (js[i].lZ > 0))
      {
        Z1Disable[i] = 1;
      }

      if (!Z2Disable[i] && (js[i].lZ < 0))
      {
        Z2Disable[i] = 1;
      }

      if (!RY1Disable[i] && (js[i].lRy > 0))
      {
        RY1Disable[i] = 1;
      }

      if (!RY2Disable[i] && (js[i].lRy < 0))
      {
        RY2Disable[i] = 1;
      }

      if (!RZ1Disable[i] && (js[i].lRz > 0))
      {
        RZ1Disable[i] = 1;
      }

      if (!RZ2Disable[i] &&(js[i].lRz < 0))
      {
        RZ2Disable[i] = 1;
      }

      if (!S01Disable[i] && (js[i].rglSlider[0] > 0))
      {
        S01Disable[i] = 1;
      }

      if (!S02Disable[i] && (js[i].rglSlider[0] < 0))
      {
        S02Disable[i] = 1;
      }

      if (!S11Disable[i] && (js[i].rglSlider[1] > 0))
      {
        S11Disable[i] = 1;
      }

      if (!S12Disable[i] && (js[i].rglSlider[1] < 0))
      {
        S12Disable[i] = 1;
      }
    }
  }
}

extern "C"
{
  //BYTE changeRes = 1;
  extern DWORD converta;
  extern unsigned int BitConv32Ptr;
  extern unsigned int RGBtoYUVPtr;
  extern unsigned short resolutn;
  extern BYTE GUIWFVID[];
  extern BYTE GUIDSIZE[];
  extern BYTE GUISMODE[];
  extern BYTE GUIDSMODE[];
  extern BYTE GUIHQ2X[];
  extern BYTE GUIHQ3X[];
  extern BYTE GUIHQ4X[];
  extern BYTE GUINTVID[];
  extern BYTE hqFilterlevel;
  BYTE changeRes = 1;
}

DWORD FirstVid = 1;
DWORD FirstFull = 1;
DWORD SMode = 0;
DWORD NTSCMode = 0;
DWORD prevHQMode = ~0;
DWORD prevNTSCMode = 0;
DWORD prevScanlines = ~0;

DWORD LockSurface();
void UnlockSurface();
void clear_ddraw();

//The big extern
extern "C"
{
  void WinUpdateDevices();
  char CheckOGLMode();
  BYTE *SurfBuf;
  DWORD DMode = 0;
  DWORD DSMode = 0;
  WORD Refresh = 0;

  short Buffer[1800 * 2];

  int X, Y;
  DWORD pitch;
  MSG msg;
  DWORD SurfBufD;
  int count, x, count2;
  HRESULT hr;
  int i;
  short *Sound;
  DWORD CurrentPos;
  DWORD WritePos;
  DWORD T60HZEnabled = 0;
  DWORD T36HZEnabled = 0;

  DWORD WINAPI SemaphoreThread(LPVOID lpParam)
  {
    while (semaphore_run)
    {
      if (T60HZEnabled)
      {
        ReleaseSemaphore(hLock, 1, NULL);
        Sleep(1);
      }
      else
      {
        Sleep(20);
      }
    }
    return 0;
  }

  void InitSemaphore()
  {
    if (!hLock)
    {
      hLock = CreateSemaphore(NULL, 1, SemaphoreMax, NULL);

      semaphore_run = 1;

      hThread = CreateThread(NULL, 0, SemaphoreThread, &dwThreadParam, 0, &dwThreadId);
    }
  }

  void ShutdownSemaphore()
  {
    if (hLock)
    {
      semaphore_run = 0;

      WaitForSingleObject(hThread, INFINITE);
      CloseHandle(hThread);

      CloseHandle(hLock);

      hLock = NULL;
    }
  }

  extern unsigned int pressed;
  extern unsigned char romispal;

  void Start60HZ()
  {
    update_ticks_pc2 = UPDATE_TICKS_UDP * freq / 1000.0;

    if (romispal == 1)
    {
      update_ticks_pc = UPDATE_TICKS_GAMEPAL * freq / 1000.0;
    }
    else
    {
      update_ticks_pc = UPDATE_TICKS_GAME * freq / 1000.0;
    }

    QueryPerformanceCounter((LARGE_INTEGER *)&start);
    QueryPerformanceCounter((LARGE_INTEGER *)&start2);

    T36HZEnabled = 0;
    T60HZEnabled = 1;

    InitSemaphore();

    if (device1 && device2)
    {
      MouseInput->Unacquire();
      MultiMouseInit();
    }

    //if (!device1 && !device2) MouseInput->Unacquire();
  }

  void Stop60HZ()
  {
    T60HZEnabled = 0;

    if (device1 && device2)
    {
      MultiMouseShutdown();
    }

    MouseInput->Acquire();

    ShutdownSemaphore();
  }

  void Start36HZ()
  {
    update_ticks_pc2 = UPDATE_TICKS_UDP * freq / 1000.0;
    update_ticks_pc = UPDATE_TICKS_GUI * freq / 1000.0;

    QueryPerformanceCounter((LARGE_INTEGER *)&start);
    QueryPerformanceCounter((LARGE_INTEGER *)&start2);

    T60HZEnabled = 0;
    T36HZEnabled = 1;
  }

  void Stop36HZ()
  {
    T36HZEnabled = 0;
  }

  char WinMessage[256];
  void clearwin();

  char WinName[] = { "ZSNESW\0" };
  void NTSCFilterInit();
  void NTSCFilterDraw(int SurfaceX, int SurfaceY, int pitch, unsigned char *buffer);

  extern "C" char GUIM7VID[];

  void SetHiresOpt()
  {
    if (CustomResX >= 512 && CustomResY >= 448)
    {
      GUIM7VID[cvidmode] = 1;
    }
    else
    {
      GUIM7VID[cvidmode] = 0;
    }
  }

  void KeepTVRatio()
  {
    int ratiox = WindowWidth*3;
    int ratioy = WindowHeight*4;

    int marginchange;
    int marginmod;

    if (ratiox < ratioy)
    {
      marginchange = (WindowHeight - (ratiox / 4)) / 2;
      marginmod = (WindowHeight - (ratiox / 4)) % 2;
      rcWindow.top += marginchange;
      rcWindow.bottom -= (marginchange + marginmod);
    }
    else if (ratiox > ratioy)
    {
      marginchange = (WindowWidth - (ratioy / 3)) / 2;
      marginmod = (WindowWidth - (ratioy / 3)) % 2;
      rcWindow.left += marginchange;
      rcWindow.right -= (marginchange + marginmod);
    }
  }

  extern char GUIKEEP43[];

  char CheckTVRatioReq()
  {
    return(GUIKEEP43[cvidmode] && Keep4_3Ratio);
  }

  WINDOWPLACEMENT wndpl;
  RECT rc1;
  DWORD newmode = 0;

  void initwinvideo()
  {
    DWORD HQMode = 0;
    newmode = 0;

    if (FirstActivate && NTSCFilter)
    {
      NTSCFilterInit();
    }

    if (cvidmode == 37 || cvidmode == 38 || cvidmode == 41)
    {
      SetHiresOpt();
    }

    if (hqFilter != 0)
    {
      if ((GUIHQ2X[cvidmode] != 0) && (hqFilterlevel == 2))
      {
        HQMode = 2;
      }
      if ((GUIHQ3X[cvidmode] != 0) && (hqFilterlevel == 3))
      {
        HQMode = 3;
      }
      if ((GUIHQ4X[cvidmode] != 0) && (hqFilterlevel == 4))
      {
        HQMode = 4;
      }
    }

    if ((CurMode != cvidmode) || (prevHQMode != HQMode) || (prevNTSCMode != NTSCFilter) ||
        (changeRes))
    {
      CurMode = cvidmode;
      prevHQMode = HQMode;
      prevNTSCMode = NTSCFilter;
      changeRes = 0;
      newmode = 1;
      SurfaceX = 256;
      SurfaceY = 240;
      X = 0;
      Y = 0;
      FullScreen = GUIWFVID[cvidmode];
      DMode = GUIDSIZE[cvidmode];
      SMode = GUISMODE[cvidmode];
      DSMode = GUIDSMODE[cvidmode];
      NTSCMode = GUINTVID[cvidmode];

      switch (cvidmode)
      {
        case 0:
          WindowWidth = 256;
          WindowHeight = 224;
          break;
        case 1:
          WindowWidth = 640;
          WindowHeight = 480;
          break;
        case 2:
        case 3:
        case 43:
          WindowWidth = 512;
          WindowHeight = 448;
          break;
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 44:
        case 45:
          WindowWidth = 640;
          WindowHeight = 480;
          break;
        case 9:
        case 10:
        case 46:
          WindowWidth = 768;
          WindowHeight = 672;
          break;
        case 11:
        case 12:
        case 13:
        case 14:
        case 15:
        case 47:
        case 48:
          WindowWidth = 800;
          WindowHeight = 600;
          break;
        case 16:
        case 17:
        case 18:
        case 19:
        case 20:
        case 49:
        case 50:
          WindowWidth = 1024;
          WindowHeight = 768;
          break;
        case 21:
        case 22:
        case 51:
          WindowWidth = 1024;
          WindowHeight = 896;
          break;
        case 23:
        case 24:
        case 25:
        case 26:
        case 27:
        case 52:
        case 53:
          WindowWidth = 1280;
          WindowHeight = 960;
          break;
        case 28:
        case 29:
        case 30:
        case 31:
        case 32:
        case 54:
        case 55:
          WindowWidth = 1280;
          WindowHeight = 1024;
          break;
        case 33:
        case 34:
        case 35:
        case 36:
        case 37:
        case 56:
        case 57:
          WindowWidth = 1600;
          WindowHeight = 1200;
          break;
        case 38:
        case 39:
        case 40:
        case 41:
        case 42:
        case 58:
        case 59:
          WindowWidth = CustomResX;
          WindowHeight = CustomResY;
          break;
        default:
          WindowWidth = 256;
          WindowHeight = 224;
          break;
      }

      if (DMode == 1)
      {
        if ((DSMode == 1) || (FullScreen == 0))
        {
          SurfaceX = 512;
        }
        else
        {
          SurfaceX = 640;
        }

        SurfaceY = 480;

        if (NTSCMode && NTSCFilter)
        {
          SurfaceX = 602;
          SurfaceY = 446;
        }
      }
      else
      {
        if ((SMode == 0) && (FullScreen == 1))
        {
          SurfaceX = 320;
        }
        else
        {
          SurfaceX = 256;
        }
        SurfaceY = 240;
      }

      switch (HQMode)
      {
        case 2:
          SurfaceX = 512;
          SurfaceY = 480;
          break;
        case 3:
          SurfaceX = 768;
          SurfaceY = 720;
          break;
        case 4:
          SurfaceX = 1024;
          SurfaceY = 960;
          break;
      }

      BlitArea.top = 0;
      BlitArea.left = 0;
      BlitArea.right = SurfaceX;

      if (PrevRes == 0)
      {
        PrevRes = resolutn;
      }
    }

    if (((PrevStereoSound != StereoSound) || (PrevSoundQuality != SoundQuality)) && FirstSound != 1)
    {
      ReInitSound();
    }

    if (!FirstVid)
    {
      /*
      if (X<0)X=0;
      if (X>(int)(GetSystemMetrics(SM_CXSCREEN) - WindowWidth)) X=(GetSystemMetrics(SM_CXSCREEN) - WindowWidth);
      if (Y<0)Y=0;
      if (Y>(int)(GetSystemMetrics(SM_CYSCREEN) - WindowHeight)) Y=(GetSystemMetrics(SM_CYSCREEN) - WindowHeight);
      */

      if (FullScreen == 1)
      {
        X = 0; Y = 0;
      }

      if (FullScreen == 0 && newmode == 1)
      {
        X = MainWindowX; Y = MainWindowY;
      }
      else if (FullScreen == 0)
      {
        MainWindowX = X; MainWindowY = Y;
      }

      MoveWindow(hMainWindow, X, Y, WindowWidth, WindowHeight, TRUE);

	  wndpl.length = sizeof(wndpl);
      GetWindowPlacement(hMainWindow, &wndpl);
      SetRect(&rc1, 0, 0, WindowWidth, WindowHeight);

      AdjustWindowRectEx(&rc1, GetWindowLong(hMainWindow, GWL_STYLE), GetMenu(hMainWindow) != NULL,
                         GetWindowLong(hMainWindow, GWL_EXSTYLE));

      GetClientRect(hMainWindow, &rcWindow);
      ClientToScreen(hMainWindow, (LPPOINT)&rcWindow);
      ClientToScreen(hMainWindow, (LPPOINT)&rcWindow + 1);

	  if (debugger) DockDebugger();

	}
    else
    {
      atexit(ExitFunction);

      if (!QueryPerformanceFrequency((LARGE_INTEGER *)&freq))
      {
        return;
      }

      if (!RegisterWinClass())
      {
        zexit_error();
      }
      X = (GetSystemMetrics(SM_CXSCREEN) - WindowWidth) / 2;
      Y = (GetSystemMetrics(SM_CYSCREEN) - WindowHeight) / 2;

      if (FullScreen == 1)
      {
        X = 0; Y = 0;
      }

      if (hMainWindow)
      {
        CloseWindow(hMainWindow);
      }

      if (SaveMainWindowPos == 1 && MainWindowX != -1 && FullScreen == 0)
      {
        X = MainWindowX; Y = MainWindowY;
      }

      hMainWindow = CreateWindow("ZSNES", WinName, WS_VISIBLE | WS_POPUP, X, Y,
                                 //WS_OVERLAPPED "ZSNES"
                                 WindowWidth, WindowHeight, NULL, NULL, hInst, NULL);

      if (!hMainWindow)
      {
        return;
      }

      // Hide the cursor
      ShowCursor(0);

      // Set window attributes
      ShowWindow(hMainWindow, SW_SHOWNORMAL);
      SetWindowText(hMainWindow, "ZSNES");

      // Run ZSNES Windows GUI callback functions to set initial values
      CheckPriority();
      CheckAlwaysOnTop();
      CheckScreenSaver();

      // Init various DirectX subsystems
      InitInput();
      InitSound();
      TestJoy();

      if (debugger) DockDebugger();
    }

    if (FirstVid == 1)
    {
      FirstVid = 0;
      if (KitchenSync)
      {
        Refresh = totlines == 263 ? 120 : 100;
      }
      else if (KitchenSyncPAL && totlines == 314)
      {
        Refresh = 100;
      }
      else if (ForceRefreshRate)
      {
        Refresh = SetRefreshRate;
      }
      InitDirectDraw();
      clearwin();
      Clear2xSaIBuffer();
      clear_display();
    }
    else if (newmode == 1 && Moving != 1)
    {
      ReleaseDirectDraw();
      InitDirectDraw();
      if (CheckTVRatioReq())
      {
        KeepTVRatio();
      }
      clearwin();
      Clear2xSaIBuffer();
      clear_display();
    }

    if (CheckOGLMode()) gl_start(WindowWidth, WindowHeight, 16, FullScreen);
  }

  extern unsigned int vidbuffer;
  extern void SoundProcess();
  extern int DSPBuffer;
  int *DSPBuffer1;
  DWORD ScreenPtr;
  DWORD ScreenPtr2;
  void GUI36hzcall();
  void Game60hzcall();

static int ds_buffer_size = 0;               //size in bytes of the direct sound buffer
static int ds_write_offset = 0;              //offset of the write cursor in the direct sound buffer
static int ds_min_free_space = 0;            //if the free space is below this value get_space() will return 0

  static int ds_write_buffer(unsigned char *data, int len)
{
  HRESULT res;
  LPVOID lpvPtr1;
  DWORD dwBytes1;
  LPVOID lpvPtr2;
  DWORD dwBytes2;

  // Lock the buffer
  res = lpSoundBuffer->Lock(ds_write_offset, len, &lpvPtr1, &dwBytes1, &lpvPtr2, &dwBytes2, 0);
  // If the buffer was lost, restore and retry lock.
  if (res == DSERR_BUFFERLOST)
  {
    lpSoundBuffer->Restore();
    res = lpSoundBuffer->Lock(ds_write_offset, len, &lpvPtr1, &dwBytes1, &lpvPtr2, &dwBytes2, 0);
  }

  if (SUCCEEDED(res))
  {
    // Write to pointers without reordering.
    memcpy(lpvPtr1, data, dwBytes1);
    if (lpvPtr2) { memcpy(lpvPtr2, data+dwBytes1, dwBytes2); }
    ds_write_offset += dwBytes1+dwBytes2;
    if (ds_write_offset >= ds_buffer_size) { ds_write_offset = dwBytes2; }

   // Release the data back to DirectSound.
    res = lpSoundBuffer->Unlock(lpvPtr1, dwBytes1, lpvPtr2, dwBytes2);
    if (SUCCEEDED(res))
    {
      // Success.
      DWORD status;
      lpSoundBuffer->GetStatus(&status);
      if (!(status & DSBSTATUS_PLAYING))
      {
        lpSoundBuffer->Play(0, 0, DSBPLAY_LOOPING);
      }
      return(dwBytes1+dwBytes2);
    }
  }
  // Lock, Unlock, or Restore failed.
  return(0);
}

static bool ds_play(char *samples_buffer, size_t samples_count)
{
  unsigned char *data = (unsigned char *)samples_buffer;
  int samples_outputted, samples_remaining;

  samples_remaining = samples_count;
  for (;;)
  {
    DWORD play_offset;
    int space, len = samples_remaining;

    // make sure we have enough space to write data
    lpSoundBuffer->GetCurrentPosition(&play_offset, 0);
    space = ds_buffer_size-(ds_write_offset-play_offset);
    if (space > ds_buffer_size) { space -= ds_buffer_size; } // ds_write_offset < play_offset
    if (space < len) { len = space; }

    samples_outputted = ds_write_buffer((unsigned char *)data, len);

    data += samples_outputted;
    samples_remaining -= samples_outputted;
    if (samples_outputted < samples_remaining) { Sleep(500); }
    else { break; }
  }
  return(true);
}

void CheckTimers()
  {
    // Lame fix for GUI using 100% CPU
    if (GUIOn || GUIOn2 || EMUPause)
    {
      Sleep(1);
    }

    QueryPerformanceCounter((LARGE_INTEGER *)&end2);

    while ((end2 - start2) >= update_ticks_pc2)
    {
      start2 += update_ticks_pc2;
    }

    if (T60HZEnabled == 1)
    {
      QueryPerformanceCounter((LARGE_INTEGER *)&end);

      while ((end - start) >= update_ticks_pc)
      {
        Game60hzcall();
        start += update_ticks_pc;
      }
    }

    if (T36HZEnabled == 1)
    {
      QueryPerformanceCounter((LARGE_INTEGER *)&end);

      while ((end - start) >= update_ticks_pc)
      {
        GUI36hzcall();
        start += update_ticks_pc;
      }
    }
  }

  volatile int SPCSize;
  volatile int buffer_ptr;
  extern unsigned char MMXSupport;

  void UpdateVFrame()
  {
    int DataNeeded;
    SPCSize = 256;

    //if (StereoSound==1) SPCSize=256;

    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }

    WinUpdateDevices();
    CheckTimers();

    if (SoundEnabled == 0)
    {
      return;
    }

    if (!UsePrimaryBuffer)
    {
      lpSoundBuffer->GetCurrentPosition(&CurrentPos, &WritePos);
    }
    else
    {
      lpPrimaryBuffer->GetCurrentPosition(&CurrentPos, &WritePos);
    }
  }

  extern unsigned char curblank;
  extern DWORD AddEndBytes;
  extern DWORD NumBytesPerLine;
  extern unsigned char *WinVidMemStart;
  void copy640x480x16bwin();
  void hq2x_16b();
  void hq2x_32b();
  void hq3x_16b();
  void hq3x_32b();
  void hq4x_16b();
  void hq4x_32b();
  extern unsigned char NGNoTransp;
  void ClearWin16();
  void ClearWin32();

  void clearwin()
  {
    pitch = LockSurface();
    if (pitch == 0)
    {
      return;
    }

    SurfBufD = (DWORD)&SurfBuf[0];

    if (AltSurface == 0)
    {
      switch (BitDepth)
      {
        case 16:
          ClearWin16();
          break;
        case 32:
          ClearWin32();
          break;
      }
    }
    else
    {
      ClearWin16();
    }

    UnlockSurface();
  }

  void initDirectDraw()
  {
    InitDirectDraw();
    if (CheckTVRatioReq())
    {
      KeepTVRatio();
    }
    clearwin();
    Clear2xSaIBuffer();
    clear_display();
  }

  void clear_display()
  {
    if(!CheckOGLMode())
    {
      clear_ddraw();
    }
  }

  void DrawWin256x224x16();
  void DrawWin256x224x32();
  void DrawWin320x240x16();

  volatile uint64_t copymaskRB = UINT64_C(0x001FF800001FF800);
  volatile uint64_t copymaskG = UINT64_C(0x0000FC000000FC00);
  volatile uint64_t copymagic = UINT64_C(0x0008010000080100);

  void drawscreenwin()
  {
    DWORD i, j, color32;
    DWORD *SURFDW;

    NGNoTransp = 0; // Set this value to 1 within the appropriate
                    // video mode if you want to add a custom
                    // transparency routine or hardware
                    // transparency.  This only works if
                    // the value of newengen is equal to 1.
                    // (see ProcessTransparencies in newgfx16.asm
                    //  for ZSNES' current transparency code)

    UpdateVFrame();
    if (curblank != 0)
    {
      return;
    }

    if (!(pitch = LockSurface()))
    {
      return;
    }

    ScreenPtr = vidbuffer;
    ScreenPtr += 16 * 2 + 32 * 2 + 256 * 2;

    DWORD HQMode = 0;

    if (MMXSupport == 0)
    {
      hqFilter = 0;
    }

    if (hqFilter != 0)
    {
      if ((GUIHQ2X[cvidmode] != 0) && (hqFilterlevel == 2))
      {
        HQMode = 2;
      }
      if ((GUIHQ3X[cvidmode] != 0) && (hqFilterlevel == 3))
      {
        HQMode = 3;
      }
      if ((GUIHQ4X[cvidmode] != 0) && (hqFilterlevel == 4))
      {
        HQMode = 4;
      }
    }

    if (PrevRes != resolutn)
    {
      if ((SurfaceX == 640) || (SurfaceX == 320))
      {
        BlitArea.bottom = SurfaceY;
      }
      else if (!NTSCFilter)
      {
        BlitArea.bottom = (SurfaceY / 240) * resolutn;
      }

      if ((FullScreen == 0) && (SMode == 0) && (DSMode == 0) && !NTSCFilter)
      {
        WindowHeight = (WindowHeight / 224) * resolutn;
      }

      initwinvideo();
      PrevRes = resolutn;
    }

    if (prevHQMode != HQMode)
    {
      initwinvideo();
    }

    if (prevNTSCMode != NTSCFilter)
    {
      initwinvideo();
    }

    if (prevScanlines != scanlines)
    {
      initwinvideo();
      prevScanlines = scanlines;
    }

    if (changeRes)
    {
      initwinvideo();
    }

    SurfBufD = (DWORD)&SurfBuf[0];
    SURFDW = (DWORD *)&SurfBuf[0];

    if (!(KitchenSync || (KitchenSyncPAL && totlines == 314)) && Refresh != 0 && !ForceRefreshRate)
    {
      Refresh = 0;
      ReleaseDirectDraw();
      InitDirectDraw();
      clearwin();
      Clear2xSaIBuffer();
      clear_display();
    }

    if (KitchenSync && Refresh != 120 && totlines == 263)
    {
      Refresh = 120;
      ReleaseDirectDraw();
      InitDirectDraw();
      clearwin();
      Clear2xSaIBuffer();
      clear_display();
    }

    if ((KitchenSync || KitchenSyncPAL) && Refresh != 100 && totlines == 314)
    {
      Refresh = 100;
      ReleaseDirectDraw();
      InitDirectDraw();
      clearwin();
      Clear2xSaIBuffer();
      clear_display();
    }

    if (!KitchenSync && KitchenSyncPAL && totlines == 263 && Refresh != SetRefreshRate &&
        ForceRefreshRate)
    {
      Refresh = SetRefreshRate;
      ReleaseDirectDraw();
      InitDirectDraw();
      clearwin();
      Clear2xSaIBuffer();
      clear_display();
    }

    if (!HQMode)
    {
      if (SurfaceX == 256 && SurfaceY == 240)
      {
        switch (BitDepth)
        {
          case 16:
            {
              DrawWin256x224x16();
              break;
            }
          case 32:
            {
              DrawWin256x224x32();
              break;
            }
          case 24:
            MessageBox(NULL,
                       "Sorry.  ZSNESw does not work in windowed 24 bit color modes. \nClick 'OK' to switch to a full screen mode.",
                       "DDRAW Error", MB_ICONERROR);
            cvidmode = 3;
            initwinvideo();
            Sleep(1000);
            drawscreenwin();
            break;
          default:
            UnlockSurface();
            MessageBox(NULL, "Mode only available in 16 and 32 bit color", "DDRAW Error",
                       MB_ICONERROR);
            cvidmode = 2;
            initwinvideo();
            Sleep(1000);
            drawscreenwin();
            break;
        }
      }

      if (SurfaceX == 320 && SurfaceY == 240)
      {
        switch (BitDepth)
        {
          case 16:
            {
              DrawWin320x240x16();
              break;
            }
          case 32:
            for (j = 0; j < 8; j++)
            {
              SURFDW = (DWORD *)&SurfBuf[j * pitch];
              color32 = 0x7F000000;

              for (i = 0; i < 320; i++)
              {
                SURFDW[i] = color32;
              }
            }

            for (j = 8; (int)j < (resolutn - 1) + 8; j++)
            {
              color32 = 0x7F000000;
              for (i = 0; i < 32; i++)
              {
                SURFDW[i] = color32;
              }

              for (i = 32; i < (256 + 32); i++)
              {
                color32 = (((*(WORD *)(ScreenPtr)) & 0xF800) << 8) +
                          (((*(WORD *)(ScreenPtr)) & 0x07E0) << 5) +
                          (((*(WORD *)(ScreenPtr)) & 0x001F) << 3) + 0xFF000000;
                //SURFDW[i]=color32;
                ScreenPtr += 2;
              }

              color32 = 0x7F000000;
              for (i = (256 + 32); i < 320; i++)
              {
                SURFDW[i] = color32;
              }

              ScreenPtr = ScreenPtr + 576 - 512;
              SURFDW = (DWORD *)&SurfBuf[(j)*pitch];
            }

            for (j = ((resolutn - 1) + 8); j < 240; j++)
            {
              SURFDW = (DWORD *)&SurfBuf[j * pitch];

              color32 = 0x7F000000;
              for (i = 0; i < 320; i++)
              {
                SURFDW[i] = color32;
              }
            }
            break;
          default:
            UnlockSurface();
            MessageBox(NULL, "Mode only available in 16 and 32 bit color", "DDRAW Error",
                       MB_ICONERROR);
            cvidmode = 2;
            initwinvideo();
            Sleep(1000);
            drawscreenwin();
            break;
        }
      }

      if (SurfaceX == 512 && SurfaceY == 480)
      {
        switch (BitDepth)
        {
          case 16:
          case 32:
            // using 16bpp AltSurface
            AddEndBytes = pitch - 1024;
            NumBytesPerLine = pitch;
            WinVidMemStart = &SurfBuf[0];
            asm_call(copy640x480x16bwin);
            break;
          default:
            UnlockSurface();
            MessageBox(NULL, "Mode only available in 16 and 32 bit color", "DDRAW Error",
                       MB_ICONERROR);
            cvidmode = 2;
            initwinvideo();
            Sleep(1000);
            drawscreenwin();
        }
      }

      if (SurfaceX == 640 && SurfaceY == 480)
      {
        switch (BitDepth)
        {
          case 16:
          case 32:
            // using 16bpp AltSurface
            AddEndBytes = pitch - 1024;
            NumBytesPerLine = pitch;
            WinVidMemStart = &SurfBuf[(240 - resolutn) * pitch + 64 * 2];
            asm_call(copy640x480x16bwin);
            break;
          default:
            UnlockSurface();
            MessageBox(NULL, "Mode only available in 16 and 32 bit color", "DDRAW Error",
                       MB_ICONERROR);
            cvidmode = 2;
            initwinvideo();
            Sleep(1000);
            drawscreenwin();
        }
      }
      if (SurfaceX == 602 && SurfaceY == 446)
      {
        switch (BitDepth)
        {
          case 16:
          case 32:
            // using 16bpp AltSurface
            AddEndBytes = pitch - 1024;
            NumBytesPerLine = pitch;
            WinVidMemStart = &SurfBuf[0];
            NTSCFilterDraw(SurfaceX, SurfaceY, pitch, WinVidMemStart);
            break;
          default:
            UnlockSurface();
            MessageBox(NULL, "Mode only available in 16 and 32 bit color", "DDRAW Error",
                       MB_ICONERROR);
            cvidmode = 2;
            initwinvideo();
            Sleep(1000);
            drawscreenwin();
        }
      }
    }
    else
    {
      // HQMode != 0
      NumBytesPerLine = pitch;
      AddEndBytes = pitch - SurfaceX * (BitDepth / 8);
      WinVidMemStart = &SurfBuf[0];
      switch (BitDepth)
      {
        case 16:
          {
            switch (HQMode)
            {
              case 2:
                hq2x_16b(); break;
              case 3:
                hq3x_16b(); break;
              case 4:
                hq4x_16b(); break;
            }
            break;
          }
        case 32:
          {
            switch (HQMode)
            {
              case 2:
                hq2x_32b(); break;
              case 3:
                hq3x_32b(); break;
              case 4:
                hq4x_32b(); break;
            }
            break;
          }
        default:
          {
            UnlockSurface();
            MessageBox(NULL, "Mode only available in 16 and 32 bit color", "DDRAW Error",
                       MB_ICONERROR);
            cvidmode = 2;
            initwinvideo();
            Sleep(1000);
            drawscreenwin();
          }
      }
    }
    UnlockSurface();
    if (CheckOGLMode()) gl_drawwin();
    else DrawScreen();
  }

  void SwitchFullScreen();
  void WriteLine();

  void WinUpdateDevices()
  {
    int i, j;
    unsigned char *keys;
    unsigned char keys2[256];
    for (i = 0; i < 256; i++)
    {
      keys2[i] = 0;
    }
    keys = (unsigned char*)&pressed;
WriteLine();
    if (KeyboardInput && InputEn == 1)
    {
      KeyboardInput->GetDeviceState(256, keys2);
    }
    else
    {
      return;
    }
    if (keys2[0x38] != 0 && keys2[0x3E] != 0)
    {
      zexit();
    }
    if (keys2[0xB8] != 0 && keys2[0x1C] != 0 || keys2[0x38] != 0 && keys2[0x1C] != 0)
    {
      asm_call(SwitchFullScreen);
      return;
    }

    for (i = 0; i < 256; i++)
    {
      if (keys2[i] == 0)
      {
        keys[i] = 0;
      }
      if (keys2[i] != 0 && keys[i] == 0)
      {
        keys[i] = 1;
      }
    }

    keys[0] = 0;

    for (i = 0; i < 5; i++)
    {
      if (JoystickInput[i])
      {
        for (j = 0; j < 32; j++)
        {
          keys[0x100 + i * 32 + j] = 0;
        }

        JoystickInput[i]->Poll();

        if (JoystickInput[i]->GetDeviceState(sizeof(DIJOYSTATE), &js[i]) == DIERR_INPUTLOST)
        {
          if (JoystickInput[i])
          {
            JoystickInput[i]->Acquire();
          }
          if (FAILED(JoystickInput[i]->GetDeviceState(sizeof(DIJOYSTATE), &js[i])))
          {
            return;
          }
        }

        if (!X1Disable[i])
        {
          if (js[i].lX > 0)
          {
            keys[0x100 + i * 32 + 0] = 1;
          }
        }

        if (!X2Disable[i])
        {
          if (js[i].lX < 0)
          {
            keys[0x100 + i * 32 + 1] = 1;
          }
        }

        if (!Y1Disable[i])
        {
          if (js[i].lY > 0)
          {
            keys[0x100 + i * 32 + 2] = 1;
          }
        }

        if (!Y2Disable[i])
        {
          if (js[i].lY < 0)
          {
            keys[0x100 + i * 32 + 3] = 1;
          }
        }

        if (!Z1Disable[i])
        {
          if (js[i].lZ > 0)
          {
            keys[0x100 + i * 32 + 4] = 1;
          }
        }

        if (!Z2Disable[i])
        {
          if (js[i].lZ < 0)
          {
            keys[0x100 + i * 32 + 5] = 1;
          }
        }

        if (!RY1Disable[i])
        {
          if (js[i].lRy > 0)
          {
            keys[0x100 + i * 32 + 6] = 1;
          }
        }

        if (!RY2Disable[i])
        {
          if (js[i].lRy < 0)
          {
            keys[0x100 + i * 32 + 7] = 1;
          }
        }

        if (!RZ1Disable[i])
        {
          if (js[i].lRz > 0)
          {
            keys[0x100 + i * 32 + 8] = 1;
          }
        }

        if (!RZ2Disable[i])
        {
          if (js[i].lRz < 0)
          {
            keys[0x100 + i * 32 + 9] = 1;
          }
        }

        if (!S01Disable[i])
        {
          if (js[i].rglSlider[0] > 0)
          {
            keys[0x100 + i * 32 + 10] = 1;
          }
        }

        if (!S02Disable[i])
        {
          if (js[i].rglSlider[0] < 0)
          {
            keys[0x100 + i * 32 + 11] = 1;
          }
        }

        if (!S11Disable[i])
        {
          if (js[i].rglSlider[1] > 0)
          {
            keys[0x100 + i * 32 + 12] = 1;
          }
        }

        if (!S12Disable[i])
        {
          if (js[i].rglSlider[1] < 0)
          {
            keys[0x100 + i * 32 + 13] = 1;
          }
        }

        if (!POVDisable[i])
        {
          for (int p = 0; (unsigned long) p < NumPOV[i]; p++)
          {
            switch (js[i].rgdwPOV[p])
            {
              case 0:
                keys[0x100 + i * 32 + 3] = 1;
                break;
              case 4500:
                keys[0x100 + i * 32 + 0] = 1;
                keys[0x100 + i * 32 + 3] = 1;
                break;
              case 9000:
                keys[0x100 + i * 32 + 0] = 1;
                break;
              case 13500:
                keys[0x100 + i * 32 + 0] = 1;
                keys[0x100 + i * 32 + 2] = 1;
                break;
              case 18000:
                keys[0x100 + i * 32 + 2] = 1;
                break;
              case 22500:
                keys[0x100 + i * 32 + 1] = 1;
                keys[0x100 + i * 32 + 2] = 1;
                break;
              case 27000:
                keys[0x100 + i * 32 + 1] = 1;
                break;
              case 31500:
                keys[0x100 + i * 32 + 1] = 1;
                keys[0x100 + i * 32 + 3] = 1;
                break;
            }
          }
        }

        if (NumBTN[i])
        {
          for (j = 0; (unsigned long) j < NumBTN[i]; j++)
          {
            if (js[i].rgbButtons[j])
            {
              keys[0x100 + i * 32 + 16 + j] = 1;
            }
          }
        }
      }
      else
      {
        for (j = 0; j < 32; j++)
        {
          keys[0x100 + i * 32 + j] = 0;
        }
      }
    }
  }

  int GetMouseX()
  {
    InputRead();
    MouseX += MouseMoveX / MouseSensitivity;

    if (MouseX > MouseMaxX)
    {
      MouseX = MouseMaxX;

      if (TrapMouseCursor == 1)
      {
        if (abs((int)MouseMoveX) > (10 / MouseSensitivity) && T36HZEnabled == 1 && FullScreen == 0 &&
            MouseButtonPressed == 0)
        {
          MouseInput->Unacquire();
          SetCursorPos(X + WindowWidth + 32, (int)(Y + (MouseY * WindowHeight / 224)));
        }
      }
      else if (FullScreen == 0 && device1 == 0 && device2 == 0 && MouseButtonPressed == 0 &&
               GUIOn2 == 1)
      {
        MouseInput->Unacquire();
        SetCursorPos(X + WindowWidth + 1, (int)(Y + (MouseY * WindowHeight / 224)));
      }
    }

    if (MouseX < MouseMinX)
    {
      MouseX = MouseMinX;

      if (TrapMouseCursor == 1)
      {
        if (abs((int)MouseMoveX) > (10 / MouseSensitivity) && T36HZEnabled == 1 && FullScreen == 0 &&
            MouseButtonPressed == 0)
        {
          MouseInput->Unacquire();
          SetCursorPos(X - 32, (int)(Y + (MouseY * WindowHeight / 224)));
        }
      }
      else if (FullScreen == 0 && device1 == 0 && device2 == 0 && MouseButtonPressed == 0 &&
               GUIOn2 == 1)
      {
        MouseInput->Unacquire();
        SetCursorPos(X - 1, (int)(Y + (MouseY * WindowHeight / 224)));
      }
    }
    return((int)MouseX);
  }

  int GetMouseY()
  {
    MouseY += MouseMoveY / MouseSensitivity;

    if (MouseY > MouseMaxY)
    {
      MouseY = MouseMaxY;

      if (TrapMouseCursor == 1)
      {
        if (abs((int)MouseMoveY) > (10 / MouseSensitivity) && T36HZEnabled == 1 && FullScreen == 0 &&
            MouseButtonPressed == 0)
        {
          MouseInput->Unacquire();
          SetCursorPos((int)(X + (MouseX * WindowWidth / 256)), Y + WindowHeight + 32);
        }
      }
      else if (FullScreen == 0 && device1 == 0 && device2 == 0 && MouseButtonPressed == 0 &&
               GUIOn2 == 1)
      {
        MouseInput->Unacquire();
        SetCursorPos((int)(X + (MouseX * WindowWidth / 256)), Y + WindowHeight + 1);
      }
    }

    if (MouseY < MouseMinY)
    {
      MouseY = MouseMinY;

      if (TrapMouseCursor == 1)
      {
        if (abs((int)MouseMoveY) > (10 / MouseSensitivity) && T36HZEnabled == 1 && FullScreen == 0 &&
            MouseButtonPressed == 0)
        {
          MouseInput->Unacquire();
          SetCursorPos((int)(X + (MouseX * WindowWidth / 256)), Y - 32);
        }
      }
      else if (FullScreen == 0 && device1 == 0 && device2 == 0 && MouseButtonPressed == 0 &&
               GUIOn2 == 1)
      {
        MouseInput->Unacquire();
        SetCursorPos((int)(X + (MouseX * WindowWidth / 256)), Y - 1);
      }
    }

    return((int)MouseY);
  }

  int GetMouseMoveX()
  {
    return((int)MouseMoveX / MouseSensitivity);
  }

  int GetMouseMoveY()
  {
    return((int)MouseMoveY / MouseSensitivity);
  }

  int GetMouseButton()
  {
    if (MouseButton == (lhguimouse ? 2 : 1))
    {
      MouseButtonPressed = 1;
    }
    else
    {
      MouseButtonPressed = 0;
    }
    if (MouseButton & (lhguimouse ? 1 : 2))
    {
      while (MouseButton != 0 && T36HZEnabled && FullScreen == 0)
      {
        Moving = 1;
        X += (int)MouseMoveX;
        Y += (int)MouseMoveY;

        InputRead();
        initwinvideo();
      }
    }
    if (Moving == 1)
    {
      Moving = 0;
      initwinvideo();
    }
    return((int)MouseButton);
  }

  void SetMouseMinX(int MinX)
  {
    MouseMinX = (float)MinX;
  }

  void SetMouseMaxX(int MaxX)
  {
    MouseMaxX = (float)MaxX;
  }

  void SetMouseMinY(int MinY)
  {
    MouseMinY = (float)MinY;
  }

  void SetMouseMaxY(int MaxY)
  {
    MouseMaxY = (float)MaxY;
  }

  void SetMouseX(int X)
  {
    MouseX = (float)X;
  }

  void SetMouseY(int Y)
  {
    MouseY = (float)Y;
  }

  void FrameSemaphore()
  {
    if (T60HZEnabled)
    {
      double delay;
      QueryPerformanceCounter((LARGE_INTEGER *)&end);

      delay = ((update_ticks_pc - (end - start)) * 1000.0 / freq) - 3.0;

      if (delay > 0.0)
      {
        WaitForSingleObject(hLock, (unsigned int) delay);
      }
    }
  }

  void ZsnesPage()
  {
    ShellExecute(NULL, NULL, "http://www.zsnes.com/", NULL, NULL, 0);
    MouseX = 0;
    MouseY = 0;
  }

  void DocsPage()
  {
    ShellExecute(NULL, NULL, "http://zsnes-docs.sourceforge.net/", NULL, NULL, 0);
    MouseX = 0;
    MouseY = 0;
  }

  char *CBBuffer;
  unsigned int CBLength;
  void PasteClipBoard()
  {
    if (OpenClipboard(0))
    {
      char *p = (char *)GetClipboardData(CF_TEXT);
      if (p)
      {
        strncpy(CBBuffer, p, CBLength);
        CBBuffer[CBLength - 1] = 0;

        for (p = CBBuffer; *p; p++)
        {
          if (isspace(*p))
          {
            *p = ' ';
          }
        }
        for (p--; p >= CBBuffer; p--)
        {
          if (isspace(*p))
          {
            *p = 0;
          }
          else
          {
            break;
          }
        }
      }
      CloseClipboard();
    }
  }

  extern int zspc_time;

  void WriteLine()
  {
    char buf[50];
    sprintf(buf, "%d\n", zspc_time);
    WriteConsole(debugWindow, buf, strlen(buf), NULL, NULL);
  }

  // This function creates the debug console
  void InitDebugger()
  {
    if (AllocConsole())
    {
      debugWindow = GetStdHandle(STD_OUTPUT_HANDLE);
      WriteConsole(debugWindow, "Welcome to the ZSNES Debugger v0.01\n",
                   sizeof("Welcome to the ZSNES Debugger v0.01\n"), NULL, NULL);
    }
  }

  int CheckBattery()
  {
    SYSTEM_POWER_STATUS SysPowerStat;
    GetSystemPowerStatus(&SysPowerStat);

    if (SysPowerStat.ACLineStatus == 0) //Definitly running off of battery
    {
      return(1); //Running off of battery
    }

    if ((SysPowerStat.BatteryFlag == 255) ||  //Unknown
        (SysPowerStat.BatteryFlag & 128)) //No battery
    {
      return(-1);
    }

    if ((SysPowerStat.BatteryFlag & 8) ||  //Charging
        (SysPowerStat.ACLineStatus == 1)) //Plugged in
    {
      return(0); //Plugged in
    }

    return(1); //Running off of battery
  }

  int CheckBatteryTime()
  {
    SYSTEM_POWER_STATUS SysPowerStat;
    GetSystemPowerStatus(&SysPowerStat);
    return SysPowerStat.BatteryLifeTime;
  }

  int CheckBatteryPercent()
  {
    SYSTEM_POWER_STATUS SysPowerStat;
    GetSystemPowerStatus(&SysPowerStat);
    return((SysPowerStat.BatteryLifePercent == 255) ? -1 : SysPowerStat.BatteryLifePercent);
  }

  extern unsigned int delayvalue;

  // Delay function for GUI
  void DoSleep()
  {
    // Fraction value for windows version of sleep
    delayvalue /= 100;

    Sleep(delayvalue);
  }

  void DisplayWIPDisclaimer()
  {
    unsigned int version_hash();
    unsigned int CurrentBuildNum = version_hash();

    if (CurrentBuildNum != PrevBuildNum)
    {
      MessageBox(NULL,
                 "This build of ZSNES is a WORK IN PROGRESS. This means that it is known to contain bugs and certain features\nmay or may not be working correctly. This build is not any representation of final work and is provided AS IS\nfor people to try bleeding edge code.\n\nPlease see http://zsnes.game-host.org/~pagefault/ for a list of current issues.",
                 "Disclaimer", MB_OK);
      PrevBuildNum = CurrentBuildNum;
    }
  }

void DockDebugger()
{
   RECT MainWindowXY;
   ZeroMemory(&MainWindowXY, sizeof(RECT));
   GetWindowRect(hMainWindow, &MainWindowXY);
   DebugWindowHandle = FindWindow(NULL ,"ZSNES Debugger");
   SetWindowPos(DebugWindowHandle, HWND_TOP, MainWindowXY.right, MainWindowXY.top, NULL, NULL, SWP_NOSIZE);
}

}

