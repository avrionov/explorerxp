/* Copyright 2002-2020 Nikolay Avrionov. All Rights Reserved.
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
* http://www.apache.org/licenses/LICENSE-2.0
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/


#pragma once

#pragma pack(push, 1)

typedef struct
{
    BYTE    IDLength;        // 0 
    BYTE    ColorMapType;    // 0
    BYTE    ImageType;       // 2: Truecolor image data
    WORD    CMapStart;       // 0
    WORD    CMapLength;      // 0
    BYTE    CMapDepth;       // 0
    WORD    XOffset;         // 0
    WORD    YOffset;         // 0
    WORD    Width;           // width
    WORD    Height;          // height
    BYTE    PixelDepth;      // 32 for 32-bpp image
    BYTE    ImageDescriptor; // 8 for 8-bit alpha
}   TGA_Header;

#pragma pack(pop)

class TGABitmap
{
public:
	TGABitmap(void);
	~TGABitmap(void);
	void loadFromResource(DWORD res);
	HBITMAP getHBITMAP();
	int getWidth();
	int getHeight();
	int copyAll (TGABitmap& tga);
	int copyRect (TGABitmap& tga, RECT rc);
	int darkenRect ( RECT rc, int darkColor);
//protected:
public:

	HBITMAP _hBmp;
	void* _pBits;
	BITMAPINFO _bmi;
	bool _bNeedRefresh;
	
};
