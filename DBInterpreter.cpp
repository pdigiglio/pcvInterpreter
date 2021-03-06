/*
 * DBInterpreter.cpp
 *
 *  Created on: Aug 28, 2014
 *      Author: wilhelma
 */
#include "DBInterpreter.h"

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <string>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include "Event.h"
#include "EventService.h"
#include "ShadowThread.h"
#include "ShadowVar.h"
#include "ShadowLock.h"
#include "LockMgr.h"
#include "ThreadMgr.h"
#include "DBTable.h"

DBInterpreter::DBInterpreter(const char* DBPath,
							 const char* logFile,
							 EventService *service,
							 LockMgr *lockMgr,
							 ThreadMgr *threadMgr) 
	: Interpreter(lockMgr, threadMgr, logFile), _dbPath(DBPath), _logFile(logFile),
	  _eventService(service) { }

DBInterpreter::~DBInterpreter(){ }

EventService* DBInterpreter::getEventService() {
	return _eventService;
}

Instruction::type DBInterpreter::transformInstrType(const instruction_t &ins) {

	if (strcmp( ins.instruction_type, "CALL") == 0)
		return Instruction::CALL;
	else if (strcmp( ins.instruction_type, "ACCESS" ) == 0)
		return Instruction::MEMACCESS;
	else if (strcmp( ins.instruction_type, "CSENTER" ) == 0)
		return Instruction::ACQUIRE;
	else if (strcmp( ins.instruction_type, "CSLEAVE" ) == 0)
		return Instruction::RELEASE;
	else if (strcmp( ins.instruction_type, "THRCREATE" ) == 0)
		return Instruction::FORK;

	return Instruction::OTHER;
}

int DBInterpreter::loadDB(const char* path, sqlite3 **db) {

	if (sqlite3_open_v2(path, db,
			SQLITE_OPEN_READONLY | SQLITE_OPEN_NOMUTEX, NULL) != SQLITE_OK) {
		BOOST_LOG_TRIVIAL(fatal) << "Can't open " << path << " - error: "
								 << sqlite3_errmsg(*db);
		sqlite3_close(*db);
		return IN_ABORT;
	} else {
		BOOST_LOG_TRIVIAL(trace) << "successfully opened " << path;
		return IN_OK;
	}
}

int DBInterpreter::closeDB(sqlite3 **db) {

	if ( sqlite3_close(*db) != SQLITE_OK)
		BOOST_LOG_TRIVIAL(fatal) <<  "Can't close database - error: "  
								 <<  sqlite3_errmsg(*db);

	return IN_OK;
}

int DBInterpreter::process() {

	sqlite3 *db;

	// open the database
	if ( loadDB(_dbPath, &db) != IN_OK ) {
		return IN_ABORT;
	}

	// fill the internal maps with database entries
	int rc = fillStructures(&db);
	if ( rc != IN_OK) {
		BOOST_LOG_TRIVIAL(error) << "Can't fill internal structures."
								 << " Error code: " << rc;
		return IN_ABORT;
	}

	// process database entries
	for (auto instruction : instructionT_)
		processInstruction(instruction.second);

	closeDB(&db);
	return 0;
}

int DBInterpreter::processInstruction(const instruction_t& ins) {

	processAccess_t accessFunc = nullptr;
	segment_t* segment = nullptr;
	call_t* call = nullptr;

	if ( segmentT_.get(ins.segment_id, &segment) == IN_OK) {  
		switch( transformInstrType(ins) ) {
		case Instruction::CALL:
				processSegment(ins.segment_id, *segment, ins);
			break;
		case Instruction::MEMACCESS:
			if ( callT_.get(segment->call_id, &call) == IN_OK) {
				accessFunc = &DBInterpreter::processMemAccess;
			}	
 			break;
		case Instruction::ACQUIRE:
			if ( callT_.get(segment->call_id, &call) == IN_OK) {
				accessFunc = &DBInterpreter::processAcqAccess;
			}
			break;
		case Instruction::RELEASE:
			if ( callT_.get(segment->call_id, &call) == IN_OK) {
				accessFunc = &DBInterpreter::processRelAccess;
			}
			break;
		case Instruction::FORK:
			{
				thread_t *thread;
				if ( threadT_.get(ins.instruction_id, &thread ) == IN_OK)
					if (callT_.get(segment->call_id, &call) == IN_OK)
					processFork(ins,
								*segment,
								*call,
								*thread);
				break;
			}
		case Instruction::JOIN:
			{
				thread_t *thread;
				if ( threadT_.get(ins.instruction_id, &thread ) == IN_OK)
					if (callT_.get(segment->call_id, &call) == IN_OK)
					processJoin(ins,
								*segment,
								*call,
								*thread);
				break;
			}
		default:
			return IN_NO_ENTRY;
		}	
	}			 

	if (accessFunc != nullptr) {
		auto search = _insAccessMap.find(ins.instruction_id);
		accessVector_t::iterator it;
		for (it = search->second.begin(); it != search->second.end(); it++) {

			auto searchAccess = accessT_.find(*it);
			if (searchAccess != accessT_.end()) {

				processAccessGeneric(search->first,
									 searchAccess->second,
									 ins,
									 *segment,
									 *call,
									 accessFunc);
			} else {
				BOOST_LOG_TRIVIAL(error) << "Access not found: " << *it;
				return IN_NO_ENTRY;
			}
		}
	}

	return IN_OK;
}

int DBInterpreter::processSegment(unsigned int segmentId,
								  const segment_t& seg,
								  const instruction_t& ins) {

	auto search = callT_.find(seg.call_id);
	if (search != callT_.end()) {
		if (!processCall(seg.call_id.c_str(), search->second, seg, ins))
			return 1;
	} else {
		BOOST_LOG_TRIVIAL(error) << "Call not found: " << seg.call_id;
		return 1;
	}

	return 0;
}

int DBInterpreter::processCall(const char* callId,
							   const call_t& call,
							   const segment_t& seg,
							   const instruction_t& ins) {

	auto search = functionT_.find(call.function_id);
	if (search != functionT_.end()) {

		switch(function_t::getFunctionType(search->second.type)) {
		case Function::FUNCTION:
		case Function::METHOD:
		{
			auto searchFile = fileT_.find(search->second.file_id);
			if (searchFile != fileT_.end()) {
				CallInfo info( std::atoi(call.end_time), // todo: use runtime!
							   search->second.signature,
							   function_t::getFunctionType(search->second.type),
							   searchFile->second.file_name,
							   searchFile->second.file_path);

				ShadowThread* thread = threadMgr_->getThread(call.thread_id);
				CallEvent event(thread, &info);
				_eventService->publish(&event);
			} else {
				BOOST_LOG_TRIVIAL(error) << "File not found: " << search->second.file_id;
				return 1;
			}
			break;
		}
		default:
			break;
		}
	} else {	 
		BOOST_LOG_TRIVIAL(error) << "Function not found: " << call.function_id;
		return IN_NO_ENTRY;
	}

	return IN_OK;
}

int DBInterpreter::processAccessGeneric(ACC_ID accessId,
										const access_t& access,
										const instruction_t& instruction,
										const segment_t& segment,
										const call_t& call,
										processAccess_t func) {

	std::string refId = std::string(access.reference_id);
	auto search = referenceT_.find(_refNoIdMap[refId]);
	if ( search != referenceT_.end() ) {

		(this->* func)(accessId, access, instruction, segment,
					   call, search->second);

	} else {
		BOOST_LOG_TRIVIAL(error) << "Reference not found: " << access.reference_id;

		return IN_NO_ENTRY;
	}

	return IN_OK;
}

int DBInterpreter::processMemAccess(ACC_ID accessId, 
									const access_t& access,
									const instruction_t& instruction,
									const segment_t& segment,
									const call_t& call,
									const reference_t& reference) {

	ShadowVar *var = 0;
	auto searchVar = _shadowVarMap.find(reference.id);
	if ( searchVar != _shadowVarMap.end() ) {
		var = searchVar->second;
	} else {
		var = new ShadowVar( getVarType(reference.memory_type),
								reference.id,
								//reference.address,
								reference.size,
								reference.name);
		_shadowVarMap[reference.id] = var;
	}

	ShadowThread* thread = threadMgr_->getThread(call.thread_id);
	AccessInfo info( access_t::getAccessType(access.access_type),
					 var,
					 instruction.instruction_id);
	AccessEvent event( thread, &info );
	_eventService->publish( &event );

	return 0;
}

int DBInterpreter::processAcqAccess(ACC_ID accessId, 
									const access_t& access,
									const instruction_t& instruction,
									const segment_t& segment,
									const call_t& call,
									const reference_t& reference) {
	
	ShadowThread* thread = threadMgr_->getThread(call.thread_id);
	ShadowLock *lock = lockMgr_->getLock(std::string(reference.reference_id));
	AcquireInfo info(lock);					  
	AcquireEvent event( thread, &info );
	_eventService->publish( &event );

	return 0;
}

int DBInterpreter::processRelAccess(ACC_ID accessId, 
									const access_t& access,
									const instruction_t& instruction,
									const segment_t& segment,
									const call_t& call,
									const reference_t& reference) {
		
	ShadowThread* thread = threadMgr_->getThread(call.thread_id);
	ShadowLock *lock = lockMgr_->getLock(std::string(reference.reference_id));
	ReleaseInfo info(lock);					  
	ReleaseEvent event( thread, &info );
	_eventService->publish( &event );

	return 0;
}

int DBInterpreter::processFork(const instruction_t& instruction,
							   const segment_t& segment,
							   const call_t& call,
							   const thread_t& thread) {
		

	ShadowThread *pT = threadMgr_->getThread(call.thread_id);
	ShadowThread *cT = threadMgr_->getThread(thread.child_thread_id);
	NewThreadInfo info(cT);					  
	NewThreadEvent event( pT, &info );
	_eventService->publish( &event );

	return 0;
}

int DBInterpreter::processJoin(const instruction_t& instruction,
							   const segment_t& segment,
							   const call_t& call,
							   const thread_t& thread) {
									   
	ShadowThread *pT = threadMgr_->getThread(call.thread_id);
	ShadowThread *cT = threadMgr_->getThread(thread.child_thread_id);
	JoinInfo info(cT);					  
	JoinEvent event( pT, &info );
	_eventService->publish( &event );

	return 0;
}

ShadowVar::VarType DBInterpreter::getVarType(REF_MTYP memType) {
	switch (memType) {
	case reference_t::LOCAL:
		return ShadowVar::STACK;
	case reference_t::HEAP:
		return ShadowVar::HEAP;
	case reference_t::GLOBAL:
		return ShadowVar::GLOBAL;
	case reference_t::STATIC:
		return ShadowVar::STATIC;
	default:
		BOOST_LOG_TRIVIAL(error) << "No valid memory type: " << memType;
		return ShadowVar::ERROR;
	}																	
}

int DBInterpreter::fillStructures(sqlite3 **db) {

	int rc;

	if ((rc = fillGeneric("SELECT * from ACCESS_TABLE;",
						  db, &DBInterpreter::fillAccess)) != 0) return rc;
	if ((rc = fillGeneric("SELECT * from CALL_TABLE;",
						  db, &DBInterpreter::fillCall)) != 0) return rc;
	if ((rc = fillGeneric("SELECT * from FILE_TABLE;",
						  db, &DBInterpreter::fillFile)) != 0) return rc;
	if ((rc = fillGeneric("SELECT * from FUNCTION_TABLE;",
						  db, &DBInterpreter::fillFunction)) != 0) return rc;
	if ((rc = fillGeneric("SELECT * from INSTRUCTION_TABLE;",
						  db, &DBInterpreter::fillInstruction)) != 0) return rc;
	if ((rc = fillGeneric("SELECT * from REFERENCE_TABLE;",
						  db, &DBInterpreter::fillReference)) != 0) return rc;
	if ((rc = fillGeneric("SELECT * from SEGMENT_TABLE;",
						  db, &DBInterpreter::fillSegment)) != 0) return rc;
	if ((rc = fillGeneric("SELECT * from THREAD_TABLE;",
						  db, &DBInterpreter::fillThread)) != 0) return rc;

	BOOST_LOG_TRIVIAL(trace) << "Rows in ACCESS_TABLE: " << accessT_.size();
	BOOST_LOG_TRIVIAL(trace) << "Rows in CALL_TABLE: " << callT_.size();
	BOOST_LOG_TRIVIAL(trace) << "Rows in FILE_TABLE: " << fileT_.size();
	BOOST_LOG_TRIVIAL(trace) << "Rows in FUNCTION_TABLE: " << functionT_.size();
	BOOST_LOG_TRIVIAL(trace) << "Rows in INSTRUCTION_TABLE: " << instructionT_.size();
	BOOST_LOG_TRIVIAL(trace) << "Rows in REFERENCE_TABLE: " << referenceT_.size();
	BOOST_LOG_TRIVIAL(trace) << "Rows in SEGMENT_TABLE: " << segmentT_.size();
	BOOST_LOG_TRIVIAL(trace) << "Rows in THREAD_TABLE: " << threadT_.size();
	return 0;
}

int DBInterpreter::fillGeneric(const char *sql, sqlite3 **db, fillFunc_t func) {

   sqlite3_stmt *sqlstmt = 0;

   /* Execute SQL statement */
   if (sqlite3_prepare_v2(*db, sql, strlen(sql), &sqlstmt, NULL) != SQLITE_OK) {
	   BOOST_LOG_TRIVIAL(error) << "Error preparing db: " << sqlite3_errmsg(*db);
	   return 1;
   }

   bool reading = true;
   while (reading) {
	   switch(sqlite3_step(sqlstmt)) {
	   case SQLITE_ROW:
		   (this->* func)(sqlstmt);
		   break;
	   case SQLITE_DONE:
		   reading = false;
		   break;
	   default:
		   BOOST_LOG_TRIVIAL(trace) << "Iterating db failed!";
		   return 2;
	   }
   }

   return 0;
}

int DBInterpreter::fillAccess(sqlite3_stmt *sqlstmt) {

   ACC_ID id = sqlite3_column_int(sqlstmt, 0);
   INS_ID instruction_id = sqlite3_column_int(sqlstmt, 1);
   int position = sqlite3_column_int(sqlstmt, 2);
   const unsigned char *reference_id = sqlite3_column_text(sqlstmt, 3);
   const unsigned char *access_type = sqlite3_column_text(sqlstmt, 4);
   const unsigned char *memory_state = sqlite3_column_text(sqlstmt, 5);

   access_t *tmp = new access_t(instruction_id,
		   	   	   	   	   	     position,
		   	   	   	   	   	     reference_id,
		   	   	   	   	   	     access_type,
		   	   	   	   	   	     memory_state); 

   accessT_.fill(id, *tmp);		 
   _insAccessMap[instruction_id].push_back(id); // create 1:n associations 
   return 0;
}

int DBInterpreter::fillCall(sqlite3_stmt *sqlstmt) {

   const unsigned char *id = sqlite3_column_text(sqlstmt, 0);
   int process_id = sqlite3_column_int(sqlstmt, 1);
   int thread_id = sqlite3_column_int(sqlstmt, 2);
   int function_id = sqlite3_column_int(sqlstmt, 3);
   int instruction_id = sqlite3_column_int(sqlstmt, 4);
   const unsigned char *start_time = sqlite3_column_text(sqlstmt, 5);
   const unsigned char *end_time = sqlite3_column_text(sqlstmt, 6);

   call_t *tmp = new call_t(process_id,
							thread_id,
							function_id,
							instruction_id,
							start_time,
							end_time);

   callT_.fill(std::string((const char*)id), *tmp);		 
   return 0;
}

int DBInterpreter::fillFile(sqlite3_stmt *sqlstmt) {

   int id = sqlite3_column_int(sqlstmt, 0);
   const unsigned char *file_name = sqlite3_column_text(sqlstmt, 1);
   const unsigned char *file_path = sqlite3_column_text(sqlstmt, 2);

   file_t *tmp = new file_t(file_name,
		   	   	   	   	   	file_path);

   fileT_.fill(id, *tmp);	 
   return 0;
}

int DBInterpreter::fillFunction(sqlite3_stmt *sqlstmt) {

   int id = sqlite3_column_int(sqlstmt, 0);
   const unsigned char *signature = sqlite3_column_text(sqlstmt, 1);
   const unsigned char *type = sqlite3_column_text(sqlstmt, 2);
   int file_id = sqlite3_column_int(sqlstmt, 3);

   function_t *tmp = new function_t(signature,
		   	   	   	  	   	   	    type,
		   	   	   	   	   	   	    file_id);

   functionT_.fill(id, *tmp);
   return 0;
}

int DBInterpreter::fillInstruction(sqlite3_stmt *sqlstmt) {

   int id = sqlite3_column_int(sqlstmt, 0);
   int segment_id = sqlite3_column_int(sqlstmt, 1);
   const unsigned char *instruction_type = sqlite3_column_text(sqlstmt, 2);
   int line_number = sqlite3_column_int(sqlstmt, 3);

   instruction_t *tmp = new instruction_t(id,
										  segment_id,
										  instruction_type,
										  line_number);

   instructionT_.fill(id, *tmp);
   return 0;
}

int DBInterpreter::fillReference(sqlite3_stmt *sqlstmt) {

   int id = sqlite3_column_int(sqlstmt, 0);
   const unsigned char *reference_id = sqlite3_column_text(sqlstmt, 1);
   //int address = sqlite3_column_int(sqlstmt, 2);
   int size = sqlite3_column_int(sqlstmt, 2);
   const unsigned char *memory_type = sqlite3_column_text(sqlstmt, 3);
   const unsigned char *name = sqlite3_column_text(sqlstmt, 4);
   int allocinstr = sqlite3_column_int(sqlstmt, 5);

   reference_t *tmp = new reference_t(reference_id,
		   	   	   	   	   	   	   	  id,
		   	   	   	   	   	   	   	  //address,
		   	   	   	   	   	   	   	  size,
		   	   	   	   	   	   	   	  memory_type,
		   	   	   	   	   	   	   	  name,
		   	   	   	   	   	   	   	  allocinstr);

   referenceT_.fill(id, *tmp);

   REF_NO no = REF_NO((const char*)reference_id);
   _refNoIdMap[no] = id; // create association between no and id

   return 0;
}

int DBInterpreter::fillSegment(sqlite3_stmt *sqlstmt) {

   int id = sqlite3_column_int(sqlstmt, 0);
   const unsigned char *call_id = sqlite3_column_text(sqlstmt, 1);
   int segment_no = sqlite3_column_int(sqlstmt, 2);
   const unsigned char *segment_type = sqlite3_column_text(sqlstmt, 3);
   int loop_pointer = sqlite3_column_int(sqlstmt, 4);

   segment_t *tmp = new segment_t(call_id,
		   	   	   	   	   	   	  segment_no,
		   	   	   	   	   	   	  segment_type,
		   	   	   	   	   	   	  loop_pointer);

   segmentT_.fill(id, *tmp);
   return 0;
}

int DBInterpreter::fillThread(sqlite3_stmt *sqlstmt) {

   int id = sqlite3_column_int(sqlstmt, 0);
   int instruction_id = sqlite3_column_int(sqlstmt, 1);
   int parent_thread_id = sqlite3_column_int(sqlstmt, 2);
   int child_thread_id = sqlite3_column_int(sqlstmt, 3);

   thread_t *tmp = new thread_t(id,
		   	   	   	   	   	   	instruction_id,
		   	   	   	   	   	   	parent_thread_id,
		   	   	   	   	   	   	child_thread_id);

   threadT_.fill(instruction_id, *tmp);
   return 0;
}
