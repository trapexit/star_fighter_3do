/*******************************************************************************************
 *	File:			PrepareStream.h
 *
 *	Contains:		definitions for high level stream playback preparation
 *
 *	Written by:		Joe Buczek
 *
 *	Copyright © 1993 The 3DO Company. All Rights Reserved.
 *
 *	History:
 *	10/06/94		dtc		Added some default constants and UseDefaultStreamHeader
 *							function.
 *	09/14/94		dtc		Removed typedef from enum declaration.
 *	10/20/93		jb		New today
 *
 *******************************************************************************************/

#ifndef	_PREPARESTREAM_H_
#define	_PREPARESTREAM_H_

#ifndef _TYPES_H
#include "Types.h"
#endif

#include "DataStreamLib.h"

#ifndef _DSSTREAMHEADER_H_
#include "DSStreamHeader.h"
#endif

/* Size of the buffer we use to read the start of the stream file.
 * For now, we require that the stream header, if present, be located
 * at the very first byte of the file. This may get better, eventually.
 */
#define	FIND_HEADER_BUFFER_SIZE		2048

#define	CTRL_CHUNK_TYPE		CHAR4LITERAL('C','T','R','L')	/* chunk type for this subscriber */
#define	SNDS_CHUNK_TYPE		CHAR4LITERAL('S','N','D','S')	/* chunk type for this subscriber */
#define	FILM_CHUNK_TYPE		CHAR4LITERAL('F','I','L','M')	/* chunk type for this subscriber */

/*********************/
/* Default constants */
/*********************/

#define kDefaultBlockSize		32768		/* size of stream buffers */
#define kDefaultNumBuffers		4			/* suggested number of stream buffers to use */
#define kStreamerDeltaPri		6			/* delta priority for streamer thread */
#define kDataAcqDeltaPri		8			/* delta priority for data acquisition thread */
#define kNumSubsMsgs			200			/* number of subscriber messages to allocate */

#define kAudioClockChan			1			/* logical channel number of audio clock channel */
#define kEnableAudioChanMask	14			/* mask of audio channels to enable (enable channels 1-3) */

#define kSoundsPriority			10
#define kCPakPriority			7
#define kControlPriority		11

/***************/
/* Error codes */
/***************/
enum {
	kPSVersionErr			= -2001,
	kPSMemFullErr			= -2002,
	kPSUnknownSubscriber	= -2003,
	kPSHeaderNotFound		= -2004
	};

/*****************************/
/* Public routine prototypes */
/*****************************/

int32			FindAndLoadStreamHeader( DSHeaderChunkPtr headerPtr, char *streamFileName );
DSDataBufPtr	CreateBufferList( long numBuffers, long bufferSize );
int32			UseDefaultStreamHeader( DSHeaderChunkPtr headerPtr );

#endif	/* _PREPARESTREAM_H_ */
