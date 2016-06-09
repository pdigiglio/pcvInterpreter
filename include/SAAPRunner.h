/*
 * SAAPRunner.h
 *
 *  Created on: Aug 27, 2014
 *      Author: wilhelma
 */

#ifndef SAAPRUNNER_H_
#define SAAPRUNNER_H_

#include "Event.h"

class Interpreter;
class Tool;
class Filter;

class SAAPRunner {
public:
	SAAPRunner(Interpreter *interpreter) : _interpreter(interpreter) {};
	~SAAPRunner() {};

	bool registerTool(Tool* tool, const Filter* filter, enum Events events);
	bool removeTool(Tool* tool);
	void interpret();

private:
	// private data members
	Interpreter* _interpreter;

	// prevent generated functions
	SAAPRunner(const SAAPRunner&);
	SAAPRunner& operator=(const SAAPRunner&);
};


#endif /* SAAPRUNNER_H_ */
