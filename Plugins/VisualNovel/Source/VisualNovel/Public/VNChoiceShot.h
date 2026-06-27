// Copyright (c) 2026 장윤제. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "StoryShotBase.h"
#include "VNCondition.h"
#include "VNStateChange.h"
#include "VNStoryState.h"
#include "VNChoiceShot.generated.h"

USTRUCT(BlueprintType, meta = (ShortToolTip = "VN 선택지 하나"))
struct VISUALNOVEL_API FVNChoiceOption
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel", meta = (ToolTip = "선택 결과 ID. 예: OptionA, OptionB, Stay"))
	FName ChoiceID = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel", meta = (ToolTip = "플레이어에게 표시할 선택지 문구"))
	FText Text;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel", meta = (ToolTip = "선택지가 목록에 보일 조건"))
	FVNConditionSet ShowCond;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel", meta = (ToolTip = "선택 가능 조건. 실패하면 비활성 상태로 표시"))
	FVNConditionSet EnableCond;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel", meta = (ToolTip = "비활성 선택지에 표시할 사유 문구"))
	FText DisabledText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel", meta = (ToolTip = "선택 직후 적용할 상태 변경"))
	TArray<FVNStateChange> OnSelect;
};

USTRUCT(BlueprintType, meta = (ShortToolTip = "평가된 VN 선택지 표시 상태"))
struct VISUALNOVEL_API FVNChoiceOptionState
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VisualNovel", meta = (ToolTip = "원본 Options 배열의 인덱스"))
	int32 OptionIndex = INDEX_NONE;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VisualNovel", meta = (ToolTip = "선택 결과 ID"))
	FName ChoiceID = NAME_None;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VisualNovel", meta = (ToolTip = "표시할 선택지 문구"))
	FText Text;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VisualNovel", meta = (ToolTip = "선택 가능한지 여부"))
	bool IsEnabled = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VisualNovel", meta = (ToolTip = "비활성 사유 문구"))
	FText DisabledText;
};

UCLASS(Blueprintable, EditInlineNew, meta = (DisplayName = "VN Choice Shot"))
class VISUALNOVEL_API UVNChoiceShot : public UStoryShotBase
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VisualNovel", meta = (AllowPrivateAccess = "true", MultiLine = true, ToolTip = "선택지 위에 표시할 질문 또는 상황 문구"))
	FText _PromptText;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VisualNovel", meta = (AllowPrivateAccess = "true", ToolTip = "선택한 ChoiceID를 저장할 NameMap 키. 비우면 결과 저장 생략"))
	FName _ResultKey = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VisualNovel", meta = (AllowPrivateAccess = "true", ToolTip = "표시/선택 가능한 선택지 목록"))
	TArray<FVNChoiceOption> _Options;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VisualNovel", meta = (AllowPrivateAccess = "true", ToolTip = "선택지 표시 전에 적용할 상태 변경"))
	TArray<FVNStateChange> _OnEnter;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VisualNovel", meta = (AllowPrivateAccess = "true", ToolTip = "선택 완료 뒤 공통 적용할 상태 변경"))
	TArray<FVNStateChange> _OnComplete;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VisualNovel", meta = (AllowPrivateAccess = "true", ToolTip = "표시 가능한 활성 선택지가 1개뿐이면 자동 선택할지 여부"))
	bool _ShouldAutoSelectSingleOption = false;

	UPROPERTY(Transient)
	bool _HasSelected = false;

	UPROPERTY(Transient)
	FName _SelectedChoiceID = NAME_None;

protected:
	virtual void OnEnterShot_Implementation() override;

public:
	UFUNCTION(BlueprintPure, Category = "VisualNovel|Choice", meta = (ToolTip = "지정한 StoryState 기준으로 표시할 선택지와 활성 상태를 계산"))
	TArray<FVNChoiceOptionState> GetVisibleOptionsFromStoryState(const FVNStoryState& _story_state) const;

	UFUNCTION(BlueprintCallable, Category = "VisualNovel|Choice", meta = (ToolTip = "지정한 StoryState 기준으로 ChoiceShot을 시작하고 OnEnter를 적용"))
	bool BeginChoiceInStoryState(UPARAM(ref) FVNStoryState& _story_state);

	UFUNCTION(BlueprintCallable, Category = "VisualNovel|Choice", meta = (ToolTip = "현재 StoryState Subsystem 기준으로 원본 Options 인덱스의 선택지를 선택"))
	bool SelectOption(int32 _option_index);

	UFUNCTION(BlueprintCallable, Category = "VisualNovel|Choice", meta = (ToolTip = "지정한 StoryState 기준으로 원본 Options 인덱스의 선택지를 선택"))
	bool SelectOptionInStoryState(int32 _option_index, UPARAM(ref) FVNStoryState& _story_state);

	UFUNCTION(BlueprintCallable, Category = "VisualNovel|Choice", meta = (ToolTip = "지정한 ChoiceID를 가진 선택지를 선택"))
	bool SelectOptionByChoiceID(FName _choice_id);

	UFUNCTION(BlueprintCallable, Category = "VisualNovel|Choice", meta = (ToolTip = "지정한 StoryState 기준으로 ChoiceID 선택지를 선택"))
	bool SelectOptionByChoiceIDInStoryState(FName _choice_id, UPARAM(ref) FVNStoryState& _story_state);

	UFUNCTION(BlueprintPure, Category = "VisualNovel|Choice")
	const TArray<FVNChoiceOption>& GetOptions() const { return _Options; }

	UFUNCTION(BlueprintPure, Category = "VisualNovel|Choice")
	bool HasSelected() const { return _HasSelected; }

	UFUNCTION(BlueprintPure, Category = "VisualNovel|Choice")
	FName GetSelectedChoiceID() const { return _SelectedChoiceID; }

	void SetOptions(const TArray<FVNChoiceOption>& _options) { _Options = _options; }
	void SetResultKey(FName _result_key) { _ResultKey = _result_key; }
	void SetOnEnter(const TArray<FVNStateChange>& _state_changes) { _OnEnter = _state_changes; }
	void SetOnComplete(const TArray<FVNStateChange>& _state_changes) { _OnComplete = _state_changes; }
	void SetShouldAutoSelectSingleOption(bool _should_auto_select) { _ShouldAutoSelectSingleOption = _should_auto_select; }

private:
	bool ApplyOnEnterToSubsystem() const;
	bool SelectSingleVisibleOptionIfNeeded();
	bool SelectOptionWithSubsystem(int32 _option_index);
	bool IsOptionVisible(const FVNStoryState& _story_state, const FVNChoiceOption& _option) const;
	bool IsOptionEnabled(const FVNStoryState& _story_state, const FVNChoiceOption& _option) const;
};
