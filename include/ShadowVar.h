/*
 * ShadowVar.h
 *
 *  Created on: Sep 2, 2014
 *      Author: wilhelma
 */

#ifndef SHADOWVAR_H_
#define SHADOWVAR_H_

#include "Reference.h"
#include "Types.h"

/******************************************************************************
 * Shadow variable
 *****************************************************************************/
class ShadowVar {
public:
	ShadowVar(const reference_t& r)
				 : type(r.memory_type), id(r.id) //addr(Addr), ptr(nullptr)
//				   size(r.size) //, name(r.name)
	{}

	const REF_ID& getId() const { return id; }
	const ReferenceType& getType() const { return type; }


private:
//	void* ptr;
	const ReferenceType type;
	const REF_ID id;
//	const REF_SIZE size;
//	const REF_NAME name;

	// prevent generated functions
	ShadowVar(const ShadowVar&);
	ShadowVar& operator=(const ShadowVar&);
};

#endif /* SHADOWVAR_H_ */
