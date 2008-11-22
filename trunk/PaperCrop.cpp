
//
// PaperCrop.cpp 
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

#include "stdafx.h"
#include "PaperCrop.h"

#include "PDFwin.h"
#include "RightPanel.h"
MainWin::MainWin(const char* title) 
	: Fl_Window(WIDTH, HEIGHT, title)		
{
	//Fl_Tile* tile=new Fl_Tile (0,0,WIDTH,HEIGHT);
	
	int ww=WIDTH-RENDERER_WIDTH;

	mPDFwin=new PDFwin(0,0,RENDERER_WIDTH,HEIGHT);
	mRightPanel	=new RightPanel(RENDERER_WIDTH,0, ww, HEIGHT, mPDFwin);
	//tile->end();
	end();		

	resizable(mPDFwin);
}

void MainWin::onIdle()
{
	mPDFwin->onIdle();
}

#include <il/il.h>
#include <il/ilu.h>

#ifdef _DEBUG	// console·Î ºôµåÇÏµµ·Ï ÇØ³ùÀ½.
int _tmain(int argc, _TCHAR* argv[])
#else
int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
#endif
{

	if (!Fl::visual(FL_DOUBLE|FL_INDEX))
		printf("Xdbe not supported, faking double buffer with pixmaps.\n"); 
	Fl::scheme("plastic");

	ilInit();
	iluInit();



	MainWin win("PaperCrop");

	win.show();

	for (;;) 
	{
		if (win.visible())
		{
			if (!Fl::check()) 
				break;	// returns immediately
		}	
		else
		{
			if (!Fl::wait()) break;	// waits until something happens
		}	

		// idle time
		win.onIdle();
	}

	return 1;

}
