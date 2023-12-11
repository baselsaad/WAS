#include "Voicelines.h"
#include "WAS.h"
#include "Tools/Debug.h"
#include "Tools/MyAssertTool.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"


#include <Kismet/GameplayStatics.h>
#include <Engine/World.h>
#include <AudioDeviceManager.h>


AVoicelines::AVoicelines()
{
	PrimaryActorTick.bCanEverTick = true;

	m_AudioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
	m_AudioComp->SetupAttachment(GetRootComponent());
}

static TSoftObjectPtr<AVoicelines> VoicelinesInstance = nullptr;

AVoicelines* AVoicelines::Get()
{
	if (VoicelinesInstance == nullptr)
	{
		Debug::LogWarn("VoicelinesInstance is null!!");
	}

	return VoicelinesInstance.Get();
}

void AVoicelines::BeginPlay()
{
	Super::BeginPlay();

	VoicelinesInstance.Reset();
	VoicelinesInstance = this;
	lastaudioComponent = NULL;
}

std::string eventToString(enum EEventType event)
{
	switch (event)
	{
		case EEventType::ORDERPUZZLE:
		return "orderpuzzle";
		case EEventType::BASEPUZZLE:
		return "basepuzzle";
		case EEventType::CARPUZZLE:
		return "carpuzzle";
		case EEventType::KEYPUZZLE:
		return "keypuzzle";
		case EEventType::WALKING:
		return "walking";
		case EEventType::OXYGEN:
		return "oxygen";
		case EEventType::PILL:
		return "pill";
		default:
		return "";
	}
}

std::string eventProgressToString(enum EEventProgress eventProgress)
{
	switch (eventProgress)
	{
		case EEventProgress::START:
		return "start";
		case EEventProgress::ONGOING:
		return "ongoing";
		case EEventProgress::END:
		return "end";
		case EEventProgress::NUMBER:
		return "number";
		default:
		return "";
	}
}


void AVoicelines::PlayVoiceLines(EEventType eventType, EEventProgress eventProgress)
{
	TPair<enum EEventType, enum EEventProgress> eventPair(eventType, eventProgress);

	if (!voiceLineCounters.Contains(eventPair))
	{
		voiceLineCounters.Add(eventPair, 0); // Initialize counter to 0 for new event and event progress combination
	}

	int counter = voiceLineCounters[eventPair];

	std::string voiceLineName = "/Game/Sound/" + eventToString(eventType) + "_" + eventProgressToString(eventProgress) + "_" + std::to_string(counter + 1) + "_Cue";
	const TCHAR* voiceLine = ANSI_TO_TCHAR(voiceLineName.c_str());
	Debug::LogWarn("Sound %s", voiceLine);

	if (m_WasPlayedSounds.Contains(voiceLine) && m_CurrentPlayedPriority > Priority::Low)
	{
		Debug::LogWarn("not play Sound %s", voiceLine);
		return;
	}

	m_WasPlayedSounds.AddUnique(voiceLine);

	if (PlayVoiceLine(voiceLine))
	{
		voiceLineCounters[eventPair] = counter + 1; // Increment the counter for the specific event and event progress combination
	}
}

void AVoicelines::PlayVoiceLinesIfPossible(EEventType event, EEventProgress eventProgress, Priority priority)
{
	if (!m_AudioComp->IsPlaying())
	{
		m_CurrentPlayedPriority = priority;
		PlayVoiceLines(event, eventProgress);
	}
	else
	{
		if (m_CurrentPlayedPriority < priority)
		{
			m_CurrentPlayedPriority = priority;
			PlayVoiceLines(event, eventProgress);
		}
	}
}

void AVoicelines::PlayVoiceLinesNow(EEventType event, EEventProgress eventProgress, Priority priority)
{
	m_CurrentPlayedPriority = priority;
	PlayVoiceLines(event, eventProgress);
}

void AVoicelines::PlayVoiceLinePill(const TCHAR* path)
{
	m_CurrentPlayedPriority = Priority::High;
	PlayVoiceLine(path);
}

void AVoicelines::StopAll()
{
	if (m_AudioComp && m_AudioComp->IsPlaying())
	{
		m_AudioComp->Stop();
	}
}

bool AVoicelines::PlayVoiceLine(const TCHAR* voiceLine)
{
	// Create an instance of UAudioComponent and assign the sound cue to it
	//UAudioComponent* audioComponent = NewObject<UAudioComponent>(GetWorld());
	//audioComponent->SetupAttachment(GetRootComponent());
	//audioComponent->RegisterComponent();

	if (m_AudioComp == nullptr)
	{
		m_AudioComp = NewObject<UAudioComponent>(GetWorld());
		m_AudioComp->SetupAttachment(GetRootComponent());
		m_AudioComp->RegisterComponent();
	}

	USoundCue* SoundCue = LoadObject<USoundCue>(nullptr, voiceLine);
	if (SoundCue)
	{
		m_AudioComp->SetSound(SoundCue);
		m_AudioComp->VolumeMultiplier = 100.0f;
	}
	else
	{
		Debug::LogErr("Failed to Load");
		return false;
	}

	if (lastaudioComponent == NULL)
	{
		m_AudioComp->Play();
		m_AudioComp->VolumeMultiplier = 100.0f;
		lastaudioComponent = m_AudioComp;
		return true;
	}
	else if (lastaudioComponent && lastaudioComponent->IsValidLowLevel() && !lastaudioComponent->IsPlaying())
	{
		lastaudioComponent->Stop();
		m_AudioComp->Play();
		m_AudioComp->VolumeMultiplier = 100.0f;
		lastaudioComponent = m_AudioComp;
		return true;
	}
	return false;
}