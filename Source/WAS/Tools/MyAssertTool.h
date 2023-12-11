#pragma once
#include "Tools/Debug.h"

#define EXPAND_MACRO(x) x
#define STRINGIFY_MACRO(x) #x

#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)

#define ASSERT(X,MSG)								\
		if (!(X))									\
		{											\
			Debug::LogErr(MSG);						\
		}
//-----------------------------------------------------------------------------------------------------------------
#define ASSERT_BREAK(X, MSG)					\
		if (!(X))								\
		{										\
			Debug::LogErr(MSG);					\
			UE_DEBUG_BREAK();					\
		}										\
//-----------------------------------------------------------------------------------------------------------------
#define ASSERT_RETURN(X,MSG)						\
		if (!(X))									\
		{											\
			Debug::LogErr(MSG);						\
			return;									\
		}


#else 

#define ASSERT_POINTER (PTR,SIZE)
#define ASSERT_MSG(MSG)
#define ASSERT(X, MSG)
#define ASSERT_BREAK(X, MSG)
#define ASSERT_RETURN(X,MSG)
#endif
