#ifndef __DEFS_H__
#define __DEFS_H__

#include "common.h"

#define NUM 9
#define BOXNUM 3
#define NUM_NONE 0
#define NUMROW NUM //alias
#define NUMCOL NUM //alias

//Text Color defines for console
#ifdef _WIN32
#define TC_RED "\033[1;31m"
#define TC_BLUE "\033[1;34m"
#define TC_CYAN "\033[1;36m"
#define TC_GREEN "\033[1;32m"
#define TC_YELLOW "\033[1;33m"
#define TC_NONE "\033[0m"
#else
#define TC_RED "\e[1;31m"
#define TC_BLUE "\e[1;34m"
#define TC_CYAN "\e[1;36m"
#define TC_GREEN "\e[1;32m"
#define TC_YELLOW "\e[1;33m"
#define TC_NONE "\e[0m"
#endif


//use LIBAPI to declare functions, classes, variables for EXPORT !
#define LIBAPI

enum EnumStatus { ST_NONE, ST_TAG, ST_NORMAL };
enum EnumSetType { ROW=0, COL=1, BOX=2, BLOCK=BOX,};
enum EnumFormat { FMT_LINE, FMT_LF, /*FMT_BOX, FMT_BIGBOX,*/ FMT_CELL_CANDS, FMT_CELL, FMT_LAST=FMT_CELL}; //put FMT_CELL at end, make an alias FMT_LAST to it.
enum EnumSActionType {ACT_SET,ACT_TAG};
enum EnumSolver { S_Manual, S_Guess, 
		S_NakedSingleNumber, S_HiddenSingleNumber, 
		S_IntersectionRemoval, 
		S_HiddenPair, S_HiddenTriplet, S_HiddenQuad, 
		S_NakedPair, S_NakedTriplet, S_NakedQuad,
		S_XWing, S_SwordFish, S_JellyFish, 
		S_XYWing, S_XYZWing,
		S_WWing,
		S_UniqueRectangle1, S_UniqueRectangle2, S_UniqueRectangle3, S_UniqueRectangle4, S_UniqueRectangle5, S_UniqueRectangle6, S_UniqueRectangle7, 
		S_XChain, S_XYChain, S_ForcingChain, 
		S_BUG1
	};

enum EnumLayerOrder { LO_BOTTOM, LO_BOARD=10, LO_CURSOR=20, LO_CLASH=30, LO_SAME=40, LO_HINT=50, LO_CAND=60, LO_NUMBER=70, LO_MENU=80 };
enum EnumColor { CBLACKBOLD, CBLACK, CBLUE , CTAG, CGREEN, CRED, CBROWN, CPURPLE, CYELLOW, CMAX=CYELLOW };
enum EnumBackgroud { BWHITE, BGREY, BYELLOW, BGREEN, BRED, BCYAN };
enum EnumSwipeDirect { SWIPE_LEFT = 1, SWIPE_RIGHT = 2, SWIPE_UP = 4, SWIPE_DOWN= 8, };
#endif
