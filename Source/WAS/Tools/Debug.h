// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
PRAGMA_ENABLE_OPTIMIZATION

#include "CoreMinimal.h"
#include <string>
#include <stdlib.h>


DEFINE_LOG_CATEGORY_STATIC(LogDebug, Warning, All);



class Debug
{

public:
	Debug() = delete;
	Debug(const Debug&) = delete;
	Debug(Debug&&) = delete;
	Debug& operator=(const Debug&) = delete;
	Debug& operator=(Debug&&) = delete;

public:

#if (UE_BUILD_SHIPPING || UE_BUILD_TEST)

	template <typename... Args>
	inline static void Log(const char* Format, const Args&... args) {}

	template <typename... Args>
	inline static void LogErr(const char* Format, const Args&... args) {}

	template <typename... Args>
	inline static void LogWarn(const char* Format, const Args&... args) {}

	template <typename... Args>
	inline static void LogWarnUI(const char* Format, const Args&... args) {}

	inline static void LogWarn(const char* name, const FVector& vector) {}

	inline static void LogWarn(const char* name, const bool& b) {}

	template <typename... Args>
	inline static void LogProfile(const char* Format, const Args&... args) {}

	FORCEINLINE static void DrawText(const int32& key = 1, const FString& msg = "", const float& displayTime = -1.0f, const FColor& color = FColor::Green) {}

	FORCEINLINE static void DrawText(const FString& msg = "", const float& displayTime = -1.0f, const FColor& color = FColor::Green) {}

#else
	template <typename... Args>
	inline static void Log(const char* Format, const Args&... args)
	{
		InternalLog(ELogVerbosity::Log, Format, args...);
	}

	template <typename... Args>
	inline static void LogErr(const char* Format, const Args&... args)
	{
		InternalLog(ELogVerbosity::Error, Format, args...);
	}

	template <typename... Args>
	inline static void LogWarn(const char* Format, const Args&... args)
	{
		InternalLog(ELogVerbosity::Warning, Format, args...);
	}

	template <typename... Args>
	inline static void LogWarnUI(const char* Format, const Args&... args)
	{
		InternalLog(ELogVerbosity::Warning, Format, args...);
		// TODO: sent message to UI 
	}

	inline static void LogWarn(const char* name, const FVector& vector)
	{
		std::string format = name + std::string(": %f %f %f");
		InternalLog(ELogVerbosity::Warning, format.c_str(), vector.X, vector.Y, vector.Z);
	}

	inline static void LogWarn(const char* name, const bool& b)
	{
		std::string value = b ? "true" : "false";
		std::string format = name + value;
		InternalLog(ELogVerbosity::Warning, format.c_str());
	}

	// Used to log profile information and measure performance
	template <typename... Args>
	inline static void LogProfile(const char* Format, const Args&... args)
	{
		InternalLog(ELogVerbosity::Warning, Format, args...);
	}

	FORCEINLINE static void DrawText(const int32& key = 1, const FString& msg = "", const float& displayTime = -1.0f, const FColor& color = FColor::Green)
	{
		GEngine->AddOnScreenDebugMessage(key, displayTime, color, msg);
	}

	FORCEINLINE static void DrawText(const FString& msg = "", const float& displayTime = -1.0f, const FColor& color = FColor::Green)
	{
		GEngine->AddOnScreenDebugMessage(m_KeyGenerator++, displayTime, color, msg);
	}

private:
	static void StrCopy(const char* src, TCHAR* dest, size_t size)
	{
		for (int i = 0; i < size; i++)
		{
			dest[i] = src[i];
		}

		dest[size] = '\0';
	}

	template <typename...Args>
	FORCEINLINE static void InternalLog(ELogVerbosity::Type verbosity, const char* format, const Args&... args)
	{
		const int32 size = strlen(format) + 1;
		if (size > 4096)
		{
			UE_LOG(LogDebug, Error, TEXT("Can not Print too big string buffer"));
			UE_DEBUG_BREAK();
			return;
		}

		{
			TCHAR text[4096];
			StrCopy(format, text, size);
			FMsg::Logf(nullptr, 0, LogDebug.GetCategoryName(), verbosity, text, args...);
		}

	}

#define LOG_WARN(MSG, ...) UE_LOG(LogTemp,Warning,TEXT(MSG), __VA_ARGS__)

#endif

	inline static int m_KeyGenerator = 0;
};

