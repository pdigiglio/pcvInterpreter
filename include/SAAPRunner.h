/*
 * SAAPRunner.h
 *
 *  Created on: Aug 27, 2014
 *      Author: wilhelma
 */

#ifndef SAAPRUNNER_H_
#define SAAPRUNNER_H_

#include "Event.h"
#include "EventService.h"
#include "Interpreter.h"

class Tool;
class Filter;

class SAAPRunner {
public:
	SAAPRunner(Interpreter *interpreter) : _interpreter(interpreter) {};
	~SAAPRunner() {};

	const bool registerTool(Tool* const tool, const Filter* const filter, Events events)
	{ return _interpreter->getEventService()->subscribe(tool, filter, events); };

	const bool removeTool(Tool* const tool)
	{ return _interpreter->getEventService()->unsubscribe(tool); };

	void interpret() { _interpreter->process(); };

private:
	// private data members
	Interpreter* const _interpreter;

	// prevent generated functions
	SAAPRunner(const SAAPRunner&);
	SAAPRunner& operator=(const SAAPRunner&);
};


#endif /* SAAPRUNNER_H_ */
