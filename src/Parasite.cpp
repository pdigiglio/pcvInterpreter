
call_site_profile_t* create_callsite_profile(CALLSITE call_site,
						 bool is_top_function,
                         double work, double span,
                         double local_work, double local_span) {


	call_site_profile_t* new_profile = new call_site_profile_t();

	new_profile->call_site = call_site;
	new_profile->local_count = 0;
  	new_profile->local_work = local_work;
  	new_profile->local_span = local_span;

  	new_profile->count = 1;

  	if (is_top_function)
		new_profile->top_count = 1;
	else
		new_profile->top_count = 0;

  	new_profile->work = work;
  	new_profile->span = span;
  	new_profile->top_work = work;
  	new_profile->top_span = span;
}


// add entries in profile_to_add to entries in profile
void combine_callsite_profile_entries(call_site_profile_t *profile,
                     const call_site_profile_t *profile_to_add) {

  profile->local_work += profile_add->local_work;
  profile->local_span += profile_add->local_span;
  profile->local_count += profile_add->local_count;
  profile->work += profile_add->work;
  profile->span += profile_add->span;
  profile->count += profile_add->count;
  profile->top_work += profile_add->top_work;
  profile->top_span += profile_add->top_span;
  profile->top_count += profile_add->top_count;
}

// adds right hashtable into left hashtable
call_site_hashtable_t* add_call_site_hashtables(call_site_hashtable_t *hashtable, 
												call_site_hashtable_t *hashtable_to_add) {

	for (auto const &it : hashtable_to_add) {

		CALLSITE key = it.first;
		call_site_profile_t* profile_to_add = hashtable_to_add[key];

		// if call site profile being added exists in both hashtables, combine them
		if (hashtable.count(key)) {

			call_site_profile_t* profile = hashtable[key];
			combine_callsite_profile_entries(profile, profile_to_add);
		}

		// if not, insert added call site profile into the hashtable
		else 
			hashtable.insert(std::pair<CALLSITE, call_site_profile_t>(key, profile_to_add));
	}

	return left;
}

// add given call site profile data to the hashtable 
bool add_to_call_site_hashtable(call_site_hashtable_t *hashtable,
                         bool is_top_function,
                         CALLSITE call_site,
                         double work, double span,
                         double local_work, double local_span) {

	if (hashtable.count(call_site)) {

		call_site_profile_t* profile = hashtable[call_site];

		profile->local_count += 1;
  		profile->local_work += local_work;
  		profile->local_span += local_span;
  	
  		profile->count += 1;
  		profile->work += work;
  		profile->span += span;

  		if (is_top_function) {
			profile->top_count += 1;
			profile->top_work += work;
  			profile->top_span += span;
  		}
	}

	else {

		call_site_profile_t* new_profile = create_callsite_profile(call_site, work, span, local_work, local_span);
		hashtable.insert(std::pair<CALLSITE, call_site_profile_t>(key, new_profile));
	}
}

// add given call site profile data (only for local variables) to the hashtable 
bool add_to_call_site_hashtable_local(call_site_hashtable_t *hashtable,
                               CALLSITE call_site,
                               int local_work, int local_span) {

	if (hashtable.count(call_site)) {

		call_site_profile_t* profile = hashtable[call_site];

		profile->local_count += 1;
  		profile->local_work += local_work;
  		profile->local_span += local_span;
	}

	else {

		call_site_profile_t* new_profile = create_callsite_profile(call_site, work, span, local_work, local_span);
		hashtable.insert(std::pair<CALLSITE, call_site_profile_t>(key, new_profile));
	}
}