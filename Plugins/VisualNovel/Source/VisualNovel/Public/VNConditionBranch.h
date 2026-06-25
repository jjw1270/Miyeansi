// Copyright (c) 2026 장윤제. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "StoryBranchBase.h"
#include "VNCondition.h"
#include "VNConditionBranch.generated.h"

USTRUCT(BlueprintType, meta = (ShortToolTip = "조건 분기 출력 하나"))
struct VISUALNOVEL_API FVNConditionBranchCase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel", meta = (ToolTip = "조건이 통과했을 때 사용할 Branch 출력 핀 표시명"))
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel", meta = (ToolTip = "이 출력 핀을 선택하기 위해 만족해야 하는 조건 묶음"))
	FVNConditionSet ConditionSet;
};

UCLASS(Blueprintable, EditInlineNew, meta = (DisplayName = "VN Condition Branch"))
class VISUALNOVEL_API UVNConditionBranch : public UStoryBranchBase
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VisualNovel", meta = (AllowPrivateAccess = "true", ToolTip = "위에서부터 순서대로 평가할 조건 분기 목록. 처음 통과한 항목의 출력 인덱스를 선택"))
	TArray<FVNConditionBranchCase> Cases;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VisualNovel", meta = (AllowPrivateAccess = "true", ToolTip = "어떤 조건도 통과하지 못했을 때 사용할 마지막 출력 핀 표시명"))
	FText DefaultOutputName;

public:
	UVNConditionBranch();

	virtual void PostInitProperties() override;
	virtual void PostLoad() override;
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& _property_changed_event) override;
#endif

	virtual int32 SelectNextIndex_Implementation(int32 _next_count) const override;

public:
	UFUNCTION(BlueprintPure, Category = "VisualNovel|Branch", meta = (ToolTip = "지정한 StoryState를 기준으로 선택될 Branch 출력 인덱스를 계산"))
	int32 SelectNextIndexFromStoryState(const FVNStoryState& _story_state, int32 _next_count) const;

	UFUNCTION(BlueprintPure, Category = "VisualNovel|Branch", meta = (ToolTip = "조건이 모두 실패했을 때 사용하는 Default 출력 인덱스"))
	int32 GetDefaultOutputIndex() const { return Cases.Num(); }

	const TArray<FVNConditionBranchCase>& GetCases() const { return Cases; }

	void SetCases(const TArray<FVNConditionBranchCase>& _cases);
	void SetDefaultOutputName(const FText& _display_name);

private:
	void RebuildBranchOutputs();
	FText GetCaseDisplayName(const FVNConditionBranchCase& _case, int32 _case_index) const;
	int32 GetSafeDefaultIndex(int32 _next_count) const;
};
