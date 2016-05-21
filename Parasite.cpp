/*
 * Parasite.cpp
 *
 *  Created on: Dec 12, 2015
 *      Author: knapp
 */

#include <map>
#include <vector>
#include "Parasite.h"


Parasite::~Parasite() {

	workSpanMap.clear();
}

void Parasite::addWorkSpan(const char* parentFunctionSignature, const char* functionSignature, double work, double prefix, double longest_child, double continuation)  {

	WorkSpan_ newWorkSpan = {parentFunctionSignature, functionSignature, work, prefix, longest_child, continuation, 0.0, 0.0};
	std::pair<const char*, WorkSpan_*> newPair(functionSignature, &newWorkSpan);
	workSpanMap.insert(newPair);
}

void Parasite::setWorkSpan(const char* functionSignature, double work, double prefix, double longest_child, double continuation)  {

	WorkSpan_* ws = workSpanMap[functionSignature];
	const char* parentFunctionSignature = ws->parentFunctionSignature;
	workSpanMap.erase(functionSignature);
	WorkSpan_ newWorkSpan = {parentFunctionSignature, functionSignature, work, prefix, longest_child, continuation, 0.0, 0.0};
	std::pair<const char*, WorkSpan_*> newPair(functionSignature, &newWorkSpan);
	workSpanMap.insert(newPair);
}

void Parasite::addToWorkSpan(const char* functionSignature, double work_diff, double prefix_diff, double longest_child_diff, 
	 								double continuation_diff)  {

	WorkSpan_* ws = workSpanMap[functionSignature];

	double old_work = ws->work;
	double old_prefix = ws->prefix;
	double old_longest_child = ws->longest_child;
	double old_continuation = ws->continuation;
	const char* parentFunctionSignature = ws->parentFunctionSignature;
	workSpanMap.erase(functionSignature);

	WorkSpan_ newWorkSpan = {parentFunctionSignature, functionSignature, old_work + work_diff, old_prefix + prefix_diff, 
							 old_longest_child + longest_child_diff, old_continuation + continuation_diff};

	std::pair<const char*, WorkSpan_*> newPair(functionSignature, &newWorkSpan);
	workSpanMap.insert(newPair);
}

void Parasite::setWork(const char* functionSignature, double wrk){

	WorkSpan_* ws = workSpanMap[functionSignature];
	ws->work = wrk;
}

void Parasite::setPrefix(const char* functionSignature, double prfix){

	WorkSpan_* ws = workSpanMap[functionSignature];
	ws->prefix = prfix;
}

void Parasite::setLongestChild(const char* functionSignature, double lngest_child){

	WorkSpan_* ws = workSpanMap[functionSignature];
	ws->longest_child = lngest_child;
}

void Parasite::setContinuation(const char* functionSignature, double cntinuation){


	WorkSpan_* ws = workSpanMap[functionSignature];
	ws->continuation = cntinuation;
}

void Parasite::setLockSpan(const char* functionSignature, double lck_span) {


	WorkSpan_* ws = workSpanMap[functionSignature];
	ws->lock_span = lck_span;
}

void Parasite::setLongestChildLockSpan(const char* functionSignature, double lngest_child_lock_span) {


	WorkSpan_* ws = workSpanMap[functionSignature];
	ws->longest_child_lock_span = lngest_child_lock_span;
}

void Parasite::setLastLockStart(const char* functionSignature, double lst_lock_start) {


	WorkSpan_* ws = workSpanMap[functionSignature];
	ws->last_lock_start = lst_lock_start;
}


void Parasite::addToWork(const char* functionSignature, double work_diff){

	WorkSpan_* ws = workSpanMap[functionSignature];
	ws->work += work_diff;
}

void Parasite::addToPrefix(const char* functionSignature, double prefix_diff){

	WorkSpan_* ws = workSpanMap[functionSignature];
	ws->prefix += prefix_diff;
}

void Parasite::addToLongestChild(const char* functionSignature, double longest_child_diff){

	WorkSpan_* ws = workSpanMap[functionSignature];
	ws->longest_child += longest_child_diff;
}

void Parasite::addToContinuation(const char* functionSignature, double continuation_diff){

	WorkSpan_* ws = workSpanMap[functionSignature];
	ws->continuation += continuation_diff;
}

void Parasite::addToLockSpan(const char* functionSignature, double lock_span_diff) {

	WorkSpan_* ws = workSpanMap[functionSignature];
	ws->lock_span += lock_span_diff;
}

void Parasite::addToLongestChildLockSpan(const char* functionSignature, double longest_child_lock_span_diff) {

	WorkSpan_* ws = workSpanMap[functionSignature];
	ws->longest_child_lock_span += longest_child_lock_span_diff;
}


void Parasite::addToLastLockStart(const char* functionSignature, double last_lock_start_diff) {

	WorkSpan_* ws = workSpanMap[functionSignature];
	ws->last_lock_start += last_lock_start_diff;
}

double Parasite::getWork(const char* functionSignature) {

	WorkSpan_* ws = workSpanMap[functionSignature];
	return ws->work;
}

double Parasite::getPrefix(const char* functionSignature) {

	WorkSpan_* ws = workSpanMap[functionSignature];
	return ws->prefix;
}

double Parasite::getLongestChild(const char* functionSignature){

	WorkSpan_* ws = workSpanMap[functionSignature];
	return ws->longest_child;
}

double Parasite::getContinuation(const char* functionSignature){

	WorkSpan_* ws = workSpanMap[functionSignature];
	return ws->continuation;
}

double Parasite::getLockSpan(const char* functionSignature) {

	WorkSpan_* ws = workSpanMap[functionSignature];
	return ws->lock_span;
}

double Parasite::getLongestChildLockSpan(const char* functionSignature) {

	WorkSpan_* ws = workSpanMap[functionSignature];
	return ws->longest_child_lock_span;
}






