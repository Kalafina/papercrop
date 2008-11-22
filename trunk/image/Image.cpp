//
// Image.cpp
//
// Copyright 2004 by Taesoo Kwon.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Library General Public License for more details.
//
// You should have received a copy of the GNU Library General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA.
//

// Image.cpp: implementation of the CImage class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Image.h"
#include <il/il.h>
#include <il/ilu.h>

#define CHECK_VALID ASSERT((!_dataPtr || _dataPtr==ilGetData()) && GetWidth()==ilGetInteger(IL_IMAGE_WIDTH) && GetHeight()==ilGetInteger(IL_IMAGE_HEIGHT))

CPixelsView CPixels::range(int start, int end, int step)
{
	return _range<CPixelsView >(start,end,step);
}

const CPixelsView CPixels::range(int start, int end, int step) const	
{
	return ((CPixels*)this)->range(start, end, step);
}

CPixelRGB8 CPixels::average() const
{
	long sumR=0, sumG=0, sumB=0;

	for(int i=0; i<size(); i++)
	{
		sumR+=(long)value(i).R;
		sumG+=(long)value(i).G;
		sumB+=(long)value(i).B;
	}
	sumR/=size();
	sumG/=size();
	sumB/=size();
	return CPixelRGB8(sumR, sumG, sumB);
}

;
//int CImage::__uniqueID=1;

CImage::CImage()
	:_dataPtr(NULL),
	_stride(0),
	_size(0,0)//,
//	_id(__uniqueID)
{
//	if(__uniqueID==1)
//	{
//	}

//	__uniqueID++;
	ILuint images[1];
	ilGenImages(1, images);
	_id=images[0];

}

CImage::~CImage()
{
#ifdef _DEBUG
	ilBindImage(_id);
	CHECK_VALID;
#endif

	if(_dataPtr)
	{
		ILuint images[1];
		images[0]=_id;
		ilDeleteImages(1, images);
	}
}

int CImage::GetWidth() const 
{return _size.x;}
int CImage::GetHeight()const 
{return _size.y;}

void CImage::CopyFrom(CImage const& other)
{
	_size.x=other.GetWidth();
	_size.y=other.GetHeight();
	ilBindImage(_id);
	ilCopyImage(other._id);
	_dataPtr=ilGetData();
	_stride=GetWidth()*3;

	CHECK_VALID;
}

void CImage::SetData(int width, int height, uchar* dataPtr, int stride)
{
	ilBindImage(_id);
	ilTexImage(width, height, 1, 3, IL_RGB, IL_UNSIGNED_BYTE, NULL);
	_size=Int2D(width, height);
	
	for(int i=0; i<GetHeight(); i++)
	{
		int ii=GetHeight()-i-1;
		ilSetPixels(0,i,0, GetWidth(), 1, 1, IL_RGB, IL_UNSIGNED_BYTE, dataPtr+stride*ii); 
	}

	_dataPtr=ilGetData();
	_stride=GetWidth()*3;

	CHECK_VALID;
}

// assumes RGB8 format.
bool CImage::Create(int width, int height)
{
	_size.x=width;
	_size.y=height;
	ilBindImage(_id);
	ilTexImage(width, height, 1, 3, IL_RGB, IL_UNSIGNED_BYTE, NULL);
	_dataPtr=ilGetData();
	_stride=GetWidth()*3;
	CHECK_VALID;

	return true;
}

void CImage::GetVertLine(int i, CPixels& out)
{
	out.setSize(GetHeight());

	for(int j=0; j<GetHeight(); j++)
	{
		out[j]=*GetPixel(i,j);
	}
}

bool CImage::Load(const char* filename)
{
	ilBindImage(_id);
	ilLoadImage(filename);
	ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE);
	_size.x= ilGetInteger(IL_IMAGE_WIDTH);
	_size.y= ilGetInteger(IL_IMAGE_HEIGHT);
	_dataPtr=ilGetData();
	_stride=GetWidth()*3;
	CHECK_VALID;

	return true;
}

#include <il/il.h>
#include <il/ilu.h>

namespace Imp
{
	bool IsFileExist(const char* filename);
}


#include "FreeImage.h"
#pragma comment(lib, "FreeImage/Dist/FreeImage.lib")

/** Generic image writer
@param dib Pointer to the dib to be saved
@param lpszPathName Pointer to the full file name
@param flag Optional save flag constant
@return Returns true if successful, returns false otherwise
*/
bool GenericWriter(FIBITMAP* dib, const char* lpszPathName, int flag=0) {
	FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
	BOOL bSuccess = FALSE;

	if(dib) {
		// try to guess the file format from the file extension
		fif = FreeImage_GetFIFFromFilename(lpszPathName);
		if(fif != FIF_UNKNOWN ) {
			// check that the plugin has sufficient writing and export capabilities ...
			WORD bpp = FreeImage_GetBPP(dib);
			if(FreeImage_FIFSupportsWriting(fif) && FreeImage_FIFSupportsExportBPP(fif, bpp)) {
				// ok, we can save the file
				bSuccess = FreeImage_Save(fif, dib, lpszPathName, flag);
				// unless an abnormal bug, we are done !
			}
		}
	}
	return (bSuccess == TRUE) ? true : false;
}


void CImage_SaveFreeImage(CImage & image, const char* filename)
{
//	applyFloydSteinberg(image, 4);

	BYTE* bits=image._dataPtr;
	int width=image.GetWidth();
	int height=image.GetHeight();
	int scan_width=image._stride;

	// convert a 32-bit raw buffer (top-left pixel first) to a FIBITMAP
	// ----------------------------------------------------------------
	FIBITMAP *dst = FreeImage_ConvertFromRawBits(bits, width, height, scan_width,
		24, FI_RGBA_BLUE_MASK, FI_RGBA_GREEN_MASK, FI_RGBA_RED_MASK, FALSE);

	FIBITMAP *dst_grey=FreeImage_ConvertToGreyscale(dst);

	TString ext=TString(filename).right(3).toUpper();
	if(ext=="BMP")
	{
		FIBITMAP *dst2 = FreeImage_ConvertTo4Bits(dst_grey);
		GenericWriter(dst2, filename);
		FreeImage_Unload(dst2);
	}
	else if(ext=="GIF")
	{
		GenericWriter(dst_grey, filename);
	}

	//FIBITMAP *dst2 = FreeImage_Dither(dst, FID_FS);
	FreeImage_Unload(dst);
	FreeImage_Unload(dst_grey);
}

bool CImage::Save(const char* filename)
{
	if(Imp::IsFileExist(filename))
	{
		if(!Msg::confirm("Do you want to overwrite file %s?", filename))
			return false;
		DeleteFile(filename);
	}

	TString ext=TString(filename).right(3).toUpper();
	if(ext=="BMP" || ext=="GIF")
	{
		CImage_SaveFreeImage(*this, filename);
	}
	else
	{
		ilBindImage(_id);
		ilSaveImage(filename);
	}
	return 1;
}


/**
* @author Ralph Hauwert
* 
* The ImageDithering class allows for simple image color quantization and dithering.
* It aims to offer some well-known spatial dithering algorithms for the AS3 Language.
* 
* ralph@unitzeroone.com
* http://www.unitzeroone.com
*/

/**
* applyFloydSteinberg(bitmapData:BitmapData, levels:int);
* 
* Floyd-Steinberg dithering is an image dithering algorithm first published in 1976 by Robert W. Floyd and Louis Steinberg.
* Using error diffusion, this form of dithering is visually much better then ordered diffusion like Bayer's.
* R.W. Floyd, L. Steinberg, "An adaptive algorithm for spatial grey scale". Proceedings of the Society of Information Display 17, 75??7 (1976).
*/
#include "ImagePixel.h"
void applyFloydSteinberg(CImage& _bitmapData, int _levels)
{
	double levels=(double)_levels;
	int lNorm= 256/_levels;

	//The FS kernel...note the 16th. Optimisation can still be done.
	double d1= 7/16.0;
	double d2= 3/16.0;
	double d3= 5/16.0;
	double d4= 1/16.0;

	int lc, r, g, b, nr, ng, nb, er, eg, eb, lr, lg, lb;
	int x=0;
	int y=0;

	CImagePixel bitmapData(&_bitmapData);
	for(y=0; y<bitmapData.Height()-1;y++)
	{
		CPixelRGB8* line=bitmapData[y];
		CPixelRGB8* nline=bitmapData[y+1];

		for(x=1; x<bitmapData.Width()-1;x++)
		{
			//Retrieve current RGB value.
			CPixelRGB8& c = line[x];
			r = c.R;
			g = c.G;
			b = c.B;

			//Normalize and scale to the number of levels.
			//basically a cheap but suboptimal form of color quantization.
//			nr = ROUND((r/256.0) * levels) * lNorm;
//			ng = ROUND((g/256.0) * levels) * lNorm;
//			nb = ROUND((b/256.0) * levels) * lNorm;

			nr = r/lNorm* lNorm+ lNorm/2;
			ng = g/lNorm* lNorm+ lNorm/2;
			nb = b/lNorm* lNorm+ lNorm/2;


			//Set the current pixel.
			c.R=nr;
			c.G=ng;
			c.B=nb;

			//Quantization error.
			er = r-nr;
			eg = g-ng;
			eb = b-nb;

			//Apply the kernel.
			//+1,0
			{
				CPixelRGB8& lc = line[x+1];
				lr = (int)lc.R + (d1*er);
				lg = (int)lc.G + (d1*eg);
				lb = (int)lc.B + (d1*eb);

				//Clip & Set
				lr < 0 ? lr = 0: lr > 255 ? lr = 255 : lr;
				lg < 0 ? lg = 0: lg > 255 ? lg = 255 : lg;
				lb < 0 ? lb = 0: lb > 255 ? lb = 255 : lb;

				lc.R=lr;
				lc.G=lg;
				lc.B=lb;
			}

			//-1,+1
			{
				CPixelRGB8& lc = nline[x-1];
				lr = (int)lc.R + (d2*er);
				lg = (int)lc.G + (d2*eg);
				lb = (int)lc.B + (d2*eb);

				//Clip & Set
				lr < 0 ? lr = 0: lr > 255 ? lr = 255 : lr;
				lg < 0 ? lg = 0: lg > 255 ? lg = 255 : lg;
				lb < 0 ? lb = 0: lb > 255 ? lb = 255 : lb;
				
				lc.R=lr;
				lc.G=lg;
				lc.B=lb;
			}

			//0,+1
			{
				CPixelRGB8& lc = nline[x];
				lr = (int)lc.R + (d3*er);
				lg = (int)lc.G + (d3*eg);
				lb = (int)lc.B + (d3*eb);

				//Clip & Set
				lr < 0 ? lr = 0: lr > 255 ? lr = 255 : lr;
				lg < 0 ? lg = 0: lg > 255 ? lg = 255 : lg;
				lb < 0 ? lb = 0: lb > 255 ? lb = 255 : lb;

				lc.R=lr;
				lc.G=lg;
				lc.B=lb;
			}

			//+1,+1
			{
				CPixelRGB8& lc = nline[x+1];
				lr = (int)lc.R + (d4*er);
				lg = (int)lc.G + (d4*eg);
				lb = (int)lc.B + (d4*eb);

				//Clip & Set
				lr < 0 ? lr = 0: lr > 255 ? lr = 255 : lr;
				lg < 0 ? lg = 0: lg > 255 ? lg = 255 : lg;
				lb < 0 ? lb = 0: lb > 255 ? lb = 255 : lb;

				lc.R=lr;
				lc.G=lg;
				lc.B=lb;
			}
		}
	}

	for(y=0; y<bitmapData.Height();y++)
	{
		CPixelRGB8* line=bitmapData[y];

		for(x=0; x<bitmapData.Width();x++)
		{
			CPixelRGB8& c = line[x];
			r = c.R;
			g = c.G;
			b = c.B;

			//Normalize and scale to the number of levels.
			//basically a cheap but suboptimal form of color quantization.
			//nr = ROUND((r/256.0) * levels) * lNorm;
			//ng = ROUND((g/256.0) * levels) * lNorm;
			//nb = ROUND((b/256.0) * levels) * lNorm;

			nr = r/lNorm* lNorm+ lNorm/2;
			ng = g/lNorm* lNorm+ lNorm/2;
			nb = b/lNorm* lNorm+ lNorm/2;

			c.R=nr;
			c.G=ng;
			c.B=nb;
		}
	}
}
