/*
 * parasite.h
 *
 * Created on: June 16, 2016
 *      Author: knapp
 * 
 * Includes struct declarations for parasite profile, call site profile, call site end profile, and hashtables 
 * Includes method declarations for hasthable operations
 */


#ifndef PARASITE_H_
#define PARASITE_H_

#include <iostream>
#include <limits>
#include <map>
#include <stdbool.h>

#include "Types.h"
#include "ParasiteProfile.h"

class CallSiteHashtable {

	public:

		// adds in hashtable into right hashtable
		void add_in_hashtable(const std::unique_ptr<call_site_hashtable_t> added_hashtable);

		// adds work, span, local_work, and local_span tto profile for call_site
		void add_data_to_hashtable(bool is_top_function,
		                         CALLSITE call_site,
		                         double work, double span,
		                         double local_work, double local_span);

		// adds local work and span to profile for call_site
		void add_local_data_to_hashtable(CALLSITE call_site,
		                                 double local_work, double local_span);

		call_site_hashtable_t hashtable;

  	private:

  		CallSiteHashtable(const CallSiteHashtable&);
		CallSiteHashtable& operator=(const CallSiteHashtable&);


}

#endif // PARASITE_H_ 


