/**
 *
 *    @file  NewThreadInfo.h
 *   @brief  
 *
 *    @date  07/06/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#ifndef  NEW_THREAD_INFO_H_
#define  NEW_THREAD_INFO_H_

#include "fwd/ShadowThread.h"

#include "Types.h"

struct NewThreadInfo {
	ShadowThread* childThread;
	ShadowThread* parentThread;
	NUM_CYCLES runtime;
	TIME startTime;

	explicit NewThreadInfo(ShadowThread* childThread,
			ShadowThread* parentThread,
			const NUM_CYCLES& runtime,
			const TIME_STRING& startTimeString) noexcept
		: childThread(childThread), parentThread(parentThread),
		runtime(runtime), startTime(timeStringToTime(startTimeString)) {}
};

#endif