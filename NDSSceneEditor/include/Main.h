#pragma once

// Suppress thrid party libry warnings
#pragma region DisableThirdPartyWarn

	#define CNN_UNREFERENCED_PARAMETER(x) (void)(x)

	#if defined _WIN32 && !defined(__MINGW32__)
	#define CNN_WINDOWS
	#endif

	#if defined(_MSC_VER)
	#define CNN_MUST_INLINE __forceinline
	#elif defined(__GNUC__) || defined(__clang__) || defined(__ICC)
	#define CNN_MUST_INLINE __attribute__((always_inline)) inline
	#else
	#define CNN_MUST_INLINE inline
	#endif

#pragma endregion DisableThirdPartyWarn