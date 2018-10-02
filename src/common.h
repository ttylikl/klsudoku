#ifndef __LIBSUDOKU_COMMON_H__
#define __LIBSUDOKU_COMMON_H__
#if !defined(_CRT_SECURE_NO_WARNINGS) && defined(_WIN32)
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "json/rapidjson.h"
#include "json/document.h"
#include "json/writer.h"
#include "json/stringbuffer.h"

//Note: for interval include from each other
#include "platforms.h"
#include "lib4b.h"
#include "defs.h"
#include "objects.h"
#include "dlx.h"
#include "funcs.h" 
#include "funcs_temp.h"


#endif