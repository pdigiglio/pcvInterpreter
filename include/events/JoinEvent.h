/**
 *
 *    @file  JoinEvent.h
 *   @brief  
 *
 *    @date  06/20/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#ifndef  JOIN_EVENT_H_
#define  JOIN_EVENT_H_

#include "Event.h"
#include "ShadowLock.h"

struct JoinInfo {
  ShadowThread* childThread;
  ShadowThread* parentThread;
  JoinInfo(ShadowThread* childThread, ShadowThread* parentThread)
    : childThread(childThread), parentThread(parentThread) {}
};

class JoinEvent : public Event {
public:
	JoinEvent(const ShadowThread *thread,
			  const JoinInfo *info) :
		Event(thread), _info(info) {};

	virtual Events getEventType() const override final { return Events::JOIN; };
	const JoinInfo* const getJoinInfo() const { return _info; };

private:
	const JoinInfo* const _info;

	// prevent generated functions
	JoinEvent(const JoinEvent&);
	JoinEvent& operator=(const JoinEvent&);
};

#endif
