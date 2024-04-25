
/******************************************************************************
**
**  Copyright (C) 1995, an unpublished work by The 3DO Company. All rights reserved.
**  This material contains confidential information that is the property of The 3DO Company.
**  Any unauthorized duplication, disclosure or use is prohibited.
**  $Id: broker_shell.c,v 1.2 1994/11/22 00:03:41 vertex Exp $
**
******************************************************************************/

#include "stdio.h"
#include "msgport.h"
#include "kernel.h"
#include "stdlib.h"
#include "string.h"
#include "broker_shell.h"

static MsgPort *msgPort=NULL;
static Message *msgPtr=NULL;
static Item msgPortItem=0,msgItem=0,brokerPortItem=0;
static int IsConnected=0;
static ConfigurationRequest config;
static int32 gMice,gPads,gSticks,gGuns;
static uint32 gOtherSignals=0;
static Item queryMsgItem=-1;
static Message *qMsg=NULL;

#define MSGSIZE	2048

int
BS_CountPeripherals(void)
{
  EventBrokerHeader queryHeader;
  PodDescriptionList *podlist;
  PodDescription	 *Pod;
  int32 iPad,cPads;
  int32 lg=0,cp=0,js=0,m=0;

  int32 sent;

	if(!IsConnected)
	{
		printf("BS_CountPeripherals():  broker not connected.\n");
		return 0;
	}

	queryHeader.ebh_Flavor = EB_DescribePods;

	/* send the message */
	if((sent=SendMsg(brokerPortItem,queryMsgItem,&queryHeader,sizeof(queryHeader)))<0)
	{
		printf("countPeripherals(): Error sending request message: ");
		PrintfSysErr(sent);
		return 0;
	}

	/* wait for the response */
	WaitPort (msgPortItem, queryMsgItem);
	if ((Err)qMsg->msg_Result < 0)
	{
		printf ("BS_CountPeripherals(): Event broker could not accept request\n");
		PrintfSysErr((Err)qMsg->msg_Result);
		return 0;
	}

	podlist = (PodDescriptionList *) qMsg->msg_DataPtr;
	if (podlist->pdl_Header.ebh_Flavor != EB_DescribePodsReply)
	{
		printf("BS_CountPeripherals():  message data was wrong flavor!(0x%x,%d)\n",
			podlist->pdl_Header.ebh_Flavor,
			podlist->pdl_Header.ebh_Flavor);

		return 0;
	}

	cPads = podlist->pdl_PodCount;

	Pod = (PodDescription *) (podlist->pdl_Pod);

	for (iPad = 0; iPad < cPads; iPad++)
	{
		if(Pod->pod_Flags & POD_IsStick)
			++js;
		if(Pod->pod_Flags & POD_IsControlPad)
			++cp;
		if(Pod->pod_Flags & POD_IsMouse)
			++m;
		if(Pod->pod_Flags & POD_IsGun)
			++lg;

		Pod = (PodDescription *) ((char *)Pod + sizeof (PodDescription));
	}

	gGuns=lg;
	gPads=cp;
	gMice=m;
	gSticks=js;

	return 1;
}


/*
 * decide what to do with each event frame we recieve
 * transfers the raw data from the event frame to the
 * appropriate device data structure.  returns a bitmask
 * coded uint32 that sez which device types got written to
 *
 */
static uint32
handleEvent(EventBrokerHeader *hdr,
			BS_StickData **sdp,
			BS_CPadData **cpdp,
			BS_MouseData **mdp,
			BS_LGunData **lgdp)
{
	EventFrame *frame;
	int32 changedF=0;
	int podI=0;

	switch (hdr->ebh_Flavor) {
		case EB_EventRecord:
			frame = (EventFrame *) (hdr + 1); /* Gawd that's ugly! */
			while (TRUE) {
				if (frame->ef_ByteCount == 0) {
					/* End of record */
					break;
				}

				switch (frame->ef_EventNumber) {
					case EVENTNUM_ControlPortChange:
						if(!BS_CountPeripherals())
							printf("handleEvent(): could not re-count peripherals!\n");
						changedF|=BS_PORT_CHANGE;
						break;
					case EVENTNUM_EventQueueOverflow:
						break;
#ifdef __BS_MOUSE_H
					case EVENTNUM_MouseButtonPressed:
					case EVENTNUM_MouseButtonReleased:
					case EVENTNUM_MouseMoved:
					case EVENTNUM_MouseUpdate:
						if(!mdp||!mdp[(podI=(frame->ef_GenericPosition-1))])
							break;
						changedF|=BS_MOUSE;
						*(BS_GetMouseRaw(mdp[podI]))=
							*((MouseEventData *)(frame->ef_EventData));
						BS_SetMouseDaisyNum(mdp[podI],
							frame->ef_PodPosition);
						BS_SetMouseNum(mdp[podI],
							frame->ef_GenericPosition);
						break;
#endif /* __BS_MOUSE_H */
#ifdef __BS_CPAD_H
					case EVENTNUM_ControlButtonPressed:
					case EVENTNUM_ControlButtonReleased:
					case EVENTNUM_ControlButtonUpdate:
						if(!cpdp||!cpdp[(podI=(frame->ef_GenericPosition-1))])
							break;
						changedF|=BS_CPAD;
						BS_SetCPadRawButtons(cpdp[podI],
							((ControlPadEventData *)(frame->ef_EventData))->cped_ButtonBits);
						BS_SetCPadDaisyNum(cpdp[podI],
								frame->ef_PodPosition);
						BS_SetCPadNum(cpdp[podI],
								frame->ef_GenericPosition);
						break;
#endif /* __BS_CPAD_H */
#ifdef __BS_LGUN_H
					case EVENTNUM_LightGunButtonPressed:
					case EVENTNUM_LightGunButtonReleased:
					case EVENTNUM_LightGunUpdate:
					case EVENTNUM_LightGunFireTracking:
						if(!lgdp||!lgdp[(podI=(frame->ef_GenericPosition-1))])
							break;
						changedF|=BS_STICK;
						*(BS_GetLGunRaw(lgdp[podI])) =
								*((LightGunEventData *)(frame->ef_EventData));
						BS_SetLGunDaisyNum(sdp[podI],
							frame->ef_PodPosition);
						BS_SetLGunNum(sdp[podI],
							frame->ef_GenericPosition);
						break;
#endif /* __BS_LGUN_H */
#ifdef __BS_JOYSTICK_H
					case EVENTNUM_StickButtonPressed:
					case EVENTNUM_StickButtonReleased:
					case EVENTNUM_StickUpdate:
					case EVENTNUM_StickMoved:
						if(!sdp||!sdp[(podI=(frame->ef_GenericPosition-1))])
							break;
						changedF|=BS_STICK;
						*(BS_GetStickRaw(sdp[podI])) =
								*((StickEventData *)(frame->ef_EventData));
						BS_SetStickDaisyNum(sdp[podI],
							frame->ef_PodPosition);
						BS_SetStickNum(sdp[podI],
							frame->ef_GenericPosition);
						break;
#endif /* __BS_JOYSTICK_H */
					default:
						/* printf("  Funky Event %d\n", frame->ef_EventNumber); */
						break;
				}
				frame = (EventFrame *) (frame->ef_ByteCount + (char *) frame);
			}
			break;
		default:
			printf("BS_NiceWaitEvent: got unexpected event-message type %d\n",
						hdr->ebh_Flavor);
			break;
	}

	return changedF;
}

uint32
BS_GetOtherSignals(void)
{
  uint32 oS;
  	oS=gOtherSignals;
	gOtherSignals=0;
	return oS;
}

int32
BS_GetPeripheralCount(uint32 devMask)
{
  int32 devs=0;

	if(devMask&BS_MOUSE)
		devs+=gMice;
	if(devMask&BS_STICK)
		devs+=gSticks;
	if(devMask&BS_LGUN)
		devs+=gGuns;
	if(devMask&BS_CPAD)
		devs+=gPads;

	return devs;
}

int32
BS_NiceWaitEvent(
	BS_StickData **sdp,
	BS_CPadData **cpdp,
	BS_MouseData **mdp,
	BS_LGunData **lgdp)
{
  Item eventItem;
  Message *event;
  EventBrokerHeader *msgHeader;
  uint32 theSignal;
  int32 eCode=0;
  uint32 otherSignals=0;

  BS_NICE_WAIT_ENTRANCE:

	/* wait non-busily for any signal */
  	/* theSignal=WaitSignal(msgPort->mp_Signal); */
	theSignal=WaitSignal(KernelBase->kb_CurrentTask->t_AllocatedSigs);

	/* collect whatever other signals we might have gotten */
	otherSignals=theSignal&(~(msgPort->mp_Signal));

	if(otherSignals)
	{
		eCode|=BS_OTHER_SIGNAL;
		gOtherSignals|=otherSignals;
	}

	/* if we got an event broker message */
	if(theSignal & msgPort->mp_Signal)
	{
		/* get the message */
		while((eventItem = GetMsg(msgPortItem))!=0)
		{
			/* message error trap */
			if (eventItem < 0)
			{
				printf("Error 0x%x getting message: ", eventItem);
				PrintfSysErr(eventItem);
				exit(1);
			}

			/* get the actual message pointer so we can examine the contents */
			event = (Message *) LookupItem(eventItem);

			/* get the event information */
			msgHeader = (EventBrokerHeader *) event->msg_DataPtr;

			/* was there a communication problem? */
			if (eventItem == msgItem)
			{
				/* mewl and puke */
				if ((int32) event->msg_Result < 0)
				{
					printf("BS_NiceWaitEvent: says broker refused configuration request: ");
					PrintfSysErr((int32) event->msg_Result);
					exit(1);
				}
			}
			/* no... */
			else
			{
				/* dispatch the event data to the appropriate structs */
				eCode|=(int32)handleEvent(msgHeader,sdp,cpdp,mdp,lgdp);

				/* reply to the event broker that we got the message */
				ReplyMsg(eventItem, 0, NULL, 0);
			}
		}
	}

	/* let's wait for a real */
	/* event before we bother the cpu again */
	if(!eCode)
		goto BS_NICE_WAIT_ENTRANCE;
	return eCode;
}

int
BS_ConnectEventBroker(void)
{
	int32 sent;
	TagArg msgTags[3];
	int iArg=0;

	/* hey! we're already connected! */
	if(IsConnected)
	{
		printf("BS_ConnectEventBroker(): already connected.\n");
		return 0;
	}

	/* knock, knock, can the event broker come out and play? */
	brokerPortItem = FindNamedItem(MKNODEID(KERNELNODE,MSGPORTNODE),
										EventPortName);

	/* No?!? Well I'm going to take my peripheral and go home! */
	if (brokerPortItem < 0)
	{
		printf("Can't find Event Broker port: ");
		PrintfSysErr(brokerPortItem);
		return 0;
	}

	/* create a communications port so we can connect to the broker */
	msgPortItem = CreateMsgPort("StickIO", 0, 0);

	/* could not do it */
	if (msgPortItem< 0)
	{
		printf("Cannot create event-listener port: ");
		PrintfSysErr(msgPortItem);
		return 0;
	}

	/* get the actual message port pointer */
	msgPort = (MsgPort *) LookupItem(msgPortItem);

	/* create a message to throw around */
	msgItem = CreateMsg(NULL, 0, msgPortItem);

	/* could not do it.  later. */
	if (msgItem < 0) {
		printf("Cannot create message: ");
		PrintfSysErr(msgItem);
		return 0;
	}

	msgPtr=(Message *)LookupItem(msgItem);

	if(!msgPtr)
	{
		printf("BS_ConnectBroker(): bad message item. (%d)\n",msgItem);
		return 0;
	}

	/* this is a config request */
	config.cr_Header.ebh_Flavor = EB_Configure;

	/* unfocused input, please */
	config.cr_Category = LC_Observer;

	/* clear out the trigger and capture masks */
	memset(&config.cr_TriggerMask, 0x00, sizeof(config.cr_TriggerMask));
	memset(&config.cr_CaptureMask, 0x00, sizeof(config.cr_CaptureMask));

	/* maximum size */
	config.cr_QueueMax = 10;

	/* don't ask for any device events yet */
	config.cr_TriggerMask[0]=0;

	/* lemme know if anything changes out there */
	config.cr_TriggerMask[2] = EVENTBIT2_ControlPortChange;

	/* ask politely for the configuration we want */
	sent = SendMsg(brokerPortItem, msgItem, &config, sizeof config);

	/* communication breakdown, baby! */
	if (sent < 0) {
		printf("Error sending config message: ");
		PrintfSysErr(sent);
		return 0;
	}

	/* wait for the reply */
	WaitPort (msgPortItem, msgItem);

	/* is everything ok? */
	if ((Err)(msgPtr->msg_Result) < 0)
	{
		printf("BS_ConnectBroker(): Event broker could not accept configuration\n");
		PrintfSysErr((Err)msgPtr->msg_Result);
		return 0;
	}

	iArg = 0;
	msgTags[iArg].ta_Tag = CREATEMSG_TAG_REPLYPORT;
	msgTags[iArg++].ta_Arg = (void *) msgPortItem;

	msgTags[iArg].ta_Tag = CREATEMSG_TAG_DATA_SIZE;
	msgTags[iArg++].ta_Arg = (void *) MSGSIZE;

	msgTags[iArg].ta_Tag = TAG_END;
	msgTags[iArg++].ta_Arg = (void *) 0;

	if ((queryMsgItem = CreateItem (
                                     MKNODEID(KERNELNODE, MESSAGENODE),
                                     msgTags)) < 0)
	{
		 printf("BS_ConnectBroker(): Problem created sized message\n");
		 PrintfSysErr(queryMsgItem);
		 return 0;
	}

	qMsg=(Message *)LookupItem(queryMsgItem);
	if(!qMsg)
	{
		printf("BS_CountPeripherals(): bad item: %d\n",queryMsgItem);
		return 0;
	}

	IsConnected=1;

	if(!BS_CountPeripherals())
	{
		printf("BS_ConnectBroker():  Could not count peripherals!\n");
		IsConnected=0;
		return 0;
	}

	/* return the thumbs up. */
	return 1;
}

/* don't have this unless we're doing joystick stuff */
#ifdef __BS_JOYSTICK_H
uint32
BS_WatchingJoyStick(void)
{
	if(!IsConnected)
		return 0;

	return 	config.cr_TriggerMask[0]&(EVENTBIT0_StickButtonPressed|
								EVENTBIT0_StickButtonReleased|
								EVENTBIT0_StickUpdate|
								EVENTBIT0_StickDataArrived|
								EVENTBIT0_StickMoved);
}

int
BS_WatchJoyStick(int debounceAllF)
{
  uint32 stickFlags=
	EVENTBIT0_StickButtonPressed|EVENTBIT0_StickUpdate|
	EVENTBIT0_StickMoved;
  int32 sent;

	/* can't ask for joystick events if we're not connected */
	if(!IsConnected)
	{
		printf("WatchJoyStick(): broker not connected.\n");
		return 0;
	}

	/* we're asking for a new configuration */
	config.cr_Header.ebh_Flavor = EB_Configure;

	/* clear out any old joystick requests from the events we want */
	config.cr_TriggerMask[0]&=~(EVENTBIT0_StickButtonPressed|
								EVENTBIT0_StickButtonReleased|
								EVENTBIT0_StickUpdate|
								EVENTBIT0_StickDataArrived|
								EVENTBIT0_StickMoved);

	/* if this was not an ignore request, then */
	/* add joystick event masks we want to the config. */
	if(debounceAllF>=0)
	{
		if(debounceAllF)
			config.cr_TriggerMask[0]|=stickFlags;
		else
			config.cr_TriggerMask[0]|=stickFlags|EVENTBIT0_StickButtonReleased;
	}

	/* send the request */
	if((sent=SendMsg(brokerPortItem,msgItem,&config,sizeof(config)))<0)
	{
		printf("WatchJoyStick(): Error sending config message: ");
		PrintfSysErr(sent);
		return 0;
	}

	/* wait for the reply */
	WaitPort (msgPortItem, msgItem);
	if ((Err)msgPtr->msg_Result < 0)
	{
		printf ("BS_WatchJoyStick(): Event broker could not accept configuration\n");
		return 0;
	}

	/* thumbs up */
	return 1;
}
#endif /* __BS_JOYSTICK_H */

/* don't compile this is we aren't using the mouse */
#ifdef __BS_MOUSE_H
uint32
BS_WatchingMouse(void)
{
	if(!IsConnected)
		return 0;

	return config.cr_TriggerMask[0]&(EVENTBIT0_MouseButtonPressed|
								EVENTBIT0_MouseButtonReleased|
								EVENTBIT0_MouseUpdate|
								EVENTBIT0_MouseDataArrived|
								EVENTBIT0_MouseMoved);
}

int
BS_WatchMouse(int debounceAllF)
{
  uint32 mouseFlags=
	EVENTBIT0_MouseButtonPressed|
	EVENTBIT0_MouseUpdate|
	EVENTBIT0_MouseMoved;
  int32 sent;

	/* have to be connected to ask for mouse events */
	if(!IsConnected)
	{
		printf("WatchMouse(): broker not connected.\n");
		return 0;
	}

	/* this is a configuration request */
	config.cr_Header.ebh_Flavor = EB_Configure;

	/* clear out the old mouse bits */
	config.cr_TriggerMask[0]&=~(EVENTBIT0_MouseButtonPressed|
								EVENTBIT0_MouseButtonReleased|
								EVENTBIT0_MouseUpdate|
								EVENTBIT0_MouseDataArrived|
								EVENTBIT0_MouseMoved);

	/* if this is not an ignore request, add our desired mouse */
	/* masks to the request */
	if(debounceAllF>=0)
	{
		if(debounceAllF)
			config.cr_TriggerMask[0]|=mouseFlags;
		else
			config.cr_TriggerMask[0]|=mouseFlags|EVENTBIT0_MouseButtonReleased;
	}

	/* send the message */
	if((sent=SendMsg(brokerPortItem,msgItem,&config,sizeof(config)))<0)
	{
		printf("WatchMouse(): Error sending config message: ");
		PrintfSysErr(sent);
		return 0;
	}

	/* wait for the reply */
	WaitPort (msgPortItem, msgItem);
	if ((Err)msgPtr->msg_Result < 0)
	{
		printf ("BS_WatchMouse(): Event broker could not accept configuration\n");
		return 0;
	}

	/* thumbs up */
	return 1;
}
#endif /* __BS_MOUSE_H */


/* don't compile if we are not using the control pad */
#ifdef __BS_CPAD_H
uint32
BS_WatchingCPad(void)
{
	if(!IsConnected)
		return 0;

	return config.cr_TriggerMask[0]&(EVENTBIT0_ControlButtonPressed|
								EVENTBIT0_ControlButtonUpdate|
								EVENTBIT0_ControlButtonArrived|
								EVENTBIT0_ControlButtonReleased);

}

int
BS_WatchCPad(int debounceAllF)
{
  uint32 padFlags=EVENTBIT0_ControlButtonPressed|EVENTBIT0_ControlButtonUpdate;
  int32 sent;

	/* have to be connected to the event broker */
	if(!IsConnected)
	{
		printf("WatchControlPad(): broker not connected.\n");
		return 0;
	}

	/* this is a configuration request */
	config.cr_Header.ebh_Flavor = EB_Configure;

	/* clear the previous cpad request bits */
	config.cr_TriggerMask[0]&=~(EVENTBIT0_ControlButtonPressed|
								EVENTBIT0_ControlButtonUpdate|
								EVENTBIT0_ControlButtonArrived|
								EVENTBIT0_ControlButtonReleased);

	/* if we are not ignoring, add our new bits */
	if(debounceAllF>=0)
	{
		if(debounceAllF)
			config.cr_TriggerMask[0]|=padFlags;
		else
			config.cr_TriggerMask[0]|=padFlags|EVENTBIT0_ControlButtonReleased;
	}

	/* send the message */
	if((sent=SendMsg(brokerPortItem,msgItem,&config,sizeof(config)))<0)
	{
		printf("WatchCPad(): Error sending config message: ");
		PrintfSysErr(sent);
		return 0;
	}

	/* wait for the response */
	WaitPort (msgPortItem, msgItem);
	if ((Err)msgPtr->msg_Result < 0)
	{
		printf ("BS_WatchCPad(): Event broker could not accept configuration\n");
		return 0;
	}

	/* thumbs up */
	return 1;
}
#endif /* __BS_CPAD_H */

/* only compile if we are doing lightgun support */
#ifdef __BS_LGUN_H
uint32
BS_WatchingLightGun(void)
{
	if(!IsConnected)
		return 0;

	return config.cr_TriggerMask[0]&(EVENTBIT0_LightGunButtonPressed|
								EVENTBIT0_LightGunButtonReleased|
								EVENTBIT0_LightGunUptdate|
								EVENTBIT0_LightGunDataArrived|
								EVENTBIT0_LightGunFireTracking);
}

int
BS_WatchLGun(int debounceAllF)
{
  uint32 gunFlags=\
    EVENTBIT0_LightGunButtonPressed|EVENTBIT0_LightGunFireTracking|\
	EVENTBIT0_LightGunUpdate;
  int32 sent;

	/* have to be connected to broker */
	if(!IsConnected)
	{
		printf("WatchLGun(): broker not connected.\n");
		return 0;
	}

	/* this is a configuration request */
	config.cr_Header.ebh_Flavor = EB_Configure;

	/* clear out any old lightgun event request bits */
	config.cr_TriggerMask[0]&=~(EVENTBIT0_LightGunButtonPressed|
								EVENTBIT0_LightGunButtonReleased|
								EVENTBIT0_LightGunUptdate|
								EVENTBIT0_LightGunDataArrived|
								EVENTBIT0_LightGunFireTracking);

	/* if this is not an ignore request */
	/* add our bits to the request */
	if(debounceAllF>=0)
	{
		if(debounceAllF)
			config.cr_TriggerMask[0]|=gunFlags;
		else
			config.cr_TriggerMask[0]|=gunFlags|EVENTBIT0_LightGunButtonReleased;
	}

	/* send the message */
	if((sent=SendMsg(brokerPortItem,msgItem,&config,sizeof(config)))<0)
	{
		printf("WatchLGun(): Error sending config message: ");
		PrintfSysErr(sent);
		return 0;
	}

	/* wait for the reply */
	WaitPort (msgPortItem, msgItem);
	if ((Err)msgPtr->msg_Result < 0)
	{
		printf ("BS_WatchLGun(): Event broker could not accept configuration\n");
		return 0;
	}

	/* thumbs up */
	return 1;
}
#endif /* __BS_LGUN_H */


/* disconnect from the event broker */
/* and clean up */
int
BS_DisconnectBroker(void)
{
	/* a configuration request */
	config.cr_Header.ebh_Flavor = EB_Configure;

	/* pidgin macro for not wanting to get any device events */
	config.cr_Category = LC_NoSeeUm;

	/* send that message off */
	if (SendMsg (
             msgPortItem,
             msgItem,
             &config,
             sizeof (config)))
	{
		printf ("BS_DisconnectBroker(): Problem sending message to event broker\n");
		return 0;
	}

	/* wait for the reply, just to be retentive */
	WaitPort (msgPortItem, msgItem);
	if ((Err)msgPtr->msg_Result < 0)
	{
		printf ("BS_DisconnectBroker(): Event broker could not accept configuration\n");
		return 0;
	}

	/* delete our message port */
	DeleteMsgPort(msgPortItem);

	/* delete our message */
	DeleteMsg(msgItem);

	/* delete our daisy chain query message */
	DeleteItem(queryMsgItem);

	/* set the pointers to these things */
	/* back to NULL */
	msgPort=NULL;
	msgPtr=NULL;
	qMsg=NULL;

	msgPortItem=0;
	msgItem=0;
	brokerPortItem=0;
	queryMsgItem=0;

	/* clear out the configuration request */
	memset(&config,0,sizeof(config));

	/* set the connected flag to false */
	IsConnected=0;

	/* we did it! */
	return 1;
}
