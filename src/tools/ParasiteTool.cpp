/**
 *
 *    @file  ParasiteTool.cpp
 *   @brief  Implementation file for the class `ParasiteTool`
 *
 *    @date  06/17/16
 *  @author  Nathaniel Knapp (github.com/deknapp),
 *           <nathaniel.knapp@gmail.com>
 *
 */

#include <algorithm>    // std::max
#include <cassert>
#include <utility>
#include <limits>
#include <climits>
#include "ParasiteTool.h"

ParasiteTool::ParasiteTool():
                             last_function_call_time(0), 
                             last_function_return_time(0),
                             last_function_runtime(0), 
                             last_thread_end_time(0), 
                             last_thread_runtime(0),
                             last_thread_start_time(0),
                             lock_span_end_time(0),
                             lock_span_start_time(0) {

  stacks = std::unique_ptr<ParasiteTracker>(new ParasiteTracker());

  parasite_profile = std::unique_ptr<parasite_profile_t>
                                                      (new parasite_profile_t);
  
  end_call_site_profile_hashtable = std::unique_ptr<call_site_end_hashtable_t>
                                                (new call_site_end_hashtable_t);

  std::unordered_map<unsigned int, int> lck_hashtable;
  lock_hashtable = lck_hashtable;
}

void ParasiteTool::getEndProfile() {
  std::shared_ptr<thread_frame_t> bottom_thread_frame = stacks->bottomThread();
  std::shared_ptr<function_frame_t> bottom_function_frame = stacks->bottomFunction();

  // Calculate span for entire program 
  parasite_profile->span =  bottom_function_frame->running_span +
                            bottom_thread_frame->prefix_span +
                            bottom_thread_frame->local_span +
                            bottom_thread_frame->local_continuation;

  CallSiteHashtable bottom_prefix_table(bottom_thread_frame->prefix_table);
  bottom_prefix_table.add(&(bottom_thread_frame->continuation_table));
  CallSiteHashtable final_on_span_table(bottom_thread_frame->prefix_table);

  // Calculate work for entire program
  parasite_profile->work = bottom_function_frame->running_work +
                           bottom_function_frame->local_work;

  // Calculate parallelism for entire program                     
  parasite_profile->parallelism = parasite_profile->work 
                                / parasite_profile->span;

  CallSiteHashtable final_table(stacks->work_table);

  // parse the final work table in the main stack data structure 
  // iterate through all entries in the hashtable containing collected
  // work profiles of call sites. 
  for (auto const &it : *final_table.hashtable) {
    std::shared_ptr<call_site_profile_t> current_call_site_profile = it.second;
    CALLSITE current_call_site_ID = it.first;
    std::shared_ptr<CallSiteEndProfile> current_call_site_end_profile(new CallSiteEndProfile(current_call_site_profile));

    // add work information into the final profile for each call site 
    std::pair<CALLSITE, std::shared_ptr<CallSiteEndProfile> > 
                   newPair(current_call_site_ID, current_call_site_end_profile);
    end_call_site_profile_hashtable->insert(newPair);
  }

  // parse the final span table in the main stack data structure 
  // iterate through all entries in the hashtable containing 
  // collected span profiles of call sites. 
  for (auto const &it : *final_on_span_table.hashtable) {
    std::shared_ptr<call_site_profile_t> current_call_site_profile = it.second;
    CALLSITE current_call_site_ID = it.first;
    std::shared_ptr<CallSiteEndProfile> current_call_site_end_profile = 
                      end_call_site_profile_hashtable->at(current_call_site_ID);

    // add span information into the final profile for each call site 
    current_call_site_end_profile->getEndCallSiteSpanProfile(current_call_site_profile);
  }
}

void ParasiteTool::printProfile() {
  // first, calculate all the end profiles before outputting them 
  getEndProfile();
  printf("PARALLELISM IS %f \n", parasite_profile->parallelism);
  printf("WORK IS %f \n", parasite_profile->work);
  printf("SPAN IS %f \n", parasite_profile->span);
  printf("LOCK SPAN IS %f \n", parasite_profile->lock_span);
}

ParasiteTool::~ParasiteTool() {
  printProfile();
}

void ParasiteTool::Call(const CallEvent* e) {
  const CallInfo* _info(e->getCallInfo());

  FUN_SG calledFunctionSignature = _info->fnSignature;
  CALLSITE callsiteID = _info->siteId;
  last_function_runtime = _info->runtime;
  last_function_call_time = _info->startTime;
  printf("starting call Event with signature %s \n", calledFunctionSignature.c_str());
  
  bool is_top_call_site_function = stacks->work_table.contains(callsiteID);

  stacks->function_push(calledFunctionSignature, 
                        callsiteID, 
                        is_top_call_site_function);
  printf("ending call Event \n");
}

void ParasiteTool::NewThread(const NewThreadEvent* e) {
  printf("starting new thread Event \n");
  const NewThreadInfo* const _info = e->getNewThreadInfo();
  const TRD_ID newThreadID = _info->childThread->threadId;

  // get information about the thread's head function
  TIME create_time = _info->startTime;
  last_thread_start_time = create_time;

  if (stacks->bottomThreadIndex() > -1) {
    std::shared_ptr<thread_frame_t> bottom_thread_frame = stacks->bottomThread();
    TIME strand_start = std::max(std::max(last_function_return_time, last_thread_end_time), last_function_call_time);
    TIME local_work = static_cast<TIME> (create_time - strand_start);
    if (strand_start == 0)
      local_work = static_cast<TIME>(0);
    printf("using local work of %llu in new thread event \n", static_cast<unsigned long long>(local_work));
    assert(local_work >= 0);
    bottom_thread_frame->local_continuation += local_work;
    std::shared_ptr<function_frame_t> bottom_function_frame = stacks->bottomFunction();
    bottom_function_frame->local_work += local_work;
  }

  std::shared_ptr<thread_frame_t> new_thread_frame = 
                            stacks->thread_push(stacks->bottomFunctionIndex());
  new_thread_frame->thread = newThreadID;


  printf("ending new thread Event \n");
}

void ParasiteTool::syncOperations() {

  printf("starting sync operations \n");
  std::shared_ptr<thread_frame_t> bottom_thread_frame(stacks->bottomThread());
  // these operations only happen at a sync - after the thread's last child has
  // joined 
  std::shared_ptr<function_frame_t> bottom_function_frame(stacks->bottomFunction());
  bottom_function_frame->running_span += bottom_thread_frame->local_continuation;
  bottom_function_frame->running_lock_span += bottom_thread_frame->local_lock_span;

  // If critical path goes through spawned child
  if (bottom_thread_frame->longest_child_span > bottom_function_frame->running_span) {
    bottom_thread_frame->prefix_span += bottom_thread_frame->longest_child_span;
    bottom_thread_frame->lock_span += lock_span_end_time - lock_span_start_time;
    assert(bottom_thread_frame->lock_span == 0);
    assert(bottom_thread_frame->longest_child_lock_span == 0);
    bottom_thread_frame->prefix_span += bottom_thread_frame->lock_span;
    bottom_thread_frame->prefix_span -= bottom_thread_frame->
                                        longest_child_lock_span;
    CallSiteHashtable prefix_table(bottom_thread_frame->prefix_table);
    prefix_table.add(&(bottom_thread_frame->longest_child_table));
    // local_span does not increase, because critical path goes 
    // through spawned child.
  } else {
    bottom_thread_frame->prefix_span += bottom_function_frame->running_span;
    // Critical path goes through continuation, which is local. Add
    // local_continuation to local_span.
    bottom_thread_frame->local_span += bottom_thread_frame->local_continuation;
    CallSiteHashtable prefix_table(bottom_thread_frame->prefix_table);
    prefix_table.add(&(bottom_thread_frame->continuation_table));
  }

    // reset longest child and continuation span variables
  bottom_thread_frame->longest_child_span = 0;
  bottom_thread_frame->longest_child_lock_span = 0;
  bottom_function_frame->running_span = 0;
  bottom_thread_frame->local_continuation = 0;

  printf("ending sync operations \n");
}

void ParasiteTool::Join(const JoinEvent* e) {

  // does nothing, as this needs to happen at the end of a thread
}

void ParasiteTool::Return(const ReturnEvent* e) {
  printf("starting return Event \n");
  const ReturnInfo* _info(e->getReturnInfo());
  TIME returnTime = _info->endTime;
  TIME local_work = static_cast<TIME>(returnTime - std::max(
                                                    std::max(last_function_call_time, 
                                                             last_function_return_time),
                                                   last_thread_end_time));
  assert(local_work >= 0);
  last_function_return_time = returnTime;
  // TIME local_work = last_function_return_time - last_function_call_time;
  
  printf("performing return operations for local work %llu \n", static_cast<unsigned long long>(local_work));

  std::shared_ptr<function_frame_t> returned_function_frame(stacks->bottomFunction());
  CALLSITE returning_call_site = returned_function_frame->call_site;
  returned_function_frame->local_work = local_work;
  TIME running_work = static_cast<TIME>(returned_function_frame->running_work + local_work);
  TIME running_span = static_cast<TIME>(returned_function_frame->running_span + local_work);
  TIME running_lock_span = static_cast<TIME>(returned_function_frame->running_lock_span + 
                                             returned_function_frame->local_lock_span);
  bool is_top_returning_function = returned_function_frame->is_top_call_site_function;

  std::shared_ptr<function_frame_t> parent_function_frame = stacks->bottomParentFunction();
  parent_function_frame->running_work += running_work;
  parent_function_frame->running_span += running_span;
  parent_function_frame->running_lock_span += running_lock_span;

  CallSiteHashtable work_table(stacks->work_table);
  CallSiteHashtable bottom_thread_continuation_table(stacks->
                                                    bottomThread()->
                                                    continuation_table);
  if (is_top_returning_function) {
    work_table.add_data(is_top_returning_function,
                              returning_call_site, 
                              running_work, running_span,
                              local_work, local_work);
    bottom_thread_continuation_table.add_data(
                              is_top_returning_function,
                              returning_call_site, 
                              running_work, running_span,
                              local_work, local_work);
  } else {
    work_table.add_local_data(is_top_returning_function,
                              returning_call_site, 
                              local_work, 
                              local_work);
    bottom_thread_continuation_table.add_local_data(is_top_returning_function,
                                                    returning_call_site,
                                                    local_work,
                                                    local_work);
  }

  stacks->function_pop();
  printf("ending return Event \n");
}

void ParasiteTool::ThreadEnd(const ThreadEndEvent* e) {
  printf("starting thread end Event \n");
  const ThreadEndInfo* _info(e->getThreadEndInfo());
  TIME threadEndTime = _info->endTime;

  last_thread_end_time = threadEndTime;
  last_thread_runtime = static_cast<TIME>(last_thread_end_time - last_thread_start_time);

  TIME local_work = static_cast<TIME>(threadEndTime - last_function_return_time);
  printf("local work in thread end event is %llu \n", static_cast<unsigned long long>(local_work));
  assert(local_work >= 0);
  // The sync happens at the thread end, which is correct, because
  // all child threads must end anyway at the end of the thread.
  // However, this operation is unnecessary for the bottom thread reached.
  if (stacks->bottomThreadIndex() != stacks->highest_thread_index)
    syncOperations();

  std::shared_ptr<thread_frame_t> parent_thread_frame(stacks->bottomParentThread());
  std::shared_ptr<thread_frame_t> ending_thread_frame(stacks->bottomThread());
  std::shared_ptr<function_frame_t> current_function_frame(stacks->bottomFunction());

  CallSiteHashtable work_table(stacks->work_table);
  CallSiteHashtable bottom_thread_prefix_table(stacks->bottomThread()->prefix_table);
  bool is_top_call_site_function = current_function_frame->is_top_call_site_function;

  if (is_top_call_site_function) {
    work_table.add_data(current_function_frame->
                                      is_top_call_site_function,
                                      current_function_frame->call_site, 
                                      current_function_frame->running_work, 
                                      ending_thread_frame->prefix_span,
                                      current_function_frame->local_work, 
                                      ending_thread_frame->local_span);

    bottom_thread_prefix_table.add_data(
                          current_function_frame->is_top_call_site_function,
                          current_function_frame->call_site, 
                          current_function_frame->running_work, 
                          ending_thread_frame->prefix_span,
                          current_function_frame->local_work, 
                          ending_thread_frame->local_span);
  } else {
    work_table.add_local_data(current_function_frame->is_top_call_site_function,
                              current_function_frame->call_site, 
                              current_function_frame->local_work, 
                              ending_thread_frame->local_span);
    
    bottom_thread_prefix_table.add_local_data(
                              current_function_frame->is_top_call_site_function,
                              current_function_frame->call_site, 
                              current_function_frame->local_work, 
                              ending_thread_frame->local_span);
  }

  // Main function thread ends here 
  if (stacks->bottomThreadIndex() == 0) {
    printf("ENDING MAIN THREAD \n");
    return;
  }

  // if the ending thread is the longest child encountered so far
  if (current_function_frame->running_span + ending_thread_frame->local_continuation  
                                           > ending_thread_frame->longest_child_span) {

    parent_thread_frame->prefix_span += current_function_frame->running_span;
    parent_thread_frame->local_span += parent_thread_frame->local_continuation;

    CallSiteHashtable prefix_table(parent_thread_frame->prefix_table);
    prefix_table.add(&(parent_thread_frame->continuation_table));

    // Save old bottom thread frame tables in 
    // parent frame's longest child variable.
    parent_thread_frame->longest_child_span = ending_thread_frame->
                                              prefix_span;
    parent_thread_frame->longest_child_table.clear();

    CallSiteHashtable temp_hashtable(parent_thread_frame->longest_child_table);
    parent_thread_frame->longest_child_table = ending_thread_frame->prefix_table;
    ending_thread_frame->prefix_table = temp_hashtable;

    ending_thread_frame->longest_child_table.clear();
    ending_thread_frame->continuation_table.clear();
  }

  else {
      ending_thread_frame->prefix_table.clear();
      ending_thread_frame->longest_child_table.clear();
      ending_thread_frame->continuation_table.clear();
  }

  // pop the thread off the stack last, 
  // because the pop operation destroys the frame
  stacks->thread_pop();
  printf("ending non-main thread end Event \n");
}

void ParasiteTool::Acquire(const AcquireEvent* e) {

  const AcquireInfo* _info(e->getAcquireInfo());
  std::shared_ptr<ShadowLock> acquiredLock(_info->lock);

  // TIME acquireTime = e->acquireTime;
	// acquiredLock->last_acquire_time = e->acquireTime;
  TIME acquire_time = static_cast<TIME>(0);

  if ((acquire_time - last_thread_runtime) < lock_span_start_time)
    lock_span_start_time = acquire_time;
  else 
    lock_span_start_time += last_thread_runtime;

  // acquiredLock->last_acquire_time = static_cast<TIME>0;
  lock_span_start_time = static_cast<TIME>(0);

  unsigned int lockId = acquiredLock->lockId;

  if (lock_hashtable.count(lockId)) {
    lock_hashtable.at(lockId) = stacks->bottomFunctionIndex();
  } else {
    std::pair<unsigned int, int> newPair(lockId, stacks->bottomFunctionIndex());
    lock_hashtable.insert(newPair);
  }

}

void ParasiteTool::Release(const ReleaseEvent* e) {

  const ReleaseInfo* _info(e->getReleaseInfo());
	std::shared_ptr<ShadowLock> releasedLock(_info->lock);
  // release_time = e->releaseTime;
  TIME release_time = static_cast<TIME>(0);

  TIME lock_span = static_cast<TIME>(0);
  // TIME lock_span = release_time - releasedLock->last_acquire_time;

  // unsigned int lockId = releasedLock->lockId;
  unsigned int lockId = (unsigned int) 0;

  int unlocked_function_index = lock_hashtable.at(lockId);
  std::shared_ptr<function_frame_t> unlocked_function_frame(stacks->functionAt(unlocked_function_index));
  unlocked_function_frame->local_lock_span += lock_span;

  if ((release_time - last_thread_runtime) > lock_span_end_time)
    lock_span_start_time = release_time;
  else 
    lock_span_start_time += last_thread_runtime;
}

void ParasiteTool::Access(const AccessEvent* e) {
  printf("ERROR: Parasite Tool does not implement access event");
  return;
}




