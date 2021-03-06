/*
 * DBInterpreter.h
 *
 *  Created on: Aug 28, 2014
 *      Author: wilhelma
 */

#ifndef DBINTERPRETER_H_
#define DBINTERPRETER_H_

#include <sqlite3.h>
#include <map>
#include <vector>
#include <string.h>
#include "Interpreter.h"
#include "EventService.h"
#include "ShadowThread.h"
#include "ShadowVar.h"
#include "DBDataModel.h"
#include "DBTable.h"

class LockMgr;
class ThreadMgr;

/******************************************************************************
 * Database Interpreter
 *****************************************************************************/
class DBInterpreter : public Interpreter {
public:
	DBInterpreter(const char* DBPath, const char* logFile, 
				  EventService *service, LockMgr *lockMgr, ThreadMgr *threadMgr);
	int process() override;
	EventService* getEventService() override;
	~DBInterpreter();

private:

	// types-------------------------------------------------------------------
	typedef int (DBInterpreter::*fillFunc_t)(sqlite3_stmt*);
	typedef int (DBInterpreter::*processAccess_t)(ACC_ID accessID,
												  const access_t& access,
												  const instruction_t& instruction,
												  const segment_t& segment,
												  const call_t& call,
												  const reference_t& reference);

	typedef std::vector<ACC_ID> accessVector_t;
	typedef std::map<INS_ID, accessVector_t> insAccessMap_t;
	typedef std::map<REF_NO, REF_ID> refNoIdMap_t;
	typedef std::map<REF_ID, ShadowVar*> shadowVarMap_t;

	// members-----------------------------------------------------------------
	DBTable<ACC_ID, access_t> accessT_;
	DBTable<CAL_ID, call_t> callT_;
	DBTable<FIL_ID, file_t> fileT_;
	DBTable<FUN_ID, function_t> functionT_;
	DBTable<INS_ID, instruction_t> instructionT_;
	DBTable<REF_ID, reference_t> referenceT_;
	DBTable<SEG_ID, segment_t> segmentT_; 
	DBTable<INS_ID, thread_t> threadT_;

	insAccessMap_t _insAccessMap;
	refNoIdMap_t _refNoIdMap;
	const char* _dbPath;
	const char* _logFile;
	EventService *_eventService;
	shadowVarMap_t _shadowVarMap;

	// private methods---------------------------------------------------------
	static Instruction::type transformInstrType(const instruction_t& ins);
	static ShadowVar::VarType getVarType(REF_MTYP memType);

	int loadDB(const char* path, sqlite3 **db);
	int closeDB(sqlite3 **db);
	int fillStructures(sqlite3 **db);
	int fillGeneric(const char *sql, sqlite3 **db, fillFunc_t func);
	int fillAccess(sqlite3_stmt *stmt);
	int fillCall(sqlite3_stmt *stmt);
	int fillFile(sqlite3_stmt *stmt);
	int fillFunction(sqlite3_stmt *stmt);
	int fillInstruction(sqlite3_stmt *stmt);
	int fillReference(sqlite3_stmt *stmt);
	int fillSegment(sqlite3_stmt *stmt);
	int fillThread(sqlite3_stmt *stmt);

	int processInstruction(const instruction_t& instruction);
	int processSegment(SEG_ID segmentId,
					   const segment_t& segment,
					   const instruction_t& instruction);
	int processCall(const char* callId,
					const call_t& call,
					const segment_t& segment,
					const instruction_t& instruction);
	int processAccessGeneric(ACC_ID accessId,
							 const access_t& access,
							 const instruction_t& instruction,
							 const segment_t& segment,
							 const call_t& call,
							 processAccess_t func);
	int processMemAccess(ACC_ID accessId,
						 const access_t& access,
						 const instruction_t& instruction,
						 const segment_t& segment,
						 const call_t& call,
						 const reference_t& reference);
	int processAcqAccess(ACC_ID accessID,
						 const access_t& access,
						 const instruction_t& instruction,
						 const segment_t& segment,
						 const call_t& call,
						 const reference_t& reference);
	int processRelAccess(ACC_ID accessID,
						 const access_t& access,
						 const instruction_t& instruction,
						 const segment_t& segment,
						 const call_t& call,
						 const reference_t& reference);
	int processJoin(const instruction_t& instruction,
					const segment_t& segment,
					const call_t& call,
					const thread_t& thread);
	int processFork(const instruction_t& instruction,
					const segment_t& segment,
					const call_t& call,
					const thread_t& thread);


	// prevent generated functions
	DBInterpreter(const DBInterpreter&);
	DBInterpreter& operator=(const DBInterpreter&);
};


#endif /* DBINTERPRETER_H_ */
