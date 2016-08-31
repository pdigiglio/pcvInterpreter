/**
 *
 *    @file  CallSiteSpanProfile.cpp
 *   @brief  Implementation file for the class `CallSiteSpanProfile`
 *
 *    @date  06/17/16
 *  @author  Nathaniel Knapp (github.com/deknapp),
 *           <nathaniel.knapp@gmail.com>
 *
 */

#include "CallSiteSpanProfile.h"

CallSiteSpanProfile::CallSiteSpanProfile() : call_site(0), start(0), span(0), 
											 lock_wait_time(0) {}

CallSiteSpanProfile::~CallSiteSpanProfile() {}

void CallSiteSpanProfile::add_in_callsite_span
			  (const std::shared_ptr<CallSiteSpanProfile> profile_to_add) {
	span += profile_to_add->span;
	start = std::min(start, profile_to_add->start);
}

void CallSiteSpanProfile::init(CALLSITE call_site, TIME start_time)
 {
 	start = start_time;
	call_site = call_site;
}

void CallSiteSpanProfile::print() {
	printf(" ================ \n");
	printf("CALL SITE %d \n", static_cast<int>(call_site));
	printf("SPAN is %llu \n", static_cast<unsigned long long>(span));
	printf("LOCK WAIT TIME is %llu \n", 
					     static_cast<unsigned long long>(lock_wait_time));
	printf("LOCK SPAN IS %f of SPAN \n", 
									static_cast<double>(lock_wait_time) / 
								    static_cast<double>(span));
	printf("================\n");
}