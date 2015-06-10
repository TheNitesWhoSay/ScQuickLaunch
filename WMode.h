/*#ifndef WMODE_H
#define WMODE_H
#include <windows.h>
#include "DataTypes.h"
#include <type_traits>
#include <array>

// Start External File Data
typedef struct __rect
{
  s16 left;
  s16 top;
  s16 right;
  s16 bottom;
} rect;
typedef __rect rect;

namespace BW
{
#ifndef STORMAPI
#define STORMAPI __stdcall
#endif
	#ifdef _MAC
	#define BWMAC 1
	#else
	#define BWMAC 0
	#endif
	#define BW_DATA(type,name,pc,mac) namespace BWDATA { static type const name = BWMAC ? (type)mac : (type)pc; }
	BW_DATA(PALETTEENTRY*, GamePalette, 0x006CE320, 0);
	#define IS_REF(name,addr) & name = *reinterpret_cast<std::remove_reference<decltype(name)>::type*>(addr);

	struct dlgEvent
  {
    u32 dwUser;
    u16  wSelection;
    u16  wUnk_0x06;
    u16  wVirtKey;
    u16  wUnk_0x0A;
    u16  wNo;
    pt    cursor;
    u16  wUnk_0x12;
  };

class FontChar
  {
  public:
    bool isValid() const;

    int width() const;
    int height() const;
    int x() const;
    int y() const;

    int colorMask(int index) const;
    int pixelOffset(int index) const;

  private:
    u8  w, h, _x, _y;
    u8  data[1];
  };

  class Font
  {
  public:
    bool  isValid() const;
    
    FontChar  *getChar(char c) const;

    int   getTextWidth(const char *pszStr) const;
    int   getTextHeight(const char *pszStr) const;

    int   getCharWidth(char c) const;

    int   maxWidth() const;
    int   maxHeight() const;

    static int getTextWidth(const char *pszStr, int size);
    static int getTextHeight(const char *pszStr, int size);
  private:
    Font();// = delete;
    Font(const Font &other);// = delete;
    Font(Font &&other);// = delete;
    ~Font();// = delete;

    u32       magic;
    u8        low, high;
    u8        Xmax, Ymax;
    FontChar  *chrs[1];
  };
  std::array<Font*, 4> IS_REF(FontBase, 0x006CE0F4);

struct grpFrame
{
    s8  x;
    s8  y;
    s8  wid;
    s8  hgt;
    u32 dataOffset;
};
struct grpHead
{
    u16      wFrames;  // official
    s16       width;
    s16       height;
    grpFrame  frames[1];
};

void* STORMAPI SMemAlloc(size_t amount, char *logfilename, int logline, char defaultValue = 0);
#define SMAlloc(amount) SMemAlloc((amount), __FILE__, __LINE__)
void* STORMAPI SMemReAlloc(void *location, size_t amount, char *logfilename, int logline, char defaultValue = 0);
#define SMReAlloc(loc,s) SMemReAlloc((loc),(s), __FILE__, __LINE__)

class Bitmap
{
  public:
    Bitmap(int width, int height);
    Bitmap(int width, int height, void *pBuffer);

    void resize(int width, int height);
    bool isValid() const;

    bool blitString(const char *pszStr, int x, int y, u8 bSize = 1);
    void blitBitmap(const Bitmap *pSrc, int x = 0, int y = 0);
    void BlitGrpFrame(const grpFrame *pGrp, int x = 0, int y = 0);
    void BlitGraphic(const grpHead *pGrp, int frame = 0, int x = 0, int y = 0);

    void plot(int x, int y, u8 color);
    void drawLine(int x1, int y1, int x2, int y2, u8 color);

    int width() const;
    int height() const;
    u8  *getData() const;

    void clear();
    
  private:
    void plotX(int x, int y, u8 col);

    void intersection(int& x1, int& y1, int x2, int y2) const;
    void clipping(int& x1, int& y1, int& x2, int& y2) const;

    u16 wid, ht;
    u8  *data;
};
Bitmap IS_REF(GameScreenBuffer, 0x006CEFF0);

class dialog   // BIN Dialog
  {
  public:
    typedef bool __fastcall FnInteract(dialog* dlg, dlgEvent* evt);
    typedef void __fastcall FnUpdate(dialog* dlg, int x, int y, rect* dst);
    typedef void __fastcall FnDrawItem(dialog* dlg, u8 selectedIndex, rect* dstRect, int x, int y);

    dialog(u16 ctrlType, short index, const char *text, u16 left, u16 top, u16 width, u16 height = 0, FnInteract* pfInteract = nullptr);
    ~dialog();

    // global functions
    dialog  *findIndex(short wIndex);         // Searches for a control that matches the specified index
    dialog  *findDialog(const char *pszName); // Searches for a dialog that matches the name specified
    dialog  *next() const;                    // Retrieves the next dialog or control in the list

    bool    setFlags(u32 dwFlags);        // Sets a flag or set of flags for the control or dialog
    bool    clearFlags(u32 dwFlags);      // Clears a flag or set of flags for the control or dialog
    bool    hasFlags(u32 dwFlags) const;  // Returns true if the dialog or control has all of the specified flags enabled
    bool    setText(char *pszStr);        // Sets the text of a control, or name of a dialog
    const char *getText() const;          // Retrieves the text of a control, or name of a dialog
    int     getHotkey() const;            // Retrieves the hotkey for the button

    BW::Bitmap  *getSourceBuffer();   // Retrieves a pointer to a bitmap structure for reading or writing to the source buffer

    bool enable();            // Enables the dialog or control
    bool disable();           // Disables the dialog or control
    bool isDisabled() const;  // Returns true if the dialog or control is disabled
    bool show();              // Shows the dialog or control
    bool hide();              // Hides the dialog or control
    bool isVisible() const;   // Returns true if the dialog or control is visible

    u16 width() const;
    u16 height() const;

    // event-specific functions
    bool doEvent(u16 wEvtNum, u32 dwUser = 0, u16 wSelect = 0, u16 wVirtKey = 0); // Calls a dialog or control's interact function by generating event info using these parameters
    // cppcheck-suppress functionConst
    bool defaultInteract(BW::dlgEvent *pEvent); // Calls a dialog or control's default interact function using this event info
    bool activate();        // Activates a control or destroys a dialog
    bool update();          // Updates a control or dialog, refreshing it on the screen

    // dialog-specific functions
    bool        isDialog() const;         // Returns true if the control type is a dialog
    dialog      *child() const;           // Retrieves the child control from the parent dialog
    bool        addControl(dialog *ctrl); // Adds a control to this dialog
    bool        initialize();             // Performs the dialog's initialization and adds it to the list
    bool        isListed() const;         // Checks to see if this dialog is initialized/listed

    // control-specific functions
    dialog  *parent() const;              // Retrieves a control's parent dialog
    short   getIndex() const;             // Retrieves the index of a control
    bool    clearFontFlags();             // Clears all font formatting flags

    // button-specific
    bool isButton() const;      // Returns true if the control type is a button

    // checkbox & option button
    bool isOption() const;      // Returns true if the control type is a checkbox or radio button
    bool isChecked() const;     // Returns true if the control (checkbox/radio) is selected

    // listbox & combobox
    bool  isList() const;               // Returns true if the control type is a listbox or combobox
    u8    getSelectedIndex() const;     // Returns the index of the selected element
    u32   getSelectedValue() const;     // Returns the value of the selected element
    char  *getSelectedString() const;   // Returns the name of the selected element

    bool  setSelectedIndex(u8 bIndex);              // Sets the selected index
    bool  setSelectedByValue(u32 dwValue);          // Sets the selected index based on the given value
    bool  setSelectedByString(const char *pszString, bool noctrl = true); // Sets the selected index based on its name, noctrl = ignore control characters
    bool  setSelectedByString(const std::string &s, bool noctrl = true); // Sets the selected index based on its name, noctrl = ignore control characters

    bool  addListEntry(char *pszString, u32 dwValue = 0, u8 bFlags = 0);  // Adds an entry to a listbox or combobox
    bool  removeListEntry(u8 bIndex = 0);   // Removes an entry from a listbox or combobox
    bool  clearList();                      // Removes all entries from a listbox or combobox
    u8  getListCount() const;               // Retrieves the number of elements in a listbox or combobox


    // Data //
    dialog  *pNext;         // 0x00
    rect    rct;            // 0x04   // official
    Bitmap  srcBits;        // 0x0C   // official
    char    *pszText;       // 0x14   // official
    u32     lFlags;         // 0x18   // official
    u16     wUnk_0x1C;
    u16     wUnk_0x1E;
    s16     wIndex;         // 0x20   // official
    u16     wCtrlType;      // 0x22   // official
    u16     wUser;          // 0x24   // official
    /*
      CHECKBOX
        0-2 Show/Hide minimap button
        5   Disabled bullet
        6   Standard bullet
        7   Active bullet
        10  Disabled checkbox
        11  Standard checkbox
        12  Active checkbox
        82  Disabled alliance
        83  Standard Alliance
        85  Disabled Skull
        86  Standard Skull (changes to bird when checked)
        87  Active Skull
        98  Grey bullet thingy
        129 Disabled Messaging
        130 Standard Messaging
        131 Active Messaging
        136 White box + white Xmark
        139 Disabled Vision
        140 Standard Vision
        141 Active Vision
        142 Weird glowy bullet 1
        143 Weird glowy bullet 2

      BUTTON
        100 Grey Left menu button
        103 Left menu button
        106 Highlighted Left menu button
        109 Grey Center menu button
        112 Center menu button
        115 Highlighted Center menu button
        118 Grey Right menu button
        121 Right menu button
        124 Highlighted Right menu button

    */
    /*s32    lUser;          // 0x26   // official name
    FnInteract* pfcnInteract;  // 0x2A    // bool __fastcall pfcnInteract(dialog *dlg, dlgEvent *evt);
    FnUpdate*   pfcnUpdate;    // 0x2E    // void __fastcall pfcnUpdate(dialog *dlg, int x, int y, rect *dest);
    
    union _u                 // 0x32   // official
    {
      struct _ctrl           // official
      {
        dialog  *pDlg;      // 0x32   // official
      } ctrl;
      
      struct _dlg            // official
      {
        u32   dwUnk_0x32;
        struct{
          u16 wid, ht;
          u8 *data;
        } dstBits;                // 0x36  // official 
        dialog  *pActiveElement;  // 0x3E
        dialog  *pFirstChild;     // 0x42  // official
        dialog  *pMouseElement;
        void    *pModalFcn;       // 0x4A   // official
      } dlg;
      
      struct _btn
      {
        dialog* pDlg;         // 0x32   // official
        rect    responseRct;   // 0x36
        u16     wHighlight;     // 0x3E
        u16     wUnknown_0x40;  // padding?
        void*   pSmk;         // 0x42
        rect    textRct;       // 0x46
        u16     wAlignment;     // 0x4E
      } btn;

      struct _optn
      {
        dialog* pDlg;        // 0x32   // official
        u8      bEnabled;       // 0x36
      } optn;
      
      struct _edit
      {
        dialog* pDlg;          // 0x32   // official
        u8      bColor;           // 0x36
        u8      bScrollPosition;  // 0x37
        u8      bLeftMargin;      // 0x38
        u8      bTopMargin;       // 0x39
        u16     wUnk_0x3A;       // 0x3A
        u16     wUnk_0x3C;
        u8      bCursorPos;       // 0x3E
      } edit;
      
      struct _scroll   // official
      {
        dialog* pDlg;      // 0x32   // official
        void*   pfcnScrollerUpdate;  // 0x36
        u16     nCurPos;    // 0x3A
        u16     nMin;       // 0x3C   // official
        u16     nMax;       // 0x3E   // official
        u16     wUnk_0x40;    
        u32     dwScrollFlags;    // 0x42
        u8      bSliderSkip;      // 0x46
        u8      bUpdateCounter;   // 0x47
        u8      bSliderGraphic;   // 0x48
        u8      bSliderSpacing;   // 0x49
        u16     wUnk_0x4A;
        u32     dwUnk_0x4C;
      } scroll;
      
      struct _list   // official
      {
        dialog* pDlg;        // 0x32   // official
        dialog* pScrlBar;    // 0x36
        char**  ppStrs;      // 0x3A   // official
        u8*     pbStrFlags;  // 0x3E   // official
          /*
            &0x0F specifies the colour of the entry (Standard colours unless noted)
              0x01  = Use previous (Uses the colour of the entry BEFORE the previous one)
              0x09 - 0x0D  = Use last (Uses the colour of the previous entry)
            &0xF0 specifies additional properties
              0x10  = Gives entry an option button
          */
        /*u32*  pdwData;         // 0x42   // official
        u8    bStrs;            // 0x46   // official
        u8    unknown_0x47;
        u8    bCurrStr;         // 0x48   // official
        u8    bSpacing;         // 0x49
        u8    bItemsPerPage;    // 0x4A
        u8    bUnknown_0x4B;
        u8    bDirection;       // 0x4C
        u8    bOffset;          // 0x4D
        u8    bSelectedIndex;   // 0x4E
        u8    bUnknown_0x4F;    // padding?
        u16   wVerticalOffset;  // 0x4A
        FnDrawItem* pDrawItemFcn;    // 0x4C  // void __fastcall pDrawItemFcn(dialog *dlg, u8 selectedIndex, rect *dstRect, int x, int y)
      } list;
    } u;
  };
}
// End external file data

#define WMODE_MIN_WIDTH  200
#define WMODE_MIN_HEIGHT 150
#define WMODE_SNAP_RANGE 20

struct BITMAPINFO256 {
    BITMAPINFOHEADER    bmiHeader;
    RGBQUAD             bmiColors[256];
};

extern BITMAPINFO256 wmodebmp;

BOOL WINAPI   _GetCursorPos(LPPOINT lpPoint);
BOOL WINAPI   _SetCursorPos(int X, int Y);
BOOL WINAPI   _ClipCursor(const RECT *lpRect);
BOOL STORMAPI _SDrawLockSurface(int surfacenumber, RECT *lpDestRect, void **lplpSurface, int *lpPitch, int arg_unused);
BOOL STORMAPI _SDrawUnlockSurface(int surfacenumber, void *lpSurface, int a3, RECT *lpRect);
BOOL STORMAPI _SDrawUpdatePalette(unsigned int firstentry, unsigned int numentries, PALETTEENTRY *pPalEntries, int a4);
BOOL STORMAPI _SDrawRealizePalette();

extern BOOL (WINAPI   *_GetCursorPosOld)(LPPOINT lpPoint);
extern BOOL (WINAPI   *_SetCursorPosOld)(int X, int Y);
extern BOOL (WINAPI   *_ClipCursorOld)(const RECT *lpRect);
extern BOOL (STORMAPI *_SDrawLockSurfaceOld)(int surfacenumber, RECT *lpDestRect, void **lplpSurface, int *lpPitch, int arg_unused);
extern BOOL (STORMAPI *_SDrawUnlockSurfaceOld)(int surfacenumber, void *lpSurface, int a3, RECT *lpRect);
extern BOOL (STORMAPI *_SDrawUpdatePaletteOld)(unsigned int firstentry, unsigned int numentries, PALETTEENTRY *pPalEntries, int a4);
extern BOOL (STORMAPI *_SDrawRealizePaletteOld)();

void InitializeWModeBitmap(int width, int height);
void GetBorderRect(HWND hWnd, LPRECT lpRect);
void GetBorderSize(HWND hWnd, LPSIZE lpSize);
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void SetWMode(int width, int height, bool state);
void SetCursorShowState(bool bShow);

extern WNDPROC  wOriginalProc;
extern HWND     ghMainWnd;
extern HDC      hdcMem;
extern void*    pBits;
extern RGBQUAD  palette[256];
extern bool     wmode;

extern bool recordingUpdated;

extern bool switchToWMode;
extern RECT windowRect;

#endif*/