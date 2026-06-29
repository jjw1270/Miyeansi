// Copyright (c) 2026 장윤제. All rights reserved.

#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "Engine/GameInstance.h"
#include "VNStoryStateSubsystem.h"

namespace VNStoryStateSubsystemTests
{
	FVNStateChange MakeIntChange(FName _key, EVNStateOp _op, int32 _value)
	{
		FVNStateChange state_change;
		state_change.Domain = EVNStateDomain::Int;
		state_change.Key = _key;
		state_change.Op = _op;
		state_change.IntValue = _value;
		return state_change;
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FVNStoryStateSubsystemTest,
	"VisualNovel.StoryStateSubsystem.StateAccess",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FVNStoryStateSubsystemTest::RunTest(const FString& _parameters)
{
	using namespace VNStoryStateSubsystemTests;

	const FName trust_key(TEXT("TrustValue"));

	UGameInstance* game_instance = NewObject<UGameInstance>();
	UVNStoryStateSubsystem* story_state_subsystem = NewObject<UVNStoryStateSubsystem>(game_instance);
	TestNotNull(TEXT("StoryState subsystem object can be constructed for API test"), story_state_subsystem);
	if (story_state_subsystem == nullptr)
	{
		return false;
	}

	FVNStoryState story_state;
	story_state.IntMap.Add(trust_key, 3);
	story_state_subsystem->SetStoryState(story_state);
	TestEqual(TEXT("SetStoryState stores the state"), story_state_subsystem->GetStoryStateRef().IntMap.FindRef(trust_key), 3);

	TestTrue(TEXT("Subsystem applies a state change"), story_state_subsystem->ApplyStateChange(MakeIntChange(trust_key, EVNStateOp::Add, 2)));
	TestEqual(TEXT("State change modifies the stored state"), story_state_subsystem->GetStoryStateRef().IntMap.FindRef(trust_key), 5);

	story_state_subsystem->ResetStoryState();
	TestFalse(TEXT("ResetStoryState clears previous state"), story_state_subsystem->GetStoryStateRef().IntMap.Contains(trust_key));

	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
