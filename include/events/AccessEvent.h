/**
 *
 *    @file  AccessEvent.h
 *   @brief  
 *
 *    @date  06/20/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#ifndef  ACCESS_INFO_H_
#define  ACCESS_INFO_H_

#include "Access.h"
#include "Event.h"
#include "ShadowThread.h"
#include "ShadowVar.h"
#include "Types.h"

struct AccessInfo {
	AccessType type;
	INS_ID instructionID;
	const ShadowVar *var;

	AccessInfo(AccessType Type, const ShadowVar *Var, INS_ID instructionID)
		: type(Type), instructionID(instructionID), var(Var) {}
};

class AccessEvent : public Event {
public:
	AccessEvent(const ShadowThread *thread,
				const AccessInfo *info) :
		Event(thread), _info(info) {};

	virtual Events getEventType() const override final { return Events::ACCESS; };
	const AccessInfo* const getAccessInfo() const { return _info; };

private:
	const AccessInfo* const _info;

	// prevent generated functions
	AccessEvent(const AccessEvent&);
	AccessEvent& operator=(const AccessEvent&);
};


#endif
