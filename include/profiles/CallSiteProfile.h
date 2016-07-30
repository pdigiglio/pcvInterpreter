/**
 *
 *    @file  CallSiteProfile.h
 *   @brief  Declarations for the CallSiteProfile class
 *
 *    @date  06/17/16
 *  @author  Nathaniel Knapp (github.com/deknapp),
 *           <nathaniel.knapp@gmail.com>
 *
 */

#ifndef CALL_SITE_PROFILE_H_
#define CALL_SITE_PROFILE_H_

#include <unordered_map>
#include <memory>

#include "CallSiteSpanProfile.h"
#include "CallSiteWorkProfile.h"
#include "Types.h"

/**
*   @struct call_site_profile_t
*   @brief Work and span information describing a call site in the program.
*/

struct call_site_profile_t {

    /**
    *    @var call_site
    *    @brief The call site whose data is contained in the profile. 
    */
    CALLSITE call_site;

    /**
    *    @var work
    *    @brief Work of callsite. 
    */
    TIME work;

    /**
    *    @var span
    *    @brief Span of callsite. 
    */
    TIME span;

    /**
    *    @var parallelism
    *    @brief Parallelism of callsite.  
    */
    double parallelism;

    /**
    *    @var count
    *    @brief Number of invocations of callsite. 
    */
    int count;

    call_site_profile_t() : call_site(0), work(0), span(0), parallelism(0), count(0) {}  
};

class CallSiteProfile {
    
 public:
    explicit CallSiteProfile(std::shared_ptr<call_site_profile_t> profile);
    explicit CallSiteProfile(std::shared_ptr<call_site_span_profile_t>
                                                                 span_profile,
                                 std::shared_ptr<call_site_work_profile_t> 
                                                                 work_profile);

    ~CallSiteProfile();

    void print();

    /**
    *    @fn add_in_callsite_profile_entries(const std::shared_ptr<call_site_profile_t> profile_to_add)
    *    @brief Add the entries in profile_to_add to prof
    */
    void add_in_callsite_profile_entries(const std::shared_ptr<call_site_profile_t> profile_to_add);

    /**
    *    @fn init_callsite_profile(...)
    *    @brief Initialize prof with the information provided in function parameters
    */
    void init_callsite_profile(CALLSITE call_site, TIME work, TIME span);
    
    /**
    *    @var prof
    *    @brief The call_site_profile_t that the profile's methods act on. 
    */
    std::shared_ptr<call_site_profile_t> prof;

 private:
    CallSiteProfile(const CallSiteProfile&);
    CallSiteProfile& operator=(const CallSiteProfile&);
};



#endif  // CALL_SITE_PROFILE_H_