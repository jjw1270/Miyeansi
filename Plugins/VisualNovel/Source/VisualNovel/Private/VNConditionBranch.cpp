// Copyright (c) 2026 장윤제. All rights reserved.

#include "VNConditionBranch.h"

#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "VNConditionEvaluator.h"
#include "VNStoryStateSubsystem.h"

#define LOCTEXT_NAMESPACE "VNConditionBranch"

UVNConditionBranch::UVNConditionBranch()
{
	_DefaultOutputName = LOCTEXT("DefaultOutputName", "Default");
	RebuildBranchOutputs();
}

void UVNConditionBranch::PostInitProperties()
{
	Super::PostInitProperties();
	RebuildBranchOutputs();
}

void UVNConditionBranch::PostLoad()
{
	Super::PostLoad();
	RebuildBranchOutputs();
}

#if WITH_EDITOR
void UVNConditionBranch::PostEditChangeProperty(FPropertyChangedEvent& _property_changed_event)
{
	Super::PostEditChangeProperty(_property_changed_event);
	RebuildBranchOutputs();
}
#endif

int32 UVNConditionBranch::SelectNextIndex_Implementation(int32 _next_count) const
{
	const UWorld* world = GetWorld();
	const UGameInstance* game_instance = IsValid(world) ? world->GetGameInstance() : nullptr;
	const UVNStoryStateSubsystem* story_state_subsystem = IsValid(game_instance) ? game_instance->GetSubsystem<UVNStoryStateSubsystem>() : nullptr;
	if (IsValid(story_state_subsystem) == false)
	{
		return GetSafeDefaultIndex(_next_count);
	}

	return SelectNextIndexFromStoryState(story_state_subsystem->GetStoryStateRef(), _next_count);
}

int32 UVNConditionBranch::SelectNextIndexFromStoryState(const FVNStoryState& _story_state, int32 _next_count) const
{
	if (_next_count <= 0)
	{
		return INDEX_NONE;
	}

	const int32 selectable_case_count = FMath::Min(_Cases.Num(), FMath::Max(0, _next_count - 1));
	for (int32 case_index = 0; case_index < selectable_case_count; ++case_index)
	{
		if (UVNConditionEvaluator::EvaluateConditionSet(_story_state, _Cases[case_index].ConditionSet))
		{
			return case_index;
		}
	}

	return GetSafeDefaultIndex(_next_count);
}

void UVNConditionBranch::SetCases(const TArray<FVNConditionBranchCase>& _cases)
{
	_Cases = _cases;
	RebuildBranchOutputs();
}

void UVNConditionBranch::SetDefaultOutputName(const FText& _display_name)
{
	_DefaultOutputName = _display_name;
	RebuildBranchOutputs();
}

void UVNConditionBranch::RebuildBranchOutputs()
{
	_BranchOutputs.Reset();
	_BranchOutputs.Reserve(_Cases.Num() + 1);

	for (int32 case_index = 0; case_index < _Cases.Num(); ++case_index)
	{
		FStoryBranchOutput output;
		output.DisplayName = GetCaseDisplayName(_Cases[case_index], case_index);
		_BranchOutputs.Add(output);
	}

	FStoryBranchOutput default_output;
	default_output.DisplayName = _DefaultOutputName.IsEmpty() ? LOCTEXT("DefaultOutputFallbackName", "Default") : _DefaultOutputName;
	_BranchOutputs.Add(default_output);
}

FText UVNConditionBranch::GetCaseDisplayName(const FVNConditionBranchCase& _case, int32 _case_index) const
{
	if (_case.DisplayName.IsEmpty() == false)
	{
		return _case.DisplayName;
	}

	return FText::Format(LOCTEXT("CaseOutputFallbackName", "Case {0}"), FText::AsNumber(_case_index));
}

int32 UVNConditionBranch::GetSafeDefaultIndex(int32 _next_count) const
{
	if (_next_count <= 0)
	{
		return INDEX_NONE;
	}

	return FMath::Clamp(_Cases.Num(), 0, _next_count - 1);
}

#undef LOCTEXT_NAMESPACE
