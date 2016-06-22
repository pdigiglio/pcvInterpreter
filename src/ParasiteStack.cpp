#include "ParasiteStack.h"

void ParasiteStack::init_function_frame(int function_index) {

	std::unique_ptr<function_frame_t> frame = function_stack.at(function_index);

	frame->local_work = 0;
	frame->local_lock_span = 0;
	frame->running_lock_span = 0;
	frame->running_work = 0;
	frame->running_span = 0;
}

void ParasiteStack::init_thread_frame(int thread_index, int head_function_index) {

	std::unique_ptr<thread_frame_t> frame = thread_stack.at(thread_index);

	frame->head_function_index = head_function_index;
	frame->local_span = 0;
	frame->lock_span = 0;
  	frame->local_continuation = 0;
	frame->prefix_span = 0; 
	frame->longest_child_span = 0;
	frame->longest_child_lock_span = 0;
}


// push a new thread frame onto the thread stack 
void ParasiteStack::thread_stack_push() {

	std::unique_ptr<thread_frame_t> new_thread_frame(new thread_frame_t);
	thread_stack.push_back(new_thread_frame);
	current_thread_index += 1;
}

// push a new function frame onto the function stack 
void ParasiteStack::function_stack_push() {

	std::unique_ptr<function_frame_t> new_function_frame(new function_frame_t);
	function_stack.push_back(new_function_frame);
	current_function_index += 1;
}