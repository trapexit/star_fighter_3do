
/******************************************************************************
**
**  Copyright (C) 1995, an unpublished work by The 3DO Company. All rights reserved.
**  This material contains confidential information that is the property of The 3DO Company.
**  Any unauthorized duplication, disclosure or use is prohibited.
**  $Id: bs_joystick.c,v 1.2 1994/11/22 00:03:41 vertex Exp $
**
******************************************************************************/


#include "broker_shell.h"

/* don't compile this if */
/* we are not using a joystick */
#ifdef __BS_JOYSTICK_H

/* macro for comparing two StickData structs */
#define StickSameP(a,b) \
	(((a)->stk_ButtonBits)==((b)->stk_ButtonBits)&&\
	 ((a)->stk_HorizPosition)==((b)->stk_HorizPosition)&&\
	 ((a)->stk_VertPosition)==((b)->stk_VertPosition)&&\
	 ((a)->stk_DepthPosition)==((b)->stk_DepthPosition))

/*
 * the analog joystick is not a precision instrument,
 * although the event data has 10 bits assigned for each axis,
 * there is lots of noise in the bottom bits.
 * This routine trys to smooth that noise out.
 *
 * It also allows you to "re-calibrate" the stick.
 */

/* simple helper macros */
#define min(a,b) ((a)<(b)?(a):(b))
#define max(a,b) ((a)>(b)?(a):(b))

/* filter the raw stick data and put the results */
/* in the filtered stick data */
/* filtering dejitters the low order significant bit */
static void
stickFilter(BS_StickData *aStk,long recalibrate)
{
	short x,y,z;
	short xtmp, ytmp, ztmp;

	/* get the raw values that we got from the broker */
	xtmp = (short) BS_GetStickRawX(aStk);
	ytmp = (short) BS_GetStickRawY(aStk);
	ztmp = (short) BS_GetStickRawZ(aStk);

	/* if we're recalibrating, call where */
	/* we are the center and */
	/* reset the calibration mins/maxes to a */
	/* very small window */
	if(recalibrate)
	{
		BS_SetStickMinX(aStk,xtmp-10);
		BS_SetStickMinY(aStk,ytmp-10);
		BS_SetStickMinZ(aStk,ztmp-10);

		BS_SetStickMaxX(aStk,xtmp+10);
		BS_SetStickMaxY(aStk,ytmp+10);
		BS_SetStickMaxZ(aStk,ztmp+10);
	}
	/* keep track of the min/max values we get */
	/* constantly to keep the stick calibrated */
	else
	{
		BS_SetStickMinX(aStk,min(BS_GetStickMinX(aStk),xtmp));
		BS_SetStickMinY(aStk,min(BS_GetStickMinY(aStk),ytmp));
		BS_SetStickMinZ(aStk,min(BS_GetStickMinZ(aStk),ztmp));

		BS_SetStickMaxX(aStk,max(BS_GetStickMaxX(aStk),xtmp));
		BS_SetStickMaxY(aStk,max(BS_GetStickMaxY(aStk),ytmp));
		BS_SetStickMaxZ(aStk,max(BS_GetStickMaxZ(aStk),ztmp));
	}

	/* "normalize" about 0<->BS_<N>RES */
	xtmp = (short) BS_XRES*(xtmp-BS_GetStickMinX(aStk))/(BS_GetStickMaxX(aStk)-BS_GetStickMinX(aStk));
	ytmp = (short) BS_YRES*(ytmp-BS_GetStickMinY(aStk))/(BS_GetStickMaxY(aStk)-BS_GetStickMinY(aStk));
	ztmp = (short) BS_ZRES*(ztmp-BS_GetStickMinZ(aStk))/(BS_GetStickMaxZ(aStk)-BS_GetStickMinZ(aStk));

	/* filter that jittery low order bit by using ((3*last)+current)/4 */
	x= (short) BS_GetStickLastX(aStk);
	y= (short) BS_GetStickLastY(aStk);
	z= (short) BS_GetStickLastZ(aStk);

	/* adding n+n+n is faster than 3*n on arm as >>2 is faster than /4 */
	x=(x+x+x+xtmp)>>2;
	y=(y+y+y+ytmp)>>2;
	z=(z+z+z+ztmp)>>2;

	/* set the values to be used by caller */
	BS_SetStickX(aStk,x);
	BS_SetStickY(aStk,y);
	BS_SetStickZ(aStk,z);
}

/* filter the cartesian data and debounce the buttons */
/* returns 1 if the state of the device has changed since last time */
/* and 0 if not */
int
BS_ProcessStickData(BS_StickData *aStk,uint32 deBounceBits,int reCal)
{
  register uint32 bBits,buttonBits,lastButtons;
  int changeF=0;

	/* preserve our history */
	BS_SetStickThisLast(aStk);

	/* filter the raw data and put it in the */
	/* filtered data area */
	stickFilter(aStk,reCal);

	/* no debouncing?  skip all the checks below */
	if(!deBounceBits)
	{
		/* dump unfiltered (by request) button bits to */
		/* processed area */
		BS_SetStickButtons(aStk,BS_GetStickRawButtons(aStk));

		if(!StickSameP(BS_GetStickFiltered(aStk),BS_GetStickLast(aStk)))
			changeF=1;

		BS_SetStickThisLastRaw(aStk);

		return changeF;
	}

	/* get the current set of button presses */
	bBits=BS_GetStickRawButtons(aStk);

	/* get the last set of button presses for comparison */
	lastButtons=BS_GetStickLastRawButtons(aStk);

	/* start with nuthin */
	buttonBits=0;

	/* unroll the 12 debouncing checks. */
	/* if there is a button that you won't be debouncing */
	/* ever in your title, #if 0/#endif out that debounce */
	/* check to shave some cycles. */
	if(bBits&StickFire)
	{
		if(deBounceBits&StickFire)
		{
			if(!(lastButtons&StickFire))
				buttonBits|=StickFire;
		}
		else
			buttonBits|=StickFire;
	}

	if(bBits&StickA)
	{
		if((deBounceBits&StickA))
		{
			if(!(lastButtons&StickA))
				buttonBits|=StickA;
		}
		else
			buttonBits|=StickA;
	}

	if(bBits&StickB)
	{
		if(deBounceBits&StickB)
		{
			if(!(lastButtons&StickB))
				buttonBits|=StickB;
		}
		else
			buttonBits|=StickB;
	}

	if(bBits&StickC)
	{
		if(deBounceBits&StickC)
		{
			if(!(lastButtons&StickC))
				buttonBits|=StickC;
		}
		else
			buttonBits|=StickC;
	}

	if(bBits&StickUp)
	{
		if(deBounceBits&StickUp)
		{
			if(!(lastButtons&StickUp))
				buttonBits|=StickUp;
		}
		else
			buttonBits|=StickUp;
	}

	if(bBits&StickDown)
	{
		if(deBounceBits&StickDown)
		{
			if(!(lastButtons&StickDown))
				buttonBits|=StickDown;
		}
		else
			buttonBits|=StickDown;
	}

	if(bBits&StickRight)
	{
		if(deBounceBits&StickRight)
		{
			if(!(lastButtons&StickRight))
				buttonBits|=StickRight;
		}
		else
			buttonBits|=StickRight;
	}

	if(bBits&StickLeft)
	{
		if(deBounceBits&StickLeft)
		{
			if(!(lastButtons&StickLeft))
				buttonBits|=StickLeft;
		}
		else
			buttonBits|=StickLeft;
	}

	if(bBits&StickPlay)
	{
		if(deBounceBits&StickPlay)
		{
			if(!(lastButtons&StickPlay))
				buttonBits|=StickPlay;
		}
		else
			buttonBits|=StickPlay;
	}

	if(bBits&StickStop)
	{
		if(deBounceBits&StickStop)
		{
			if(!(lastButtons&StickStop))
				buttonBits|=StickStop;
		}
		else
			buttonBits|=StickStop;
	}

	if(bBits&StickLeftShift)
	{
		if(deBounceBits&StickLeftShift)
		{
			if(!(lastButtons&StickLeftShift))
				buttonBits|=StickLeftShift;
		}
		else
			buttonBits|=StickLeftShift;
	}

	if(bBits&StickRightShift)
	{
		if(deBounceBits&StickRightShift)
		{
			if(!(lastButtons&StickRightShift))
				buttonBits|=StickRightShift;
		}
		else
			buttonBits|=StickRightShift;
	}

	/* put filtered buttons into processed area */
	BS_SetStickButtons(aStk,buttonBits);

	if(!StickSameP(BS_GetStickFiltered(aStk),BS_GetStickLast(aStk)))
		changeF=1;

	/* update raw history */
	BS_SetStickThisLastRaw(aStk);

	return changeF;
}

#endif /* __BS_JOYSTICK_H */
