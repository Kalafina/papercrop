//
// FlLayout.cpp
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
#include "FlLayout.h"
#include "FL/Fl_Scroll.h"
#include "FL/Fl_Adjuster.h"
#include "FL/Fl_Multi_Browser.h"
#include "FL/Fl_Select_Browser.h"
#include "FL/Fl_Multiline_Input.h"
#include "math/operator.h"
FlLayout ::Widget::Widget()
{
	mWidget=NULL;
	mType="None";
	mState.mButtonType=FlLayout::BUTTON;	// default type
	mState.mSliderType=FlLayout::VALUE_SLIDER; // default type	
	mState.mGuideLines.linspace(0.0, 1.0, 4);
	mState.mStartSlot=0;
	mState.mEndSlot=3;
	mState.mWidgetHeight=20;
	mState.mLineSpace=5;
}

FlLayout ::FlLayout ()
:Fl_Double_Window(640,480)
{
	Msg::error("do not use default constructor");
}

FlLayout ::FlLayout (int ww, int hh, const char* title)
:Fl_Double_Window(ww, hh, title)
{
	_callbackRouter=this;
	mWidgets.resize(1);
	mScroll=new Fl_Scroll(5,5, w()-5, h()-5);
	mScroll->type(Fl_Scroll::VERTICAL);
}

FlLayout ::FlLayout (int x, int y, int ww, int hh, const char* title)
	:Fl_Double_Window(x, y, ww, hh, title)
{
	_callbackRouter=this;
	mWidgets.resize(1);
	mScroll=new Fl_Scroll(5,5, w()-5, h()-5);
	mScroll->type(Fl_Scroll::VERTICAL);
}

Fl_Widget* FlLayout::create(const char* type, const char* id, const char* title, int startSlot, int endSlot, int height)
{
	setWidgetPos(startSlot, endSlot);
	return createWidget(type, id, title);
}

FlLayout ::~FlLayout (void)
{

}

void FlLayout ::resize(int x,int y,int w,int h)
{
	Fl_Double_Window::resize(x,y,w,h);

	updateLayout();

}

vectorn& FlLayout::guideLines()
{
	return widgetRaw(1).mState.mGuideLines;
}

void FlLayout::setWidgetPos(int startSlot, int endSlot)
{
	if(endSlot>widgetRaw(1).mState.mGuideLines.size()-1)
		endSlot=widgetRaw(1).mState.mGuideLines.size()-1;
	widgetRaw(1).mState.mStartSlot=startSlot;
	widgetRaw(1).mState.mEndSlot=endSlot;
}

void FlLayout::setUniformGuidelines(int totalSlot)
{
	guideLines().linspace(0, 1.0, totalSlot+1);
}

void FlLayout::setWidgetPosUniform(int totalSlot, int slot)
{
	// 가로로 totalSlot등분을 해서, 몇번째에 넣어라.
	guideLines().linspace(0, 1.0, totalSlot+1);
	setWidgetPos(slot, slot+1);
}

Fl_Button* FlLayout::createButton(const char* id, const char* title)
{
	Fl_Button* o;
	switch(widgetRaw(1).mState.mButtonType)
	{
	case CHECK_BUTTON:
		o=new Fl_Check_Button(0,0,80, 20);
		break;
	case LIGHT_BUTTON:
		o=new Fl_Light_Button(0,0,80, 20);
		break;
	case BUTTON:
		o=new Fl_Button(0,0,80, 20);
		break;
	default: 
		Msg::error("create button");
	}
	o->copy_label(title);// otherwise, it just store the address which often will not be valid.
	_createWidget(id, o);
	widgetRaw(0).mType="Button";
	return o;
}
Fl_Slider* FlLayout::createSlider(const char* id, const char* title)
{
	Fl_Slider* o;
	switch(widgetRaw(1).mState.mSliderType)
	{
	case SLIDER:
		o=new Fl_Slider(0,0,80, 20);
		o->type(FL_HOR_SLIDER);
		o->align(FL_ALIGN_LEFT);
		break;
	case VALUE_SLIDER:
		o=new Fl_Value_Slider(0,0,80, 20);
		o->type(FL_HOR_SLIDER);
		o->align(FL_ALIGN_LEFT);
		break;
	case VALUE_VERT_SLIDER:
		o=new Fl_Value_Slider(0,0,80, 20);
		o->type(FL_VERT_SLIDER);
		break;
	default:
		Msg::error("create slider");
	}
	o->copy_label(title);
	_createWidget(id, o);	
	widgetRaw(0).mType="Slider";
	return o;
}

class Fl_new_adjuster: public Fl_Adjuster
{
public:
	Fl_Box b;
	char buf2[100];
	Fl_new_adjuster(int X, int Y, int W, int H, const char* L=0) 
		:Fl_Adjuster(X,Y,W/2,H, L)
		,b(FL_DOWN_BOX, X+W/2,Y, W/2,H,buf2)
	{
		if(b.h()<=20 && b.labelsize()>11)
			b.labelsize(11);

		align(FL_ALIGN_LEFT);
		step(1);
		value_damage();
	}

	virtual void resize(int x,int y,int w,int h)
	{
		b.resize(x+w/2,y,w/2,h);
		__super::resize(x,y,w/2,h);
	}
	virtual void value_damage()
	{
		__super::value_damage();

		if(value()<minimum())
			value(minimum());
		if(value()>maximum())
			value(maximum());
		format((char *)(b.label()));
		redraw();
	}

	virtual void redraw()
	{
		format((char *)(b.label()));
		b.redraw();
		__super::redraw();
	}
};

class FlLayoutGroup : public Fl_Group
{
	std::vector<FlLayout*> layouts;
public:
	FlLayout* layout() { return layouts[0];}
	FlLayoutGroup (int x, int y, int w, int h, const char* l=0)
		:Fl_Group(x,y,w,h,l)
	{
		box(FL_THIN_UP_FRAME);
		layouts.resize(1);
		layouts[0]=new FlLayout(x,y,w,h);
		resizable(NULL);
		end();
	}

	virtual void resize(int x,int y,int w,int h)
	{
		__super::resize(x,y,w,h);
		layout()->resize(x+5, y+5, w-10, h-10);
	}

};

Fl_Widget* FlLayout::create(const char* type, const char* id, const char* title)
{
	begin();
	TString tid=type;
	Fl_Widget* o=NULL;

	if(tid=="Menu") 
		tid="Choice";

	widgetRaw(1).mType=tid;

	if(tid=="Layout")
	{
		FlLayoutGroup * g = new FlLayoutGroup (0,0,80,20);
		g->layout()->_callbackRouter=_callbackRouter;
		o=g;
	}
	else if(tid=="Box")
	{
		Fl_Box* b;
		b=new Fl_Box(0,0,80,20);
		o=b;
	}
	else if(tid=="Input")
	{
		Fl_Input* i;
		i=new Fl_Input(0,0,80,20);
		o=i;
	}
	else if(tid=="Adjuster")
	{
		Fl_new_adjuster* a;
		a=new Fl_new_adjuster(0,0,80,20);
		o=a;
	}
	else if(tid=="Check_Button")
	{
		o=new Fl_Check_Button(0,0,80,20);
	}
	else if(tid=="Light_Button")
	{
		o=new Fl_Light_Button(0,0,80,20);
	}
	else if(tid=="Button")
	{
		o=new Fl_Button(0,0,80,20);
	}
	else if(tid=="Choice")
	{		
		o=new FlChoice(0,0,80,20);
	}
	else if(tid=="Multi_Browser")
	{
		o=new Fl_Multi_Browser(0,0,80,20);
	}
	else if(tid=="Select_Browser")
	{
		o=new Fl_Select_Browser(0,0,80,20);
	}
	else if(tid=="Multiline_Input")
	{
		o=new Fl_Multiline_Input(0,0,80,20);
	}
	else if(tid=="Slider")
	{
		o=new Fl_Slider(0,0,80,20);
		o->type(FL_HOR_SLIDER);
		o->align(FL_ALIGN_LEFT);
	}
	else if(tid=="Value_Slider")
	{
		o=new Fl_Value_Slider(0,0,80,20);
		o->type(FL_HOR_SLIDER);
		o->align(FL_ALIGN_LEFT);
	}
	if(!o)	Msg::error("Unknown type (%s)",type );
	o->copy_label(title);
	_createWidget(id, o);	
	return NULL;
}

void FlLayout::newLine()
{	
	setWidgetPos(0);
	_createWidget("__empty", NULL);
}

FlChoice * FlLayout::createMenu(const char* id, const char* title)
{
	return (FlChoice*)create("Choice",id, title);
}

void FlLayout::updateLayout()
{
	int cury=5;

	intvectorn guidelines;
	for(int i=0; i<mWidgets.size()-1; i++)
	{
		guidelines.setSize(mWidgets[i].mState.mGuideLines.size());

		for(int j=0; j<guidelines.size(); j++)
			guidelines[j]=sop::interpolateInt(mWidgets[i].mState.mGuideLines[j], 3, w()-2);

		// widget간 5픽셀 간격이 생기도록 한다.
#define _left(i)	(guidelines[mWidgets[i].mState.mStartSlot]+2)
#define _right(i) (guidelines[mWidgets[i].mState.mEndSlot]-3)
#define _currWidth(i)	   (_right(i)-_left(i))
#define _curHeight(i)		(mWidgets[i].mState.mWidgetHeight)
		

		/*
		// title 만큼 자동으로 offset 하는 기능은 없앴음. 수작업으로 적당히 widget pos를 조정해서 할 것.
		if((mWidgets[i].mType=="Slider" ||
			mWidgets[i].mType=="Choice" ||
			mWidgets[i].mType=="Input"||
			mWidgets[i].mType.right(8)=="Adjuster") && mWidgets[i].widget()->label())
		{
			int offset=60;
			if(mWidgets[i].mType=="Slider" && mWidgets[i].mState.mSliderType==VALUE_VERT_SLIDER)
				offset=0;

			Fl_Widget* o=(Fl_Widget*)mWidgets[i].mWidget;
			o->resize(_left(i)+offset, cury, _currWidth(i)-offset, _curHeight(i)); 
		}
		else */
		if(mWidgets[i].mType=="Layout")
		{
			FlLayoutGroup* o=(FlLayoutGroup*)mWidgets[i].mWidget;

			_curHeight(i)=o->layout()->minimumHeight()+25;
			if(o)
				o->resize(_left(i), cury+15, _currWidth(i), _curHeight(i)-15); 

			o->layout()->updateLayout();
		}
		else
		{
			Fl_Widget* o=(Fl_Widget*)mWidgets[i].mWidget;
			if(o)
				o->resize(_left(i), cury, _currWidth(i), _curHeight(i)); 
		}

		if(mWidgets[i].mState.mEndSlot==mWidgets[i].mState.mGuideLines.size()-1)
		{
			cury+=_curHeight(i)+mWidgets[i+1].mState.mLineSpace;
		}
	}

	mScroll->end();
	resizable(mScroll);
	end();

	m_minimumHeight=cury;
	// connect
	for(int i=0; i<mWidgets.size()-1; i++)
	{
		// parent가 갖고 있는 모든 widget과 메뉴는 같은 layout에 연결됨을 가정하였다.
		// 실제적으로 구현할때는 parent자체가 callee가 되도록 하는게 가장 확실하다.
		// 즉 parent는  Fl_Group(또는 Fl_Window 등)과 FlLayout를 동시상속.

		Fl_Widget*o=mWidgets[i].mWidget;


		if(mWidgets[i].mType=="Choice")
		{
			FlMenu* o=&(((FlChoice*)mWidgets[i].mWidget)->menu);
			for(int item=0; item<o->m_nMenu; item++)
				o->m_aMenuItem[item].callback(cbFunc);
		}
	}
}

int FlLayout::minimumHeight()
{
	return m_minimumHeight;
}

Fl_Widget* FlLayout::_createWidget(const char* id, Fl_Widget* o)
{
	if(o)
	{
		// Auto-size
		if(o->h()<=20 && o->labelsize()>11)
			o->labelsize(11);

		// connect
		void* prev_user_data=o->parent()->user_data();
		Msg::verify(prev_user_data==NULL || prev_user_data==((void*)this), "FlLayout::_createWidget error");
		o->parent()->user_data((void*)this);
		o->callback(cbFunc, o->user_data());
	}

	mWidgets.resize(mWidgets.size()+1);
	// state는 이전 state를 유지한다.
	mWidgets[mWidgets.size()-1].mState=mWidgets[mWidgets.size()-2].mState;

	mWidgets[mWidgets.size()-2].mId=id;
	mWidgets[mWidgets.size()-2].mWidget=o;
	mNamedmapWidgets[mWidgets[mWidgets.size()-2].mId]=mWidgets.size()-2;
	return o;
}

FlLayout::Widget& FlLayout::widgetRaw(int n)
{
	int iwidget=mWidgets.size()-2+n;
	if(iwidget<0)
		return mWidgets[mWidgets.size()-1];

	return mWidgets[iwidget];
}

int FlLayout::widgetIndex(const char* id)
{
	for(int i=0; i<mWidgets.size()-1; i++)
	{
		if(mWidgets[i].mId==id)
			return i-(mWidgets.size()-2);
	}

	Msg::error("no widget %s", id);
	return 1;
}

FlLayout::Widget& FlLayout::findWidget(const char* id)
{
#ifdef USE_FULL_SEARCH
	for(int i=0; i<mWidgets.size()-1; i++)
	{
		if(mWidgets[i].mId==id)
			return mWidgets[i];
	}

	Msg::error("no widget %s", id);
	return mWidgets[mWidgets.size()-1];
#else
	return mWidgets[mNamedmapWidgets[id]];
#endif
}

FlLayout::Widget& FlLayout::_findWidget(Fl_Widget* id)
{
	for(int i=0; i<mWidgets.size()-1; i++)
	{
		if(mWidgets[i].mWidget==id)
			return mWidgets[i];
	}

	Msg::error("no widget %x", id);
	return mWidgets[mWidgets.size()-1];
}

Fl_Slider* FlLayout::Widget::slider()const
{
	return dynamic_cast<Fl_Slider*>(mWidget);
}
Fl_Button* FlLayout::Widget::button()const
{
	return dynamic_cast<Fl_Button*>(mWidget);
}
Fl_Light_Button* FlLayout::Widget::checkButton()const
{
	return dynamic_cast<Fl_Light_Button*>(mWidget);
}

Fl_Valuator* FlLayout::Widget::valuator() const
{
	return dynamic_cast<Fl_Valuator*>(mWidget);
}

FlChoice* FlLayout::Widget::menu()const
{
	return dynamic_cast<FlChoice*>(mWidget);
}

FlLayout* FlLayout::Widget::layout() const
{
	FlLayoutGroup* g=dynamic_cast<FlLayoutGroup*>(mWidget);
	return (g)?g->layout():NULL;
}

FlChoice* FlLayout::menu(int n)
{
	return widgetRaw(n).menu();
}

FlLayout* FlLayout::layout(int n)
{
	return widgetRaw(n).layout();
}

FlLayout* FlLayout::findLayout(const char* id)
{
	return findWidget(id).layout();
}

FlChoice* FlLayout::findMenu(const char* id)
{
	return findWidget(id).menu();
}

Fl_Slider* FlLayout::slider(int n)
{
	return widgetRaw(n).slider();
}


Fl_Slider* FlLayout::findSlider(const char* id)
{
	return findWidget(id).slider();
}

Fl_Valuator* FlLayout::valuator(int n)
{
	return widgetRaw(n).valuator();
}

Fl_Valuator* FlLayout::findValuator(const char* id)
{
	return findWidget(id).valuator();
}

Fl_Button* FlLayout::button(int n)
{
	return widgetRaw(n).button();
}

Fl_Button* FlLayout::findButton(const char* id)
{
	return findWidget(id).button();
}

Fl_Light_Button* FlLayout::checkButton(int n)
{
	return widgetRaw(n).checkButton();
}

Fl_Light_Button* FlLayout::findCheckButton(const char* id)
{
	return findWidget(id).checkButton();
}

void FlLayout::cbFunc(Fl_Widget * pWidget, void *data)
{
	FlLayout* pLayout=((FlLayout*)pWidget->parent()->user_data());
	pLayout->onCallback(pLayout->_findWidget(pWidget), pWidget, (int)data);
}

void FlLayout::onCallback(FlLayout::Widget const& w, Fl_Widget * pWidget, int userData)
{
	// default: do nothing
	
	if(_callbackRouter!=this)
		_callbackRouter->onCallback(w, pWidget, userData);
}

void FlLayout::setButtonClass(buttonType b)		{mWidgets[mWidgets.size()-1].mState.mButtonType=b;	}
void FlLayout::setSliderClass(sliderType s)		{mWidgets[mWidgets.size()-1].mState.mSliderType=s;	}
void FlLayout::setLineSpace(int l)				{mWidgets[mWidgets.size()-1].mState.mLineSpace=l;}
void FlLayout::setWidgetHeight(int h)			{mWidgets[mWidgets.size()-1].mState.mWidgetHeight=h;}

