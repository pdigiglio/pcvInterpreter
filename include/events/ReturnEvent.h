/**
 *
 *    @file  ReturnEvent.h
 *   @brief  
 *
 *    @date  06/20/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#ifndef  RETURN_EVENT_H_
#define  RETURN_EVENT_H_

#include "Event.h"
#include "Types.h"

typedef struct ReturnInfo {
	CAL_ID call;
	TIME   endTime;

	ReturnInfo(CAL_ID call,
			   TIME endTime)
		: call(call), endTime(endTime)
	{}
} ReturnInfo;

class ReturnEvent : public Event {
	public:
		ReturnEvent(const ShadowThread *thread,
				    const ReturnInfo *info) :
			Event(thread), _info(info) {}

		Events getEventType() const override { return Events::RETURN; }
		const ReturnInfo* getReturnInfo() const { return _info; };
	private:
		const ReturnInfo *_info;
};

#endif
