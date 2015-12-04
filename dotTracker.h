/**
 *
 *   @name  dotTracker.h
 *  @brief  
 *
 *   @date  12/03/15
 *
 * @author  Paolo Di Giglio (github.com/pdigiglio),
 *          <p.digiglio91@gmail.com>
 *
 */

#ifndef  dottracker_INC
#define  dottracker_INC

#include "Tool.h"
#include "Event.h"

/**
 * @class  DotTracker
 * @brief  
 */
class DotTracker: public Tool  {
	public:
        DotTracker ( const char* outFile = "out.dot" );
        ~DotTracker ();

		/* reimplement methods from Tool base class */
		void create(const Event* e) override;
		void join(const Event* e) override;
		void acquire(const Event* e) override;
		void release(const Event* e) override;
		void access(const Event* e) override;
		void call(const Event* e) override;

	private:
		const char* _outFile;
}; /* -----  end of class DotTracker  ----- */

#endif   /* ----- #ifndef dottracker_INC  ----- */
