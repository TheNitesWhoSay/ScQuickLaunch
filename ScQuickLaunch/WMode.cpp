/*#include "WMode.h"
#include <algorithm>
/*#include "Resolution.h"
#include "BW/Offsets.h"
#include "BW/Dialog.h"
#include "BW/Bitmap.h"

#include "BWAPI/GameImpl.h"

#include "Config.h"
#include "Thread.h"

#include <Util/Convenience.h>
#include "../../Debug.h"*/

// Start external file data

/*template <typename _T>
static inline _T clamp(const _T &val, const _T &low, const _T &high)
{
  return min(max(val,low), high);
}

bool isCorrectVersion = true;
#define MemZero(x) memset(&x, 0, sizeof(x))
namespace BW
{
	struct pt
  {
    u16  x;
    u16  y;
  };

Bitmap::Bitmap(int width, int height)
    : wid((u16)width)
    , ht( (u16)height)
    , data( (u8*)SMAlloc(width*height) )
  {}
  
  Bitmap::Bitmap(int width, int height, void *pBuffer)
    : wid((u16)width)
    , ht( (u16)height)
    , data( (u8*)pBuffer )
  {}

  void Bitmap::resize(int width, int height)
  {
    if ( !this || width <= 0 || height <= 0 )
      return;

    this->wid   = (u16)width;
    this->ht    = (u16)height;
    this->data  = (u8*)SMReAlloc(this->data, width*height);
  }

  bool Bitmap::isValid() const
  {
    return this && this->data && this->wid > 0 && this->ht > 0;
  }

  u8 *Bitmap::getData() const
  {
    if ( this )
      return this->data;
    return nullptr;
  }

  int Bitmap::width() const
  {
    if ( this )
      return this->wid;
    return 0;
  }

  int Bitmap::height() const
  {
    if ( this )
      return this->ht;
    return 0;
  }

  void Bitmap::blitBitmap(const Bitmap *pSrc, int x, int y)
  {
    for ( int iy = 0; iy < min(this->height()-y, pSrc->height()); ++iy )
      memcpy(&this->data[(y + iy)*this->width() + x], &pSrc->data[iy*pSrc->width()], min(this->width(),pSrc->width()) );
  }

  void Bitmap::BlitGrpFrame(const grpFrame *pGrp, int x, int y)
  {
    if ( !this->isValid() || !pGrp )
      return;

    // Get pointers to grp frame data
    WORD *pwLineData = (WORD*)pGrp->dataOffset;
    BYTE *pbBase = (BYTE*)pGrp->dataOffset;
    
    // Set GRP offsets
    x += pGrp->x;
    y += pGrp->y;

    int yOff = y;
    for ( int i = 0; i < pGrp->hgt; ++i )
    {
      // Get pointer to current line's byte code data
      BYTE *pData = &pbBase[ pwLineData[i] ];

      int xOff = x;
      int pxRemaining = pGrp->wid;
      do
      {
        //BWAPI::Broodwar << "Reading" << std::endl;
        BYTE op = *pData++;

        if ( op & 0x80 )
        {
          op &= 0x7F;
          pxRemaining -= op;
          xOff += op;
        }
        else if ( op & 0x40 )
        {
          op &= 0xBF;
          pxRemaining -= op;
          BYTE px = *pData++;
          while ( op-- )    // @TODO: optimize to memset
            this->plot(xOff++, yOff, px);
        }
        else
        {
          pxRemaining -= op;
          while ( op-- )
            this->plot(xOff++, yOff, *pData++);
        }
      } while ( pxRemaining > 0 );
      ++yOff;
    } // for each line
  }

  void Bitmap::BlitGraphic(const grpHead *pGrp, int frame, int x, int y)
  {
    if ( 0 <= frame && pGrp && frame < pGrp->wFrames && this->isValid() )
      this->BlitGrpFrame(&pGrp->frames[frame], x, y);
  }

  BYTE gbColorTable[] = { 
     0,  0,  0,  1,  2,  3,  4,  5,  8,  0,  0,  0,  0,  0,  9, 10, 
    11, 12,  0,  0,  0, 13, 14, 15, 16, 17,  0, 18, 19, 20, 21, 23
  };

  BYTE gbFontColors[24][8] = {
    { 0xC0, 0x9B, 0x9A, 0x95, 0x43, 0x00, 0x00, 0x28 }, 
    { 0x56, 0xA7, 0x6D, 0x65, 0x5C, 0x00, 0x00, 0x8A }, 
    { 0x41, 0xFF, 0x53, 0x97, 0x47, 0x00, 0x00, 0x8A }, 
    { 0x40, 0x96, 0x49, 0x90, 0x42, 0x00, 0x00, 0x8A }, 
    { 0xA8, 0xAE, 0x17, 0x5E, 0xAA, 0x00, 0x00, 0x8A }, 
    { 0xB5, 0x75, 0xBA, 0xB9, 0xB7, 0x00, 0x00, 0x8A }, 
    { 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08 }, 
    { 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08 }, 
    { 0x8A, 0x6F, 0x17, 0x5E, 0xAA, 0x8A, 0x8A, 0x8A }, 
    { 0x28, 0xA5, 0xA2, 0x2D, 0xA0, 0x8A, 0x8A, 0x8A }, 
    { 0x8A, 0x9F, 0x9E, 0x9D, 0xB7, 0x8A, 0x8A, 0x8A }, 
    { 0x8A, 0xA4, 0xA3, 0xA1, 0x0E, 0x8A, 0x8A, 0x8A }, 
    { 0x8A, 0x9C, 0x1C, 0x1A, 0x13, 0x8A, 0x8A, 0x8A }, 
    { 0x8A, 0x13, 0x12, 0x11, 0x57, 0x8A, 0x8A, 0x8A }, 
    { 0x8A, 0x54, 0x51, 0x4E, 0x4A, 0x8A, 0x8A, 0x8A }, 
    { 0x8A, 0x87, 0xA6, 0x81, 0x93, 0x8A, 0x8A, 0x8A }, 
    { 0xB5, 0xB9, 0xB8, 0xB7, 0xB6, 0x8A, 0x8A, 0x8A }, 
    { 0x8A, 0x88, 0x84, 0x81, 0x60, 0x8A, 0x8A, 0x8A }, 
    { 0x8A, 0x86, 0x72, 0x70, 0x69, 0x8A, 0x8A, 0x8A }, 
    { 0x8A, 0x33, 0x7C, 0x7A, 0xA0, 0x8A, 0x8A, 0x8A }, 
    { 0x8A, 0x4D, 0x26, 0x23, 0x22, 0x8A, 0x8A, 0x8A }, 
    { 0x8A, 0x9A, 0x97, 0x95, 0x91, 0x8A, 0x8A, 0x8A }, 
    { 0x8A, 0x88, 0x84, 0x81, 0x60, 0x8A, 0x8A, 0x8A }, 
    { 0x8A, 0x80, 0x34, 0x31, 0x2E, 0x8A, 0x8A, 0x8A }
  };

  bool Bitmap::blitString(const char *pszStr, int x, int y, u8 bSize)
  {
    // verify valid size index
    if ( bSize > 3 || !pszStr )
      return false;

    // localize pointer

	Font *fnt = FontBase[bSize];
    if ( !fnt->isValid() )
      return false;

    // verify if drawing should be done
    if (  x + fnt->getTextWidth(pszStr)   < 0 ||
          y + fnt->getTextHeight(pszStr)  < 0 ||
          x >= this->width() ||
          y >= this->height() )
      return false;

    // Reference an unsigned character array
    const u8 *pbChars = (BYTE*)pszStr;

    u8 lastColor = 0, color   = 0;
    int  Xoffset   = x, Yoffset = y;

    // Iterate all characters in the message
    for ( int c = 0; pbChars[c]; ++c )
    {
      // Perform control character and whitespace functions
      if ( pbChars[c] <= ' ' )
      {
        switch ( pbChars[c] )
        {
        case 1:       // restore last colour
          color = lastColor;
          continue;
        case '\t':    // 9    tab
          Xoffset += fnt->getCharWidth( pbChars[c] );
          continue;
        case '\n':    // 10   newline
          Xoffset = x;
          Yoffset += fnt->maxHeight();
          continue;
        case 11:      // invisible
        case 20:
          color = (BYTE)~0;
          continue;
        case '\f':    // 12   formfeed
          break;
        case '\r':    // 13   carriage return
        case 26:
          continue;
        case 18:      // right align
          Xoffset += this->width() - fnt->getTextWidth(pszStr) - x;
          continue;
        case 19:      // center align
          Xoffset += (this->width() - fnt->getTextWidth(pszStr))/2 - x;
          continue;
        case ' ':     // space
          Xoffset += fnt->maxWidth() / 2;
          continue;
        default:      // colour code
          lastColor = color;
          color     = gbColorTable[ pbChars[c] ];
          continue;
        }
      }

      // Skip if the character is not supported by the font
      FontChar *fntChr = fnt->getChar( pbChars[c] );
      if ( !fntChr->isValid() )
        continue;

      // If the colour is not "invisible"
      if ( color != ~0 )
      {
        // begin drawing character process
        for ( int i = 0, pos = 0; pos < fntChr->height() * fntChr->width(); ++i, ++pos )
        {
          // font position
          pos += fntChr->pixelOffset(i);

          // x offset
          int newX = Xoffset + (fntChr->x() + pos % fntChr->width());
          if ( newX >= this->width() ) break;
          if ( newX < 0 ) continue;

          // y offset
          int newY = Yoffset + (fntChr->y() + pos / fntChr->width());
          if ( newY >= this->height() ) break;
          if ( newY < 0 ) continue;

          // blit offset
          int offset = newY * this->width() + newX;
          if ( offset >= this->width() * this->height() ) break;
          if ( offset < 0 ) continue;

          // Plot pixel
          this->data[offset] = gbFontColors[color][fntChr->colorMask(i)];
        }
      } // invis colour

      // Increment the X offset for the width of the character
      Xoffset += fntChr->width();
    }
    return true;
  }

  void Bitmap::plot(int x, int y, u8 color)
  {
    if ( x < 0 || y < 0 || x >= this->width() || y >= this->height() )
      return;
    this->plotX(x, y, color);
  }
  void Bitmap::plotX(int x, int y, u8 col)
  {
    this->data[ x + y*this->width() ] = col;
  }
  /*
  void Bitmap::intersection(int& x1, int& y1, int x2, int y2) const
  {
    // Line clipping source: http://gamebub.com/cpp_algorithms.php#lineclip
    if ( y2 != y1 )
    {
      if ( y1 >= this->height() )
      {
        x1 += (x2 - x1) * ((this->height()-1) - y1) / (y2 - y1);
        y1 = this->height()-1;
      }
      else if ( y1 < 0 )
      {
        x1 += (x2 - x1) * (0 - y1) / (y2 - y1);
        y1 = 0;
      }
    }
    if ( x2 != x1 )
    {
      if ( x1 >= this->width() )
      {
        y1 += (y2 - y1) * ((this->width()-1) - x1) / (x2 - x1);
        x1 = this->width()-1;
      }
      else if ( x1 < 0 )
      {
        y1 += (y2 - y1) * (0 - x1) / (x2 - x1);
        x1 = 0;
      }
    }
  }
  void Bitmap::clipping(int& x1, int& y1, int& x2, int& y2) const
  {
    // Line clipping source: http://gamebub.com/cpp_algorithms.php#lineclip
    intersection(x1, y1, x2, y2);
    intersection(x2, y2, x1, y1);
  }*/

  /*void Bitmap::drawLine(int x1, int y1, int x2, int y2, u8 color)
  {
    // If the line doesn't intersect this rect
    if (  max(x1,x2) < 0 ||
          max(y1,y2) < 0 ||
          min(x1,x2) >= this->width() ||
          min(y1,y2) >= this->height()  )
      return;

    // If the line is completely vertical or horizontal
    if ( x1 == x2 )
    {
      int ymin = clamp(min(y1,y2), 0, this->height()-1);
      int ymax = clamp(max(y1,y2), 0, this->height()-1);

      // Plot vertical line
      for ( int i = ymin; i < ymax; ++i )
        this->plotX(x1, i, color);
      return;
    }
    else if ( y1 == y2 )
    {
      int xmin = clamp(min(x1,x2), 0, this->width()-1);
      int xmax = clamp(max(x1,x2), 0, this->width()-1);

      // Plot horizontal line
      memset(&this->data[y1 * this->width() + xmin], color, xmax - xmin );
      return;
    }

    // If the line does intersect but needs to be clipped
    //if ( x1 < 0 || y1 < 0 || x2 >= this->width() || y2 >= this->height() )
      //this->clipping(x1,y1,x2,y2);

    // Line algorithm source: http://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm#Simplification
    int dx = std::abs(x2 - x1),     dy = std::abs(y2 - y1);
    int sx = x1 < x2 ? 1 : -1,      sy = y1 < y2 ? 1 : -1;
    int err = dx - dy;

    // @TODO: optimize
    while ( this->plot(x1, y1, color), x1 != x2 || y1 != y2 )
    {
      int e2 = 2*err;
      if ( e2 > -dy )
      {
        err -= dy;
        x1 += sx;
      }
      if ( e2 < dx )
      {
        err += dx;
        y1 += sy;
      }
    }
  }

  void Bitmap::clear()
  {
    if ( this->isValid() )
      memset(this->data, 0, this->width()*this->height());
  }
};
// End external file data

WNDPROC wOriginalProc;
HWND ghMainWnd;
bool wmode;
HDC  hdcMem;
void* pBits;
bool gbWantUpdate     = false;
bool gbIsCursorHidden = true;
bool gbHoldingAlt     = false;

bool recordingUpdated;

bool switchToWMode = false;
RECT windowRect    = { 0, 0, 640, 480 };

BOOL (WINAPI   *_GetCursorPosOld)(LPPOINT lpPoint);
BOOL (WINAPI   *_SetCursorPosOld)(int X, int Y);
BOOL (WINAPI   *_ClipCursorOld)(const RECT *lpRect);
BOOL (STORMAPI *_SDrawLockSurfaceOld)(int surfacenumber, RECT *lpDestRect, void **lplpSurface, int *lpPitch, int arg_unused);
BOOL (STORMAPI *_SDrawUnlockSurfaceOld)(int surfacenumber, void *lpSurface, int a3, RECT *lpRect);
BOOL (STORMAPI *_SDrawUpdatePaletteOld)(unsigned int firstentry, unsigned int numentries, PALETTEENTRY *pPalEntries, int a4);
BOOL (STORMAPI *_SDrawRealizePaletteOld)();

BITMAPINFO256 wmodebmp;
HBITMAP hwmodeBmp;
void InitializeWModeBitmap(int width, int height)
{
  if ( hdcMem )
    DeleteDC(hdcMem);
  hdcMem = NULL;
  
  // Create Bitmap HDC
  MemZero(wmodebmp);
  wmodebmp.bmiHeader.biSize          = sizeof(BITMAPINFOHEADER);
  wmodebmp.bmiHeader.biWidth         = width;
  wmodebmp.bmiHeader.biHeight        = -(height);
  wmodebmp.bmiHeader.biPlanes        = 1;
  wmodebmp.bmiHeader.biBitCount      = 8;
  wmodebmp.bmiHeader.biCompression   = BI_RGB;
  wmodebmp.bmiHeader.biSizeImage     = width * height;

  if ( isCorrectVersion )
  {
    for ( int i = 0; i < 256; ++i )
    {
      wmodebmp.bmiColors[i].rgbRed   = BW::BWDATA::GamePalette[i].peRed;
      wmodebmp.bmiColors[i].rgbGreen = BW::BWDATA::GamePalette[i].peGreen;
      wmodebmp.bmiColors[i].rgbBlue  = BW::BWDATA::GamePalette[i].peBlue;
    }
  }
  HDC     hdc   = GetDC(ghMainWnd);
  hwmodeBmp     = CreateDIBSection(hdc, (BITMAPINFO*)&wmodebmp, DIB_RGB_COLORS, &pBits, NULL, 0);
  hdcMem        = CreateCompatibleDC(hdc);
  ReleaseDC(ghMainWnd, hdc);
  SelectObject(hdcMem, hwmodeBmp);
}

void GetBorderRect(HWND hWnd, LPRECT lpRect)
{
  RECT clientRct;
  RECT windowRct;
  GetClientRect(hWnd, &clientRct);
  ClientToScreen(hWnd, (LPPOINT)&clientRct.left);
  ClientToScreen(hWnd, (LPPOINT)&clientRct.right);
  GetWindowRect(hWnd, &windowRct);
  if ( lpRect )
  {
    lpRect->left    = clientRct.left   - windowRct.left;
    lpRect->top     = clientRct.top    - windowRct.top;
    lpRect->right   = windowRct.right  - clientRct.right;
    lpRect->bottom  = windowRct.bottom - clientRct.bottom;
  }
}

void GetBorderSize(HWND hWnd, LPSIZE lpSize)
{
  RECT diff;
  GetBorderRect(hWnd, &diff);
  if ( lpSize )
  {
    lpSize->cx = diff.left + diff.right;
    lpSize->cy = diff.top + diff.bottom;
  }
}

LPARAM FixPoints(LPARAM lParam)
{
  POINTS pt = MAKEPOINTS(lParam);

  if ( isCorrectVersion )
  {
    RECT clientRct;
    GetClientRect(ghMainWnd, &clientRct);

    if ( clientRct.right != BW::GameScreenBuffer.width() )
      pt.x = (SHORT)((float)pt.x * ((float)BW::GameScreenBuffer.width() / (float)clientRct.right));
    if ( clientRct.bottom != BW::GameScreenBuffer.height() )
      pt.y = (SHORT)((float)pt.y * ((float)BW::GameScreenBuffer.height()  / (float)clientRct.bottom));
  }
  return MAKELPARAM(pt.x, pt.y);
}

bool SendHotkey(BW::dlgEvent *pEvent)
{
  BW::dialog *dlg = BW::BWDATA::EventDialogs[pEvent->wNo];
  if ( dlg && dlg->pfcnInteract(dlg, pEvent) )
    return true;

  dlg = BW::BWDATA::DialogList;
  while ( dlg )
  {
    if ( dlg->pfcnInteract(dlg, pEvent) )
      return true;
    dlg = dlg->next();
  }
  return false;
}

void ButtonEvent(DWORD dwEvent, LPARAM lParam)
{
  BYTE bFlag = 0;
  switch( dwEvent )
  {
  case BW_EVN_LBUTTONDOWN:
  case BW_EVN_LBUTTONUP:
  case BW_EVN_LBUTTONDBLCLK:
    bFlag = 0x02;
    break;
  case BW_EVN_RBUTTONDOWN:
  case BW_EVN_RBUTTONUP:
  case BW_EVN_RBUTTONDBLCLK:
    bFlag = 0x08;
    break;
  case BW_EVN_MBUTTONDOWN:
  case BW_EVN_MBUTTONUP:
  case BW_EVN_MBUTTONDBLCLK:
    bFlag = 0x20;
    break;
  }
  if ( !( (*BW::BWDATA::InputFlags) & ~bFlag & 0x2A) )
  {
    switch( dwEvent )
    {
    case BW_EVN_LBUTTONDOWN:
    case BW_EVN_RBUTTONDOWN:
    case BW_EVN_MBUTTONDOWN:
    case BW_EVN_LBUTTONDBLCLK:
    case BW_EVN_RBUTTONDBLCLK:
    case BW_EVN_MBUTTONDBLCLK:
      *BW::BWDATA::InputFlags |= bFlag;
      SetCapture(ghMainWnd);
      break;
    case BW_EVN_LBUTTONUP:
    case BW_EVN_RBUTTONUP:
    case BW_EVN_MBUTTONUP:
      *BW::BWDATA::InputFlags &= ~bFlag;
      ReleaseCapture();
      break;
    }
    POINTS pt = MAKEPOINTS(lParam);
    BW::dlgEvent evt = { 0 };
    
    evt.wNo = (WORD)dwEvent;
    evt.cursor.x = pt.x;
    evt.cursor.y = pt.y;
    BW::BWDATA::Mouse->x = pt.x;
    BW::BWDATA::Mouse->y = pt.y;
    if ( !SendHotkey(&evt) && BW::InputProcedures[dwEvent] )
      BW::InputProcedures[dwEvent](&evt);
  }
}

void CorrectWindowWidth(int type, SIZE *ws, RECT *rct, SIZE *border)
{
  switch ( type )
  {
    case WMSZ_LEFT:
    case WMSZ_TOPLEFT:
    case WMSZ_BOTTOMLEFT:
      rct->left = rct->right - ws->cx - border->cx;
      break;
    case WMSZ_RIGHT:
    case WMSZ_TOPRIGHT:
    case WMSZ_BOTTOMRIGHT:
      rct->right = rct->left + ws->cx + border->cx;
      break;
  }
}
void CorrectWindowHeight(int type, SIZE *ws, RECT *rct, SIZE *border)
{
  switch ( type )
  {
    case WMSZ_TOP:
    case WMSZ_TOPLEFT:
    case WMSZ_TOPRIGHT:
      rct->top = rct->bottom - ws->cy - border->cy;
      break;
    case WMSZ_BOTTOM:
    case WMSZ_BOTTOMRIGHT:
    case WMSZ_BOTTOMLEFT:
      rct->bottom = rct->top + ws->cy + border->cy;
      break;
  }
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  if ( wmode )
  {
    // Perform W-Mode only functionality
    switch ( uMsg )
    {
    case WM_SIZING:
      {
        SIZE border;
        GetBorderSize(hWnd, &border);

        RECT *rct = (RECT*)lParam;
        SIZE ws   = { rct->right - rct->left - border.cx, rct->bottom - rct->top - border.cy };
        if ( ws.cx < WMODE_MIN_WIDTH )
        {
          ws.cx = WMODE_MIN_WIDTH;
          CorrectWindowWidth(wParam, &ws, rct, &border);
        }
        if ( ws.cy < WMODE_MIN_HEIGHT )
        {
          ws.cy = WMODE_MIN_HEIGHT;
          CorrectWindowHeight(wParam, &ws, rct, &border);
        }

        if ( GetKeyState(VK_CONTROL) & 0x8000 && ws.cy != ws.cx * 3 / 4 )
        {
          if ( wParam == WMSZ_TOP || wParam == WMSZ_BOTTOM )
          {
            ws.cx = ws.cy * 4 / 3;
            CorrectWindowWidth(WMSZ_RIGHT, &ws, rct, &border);
          }
          else
          {
            ws.cy = ws.cx * 3 / 4;
            CorrectWindowHeight( (wParam == WMSZ_RIGHT || wParam == WMSZ_LEFT) ? WMSZ_BOTTOM : wParam, &ws, rct, &border);
          }
        }

        if ( isCorrectVersion ) // must be correct version to reference BWDATA
        {
          if ( ws.cx >= BW::BWDATA::GameScreenBuffer.width() - WMODE_SNAP_RANGE &&
               ws.cx <= BW::BWDATA::GameScreenBuffer.width() + WMODE_SNAP_RANGE )
          {
            ws.cx = BW::BWDATA::GameScreenBuffer.width();
            CorrectWindowWidth( (wParam == WMSZ_TOP || wParam == WMSZ_BOTTOM) ? WMSZ_RIGHT : wParam, &ws, rct, &border);
          }
          if ( ws.cy >= BW::BWDATA::GameScreenBuffer.height() - WMODE_SNAP_RANGE &&
               ws.cy <= BW::BWDATA::GameScreenBuffer.height() + WMODE_SNAP_RANGE )
          {
            ws.cy = BW::BWDATA::GameScreenBuffer.height();
            CorrectWindowHeight( (wParam == WMSZ_RIGHT || wParam == WMSZ_LEFT) ? WMSZ_BOTTOM : wParam, &ws, rct, &border);
          }
        }
        break;
      } // case WM_SIZING
    case WM_SIZE:
      {
        switch ( wParam )
        {
        case SIZE_RESTORED:
          {
            char szTemp[32];
            RECT tempRect;
            GetClientRect(hWnd, &tempRect);
            windowRect.right  = tempRect.right;
            windowRect.bottom = tempRect.bottom;
            WritePrivateProfileStringA("window", "width",  _itoa(tempRect.right,  szTemp, 10), configPath.c_str());
            WritePrivateProfileStringA("window", "height", _itoa(tempRect.bottom, szTemp, 10), configPath.c_str());
            break;
          }
        }// wParam switch
        break;
      } // case WM_SIZE
    case WM_MOVE:
      {
        RECT tempRect;
        GetWindowRect(hWnd, &tempRect);
        if ( tempRect.right > 0 && 
             tempRect.bottom > 0 && 
             tempRect.left < GetSystemMetrics(SM_CXFULLSCREEN) &&
             tempRect.top  < GetSystemMetrics(SM_CYFULLSCREEN) )
        {
          windowRect.left = tempRect.left;
          windowRect.top  = tempRect.top;

          char szTemp[32];
          WritePrivateProfileStringA("window", "left", _itoa(tempRect.left, szTemp, 10), configPath.c_str());
          WritePrivateProfileStringA("window", "top",  _itoa(tempRect.top, szTemp, 10), configPath.c_str());
        }
        break;
      } // case WM_MOVE
    case WM_PAINT:
      if ( gbWantUpdate && pBits)
      {
        static DWORD dwLastUpdate = 0;
        DWORD dwNewTick = GetTickCount();
        if ( dwLastUpdate + (IsIconic(hWnd) ? 200 : 20) > dwNewTick )
          break;
        dwLastUpdate = dwNewTick;
        gbWantUpdate = false;

        // begin paint
        PAINTSTRUCT paint;
        HDC hdc = BeginPaint(hWnd, &paint);

        if ( isCorrectVersion ) // must be correct version, @todo: make independent
        {
          // Blit to the screen
          RECT cRect;
          GetClientRect(hWnd, &cRect);        
          if ( cRect.right == BW::BWDATA::GameScreenBuffer.width() && cRect.bottom == BW::BWDATA::GameScreenBuffer.height() )
          {
            BitBlt(hdc, 0, 0, BW::BWDATA::GameScreenBuffer.width(), BW::BWDATA::GameScreenBuffer.height(), hdcMem, 0, 0, SRCCOPY);
          }
          else
          {
            SetStretchBltMode(hdc, HALFTONE);
            //StretchBlt(hdc, 0, 0, cRect.right, cRect.bottom, hdcMem, 0, 0, BW::BWDATA::GameScreenBuffer->wid, BW::BWDATA::GameScreenBuffer->ht, SRCCOPY);
            StretchDIBits(hdc, 0, 0, cRect.right, cRect.bottom, 0, 0, BW::BWDATA::GameScreenBuffer.width(), BW::BWDATA::GameScreenBuffer.height(), pBits, (BITMAPINFO*)&wmodebmp, DIB_RGB_COLORS, SRCCOPY);
          }
        }

        // end paint
        EndPaint(hWnd, &paint);
      } // data
      break;
    case WM_NCMOUSEMOVE:
      SetCursorShowState(true);
      break;
    case WM_MOUSEMOVE:
      SetCursorShowState(false);
      lParam = FixPoints(lParam);
      break;
    case WM_LBUTTONDOWN:
    case WM_RBUTTONDOWN:
    case WM_MBUTTONDOWN:
      {
        RECT clientRct;
        GetClientRect(hWnd, &clientRct);
        ClientToScreen(hWnd, (LPPOINT)&clientRct.left);
        ClientToScreen(hWnd, (LPPOINT)&clientRct.right);
        ClipCursor(&clientRct);
        lParam = FixPoints(lParam);
        break;
      }
    case WM_MBUTTONUP:
    case WM_LBUTTONUP:
    case WM_RBUTTONUP:
      ClipCursor(NULL);
      lParam = FixPoints(lParam);
      break;
    case WM_MOUSEWHEEL:
    case WM_RBUTTONDBLCLK:
    case WM_LBUTTONDBLCLK:
    case WM_MBUTTONDBLCLK:
      lParam = FixPoints(lParam);
      break;
    case WM_ACTIVATEAPP:
      if ( wOriginalProc )
        return wOriginalProc(hWnd, WM_ACTIVATEAPP, (WPARAM)1, NULL);
    case WM_SETCURSOR:
    case WM_ERASEBKGND:
      return DefWindowProc(hWnd, uMsg, wParam, lParam);
    case WM_SYSKEYDOWN:
    case WM_KEYDOWN:
      if ( wParam == VK_MENU && !(lParam & 0x40000000))
      {
        RECT rct;
        GetClientRect(hWnd, &rct);
        ClientToScreen(hWnd, (LPPOINT)&rct.left);
        ClientToScreen(hWnd, (LPPOINT)&rct.right);
        ClipCursor(&rct);
        gbHoldingAlt = true;
      }
      break;
    case WM_SYSKEYUP:
    case WM_KEYUP:
      if ( wParam == VK_MENU )
      {
        ClipCursor(NULL);
        gbHoldingAlt = false;
      }
      break;
    } // switch
  } // if wmode

  if ( isCorrectVersion )
  {
    // Perform BWAPI-added functionality
    switch ( uMsg )
    {
    case WM_SYSKEYDOWN:
      if ( wParam == VK_RETURN && (lParam & 0x20000000) && !(lParam & 0x40000000) )
      {
        SetWMode(BW::BWDATA::GameScreenBuffer.width(), BW::BWDATA::GameScreenBuffer.height(), !wmode);
        return TRUE;
      }
      break;
    case WM_MOUSEMOVE:
      if ( GetWindowLong(ghMainWnd, GWL_STYLE) & WS_SYSMENU ) // Compatibility for Xen W-Mode
      {
        (*BW::BWDATA::InputFlags) |= 1;
        POINTS pt = MAKEPOINTS(lParam);
        BW::BWDATA::Mouse->x = pt.x;
        BW::BWDATA::Mouse->y = pt.y;
        return TRUE;
      }
      break;
    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_LBUTTONDBLCLK:
    case WM_RBUTTONDOWN:
    case WM_RBUTTONUP:
    case WM_RBUTTONDBLCLK:
    case WM_MBUTTONDOWN:
    case WM_MBUTTONUP:
    case WM_MBUTTONDBLCLK:
      if ( GetWindowLong(ghMainWnd, GWL_STYLE) & WS_SYSMENU ) // Compatibility for Xen W-Mode
      {
        ButtonEvent(uMsg - WM_MOUSEFIRST + BW_EVN_MOUSEFIRST, lParam);
        return TRUE;
      }
      break;
    case WM_SYSCOMMAND:
      if ( wParam == SC_MAXIMIZE )
      {
        SetWMode(BW::BWDATA::GameScreenBuffer.width(), BW::BWDATA::GameScreenBuffer.height(), false);
        return TRUE;
      }
      break;
    }
  }

  // Register Broodwar thread name
  RegisterThreadName("Broodwar Main");

  // Call the original WndProc
  if ( wOriginalProc )
    return wOriginalProc(hWnd, uMsg, wParam, lParam);
  return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

BOOL WINAPI _GetCursorPos(LPPOINT lpPoint)
{
  if ( !lpPoint )
    return FALSE;

  if ( !wmode )
  {
    if ( _GetCursorPosOld )
      return _GetCursorPosOld(lpPoint);
    return GetCursorPos(lpPoint);
  }

  if ( !gbHoldingAlt )
  {
    lpPoint->x = 320;
    lpPoint->y = 240;
  }
  else
  {
    POINT tempPoint;
    if ( _GetCursorPosOld )
      _GetCursorPosOld(&tempPoint);
    else
      GetCursorPos(&tempPoint);
    ScreenToClient(ghMainWnd, &tempPoint);

    LPARAM lConvert = FixPoints(MAKELPARAM(tempPoint.x, tempPoint.y));
    POINTS final = MAKEPOINTS(lConvert);
    lpPoint->x = final.x;
    lpPoint->y = final.y;
  }
  return TRUE;
}

BOOL WINAPI _SetCursorPos(int X, int Y)
{
  if ( !wmode )
  {
    if ( _SetCursorPosOld )
      return _SetCursorPosOld(X, Y);
    return SetCursorPos(X, Y);
  }
  return TRUE;
}

BOOL WINAPI _ClipCursor(const RECT *lpRect)
{
  if ( !wmode )
  {
    if ( _ClipCursorOld )
      return _ClipCursorOld(lpRect);
    return ClipCursor(lpRect);
  }
  return TRUE;
}

BOOL __stdcall _SDrawLockSurface(int surfacenumber, RECT *lpDestRect, void **lplpSurface, int *lpPitch, int arg_unused)
{
  if ( !wmode )
  {
    if ( _SDrawLockSurfaceOld )
      return _SDrawLockSurfaceOld(surfacenumber, lpDestRect, lplpSurface, lpPitch, arg_unused);
    return SDrawLockSurface(surfacenumber, lpDestRect, lplpSurface, lpPitch, arg_unused);
  }

  if ( lplpSurface )
    *lplpSurface = pBits;
  if ( lpPitch )
    *lpPitch = 640;
  return TRUE;
}

BOOL __stdcall _SDrawUnlockSurface(int surfacenumber, void *lpSurface, int a3, RECT *lpRect)
{
  if ( !wmode )
  {
    if ( _SDrawUnlockSurfaceOld )
      return _SDrawUnlockSurfaceOld(surfacenumber, lpSurface, a3, lpRect);
    return SDrawUnlockSurface(surfacenumber, lpSurface, a3, lpRect);
  }

  if ( ghMainWnd && lpSurface )
  {
    gbWantUpdate = true;
    InvalidateRect(ghMainWnd, lpRect, FALSE);
  }
  return TRUE;
}

BOOL __stdcall _SDrawUpdatePalette(unsigned int firstentry, unsigned int numentries, PALETTEENTRY *pPalEntries, int a4)
{
  for ( unsigned int i = firstentry; i < firstentry + numentries; ++i )
  {
    wmodebmp.bmiColors[i].rgbRed   = pPalEntries[i].peRed;
    wmodebmp.bmiColors[i].rgbGreen = pPalEntries[i].peGreen;
    wmodebmp.bmiColors[i].rgbBlue  = pPalEntries[i].peBlue;
  }
  if ( !wmode || !ghMainWnd )
  {
    if ( _SDrawUpdatePaletteOld )
      return _SDrawUpdatePaletteOld(firstentry, numentries, pPalEntries, a4);
    return SDrawUpdatePalette(firstentry, numentries, pPalEntries, a4);
  }

  if ( !IsIconic(ghMainWnd) )
    SetDIBColorTable(hdcMem, firstentry, numentries, wmodebmp.bmiColors);
  return TRUE;
}

BOOL __stdcall _SDrawRealizePalette()
{
  if ( !wmode || !ghMainWnd )
  {
    if ( _SDrawRealizePaletteOld )
      return _SDrawRealizePaletteOld();
    return SDrawRealizePalette();
  }

  if ( IsIconic(ghMainWnd) )
    return FALSE;

  return TRUE;
}

void SetWMode(int width, int height, bool state)
{
  // Compatibility for Xen W-Mode
  if ( ghMainWnd && !(GetWindowLong(ghMainWnd, GWL_STYLE) & WS_SYSMENU) )
    return;

  if ( !isCorrectVersion )
    return;

  if ( state )
  {
    wmode = true;
    if ( !ghMainWnd )
      return;

    // Call the DirectDraw destructor
    DDrawDestroy();
    InitializeWModeBitmap(BW::BWDATA::GameScreenBuffer.width(), BW::BWDATA::GameScreenBuffer.height());

    // Hack to enable drawing in Broodwar
    BW::BWDATA::PrimarySurface = (LPDIRECTDRAWSURFACE)1;

    const int shift = gdwProcNum > 1 ? 32 : 0;
    POINT pos = { windowRect.left + shift, windowRect.top + shift };
    // Change the window settings
    SetWindowLong(ghMainWnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);
    SetWindowPos(ghMainWnd, HWND_NOTOPMOST, pos.x, pos.y, width, height, SWP_SHOWWINDOW);
    ShowWindow(ghMainWnd, SW_RESTORE);

    SIZE border;
    GetBorderSize(ghMainWnd, &border);
    int w = width + border.cx;
    int h = height + border.cy;

    int cx = GetSystemMetrics(SM_CXFULLSCREEN);
    int cy = GetSystemMetrics(SM_CYFULLSCREEN);
    while ( pos.x < 0 )
      pos.x = 0;
    while ( pos.y < 0 )
      pos.y = 0;
    if ( pos.y + h >= cy )
    {
      if (gdwProcNum > 1)
        pos.y -= cy - h;
      else
        pos.y = cy - h;
    }
    if ( pos.x + w >= cx )
    {
      if (gdwProcNum > 1)
        pos.x -= cx - w;
      else
        pos.x = cx - w;
    }
    MoveWindow(ghMainWnd, pos.x, pos.y, w, h, TRUE);
    SetCursor(NULL);
    SetCursorShowState(false);

    SetDIBColorTable(hdcMem, 0, 256, wmodebmp.bmiColors);
    WritePrivateProfileStringA("window", "windowed", "ON", configPath.c_str());
  }
  else
  {
    wmode = false;
    BW::BWDATA::PrimarySurface = NULL;
    if ( hdcMem )
      DeleteDC(hdcMem);
    hdcMem = NULL;

    if (ghMainWnd)
    {
      SetWindowLong(ghMainWnd, GWL_STYLE, WS_POPUP | WS_VISIBLE | WS_SYSMENU);
      SetWindowPos(ghMainWnd, HWND_TOPMOST, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), SWP_SHOWWINDOW);
      SetCursor(NULL);
      SetCursorShowState(false);
      SetFocus(ghMainWnd);
    }
    DDrawDestroy();
    DDrawInitialize(width, height);
    WritePrivateProfileStringA("window", "windowed", "OFF", configPath.c_str());
  }
}

void SetCursorShowState(bool bShow)
{
  if ( bShow == gbIsCursorHidden )
  {
    ShowCursor(bShow);
    gbIsCursorHidden = !bShow;
  }
}
*/