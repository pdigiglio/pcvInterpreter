/**
 *
 *   @name  dotTracker.cpp
 *  @brief  
 *
 *   @date  12/03/15
 *
 * @author  Paolo Di Giglio (github.com/pdigiglio),
 *          <p.digiglio91@gmail.com>
 *
 */


#include <iostream>
#include "dotTracker.h"

#include "Tool.h"
#include "Event.h"

using namespace std;

/**
 *       Class:  DotTracker
 *      Method:  DotTracker
 * Description:  constructor
 */
DotTracker::DotTracker ( const char* outFile ): _outFile( outFile ) {
}  /* -----  end of method DotTracker::DotTracker  (constructor)  ----- */

/**
 *       Class:  DotTracker
 *      Method:  ~DotTracker
 * Description:  destructor
 */
DotTracker::~DotTracker () {
}  /* -----  end of method DotTracker::~DotTracker  (destructor)  ----- */

void
DotTracker::create ( const Event *e ) {
}		/* -----  end of method DotTracker::create  ----- */

void
DotTracker::join ( const Event *e ) {
}		/* -----  end of method DotTracker::join  ----- */

void
DotTracker::acquire ( const Event *e ) {
}		/* -----  end of method DotTracker::acquire  ----- */

void
DotTracker::release ( const Event *e ) {
}		/* -----  end of method DotTracker::release  ----- */

void
DotTracker::access ( const Event *e ) {
}		/* -----  end of method DotTracker::access  ----- */

void
DotTracker::call ( const Event *e ) {
	cout << dynamic_cast<const CallEvent*>(e)->getCallInfo()->runtime << " ";
	cout << dynamic_cast<const CallEvent*>(e)->getCallInfo()->fnSignature << " ";
	cout << dynamic_cast<const CallEvent*>(e)->getCallInfo()->fnType << " ";
	cout << dynamic_cast<const CallEvent*>(e)->getCallInfo()->fileName << " ";
	cout << dynamic_cast<const CallEvent*>(e)->getCallInfo()->filePath << endl;
}		/* -----  end of method DotTracker::call  ----- */
