/**
 *
 *    @file  Types.h
 *   @brief  Collects all `typedef`s and `BOOST_STRONG_TYPEDEF()`s.
 *
 *    @date  04/19/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#ifndef TYPES_H_
#define TYPES_H_

// Define ID to be unsigned
typedef unsigned ID;

// All others IDs should have the same type as ID
BOOST_STRONG_TYPEDEF( ID, ACC_ID )
BOOST_STRONG_TYPEDEF( ID, CAL_ID )
BOOST_STRONG_TYPEDEF( ID, FIL_ID )
BOOST_STRONG_TYPEDEF( ID, FUN_ID )
BOOST_STRONG_TYPEDEF( ID, INS_ID )
BOOST_STRONG_TYPEDEF( ID, LOP_ID )
BOOST_STRONG_TYPEDEF( ID, LOE_ID )
BOOST_STRONG_TYPEDEF( ID, LOI_ID )
BOOST_STRONG_TYPEDEF( ID, REF_ID )
BOOST_STRONG_TYPEDEF( ID, SEG_ID )
BOOST_STRONG_TYPEDEF( ID, TRD_ID )

// Access Types
BOOST_STRONG_TYPEDEF( unsigned, POS )
BOOST_STRONG_TYPEDEF( unsigned, ACC_TYP )
BOOST_STRONG_TYPEDEF( unsigned, MEM_ST )

// Call Types
BOOST_STRONG_TYPEDEF( clock_t, TIME )

// File Types
typedef std::string FIL_PT;

// Function Types
typedef std::string FUN_SG; 
BOOST_STRONG_TYPEDEF( uint32_t, FUN_TYP )
BOOST_STRONG_TYPEDEF( unsigned, LIN_NO )

// Instruction Types
BOOST_STRONG_TYPEDEF( unsigned, INS_TYP )

// TODO loops

// Reference Types
BOOST_STRONG_TYPEDEF( size_t, REF_SIZE )
BOOST_STRONG_TYPEDEF( unsigned, REF_MTYP )
typedef std::string REF_NAME;

// Segment Types
BOOST_STRONG_TYPEDEF( bool, SEG_TYP )

// Thread Types
BOOST_STRONG_TYPEDEF( unsigned, PID )


#endif /* TYPES_H_ */