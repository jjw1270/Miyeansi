// Copyright (c) 2026 장윤제. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "VNCondition.h"
#include "VNStateChange.h"
#include "VNStoryState.h"
#include "VNConditionEvaluator.generated.h"

UCLASS()
class VISUALNOVEL_API UVNConditionEvaluator : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "VisualNovel|Condition")
	static bool EvaluateCondition(const FVNStoryState& _story_state, const FVNCondition& _condition);

	UFUNCTION(BlueprintPure, Category = "VisualNovel|Condition")
	static bool EvaluateConditionSet(const FVNStoryState& _story_state, const FVNConditionSet& _condition_set);

	UFUNCTION(BlueprintCallable, Category = "VisualNovel|State")
	static bool ApplyStateChange(UPARAM(ref) FVNStoryState& _story_state, const FVNStateChange& _state_change);

	UFUNCTION(BlueprintCallable, Category = "VisualNovel|State")
	static bool ApplyStateChanges(UPARAM(ref) FVNStoryState& _story_state, const TArray<FVNStateChange>& _state_changes);
};