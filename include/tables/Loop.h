/**
 *
 *    @file  Loop.h
 *   @brief  
 *
 *    @date  06/16/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#ifndef  LOOP_H_
#define  LOOP_H_

#include "Types.h"

typedef struct loop_t {
    LOP_ID sql_id;
    LIN_NO line_number;

    explicit
    loop_t(LOP_ID sqlID,
           LIN_NO lineNumber) noexcept
        : sql_id(sqlID), line_number(lineNumber)
    {}

} loop_t;

#endif
