// Copyright (c) 2026 장윤제. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "StoryShotBase.h"
#include "VNItemIDs.h"
#include "VNStateChange.h"
#include "VNStoryState.h"
#include "VNDialogueShot.generated.h"

UENUM(BlueprintType, meta = (ShortToolTip = "VN 대사 줄 종류"))
enum class EVNDialogueLineKind : uint8
{
	Dialogue		UMETA(ToolTip = "캐릭터 이름이 표시되는 일반 대사"),
	Narration	UMETA(ToolTip = "이름 없이 출력되는 지문 또는 내레이션"),
	Monologue	UMETA(ToolTip = "주인공 독백"),
	System		UMETA(ToolTip = "날짜 전환, 안내, 디버그용 시스템 문구"),
};

USTRUCT(BlueprintType, meta = (ShortToolTip = "한 줄 동안 표시할 캐릭터 연출 큐"))
struct VISUALNOVEL_API FVNCharacterCue
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel", meta = (ToolTip = "화면에 표시할 캐릭터 Character 타입 ItemID"))
	FVNCharacterID CharacterID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel", meta = (ToolTip = "표정/의상/포즈 같은 캐릭터 스프라이트 리소스 ID"))
	FVNItemID CharacterSpriteID = FItemID::Zero;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel", meta = (ToolTip = "화면 배치 위치 ID. 예: Left, Center, Right"))
	FName PositionID = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel", meta = (ToolTip = "현재 대사 화자로 강조할지 여부"))
	bool HasFocus = false;
};

USTRUCT(BlueprintType, meta = (ShortToolTip = "VN 대사 한 줄"))
struct VISUALNOVEL_API FVNDialogueLine
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel", meta = (ToolTip = "대사/내레이션/독백/시스템 문구 구분"))
	EVNDialogueLineKind Kind = EVNDialogueLineKind::Dialogue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel", meta = (ToolTip = "일반 대사의 화자 Character 타입 ItemID. 내레이션은 비워도 됨"))
	FVNCharacterID SpeakerID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel", meta = (ToolTip = "표시 이름을 직접 덮어쓸 때 사용. 비우면 SpeakerID로 해석"))
	FText SpeakerName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel", meta = (MultiLine = true, ToolTip = "실제로 출력할 대사 또는 지문"))
	FText Text;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel", meta = (ToolTip = "이 줄에서 전환할 배경 리소스 ID. 비우면 유지"))
	FVNItemID BackgroundID = FItemID::Zero;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel", meta = (ToolTip = "이 줄에서 전환할 BGM 리소스 ID. 비우면 유지"))
	FVNItemID BgmID = FItemID::Zero;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel", meta = (ToolTip = "이 줄에서 재생할 효과음 리소스 ID"))
	FVNItemID SfxID = FItemID::Zero;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel", meta = (ToolTip = "이 줄에서 화면에 표시할 캐릭터 큐 목록"))
	TArray<FVNCharacterCue> Characters;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel", meta = (ToolTip = "이 줄이 표시되는 순간 적용할 상태 변경"))
	TArray<FVNStateChange> OnShow;
};

UCLASS(Blueprintable, EditInlineNew, meta = (DisplayName = "VN Dialogue Shot"))
class VISUALNOVEL_API UVNDialogueShot : public UStoryShotBase
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VisualNovel", meta = (AllowPrivateAccess = "true", ToolTip = "출력할 대사 줄 목록. 최소 1개 권장"))
	TArray<FVNDialogueLine> Lines;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VisualNovel", meta = (AllowPrivateAccess = "true", ToolTip = "Shot 시작 시 1회 적용할 상태 변경"))
	TArray<FVNStateChange> OnEnter;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VisualNovel", meta = (AllowPrivateAccess = "true", ToolTip = "모든 줄을 넘긴 뒤 적용할 상태 변경"))
	TArray<FVNStateChange> OnComplete;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VisualNovel", meta = (AllowPrivateAccess = "true", ToolTip = "true면 플레이어 입력으로 다음 줄 진행"))
	bool ShouldWaitForInput = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VisualNovel", meta = (AllowPrivateAccess = "true", ToolTip = "true면 대사 로그에 기록할 수 있음"))
	bool ShouldAddToBacklog = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VisualNovel", meta = (AllowPrivateAccess = "true", ClampMin = "0.0", ToolTip = "0보다 크면 지정 시간 뒤 자동 진행 후보로 사용"))
	float AutoAdvanceDelay = 0.0f;

	UPROPERTY(Transient)
	int32 _CurrentLineIndex = INDEX_NONE;

	UPROPERTY(Transient)
	bool _HasCompletedDialogue = false;

protected:
	virtual void OnEnterShot_Implementation() override;

public:
	UFUNCTION(BlueprintCallable, Category = "VisualNovel|Dialogue", meta = (ToolTip = "다음 대사 줄로 진행하고 해당 줄의 OnShow를 적용"))
	bool AdvanceLine();

	UFUNCTION(BlueprintCallable, Category = "VisualNovel|Dialogue", meta = (ToolTip = "지정한 StoryState 기준으로 DialogueShot을 시작하고 OnEnter와 첫 줄 OnShow를 적용"))
	bool BeginDialogueInStoryState(UPARAM(ref) FVNStoryState& _story_state);

	UFUNCTION(BlueprintCallable, Category = "VisualNovel|Dialogue", meta = (ToolTip = "DialogueShot을 완료 처리하고 OnComplete를 적용"))
	bool CompleteDialogue();

	UFUNCTION(BlueprintCallable, Category = "VisualNovel|Dialogue", meta = (ToolTip = "지정한 StoryState 기준으로 다음 대사 줄 상태 변경을 적용"))
	bool AdvanceLineInStoryState(UPARAM(ref) FVNStoryState& _story_state);

	UFUNCTION(BlueprintCallable, Category = "VisualNovel|Dialogue", meta = (ToolTip = "지정한 StoryState 기준으로 DialogueShot 완료 상태 변경을 적용"))
	bool CompleteDialogueInStoryState(UPARAM(ref) FVNStoryState& _story_state);

	UFUNCTION(BlueprintPure, Category = "VisualNovel|Dialogue")
	const TArray<FVNDialogueLine>& GetLines() const { return Lines; }

	UFUNCTION(BlueprintPure, Category = "VisualNovel|Dialogue")
	int32 GetCurrentLineIndex() const { return _CurrentLineIndex; }

	UFUNCTION(BlueprintPure, Category = "VisualNovel|Dialogue")
	bool HasCompletedDialogue() const { return _HasCompletedDialogue; }

	UFUNCTION(BlueprintPure, Category = "VisualNovel|Dialogue")
	bool IsWaitingForInput() const { return ShouldWaitForInput; }

	void SetLines(const TArray<FVNDialogueLine>& _lines) { Lines = _lines; }
	void SetOnEnter(const TArray<FVNStateChange>& _state_changes) { OnEnter = _state_changes; }
	void SetOnComplete(const TArray<FVNStateChange>& _state_changes) { OnComplete = _state_changes; }

private:
	bool ApplyStateChangesToSubsystem(const TArray<FVNStateChange>& _state_changes) const;
};
