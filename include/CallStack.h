/**
 *
 *    @file  CallStack.h
 *   @brief  
 *
 *    @date  06/17/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#ifndef  CALL_STACK_H_
#define  CALL_STACK_H_

#include "Types.h"

#include <vector>

//enum class StackAction: bool {
//	PUSH = 0,
//	POP  = 1
//};

/// Simple stack wrapping a `std::vector`
template <class T>
class Stack {
	public:
		/// Pop a value from the top of the stack
		const T pop() {
			T call = Stack_.back();
			Stack_.pop_back();
			return call;
		}
		/// Push a value on top of the stack
		void push(T id) { Stack_.push_back(id); }

		/// Check whether the stack is empty
		const bool isEmpty() const { return Stack_.empty(); }

		/// Top of the stack
		/// @return The value at the top of the stack without popping it
		const T& top() const { return Stack_.back(); }

		
	private:
		std::vector<T> Stack_;
};

using CallStack = Stack<CAL_ID>;

#endif
