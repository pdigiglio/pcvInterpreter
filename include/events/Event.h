#ifndef EVENT_H_
#define EVENT_H_

#include "fwd/ShadowThread.h"

#include "Types.h"

#include <string>

enum class Events: unsigned char {
    NEWTHREAD = 0x1,  // 00000001
    THREADEND = 0x2,  // 00000010
    JOIN      = 0x4,  // 00000100
    ACQUIRE   = 0x8,  // 00001000
    RELEASE   = 0x10, // 00010000
    ACCESS    = 0x20, // 00100000
	CALL      = 0x40, // 01000000
    RETURN    = 0x80, // 10000000
    ALL       = 0xFF  // 11111111 
};

constexpr inline
Events operator| (Events A, Events B) noexcept {
    return static_cast<Events>(
        static_cast<std::underlying_type<Events>::type>(A) |
        static_cast<std::underlying_type<Events>::type>(B)   );
}

constexpr inline
Events operator& (Events A, Events B) noexcept {
    return static_cast<Events>(
        static_cast<std::underlying_type<Events>::type>(A) &
        static_cast<std::underlying_type<Events>::type>(B)   );
}

template <typename T>
constexpr inline
bool operator== (Events A, T b ) noexcept {
  return static_cast<bool>(
        static_cast<T>(
          static_cast<std::underlying_type<Events>::type>(A)) == b );
}

template <typename T>
constexpr inline
bool operator!= (Events A, T b ) noexcept {
	return !( A == b );
}

/// Converts a time in the format `YYYY-MM-DD|T|hh:mm:ss|.|CLOCK|Z| into 
/// a `TIME` variable (i.e. `clock_t`)
inline
const TIME timeStringToTime(const TIME_STRING& t) {
	int found = t.find_last_of('.');
	return static_cast<TIME>(atoi(t.substr(found + 1).c_str()));
}



/// Abstract Event
class Event {
public:
	Event(const ShadowThread *thread) :
		Thread_(thread) {};

	/// _Default_ destructor.
	virtual ~Event() = default;

	const ShadowThread* const getThread() const { return Thread_; };
	virtual Events getEventType() const = 0;

private:
	const ShadowThread* const Thread_;
};

#endif /* EVENT_H_ */
