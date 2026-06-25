// Copyright (c) 2026 장윤제. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "VNStateChange.h"
#include "VNStoryState.h"
#include "VNStoryStateSubsystem.generated.h"

UCLASS(BlueprintType)
class VISUALNOVEL_API UVNStoryStateSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

private:
	UPROPERTY(Transient)
	FVNStoryState _StoryState;

public:
	UFUNCTION(BlueprintPure, Category = "VisualNovel|State", meta = (ToolTip = "현재 GameInstance에 보관 중인 VN 진행 상태를 반환"))
	FVNStoryState GetStoryState() const { return _StoryState; }

	const FVNStoryState& GetStoryStateRef() const { return _StoryState; }
	FVNStoryState& GetMutableStoryState() { return _StoryState; }

	UFUNCTION(BlueprintCallable, Category = "VisualNovel|State", meta = (ToolTip = "현재 VN 진행 상태를 지정한 값으로 교체"))
	void SetStoryState(const FVNStoryState& _story_state);

	UFUNCTION(BlueprintCallable, Category = "VisualNovel|State", meta = (ToolTip = "현재 VN 진행 상태를 기본값으로 초기화"))
	void ResetStoryState();

	UFUNCTION(BlueprintCallable, Category = "VisualNovel|State", meta = (ToolTip = "현재 VN 진행 상태에 상태 변경 명령 하나를 적용"))
	bool ApplyStateChange(const FVNStateChange& _state_change);

	UFUNCTION(BlueprintCallable, Category = "VisualNovel|State", meta = (ToolTip = "현재 VN 진행 상태에 상태 변경 명령 배열을 순서대로 적용"))
	bool ApplyStateChanges(const TArray<FVNStateChange>& _state_changes);
};
