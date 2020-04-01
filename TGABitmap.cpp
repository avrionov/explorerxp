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


#include "StdAfx.h"
#include ".\tgabitmap.h"

TGABitmap::TGABitmap(void)
{
}

TGABitmap::~TGABitmap(void)
{
}


void TGABitmap::loadFromResource (DWORD resourceNum)
{
	HRSRC res = FindResource(GetModuleHandle(NULL), MAKEINTRESOURCE(resourceNum),_T("BINARY"));

	if (!res)  return;
		
	HGLOBAL mem = LoadResource(GetModuleHandle(NULL), res);
	void *data = LockResource(mem);
		
	TGA_Header *header;

    //DWORD dwRead = 0;
    //ReadFile(handle, & header, sizeof(header), & dwRead, NULL);

	header = (TGA_Header *)data;

	_hBmp = NULL;
    if ( (header->IDLength!=0) || (header->ColorMapType!=0) || (header->ImageType!=2) ||
         (header->PixelDepth!=32) || (header->ImageDescriptor!=8) )
    {        
		FreeResource (mem);
        return;
    }

    BITMAPINFO bmp = { { sizeof(BITMAPINFOHEADER), header->Width, header->Height, 1, 32 } };

	_bmi = bmp;

    _pBits = NULL;

    _hBmp = CreateDIBSection(NULL, & bmp, DIB_RGB_COLORS, & _pBits, NULL, NULL);

    if ( _hBmp==NULL )
    {
        FreeResource (mem);
        return ;
    }

	memcpy ((BYTE*)_pBits, (BYTE*)data + sizeof (TGA_Header), header->Width * header->Height * 4);
    	
    for (int y=0; y < header->Height; y++)
    {
        BYTE * pPixel = (BYTE *) _pBits + header->Width * 4 * y;

        for (int x=0; x < header->Width; x++)
        {
            pPixel[0] = pPixel[0] * pPixel[3] / 255; 
            pPixel[1] = pPixel[1] * pPixel[3] / 255; 
            pPixel[2] = pPixel[2] * pPixel[3] / 255; 

            pPixel += 4;
        }
    }

	FreeResource (mem);
}

HBITMAP TGABitmap::getHBITMAP() 
{
	return _hBmp;
}

int TGABitmap::getWidth()
{
	return _bmi.bmiHeader.biWidth;
}

int TGABitmap::getHeight() 
{
	return _bmi.bmiHeader.biHeight;
}

int TGABitmap::copyAll (TGABitmap& tga) {
	memcpy ((BYTE*)_pBits, tga._pBits, tga.getWidth() *4 * getHeight());
	return TRUE;
}

int TGABitmap::copyRect (TGABitmap& tga, RECT rc) 
{
	for (int y = rc.top; y < rc.bottom; y++)
	{
		int inv_y = getHeight() - y;

		BYTE* bits_src = (BYTE*)tga._pBits + inv_y * tga.getWidth()*4 + rc.left *4;
		BYTE* bits_dst = (BYTE*)_pBits + inv_y * getWidth()*4 + rc.left *4;
		
		for (int x = rc.left; x < rc.right; x++)
		{
			bits_dst[0] = bits_src[0]; 
            bits_dst[1] = bits_src[1]; 
            bits_dst[2] = bits_src[2]; 
			bits_dst[3] = bits_src[3];

			bits_dst+=4;
			bits_src+=4;
		}
	}
		
	return TRUE;
}

void addPixel (BYTE *bits_dst, int add)
{
	bits_dst[0] = max(0, bits_dst[0] + static_cast<BYTE>(add)); 
    bits_dst[1] = max(0, bits_dst[1] + static_cast<BYTE>(add)); 
    bits_dst[2] = max(0, bits_dst[2] + static_cast<BYTE>(add)); 	
}

int TGABitmap::darkenRect ( RECT rc, int darkColor)
{
	for (int y = rc.top; y <= rc.bottom; y +=2)
	{
		int inv_y = getHeight() - y;				
		BYTE* bits_dst = (BYTE*)_pBits + inv_y * getWidth()*4 + rc.left *4;
		addPixel (bits_dst, darkColor);
		bits_dst = (BYTE*)_pBits + inv_y * getWidth()*4 + rc.right *4;
		addPixel (bits_dst, darkColor);		
	}

	for (int x = rc.left; x < rc.right; x +=2)
		{
			int inv_y = getHeight() - rc.top;				

			BYTE* bits_dst = (BYTE*)_pBits + inv_y * getWidth()*4 + x *4;
			addPixel (bits_dst, darkColor);
			inv_y = getHeight() - rc.bottom;							
			bits_dst = (BYTE*)_pBits + inv_y * getWidth()*4 + x *4;
			addPixel (bits_dst, darkColor);
		}

	return TRUE;
}