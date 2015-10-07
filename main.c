
/* Shen,Xiaozhe   March 13,2011 */
/* simple shell program enforcd   */

/* SIS */
#include <stdio.h>
#include "control.h"
#include "history.h"
#include "global.h"


int main()
{
	hismaxsize=10;
	readhistory();
	rcfile();
	while(1)	
	{
		control(0);
	}	
	
}