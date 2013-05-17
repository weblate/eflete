#ifndef _EDJE_COMPILE
#define _EDJE_COMPILE

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* include config.h */

#include <Eina.h>
#include <Ecore.h>
#include <time.h>

struct _Compiler_Message
{
	EINA_INLIST;
	time_t time;
	char *text;
};

/***
 *
 */
typedef struct _Compiler_Message Compiler_Message;

struct _Edje_CC
{
	Ecore_Exe *exe;
	char *cmd;
	Eina_Inlist *messages;
};

/***
 *
 */
typedef struct _Edje_CC Edje_CC;

/***
 *
 */
typedef struct _Edje_CC Edje_DeCC;

/***
 *
 */
Edje_CC *
compile(const char *edc,
		const char *edj,
		const char *image_directory,
		const char *font_directory,
		const char *sound_directory);

/***
 *
 */
Edje_DeCC *
decompile(char *edj, char *edc);

/***
 *
 */
void
compiler_free(Edje_CC *compiler);

/***
 *
 */
void
decompiler_free(Edje_DeCC *decompiler);

#endif /* _EDJE_COMPILE */
