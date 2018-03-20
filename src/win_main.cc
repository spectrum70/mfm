/*
 * mfm
 *
 * (C) 2017 Angelo Dureghello <angelo@sysam.it>
 *
 * This file is part of mfm application.
 *
 * minilibc library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * gasc is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Nome-Programma.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "win_main.hh"
#include "pixmaps.hh"
#include <FL/Fl_Window.H>
#include <FL/Fl_Pack.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Pixmap.H>

#include <tablefiles_files.hh>

constexpr int window_border = 2;
constexpr int command_bar_height = 24;

static const char * xpm_icon_trashcan[] = {
"24 24 66 1",
" 	c None",
".	c #BDC3C7",
"+	c #BCC2C6",
"@	c #8C9799",
"#	c #7F8C8D",
"$	c #7E8B8C",
"%	c #8D989A",
"&	c #B9C0C4",
"*	c #98A7A9",
"=	c #95A5A6",
"-	c #899A9A",
";	c #7E8C8C",
">	c #8A9A9A",
",	c #AEC0BD",
"'	c #2FAF65",
")	c #27AD60",
"!	c #30AF66",
"~	c #B0C1BE",
"{	c #5FB586",
"]	c #26AD5F",
"^	c #52B47E",
"/	c #53B47E",
"(	c #56B480",
"_	c #91BCA8",
":	c #BBC2C5",
"<	c #5DB585",
"[	c #2BAE63",
"}	c #A6BFB7",
"|	c #9BBEAF",
"1	c #27AE60",
"2	c #95BDAC",
"3	c #ADC0BC",
"4	c #97BDAD",
"5	c #63B689",
"6	c #3CB06E",
"7	c #B7C2C3",
"8	c #B6C2C2",
"9	c #7BB99A",
"0	c #4FB37C",
"a	c #6CB78F",
"b	c #B8C2C4",
"c	c #A5BFB7",
"d	c #59B582",
"e	c #41B172",
"f	c #A9C0B9",
"g	c #3BB06E",
"h	c #B9C2C4",
"i	c #A4BFB6",
"j	c #29AE61",
"k	c #70B892",
"l	c #40B171",
"m	c #78B997",
"n	c #4EB37B",
"o	c #66B68B",
"p	c #72B894",
"q	c #92BDAA",
"r	c #2FAF66",
"s	c #4DB37A",
"t	c #71B893",
"u	c #A2BFB4",
"v	c #28AE61",
"w	c #72B893",
"x	c #76B996",
"y	c #BCC3C7",
"z	c #B9C0C3",
"A	c #A1AEB0",
"                        ",
"   .+++++++++++++++++   ",
"  ++@##############@+.  ",
"  ++#$$$$$$$$$$$$$$#++  ",
"  ++#$##$###$#$##$##+.  ",
"  ++#$##$###$#$##$##+.  ",
"  ++%##############%++  ",
"  +.+++++++++++++++++.  ",
"   &.+..+...+.+..+..&   ",
"   *======-;;>======*   ",
"   ++++++,'))!~++++++   ",
"   ++++++{]^/](_+++++   ",
"   ..+..:<[}|1]2.+..+   ",
"   ..+..++3.4567.+..+   ",
"   ..+890a..+.+bc+..+   ",
"   ++++d]e++++fggh+++   ",
"   ..+ijjk8.+.:l1m..+   ",
"   ..+n1o...+p+q1r~.+   ",
"   +++s]]]1+t)]]]'~++   ",
"   ..+uj11v+w1111x..+   ",
"   ..+.++++y+p:+++..+   ",
"   z++++++++++++++++z   ",
"   Az++++++++++++++zA   ",
"    ================    "
};

class MyToolbar : public Fl_Pack {
public:
    // CTOR
    MyToolbar(int X,int Y,int W,int H):Fl_Pack(X,Y,W,H) {
        type(Fl_Pack::HORIZONTAL);    // horizontal packing of buttons
    box(FL_UP_FRAME);
    spacing(4);            // spacing between buttons
        end();
    }
    // ADD A TOOLBAR BUTTON TO THE PACK
    void AddButton(const char *name, Fl_Pixmap *img=0, Fl_Callback *cb=0, void *data=0) {
        begin();
            Fl_Button *b = new Fl_Button(0,0,24,24);
        b->box(FL_FLAT_BOX);    // buttons won't have 'edges'
        b->clear_visible_focus();
            if ( name ) b->tooltip(name);
            if ( img  ) b->image(img);
            if ( cb   ) b->callback(cb,data);
        end();
    }
};

void Open_CB(Fl_Widget*,void*) {
    printf("Open..\n");
}

win_main::win_main()
{
	s.width = 800;
	s.heigth = 600;

	w = make_shared<Fl_Window>(s.width, s.heigth, "mfm");

	w->begin();
	//t = make_shared<table_files>(window_border, command_bar_height,
	//			s.width - (window_border * 2),
	//			s.heigth - command_bar_height - window_border);

	//w->resizable(*t);


	// CREATE TOOLBAR
	Fl_Pixmap *trash_pixmap = new Fl_Pixmap(xpm_icon_trashcan);

	MyToolbar *toolbar = new MyToolbar(0, 0, w->w(), 30);
	toolbar->AddButton(0, trash_pixmap, Open_CB);

	w->end();
	w->show();
}

int win_main::run()
{
	return Fl::run();
}
