#ifdef WIN32
#include <windows.h>
#include <Commctrl.h>
#include <vector>
#else
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#endif
#include <cstdio>
#include "Utils.hpp"
#include <cstring>
#include "../String_utils.hpp"

#ifdef WIN32
std::wstring Utils::string2wstring(const std::string &s)
{
  int len;
  int slength = (int)s.length() + 1;
  len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
  wchar_t* buf = new wchar_t[len];
  MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
  std::wstring r(buf);
  delete[] buf;

  return r;
}

std::wstring Utils::string2wstring(const char *s)
{
  std::string str;

  if (!s)
    return Utils::string2wstring(str);
  str = s;
  return Utils::string2wstring(str);
}

std::string Utils::wstring2string(const std::wstring &wstr)
{
  std::string strTo;

  char *szTo = new char[wstr.length() + 1];
  szTo[wstr.size()] = '\0';
  WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, szTo, (int)wstr.length(), NULL, NULL);
  strTo = szTo;
  delete[] szTo;

  return strTo;
}

std::string Utils::wstring2string(const wchar_t *s)
{
  std::wstring  st;

  if (!s)
    return Utils::wstring2string(st);
  st = s;
  return Utils::wstring2string(st);
}
#endif

std::vector<std::pair<std::string, int> >    Utils::getFolderDatas(const char *s, std::string *lastError)
{
  std::vector<std::pair<std::string, int> >    ret;

  if (!s || strlen(s) < 1){
      if (lastError){
          (*lastError) = "Invalid null path";
        }
      return ret;
    }
  std::string path(s);

  path = Utility::replace<std::string>(path, std::string("\\").c_str(), std::string("/").c_str());
  if (path[path.length() - 1] != '/')
    path += '/';
#ifdef WIN32
  WIN32_FIND_DATA File;
  HANDLE hSearch;
  std::wstring dir = Utils::string2wstring(path);

  dir += L"*";
  if ((hSearch = FindFirstFile(dir.c_str(), &File)) == INVALID_HANDLE_VALUE){
      if (lastError){
          (*lastError) = s + std::string(": Invalid directory");
        }
      return ret;
    }
  do {
      ret.push_back(std::pair<std::string, int>(wstring2string(std::wstring(File.cFileName)),
                                                (File.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ? Utils::DIRECTORY : Utils::FILE));
    } while (FindNextFile(hSearch, &File));
  FindClose(hSearch);
#else
  DIR             *rep = opendir(s);
  struct dirent   *ent;

  if (!rep){
      (*lastError) = s + std::string(": Invalid directory");
      return ret;
    }
  if (!rep){
      if (lastError){
          (*lastError) = s + std::string(": Invalid directory");
        }
      return ret;
    }
  while ((ent = readdir(rep))) {
      std::string tmp = path + ent->d_name;
      struct stat buf;

      stat(tmp.c_str(), &buf);
      ret.push_back(std::pair<std::string, int>(std::string(ent->d_name), (buf.st_mode & S_IFMT) == S_IFDIR ? DIRECTORY : FILE));
    }
  closedir(rep);
#endif
  return ret;
}

std::vector<std::pair<std::string, int> >    Utils::getFolderDatas(std::string const &s, std::string *lastError)
{
  return Utils::getFolderDatas(s.c_str(), lastError);
}

void  Utils::sleep(unsigned int ms)
{
#ifdef WIN32
  Sleep(ms);
#else
  usleep(ms * 1000);
#endif
}

bool  Utils::playSound(const char *sound_name)
{
#ifdef WIN32
  return PlaySound(Utils::string2wstring(sound_name).c_str(), 0, SND_ASYNC) == TRUE;
#else
  return true;
#endif
}

HBITMAP Utils::getIconFromFileType(const char *sz, int *width, int *height, int *bitsPerPixel)
{
  char          drive[_MAX_DRIVE] = {0};
  char          dir[_MAX_DIR] = {0};
  char          fname[_MAX_FNAME] = {0};
  char          ext[_MAX_EXT] = {0};

  SHFILEINFO  shfi;

  HICON       hicon = 0;
  HIMAGELIST  hSysImageList;

  if (!sz)
    return 0;

  _splitpath(sz, drive, dir, fname, ext);
  std::string szext(fname);
  szext += ext;
  if ((szext.length() < 4 || szext.find(".exe") != std::string::npos ||
       szext.find(".pif") != std::string::npos || szext.find(".lnk") != std::string::npos) &&
      SHGetFileInfo(string2wstring(szext).c_str(), FILE_ATTRIBUTE_NORMAL, &shfi, sizeof(shfi),
                    SHGFI_USEFILEATTRIBUTES | SHGFI_ICON | SHGFI_LARGEICON))
    {
      hicon = shfi.hIcon;
    }
  if (!hicon)
    {
      hSysImageList = (HIMAGELIST)SHGetFileInfo(string2wstring(szext).c_str(), FILE_ATTRIBUTE_NORMAL, &shfi, sizeof(shfi),
                                                SHGFI_SYSICONINDEX | SHGFI_LARGEICON);

      if (hSysImageList)
        {
          int nic = shfi.iIcon;
          hicon = ImageList_GetIcon(hSysImageList, nic, ILD_NORMAL);
        }
    }
  if (hicon)
    {
      ICONINFO  inf;

      if (GetIconInfo(hicon, &inf) == TRUE){
          BITMAP  bmp;

          if (inf.hbmColor){
              if (GetObject(inf.hbmColor, sizeof(bmp), (LPSTR)&bmp)){
                  *width = bmp.bmWidth;
                  *height = bmp.bmHeight;
                  *bitsPerPixel = bmp.bmBitsPixel;

                  return inf.hbmColor;
                  //DeleteObject(inf.hbmColor);
                }
            }
          else{
              if (GetObject(inf.hbmMask, sizeof(bmp), (LPSTR)&bmp)){
                  *width = bmp.bmWidth;
                  *height = bmp.bmHeight / 2;
                  *bitsPerPixel = 1;

                  return inf.hbmMask;

                  //DeleteObject(inf.hbmMask);
                }
            }
        }
      /*HDC hDC = CreateCompatibleDC(NULL);
      HBITMAP hBitmap = CreateCompatibleBitmap(hDC, GetSystemMetrics(SM_CXICON), GetSystemMetrics(SM_CYICON));
      HBITMAP hOldBitmap = (HBITMAP)SelectObject(hDC, hBitmap);
      DrawIcon(hDC, 0, 0, hicon);
      SelectObject(hDC, hOldBitmap);
      DeleteDC(hDC);

      return hBitmap;*/
    }
  return 0;
}

void  Utils::destroyIcon(void *i)
{
  if (!i)
    return;
  DestroyIcon((HICON)i);
}

/*bool  Utils::saveBMP(const char *name, std::vector<unsigned char> const &image, unsigned int width, unsigned int height,
                     unsigned int depth)
{
  BITMAPFILEHEADER  header;
  BITMAPINFOHEADER  info;
  ::FILE            *fd;

  if (!name)
    return false;
  memset(&header, 0, sizeof(header));
  memset(&info, 0, sizeof(info));

  header.bfType = 0x4d42; // = 'BM'
  //header.bfReserved1 = 0;
  //header.bfReserved2 = 0;
  header.bfSize = sizeof(header) + sizeof(info) + depth;
  header.bfOffBits = 0x36;

  info.biSize = sizeof(info);
  info.biWidth = width;
  info.biHeight = height;
  info.biPlanes = 1;
  info.biBitCount = 24;
  info.biCompression = BI_RGB;
  //info.biSizeImage = 0;
  info.biXPelsPerMeter = 0x0ec4;
  info.biYPelsPerMeter = 0x0ec4;
  //info.biClrUsed = 0;
  //info.biClrImportant = 0;

  if (!(fd = fopen(name, "w"))){
      return false;
    }
  fwrite(&header, sizeof(header), 1, fd);
  fwrite(&info, sizeof(info), 1, fd);
  fwrite(&image[0], image.size(), sizeof(image[0]), fd);
  fclose(fd);
  return true;
}

bool Utils::decode(unsigned char *buffer, unsigned int &width, unsigned int &height,
                   std::vector<unsigned char> &image)
{
  LPICONDIR icoDir = (LPICONDIR)buffer;

  //LOG << "ICO:" <<  icoDir->idReserved << " "<<  icoDir->idType << " "<<  icoDir->idCount << NL;
  int iconsCount = icoDir->idCount;

  if (icoDir->idReserved != 0 || icoDir->idType != 1 || iconsCount == 0 || iconsCount > 20)
    return false;

  unsigned char *cursor = buffer;
  cursor += 6;
  ICONDIRENTRY* dirEntry = (ICONDIRENTRY*)(cursor);

  int maxSize = 0;
  int offset = 0;
  for (int i = 0; i < iconsCount; i++)
    {
      int w = dirEntry->bWidth;
      int h = dirEntry->bHeight;
      int colorCount = dirEntry->bColorCount;
      int bitCount = dirEntry->wBitCount;

      if (w * h > maxSize) // we choose icon with max resolution
        {
          width = w;
          height = h;
          offset = dirEntry->dwImageOffset;
        }

      //LOG << "Size:" << w << "x" << h << " bits:" << bitCount  << " colors: " << colorCount << " offset: " << (int)(dirEntry->dwImageOffset)  << " bytes: " << (int)(dirEntry->dwBytesInRes) << NL;

      dirEntry++;
    }

  if (offset == 0)
    return false;

  //LOG << "Offset: " << offset << NL;
  cursor = buffer;
  cursor += offset;


  ICONIMAGE* icon = (ICONIMAGE*)(cursor);
  //LOG << "BitmapInfo: Struct: " << (int)(icon->icHeader.biSize) << " Size: " << (int)(icon->icHeader.biWidth) << "x" << (int)(icon->icHeader.biHeight) << " Bytes: " << (int)(icon->icHeader.biSizeImage) << NL;
  //LOG << "BitmapInfo: Planes: " << (int)icon->icHeader.biPlanes << " bits: " << (int)icon->icHeader.biBitCount << " Compression: " << (int)icon->icHeader.biCompression << NL;
  int realBitsCount = (int)icon->icHeader.biBitCount;
  bool hasAndMask = (height != icon->icHeader.biHeight);

  cursor += 40;
  int numBytes = width * height * 4;
  image.resize(numBytes);

  // rgba + vertical swap
  if (realBitsCount == 32)
    {
      int shift;
      int shift2;
      for (int x = 0; x < width; x++)
        for (int y = 0; y < height; y++)
          {
            shift = 4 * (x + y * width);
            shift2 = 4 * (x + (height - y - 1) * width);
            image[shift] = cursor[shift2 + 2];
            image[shift + 1] = cursor[shift2 + 1];
            image[shift + 2] = cursor[shift2];
            image[shift + 3] = cursor[shift2 + 3];
          }
    }
  else if (realBitsCount == 24)
    {
      int shift;
      int shift2;
      for (int x = 0; x < width; x++)
        for (int y = 0; y < height; y++)
          {
            shift = 4 * (x + y * width);
            shift2 = 3 * (x + (height - y - 1) * width);
            image[shift] = cursor[shift2 + 2];
            image[shift + 1] = cursor[shift2 + 1];
            image[shift + 2] = cursor[shift2];
            image[shift + 3] = 255;
          }
    }

  else if (realBitsCount == 8)  /// 256 colors
    {
      // 256 color table
      unsigned char * colors = (unsigned char *)cursor;
      cursor += 256 * 4;
      int shift;
      int shift2;
      int index;
      for (int x = 0; x < width; x++)
        for (int y = 0; y < height; y++)
          {
            shift = 4 * (x + y * width);
            shift2 = (x + (height - y - 1) * width);
            index = 4 * cursor[shift2];
            image[shift] = colors[index + 2];
            image[shift + 1] = colors[index + 1];
            image[shift + 2] = colors[index];
            image[shift + 3] = 255;
          }
    }

  else if (realBitsCount == 4)  /// 16 colors
    {
      // 16 color table
      unsigned char * colors = (unsigned char *)cursor;
      cursor += 16 * 4;
      int shift;
      int shift2;
      unsigned char index;

      for (int x = 0; x < width; x++)
        for (int y = 0; y < height; y++)
          {
            shift = 4 * (x + y * width);
            shift2 = (x + (height - y - 1) * width);
            index = cursor[shift2 / 2];
            if (shift2 % 2 == 0)
              index = (index >> 4) & 0xF;
            else
              index = index & 0xF;
            index *= 4;

            image[shift] = colors[index + 2];
            image[shift + 1] = colors[index + 1];
            image[shift + 2] = colors[index];
            image[shift + 3] = 255;
          }
    }

  // Read AND mask after base color data - 1 BIT MASK
  if (hasAndMask)
    {
      int shift;
      int shift2;
      unsigned char bit;
      int mask;
      cursor += realBitsCount * width * height / 8;
      int boundary = 32; //!!! 32 bit boundary (http://www.daubnet.com/en/file-format-ico)
      if (width > boundary)
        boundary = width;

      for (int y = 0; y < height; y++)
        for (int x = 0; x < width; x++)
          {
            shift = 4 * (x + y * width) + 3;
            bit = 7 - (x % 8);
            shift2 = (x + (height - y - 1) * boundary) / 8;
            mask = (0x01 & ((unsigned char)cursor[shift2] >> bit));
            //LOG << "Bit: " << bit << "Value: " << mask << " from byte: " << cursor[shift2] << " row: " << y << " index:" << shift2 << NL;
            image[shift] *= 1 - mask;
          }
    }
  return true;
}*/

char  *Utils::getBitmapDatas(HBITMAP bitmap, int *size)
{
  if (!size)
    return 0;
  *size = 0;
  BITMAP bmp;
  PBITMAPINFO pbmi;
  WORD cClrBits;
  BITMAPFILEHEADER hdr;
  PBITMAPINFOHEADER pbih;
  LPBYTE lpBits;
  BYTE *hp;
  HDC   hDC;

  hDC = CreateCompatibleDC(0);

  if (!GetObject(bitmap, sizeof(BITMAP), (LPSTR)&bmp))
    {
      DeleteDC(hDC);
      return 0;
    }

  cClrBits = (WORD)(bmp.bmPlanes * bmp.bmBitsPixel);
  if (cClrBits == 1)
    cClrBits = 1;
  else if (cClrBits <= 4)
    cClrBits = 4;
  else if (cClrBits <= 8)
    cClrBits = 8;
  else if (cClrBits <= 16)
    cClrBits = 16;
  else if (cClrBits <= 24)
    cClrBits = 24;
  else
    cClrBits = 32;
  if (cClrBits != 24)
    pbmi = (PBITMAPINFO) LocalAlloc(LPTR, sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * (1<< cClrBits));
  else
    pbmi = (PBITMAPINFO) LocalAlloc(LPTR, sizeof(BITMAPINFOHEADER));

  pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
  pbmi->bmiHeader.biWidth = bmp.bmWidth;
  pbmi->bmiHeader.biHeight = bmp.bmHeight;
  pbmi->bmiHeader.biPlanes = bmp.bmPlanes;
  pbmi->bmiHeader.biBitCount = bmp.bmBitsPixel;
  if (cClrBits < 24)
    pbmi->bmiHeader.biClrUsed = (1<<cClrBits);

  pbmi->bmiHeader.biCompression = BI_RGB;

  pbmi->bmiHeader.biSizeImage = (pbmi->bmiHeader.biWidth + 7) /8 * pbmi->bmiHeader.biHeight * cClrBits;
  pbmi->bmiHeader.biClrImportant = 0;

  pbih = (PBITMAPINFOHEADER) pbmi;
  lpBits = (LPBYTE) GlobalAlloc(GMEM_FIXED, pbih->biSizeImage);

  if (!lpBits) {
      DeleteDC(hDC);
      return 0;
    }

  if (!GetDIBits(hDC, HBITMAP(bitmap), 0, (WORD) pbih->biHeight, lpBits, pbmi, DIB_RGB_COLORS)) {
      DeleteDC(hDC);
      return 0;
    }

  hdr.bfType = 0x4d42;
  hdr.bfSize = (DWORD) (sizeof(BITMAPFILEHEADER) + pbih->biSize + pbih->biClrUsed * sizeof(RGBQUAD) + pbih->biSizeImage);
  hdr.bfReserved1 = 0;
  hdr.bfReserved2 = 0;

  // Compute the offset to the array of color indices.
  hdr.bfOffBits = (DWORD) sizeof(BITMAPFILEHEADER) + pbih->biSize + pbih->biClrUsed * sizeof (RGBQUAD);


  char *datas = new char[sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER)
      + pbih->biClrUsed * sizeof (RGBQUAD) + pbih->biSizeImage];

  hp = lpBits;

  memcpy(datas, (LPVOID)&hdr, sizeof(BITMAPFILEHEADER));
  *size += sizeof(BITMAPFILEHEADER);
  memcpy(datas + *size, (LPVOID) pbih, sizeof(BITMAPINFOHEADER) + pbih->biClrUsed * sizeof (RGBQUAD));
  *size += sizeof(BITMAPINFOHEADER) + pbih->biClrUsed * sizeof (RGBQUAD);
  memcpy(datas + *size, (LPSTR) hp, pbih->biSizeImage);
  *size += pbih->biSizeImage;
  GlobalFree((HGLOBAL)lpBits);
  DeleteDC(hDC);
  return datas;
}

bool  Utils::saveBMP(const char *filename, HBITMAP bitmap)
{
  BITMAP bmp;
  PBITMAPINFO pbmi;
  WORD cClrBits;
  HANDLE hf; // file handle
  BITMAPFILEHEADER hdr; // bitmap file-header
  PBITMAPINFOHEADER pbih; // bitmap info-header
  LPBYTE lpBits; // memory pointer
  DWORD cb; // incremental count of bytes
  BYTE *hp; // byte pointer
  DWORD dwTmp;
  HDC   hDC;

  hDC = CreateCompatibleDC(0);
  // create the bitmapinfo header information

  if (!GetObject(bitmap, sizeof(BITMAP), (LPSTR)&bmp))
    {
      //AfxMessageBox(L"Could not retrieve bitmap info");
      DeleteDC(hDC);
      return false;
    }

  // Convert the color format to a count of bits.
  cClrBits = (WORD)(bmp.bmPlanes * bmp.bmBitsPixel);
  if (cClrBits == 1)
    cClrBits = 1;
  else if (cClrBits <= 4)
    cClrBits = 4;
  else if (cClrBits <= 8)
    cClrBits = 8;
  else if (cClrBits <= 16)
    cClrBits = 16;
  else if (cClrBits <= 24)
    cClrBits = 24;
  else
    cClrBits = 32;
  // Allocate memory for the BITMAPINFO structure.
  if (cClrBits != 24)
    pbmi = (PBITMAPINFO) LocalAlloc(LPTR, sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * (1<< cClrBits));
  else
    pbmi = (PBITMAPINFO) LocalAlloc(LPTR, sizeof(BITMAPINFOHEADER));

  // Initialize the fields in the BITMAPINFO structure.

  pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
  pbmi->bmiHeader.biWidth = bmp.bmWidth;
  pbmi->bmiHeader.biHeight = bmp.bmHeight;
  pbmi->bmiHeader.biPlanes = bmp.bmPlanes;
  pbmi->bmiHeader.biBitCount = bmp.bmBitsPixel;
  if (cClrBits < 24)
    pbmi->bmiHeader.biClrUsed = (1<<cClrBits);

  // If the bitmap is not compressed, set the BI_RGB flag.
  pbmi->bmiHeader.biCompression = BI_RGB;

  // Compute the number of bytes in the array of color
  // indices and store the result in biSizeImage.
  pbmi->bmiHeader.biSizeImage = (pbmi->bmiHeader.biWidth + 7) /8 * pbmi->bmiHeader.biHeight * cClrBits;
  // Set biClrImportant to 0, indicating that all of the
  // device colors are important.
  pbmi->bmiHeader.biClrImportant = 0;

  // now open file and save the data
  pbih = (PBITMAPINFOHEADER) pbmi;
  lpBits = (LPBYTE) GlobalAlloc(GMEM_FIXED, pbih->biSizeImage);

  if (!lpBits) {
      //AfxMessageBox(L"writeBMP::Could not allocate memory");
      DeleteDC(hDC);
      return false;
    }

  // Retrieve the color table (RGBQUAD array) and the bits
  if (!GetDIBits(hDC, HBITMAP(bitmap), 0, (WORD) pbih->biHeight, lpBits, pbmi, DIB_RGB_COLORS)) {
      //AfxMessageBox(L"writeBMP::GetDIB error");
      DeleteDC(hDC);
      return false;
    }

  // Create the .BMP file.
  hf = CreateFile(string2wstring(filename).c_str(), GENERIC_READ | GENERIC_WRITE, (DWORD) 0,
                  NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL,
                  (HANDLE) NULL);
  if (hf == INVALID_HANDLE_VALUE){
      //AfxMessageBox(L"Could not create file for writing");
      DeleteDC(hDC);
      return false;
    }
  hdr.bfType = 0x4d42; // 0x42 = "B" 0x4d = "M"
  // Compute the size of the entire file.
  hdr.bfSize = (DWORD) (sizeof(BITMAPFILEHEADER) + pbih->biSize + pbih->biClrUsed * sizeof(RGBQUAD) + pbih->biSizeImage);
  hdr.bfReserved1 = 0;
  hdr.bfReserved2 = 0;

  // Compute the offset to the array of color indices.
  hdr.bfOffBits = (DWORD) sizeof(BITMAPFILEHEADER) + pbih->biSize + pbih->biClrUsed * sizeof (RGBQUAD);

  // Copy the BITMAPFILEHEADER into the .BMP file.
  if (!WriteFile(hf, (LPVOID) &hdr, sizeof(BITMAPFILEHEADER),
                 (LPDWORD) &dwTmp, NULL)) {
      //AfxMessageBox(L"Could not write in to file");
      DeleteDC(hDC);
      return false;
    }

  // Copy the BITMAPINFOHEADER and RGBQUAD array into the file.
  if (!WriteFile(hf, (LPVOID) pbih, sizeof(BITMAPINFOHEADER) + pbih->biClrUsed * sizeof (RGBQUAD),
                 (LPDWORD) &dwTmp, ( NULL))){
      //AfxMessageBox(L"Could not write in to file");
      DeleteDC(hDC);
      return false;
    }


  // Copy the array of color indices into the .BMP file.
  cb = pbih->biSizeImage;
  hp = lpBits;
  if (!WriteFile(hf, (LPSTR) hp, (int) cb, (LPDWORD) &dwTmp,NULL)){
      //AfxMessageBox(L"Could not write in to file");
      DeleteDC(hDC);
      return false;
    }

  // Close the .BMP file.
  if (!CloseHandle(hf)){
      //AfxMessageBox(L"Could not close file");
      DeleteDC(hDC);
      return false;
    }

  // Free memory.
  GlobalFree((HGLOBAL)lpBits);
  DeleteDC(hDC);
  return true;
}
