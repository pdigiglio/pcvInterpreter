/**
 * @file DBDataModel.h
 *
 *  Created on: Sep 2, 2014
 *      Author: wilhelma
 */

#ifndef DBDATAMODEL_H_
#define DBDATAMODEL_H_

#include <boost/serialization/strong_typedef.hpp>

#include <cstring>
#include <string>
#include <iostream>
#include "DataModel.h"
#include "Types.h"

typedef struct access_t {
    ACC_ID sql_id;
    INS_ID instruction_id;
    POS position;
    REF_ID reference_id;
    AccessType access_type;
    AccessState memory_state;

    explicit
    access_t(ACC_ID sqlID,
             INS_ID instructionID,
             POS pos,
             REF_ID referenceID,
             AccessType accessType,
             AccessState memoryState)
        : sql_id(sqlID), instruction_id(instructionID), position(pos),
          reference_id(referenceID), access_type(accessType),
          memory_state(memoryState)
    {
    }
} access_t;

// Try to make a new struct for call_t as the database format has changed
typedef struct call_t {
    CAL_ID sql_id;
    TRD_ID thread_id;
    FUN_ID function_id;
    INS_ID instruction_id;
    TIME start_time;
    TIME end_time;

    explicit
    call_t(CAL_ID sqlID,
           TRD_ID threadID,
           FUN_ID functionID,
           INS_ID instructionID,
           TIME startTime,
           TIME endTime)
        : sql_id(sqlID), thread_id(threadID),
          function_id(functionID), instruction_id(instructionID),
          start_time(startTime), end_time(endTime)
    {
    }
} call_t;

/**
 * @brief Record to process the `File` table in the database.
 */
typedef struct file_t {
    FIL_ID sql_id;
    FIL_PT file_path;
    FIL_PT file_name; // retrieved from file_path

    explicit
    file_t(FIL_ID sqlID,
           FIL_PT filePath)
        : sql_id(sqlID), file_path(filePath)
    {
        retrieveFileName( filePath );
    }

    private:
    int retrieveFileName( FIL_PT filePath ) {
        // Separator
        const char directorySeparator = '/';
        // search for last occurrence
        size_t pos = filePath.find_last_of( directorySeparator );

        // if separator is not found
        if( pos > filePath.size() ) {
            this->file_name = filePath;
            return 1;
        }

        this->file_name = filePath.substr( pos );
        return 0;
    }
} file_t;

typedef struct function_t {
    FUN_ID sql_id;
    FUN_SG signature;
    FunctionType type;
    FIL_ID file_id;
    LIN_NO line_number;

    explicit
    function_t(FUN_ID sqlID,
               FUN_SG fnSignature,
               FunctionType fnType,
               FIL_ID fileId,
               LIN_NO lineNumber)
        : sql_id(sqlID), signature(fnSignature), type(fnType),
          file_id(fileId), line_number(lineNumber)
    {
    }

    static void printFunctionType(unsigned short shift) {
        switch(shift) {
            case 0:
                std::cout << "ENTRY_ROUTINE" << std::endl;
                break;
            case 1:
                std::cout << "EXIT_ROUTINE"  << std::endl;
                break;
            case 2:
                std::cout << "FUNCTION"  << std::endl;
                break;
            case 3:
                std::cout << "METHOD"  << std::endl;
                break;
            case 4:
                std::cout << "FREE"  << std::endl;
                break;
            case 5:
                std::cout << "FORK"  << std::endl;
                break;
            case 6:
                std::cout << "JOIN"  << std::endl;
                break;
            case 7:
                std::cout << "ACQUIRE"  << std::endl;
                break;
            case 8:
                std::cout << "RELEASE"  << std::endl;
                break;
            case 9:
                std::cout << "EXTERNAL"  << std::endl;
                break;
            case 10:
                std::cout << "ALLOC"  << std::endl;
                break;
            case 11:
                std::cout << "BARRIER"  << std::endl;
                break;
            case 12:
                std::cout << "WAIT"  << std::endl;
                break;
            case 13:
                std::cout << "SIGNAL_SINGLE"  << std::endl;
                break;
            case 14:
                std::cout << "SIGNAL_BROADCAST"  << std::endl;
                break;
        }
    }

    /**
     * @brief Decodes the function type
     * @todo Return a proper thing!!
     */
//  static Function::type getFunctionType(FUN_TYP fnType) {
//
//      // shift right functionType till there is no information left
//      unsigned int counter = 0;
//      while( fnType != 0 ){
//          if( fnType & 1 ) {
//              std::cout << "   -- has type " << (1 << counter) << std::endl;
//              std::cout << "    -> ";
//              printFunctionType( counter );
//          }
//
//          counter ++;
//          // shift right by one
//          fnType >>= 1;
//      }
//      return Function::OTHER;
//  }
} function_t;

typedef struct instruction_t {
    INS_ID instruction_id;
    SEG_ID segment_id;
    InstructionType instruction_type;
    LIN_NO line_number;

    explicit
    instruction_t(INS_ID instructionId,
                  SEG_ID segmentId,
                  InstructionType instructionType,
                  LIN_NO lineNumber) 
                  : instruction_id(instructionId), segment_id(segmentId),
                  instruction_type(instructionType), line_number(lineNumber)
    {
    }
} instruction_t;

typedef struct loop_t {
    LOP_ID sql_id;
    unsigned line_number;

    explicit
    loop_t(LOP_ID sqlID,
           int lineNumber)
        : sql_id(sqlID), line_number(lineNumber)
    {}

} loop_t;

typedef struct loopExecution_t {
    LOE_ID sql_id;
    LOE_ID loop_id;
    unsigned parent_iteration;
    unsigned loop_duration;

    explicit
    loopExecution_t(LOE_ID sqlID,
                    LOE_ID loopID,
                    int parentIteration,
                    int loopDuration)
                    : sql_id(sqlID), loop_id(loopID),
                      parent_iteration(parentIteration), loop_duration(loopDuration)
    {}

} loopExecution_t;

typedef struct loopIteration_t {
    LOI_ID sql_id;
    LOE_ID loop_execution;
    LOI_ID loop_iteration;

    explicit
    loopIteration_t(LOI_ID sqlID,
                    LOE_ID loopExecution,
                    LOI_ID loopIteration)
                    : sql_id(sqlID),
                      loop_execution(loopExecution),
                      loop_iteration(loopIteration)
    {}

} loopIteration_t;

typedef struct reference_t {
    REF_ID id;
    //REF_ADDR address;
    REF_SIZE size;
    ReferenceType memory_type;
    REF_NAME name;
    INS_ID allocinstr;

    explicit
    reference_t(REF_ID refId,
                REF_SIZE refSize,
                ReferenceType memoryType,
                REF_NAME refName,
                INS_ID allocInstr)
        : id(refId), size(refSize), memory_type(memoryType),
          name(refName), allocinstr(allocInstr)
    {
    }

} reference_t;

typedef struct segment_t {
    SEG_ID sql_id;
    CAL_ID call_id;
    SegmentType segment_type;
    LOI_ID loop_pointer;

    explicit
    segment_t(SEG_ID sqlID,
              CAL_ID callId,
              SegmentType segmentType,
              LOI_ID loopPointer)
        : sql_id(sqlID), call_id(callId),
          segment_type(segmentType),
          loop_pointer(loopPointer)
    {
    }

} segment_t;

typedef struct thread_t {
    TRD_ID id;
    TIME_STRING start_time;
    TIME_STRING end_time;
    NUM_CYCLES num_cycles;
    INS_ID create_instruction_id;
    INS_ID join_instruction_id;
    TRD_ID parent_thread_id;
    PID process_id;

    explicit
    thread_t(TRD_ID id,
             TIME_STRING start_time,
             TIME_STRING end_time,
             NUM_CYCLES num_cycles,
             INS_ID createINS_id,
             INS_ID joinIns_id,
             TRD_ID parentThreadId,
             PID processId)
        : id(id), start_time(start_time), end_time(end_time),
          num_cycles(num_cycles), create_instruction_id(createINS_id),
          join_instruction_id(joinIns_id), parent_thread_id(parentThreadId),
          process_id(processId) {}
} thread_t;


#endif /* DBDATAMODEL_H_ */
