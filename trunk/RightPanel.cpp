//
// RightPanel.cpp 
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
#include "RightPanel.h"
#include "PDFwin.h"
#include "utility/operatorString.h"
#include "WrapperLua/BaselibLUA.h"
#include <luabind/luabind.hpp>
#include <luabind/function.hpp>
//#include <luabind/policy.hpp>
#include <luabind/operator.hpp>
#include "PDFWriter.h"
#include "utility/FltkAddon.h"

void reflow(CImage& inout, int desired_width, int min_gap, int max_gap, int thr_white, double right_max_margin, int figure_min_height);
void trimVertSpaces(CImage& inout, int min_gap, int max_gap, int thr_white) ;


using namespace luabind;

TString processOption(const char* option)
{
	TString opt=option;
	opt.replace(") ", ")\n");
	opt.replace(" (", " \n(");
	return opt;
}

TString unprocessOption(const char* option)
{
	TString opt=option;
	opt.replace(")\n", ") ");
	opt.replace(" \n(", " (");
	return opt;
}

void RightPanel::doLUAFile(const char* filename)
{
	LUAwrapper L;
	L.dofile(filename);

	double min_gap=L.getValue<double>("MIN_gap");
	double margin=L.getValue<double>("margin");
	double nColumns=L.getValue<double>("N_columns");
	int white_point=L.getValue<int>("white_point");
	double cropT=L.getValue<double>("crop_T");
	double cropB=L.getValue<double>("crop_B");
	double cropL=L.getValue<double>("crop_L");
	double cropR=L.getValue<double>("crop_R");
	std::string option=L.getValue<std::string>("option");

	FlLayout* layout=findLayout("Automatic segmentation");
	layout->findSlider("MIN gap")->value(min_gap);
	layout->findSlider("Margin")->value(margin);
	layout->findSlider("N columns")->value(nColumns);
	layout->findSlider("white point")->value(white_point);
	layout->findSlider("Crop T")->value(cropT);
	layout->findSlider("Crop B")->value(cropB);
	layout->findSlider("Crop L")->value(cropL);
	layout->findSlider("Crop R")->value(cropR);
	find<Fl_Input>("Option_Input")->value(processOption(option.c_str()));

	redraw();
}

RightPanel::RightPanel(int x, int y, int w, int h, PDFwin* pdfwin)
:FlLayout(x,y, w, h)
{
	create("Button", "Load a PDF file", "Load a PDF file");

	button(0)->tooltip("Alt+L");
	button(0)->shortcut(FL_ALT+'l');

	create("Button", "Preset", "default preset");

	create("Check_Button", "Use automatic segmentation", "Use automatic segmentation");
	checkButton(0)->value(1);

	

	create("Layout", "Automatic segmentation", "segmentation parameters");
	layout(0)->create("Value_Slider", "MIN gap","MIN gap", 1);
	layout(0)->slider(0)->range(0.0, 10.0);
	layout(0)->create("Value_Slider", "MIN text-gap","MIN text-gap");
	layout(0)->slider(0)->range(1.0, 10.0);
	layout(0)->slider(0)->value(2.0);
	layout(0)->create("Value_Slider", "Margin","Margin");
	layout(0)->slider(0)->range(0.5, 10.0);
	layout(0)->create("Value_Slider", "N columns","N columns");
	layout(0)->slider(0)->range(0.7, 10);
	layout(0)->slider(0)->step(0.1);
	layout(0)->create("Value_Slider", "white point","white point");
	layout(0)->slider(0)->range(230, 255);
	layout(0)->slider(0)->step(1);

	layout(0)->create("Value_Slider", "Crop T","Crop Top");
	layout(0)->slider(0)->range(0, 20);
	layout(0)->slider(0)->step(0.1);
	layout(0)->create("Value_Slider", "Crop B","Crop Bttm");
	layout(0)->slider(0)->range(0, 20);
	layout(0)->slider(0)->step(0.1);
	layout(0)->create("Value_Slider", "Crop L","Crop Left");
	layout(0)->slider(0)->range(0, 20);
	layout(0)->slider(0)->step(0.1);
	layout(0)->create("Value_Slider", "Crop R","Crop Right");
	layout(0)->slider(0)->range(0, 20);
	layout(0)->slider(0)->step(0.1);

	layout(0)->create("Button", "update","update");
	layout(0)->updateLayout();

	setWidgetHeight(60);
	setUniformGuidelines(5);
	create("Input", "Option_Input", "Option",1);
	widget<Fl_Input>(0)->type(FL_MULTILINE_OUTPUT);


	resetToDefault();

	create("Button", "Option", "Change option", 1);
	
	create("Button", "Process current page", "Process current page",0);
	button(0)->deactivate();

	create("Button", "Process all pages", "Process all pages",0);

	button(0)->deactivate();
	button(0)->tooltip("Alt+P");
	button(0)->shortcut(FL_ALT+'p');

	create("Button", "Batch process", "Batch process",0);

	create("Box", "Status");
	updateLayout();

	doLUAFile("presets/default preset.lua");
	mPDFwin=pdfwin;
	mPDFwin->mLayout=this;
}

RightPanel::~RightPanel(void)
{
}

bool CreateDirectory(const char *PathToCreate);
bool DeleteAllFiles(const char* path, const char* mask,bool bConfirm);

#include <FL/Fl_Native_File_Chooser.H>
void RightPanel::onCallback(FlLayout::Widget const& w, Fl_Widget * pWidget, int userData)
{
	if(w.mId=="update")
	{
		mPDFwin->pageChanged();
		mPDFwin->redraw();
	}
	else if(w.mId=="Preset")
	{
		TString fn=FlChooseFile("Choose preset", "presets","*.lua");
		if(fn.length())
		{
			TString ff, dir;
			ff=sz1::filename(fn, dir);
			w.button()->copy_label(ff.left(-4).ptr());
			doLUAFile(TString("presets/")+w.button()->label()+TString(".lua"));		
			mPDFwin->pageChanged();
			mPDFwin->redraw();
			redraw();
		}
	}
	else if(w.mId=="Use automatic segmentation")
	{
		if(w.checkButton()->value())
		{
			findLayout("Automatic segmentation")->activate();
			redraw();
		}
		else
		{
			findLayout("Automatic segmentation")->deactivate();
			redraw();
		}
	}
	else if(w.mId=="Option")
	{
		TString fn=FlChooseFile("Choose option", "script","*.lua");
		if(fn.length())
		{
			TString ff, dir;
			ff=sz1::filename(fn, dir);
			find<Fl_Input>("Option_Input")->value(processOption(ff.left(-4)));
			redraw();
		}

	}
	else if(w.mId=="Load a PDF file")
	{
		Fl_Native_File_Chooser *chooser = new Fl_Native_File_Chooser();
		chooser->type(Fl_Native_File_Chooser::BROWSE_FILE);   // let user browse a single file
		chooser->title("Open a file");                        // optional title
		//chooser->preset_file("/var/tmp/somefile.txt");        // optional filename preset
		chooser->filter("PDF Files\t*.pdf");                 // optional filter
		TString fn;
		switch ( chooser->show() ) {
			case -1:    // ERROR
			fprintf(stderr, "*** ERROR show() failed:%s\n", chooser->errmsg());
			break;
			case 1:     // CANCEL
			fprintf(stderr, "*** CANCEL\n");
			break;
			default:    // USER PICKED A FILE
			fn=chooser->filename();
			break;
		}
		//TString fn=fl_file_chooser("Choose a PDF file", "*.pdf", NULL);
		if(fn.length())
		{
			mPDFwin->load(fn);

		}
	}
	else if(w.mId=="Batch process")
	{
		Fl_Native_File_Chooser *chooser = new Fl_Native_File_Chooser();
		chooser->type(Fl_Native_File_Chooser::BROWSE_MULTI_FILE);   // let user browse a single file
		chooser->title("Open files");                        // optional title
		//chooser->preset_file("/var/tmp/somefile.txt");        // optional filename preset
		chooser->filter("PDF Files\t*.pdf");                 // optional filter
		TStrings fn;
		switch ( chooser->show() ) {
			case -1:    // ERROR
			fprintf(stderr, "*** ERROR show() failed:%s\n", chooser->errmsg());
			break;
			case 1:     // CANCEL
			fprintf(stderr, "*** CANCEL\n");
			break;
			default:    // USER PICKED A FILE
				{
					fn.resize(chooser->count());
					for (int n = 0; n < chooser->count(); n++ ) 
      				fn[n]=chooser->filename(n);
				}
			break;
		}
		//TString fn=fl_file_chooser("Choose a PDF file", "*.pdf", NULL);
		if(fn.size())
		{

			for(int i=0; i<fn.size(); i++)
			{
				mPDFwin->load(fn[i]);
				onCallback(findWidget("Process all pages"), w.widgetRaw(), 0);
			}
		}
	}
	else if(w.mId=="Process current page"|| w.mId=="Process all pages")
	{
		findButton("Process current page")->deactivate();
		findButton("Process all pages")->deactivate();
	
		bool bAll=w.mId=="Process all pages";

		LUAwrapper L;
		
		// export member functions of PDFwin for use in LUA script.
		// to understand the following codes, please refer to "luabind" manual.
		module(L.L)[
			def("reflow", &reflow),
			def("trimVertSpaces", &trimVertSpaces),			
			class_<PDFwin>("PDFwin")
				.def("getRectImage_width", &PDFwin::getRectImage_width)
				.def("getDPI_width", &PDFwin::getDPI_width)
				.def("getDPI_height", &PDFwin::getDPI_height)
				.def("getRectImage_height", &PDFwin::getRectImage_height)
				.def("setStatus", &PDFwin::setStatus)
				.def("getNumPages", &PDFwin::getNumPages)
				.def("getNumRects", &PDFwin::getNumRects)
				.def("setCurPage", &PDFwin::setCurPage)
				.def("deleteAllFiles", &PDFwin::deleteAllFiles)
				.def("deleteAllFilesWithoutConfirm", &PDFwin::deleteAllFilesWithoutConfirm),
			class_<PDFWriter>("PDFWriter")
				.def(constructor<>())
				.def("init", &PDFWriter::init)
				.def("addPage", &PDFWriter::addPage)
				.def("addPageColor", &PDFWriter::addPageColor)
				.def("save", &PDFWriter::save)
				.def("isValid",&PDFWriter::isValid)
		];

		TString fn;
		fn.format("script/%s.lua", unprocessOption(find<Fl_Input>("Option_Input")->value()).ptr());


		L.setRef<PDFwin>("win", *mPDFwin);
		L.dofile(fn);

		CreateDirectory(mPDFwin->mOutDir);

		try
		{
			if(bAll)
			{
				call_function<int>(L.L, "processAllPages", mPDFwin->mOutDir.ptr());
				/*
				DeleteAllFiles(mPDFwin->mOutDir, "*",true);
				for(int pageNo=0; pageNo<mPDFwin->getNumPages(); pageNo++)
				{
					mPDFwin->mCurrPage=pageNo;
					mPDFwin->pageChanged();
					mPDFwin->redraw();
					
					int res=call_function<int>(L.L, "processPage", mPDFwin->mOutDir.ptr(), pageNo, mPDFwin->getNumRects());
				}*/

			}
			else
			{
				call_function<int>(L.L, "processPage", mPDFwin->mOutDir.ptr(), mPDFwin->mCurrPage, mPDFwin->getNumRects());
			}
		}
		catch(luabind::error& e)
		{
			printf("lua error %s\n", e.what());
			int n=lua_gettop(e.state());
			Msg::msgBox("lua error %s", lua_tostring(e.state(), n));
			ASSERT(0);
		}
		catch (char* error)
		{
			fl_message("%s", error);
		}
		catch(std::exception& e)
		{
			Msg::msgBox("c++ error : %s", e.what());
			ASSERT(0);
		}
		catch(...)
		{
			fl_message("some error");
			ASSERT(0);
		}
		/*
			for(int rectNo=0; rectNo<mRects.size(); rectNo++)
			{
				CImage image;
				getRectImage_width(pageNo, rectNo, 600, image);
				TString filename;
				filename.format("%d_%d.png", pageNo, rectNo);
				image.Save(filename.ptr());
				setStatus("processing "+filename);
			}
		}*/

#ifndef _DEBUG
		mPDFwin->setStatus("processing ended");
#endif
		findButton("Process current page")->activate();
		findButton("Process all pages")->activate();

	}
}
