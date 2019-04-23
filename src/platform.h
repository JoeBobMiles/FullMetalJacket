#ifndef _PLATFORM_H_
#define _PLATFORM_H_

#ifdef DEBUG
static void Assert(bool, const char*);
#else
// TODO[joe] Make this do something?
// Right now this just gobbles Assert() calls where they appear, meaning that
// if the application fails for a reason that an Assert() would've caught, it
// will crash inexplicably. Which will frustrate the user.
#define Assert(F,M)
#endif

static void Abort(const char*);

#endif
