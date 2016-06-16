/**
 *
 *    @file  Instruction.h
 *   @brief  
 *
 *    @date  06/16/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#ifndef  INSTRUCTION_H_
#define  INSTRUCTION_H_

#include "Types.h"

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

#endif
