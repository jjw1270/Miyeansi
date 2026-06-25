# VisualNovelPlugin 데이터 상세 설계

이 문서는 `VisualNovelPlugin` 1차 구현에 필요한 `FVNStoryState`와 `UVNEventSetAsset`의 상세 필드를 확정한다.

상위 범위는 [VisualNovelPlugin 1차 구현 범위](./VisualNovelPlugin_1차_구현범위.md)를 따른다. 이 문서는 실제 C++ `USTRUCT`, `UDataAsset`, SaveGame, 에디터 입력 필드로 옮길 때의 기준이다.

## 1. 네이밍 기준

프로젝트 코드 스타일과 StoryFlow 구조를 따른다.

| 대상 | 규칙 | 예시 |
|---|---|---|
| USTRUCT 타입 | `FVN` 접두어 + 짧은 PascalCase | `FVNStoryState`, `FVNEventDef` |
| UObject/UDataAsset 타입 | `UVN` 접두어 + 짧은 PascalCase | `UVNEventSetAsset` |
| enum 타입 | `EVN` 접두어 + 짧은 PascalCase | `EVNDaySlot` |
| UPROPERTY 필드 | 짧은 PascalCase | `CurrentDay`, `BoolMap`, `StartSceneID` |
| 내부 멤버 변수 | 기존 코드처럼 앞 `_` + PascalCase | `_StoryState`, `_EventSet` |
| 함수 인자 | 기존 코드처럼 앞 `_` + snake_case | `_event_id`, `_state` |
| StoryState 키 | 짧은 PascalCase | `HayeonTrust`, `IsSohaResolved`, `HasReachedTrueEndingMonday` |

문장처럼 긴 필드명은 쓰지 않는다. 의미가 긴 경우 필드명은 짧게 두고 툴팁/문서 설명으로 보완한다.

## 2. 공통 ID 타입

StoryFlow의 `FStorySceneID`, `FStoryShotID`, `FStoryFlowRef`를 그대로 사용한다.

VN 콘텐츠 카탈로그 ID는 `ItemCore`의 `FItemID` 값을 기반으로 하되, C++ UPROPERTY에는 `FVNCharacterID`, `FVNFragmentID`, `FVNEventID`, `FVNEndingID`, `FVNItemID` 같은 VN 전용 wrapper를 우선 사용한다. wrapper는 `FItemID` 직접 노출을 줄이고 도메인별 기대 타입을 분명하게 하기 위한 기준 ID다.

상태 키는 1차에서 계속 `FName`을 사용한다. 상태 키는 조건 평가용 내부 키이고, 콘텐츠 카탈로그 항목과 성격이 다르다.

| 구분 | 타입 | 예시 | 용도 |
|---|---|---|---|
| StoryFlow Scene/Shot | `FStorySceneID`, `FStoryShotID`, `FStoryFlowRef` | `DDay_05True` | StoryFlow 그래프 위치와 전환 |
| 상태 키 | `FName` | `IsSohaResolved`, `HayeonTrust` | Bool/Int/NameMap 조건과 상태 변경 |
| 날짜/주차 ID | `FName` | `D25`, `DDay`, `FinalWeek` | 진행 상태와 EventHub 필터 |
| 선택 결과 값 | `FName` | `Hayeon`, `Alone`, `True` | Shot 내부 선택 결과와 간단한 분기 값 |
| VN 콘텐츠 ID | `FVNCharacterID`, `FVNFragmentID`, `FVNEventID`, `FVNEndingID`, `FVNItemID` | `Character:Hayeon`, `BGM:HayeonTheme` | 캐릭터, 리소스, 이벤트, 수집/엔딩 카탈로그 |

### 2.1 VN ItemType

`ItemCore`의 `EItemType`에는 VisualNovelPlugin에서 쓰는 콘텐츠 타입을 단계적으로 추가한다. 현재 C++ 구현에서 확정된 타입은 `Character`, `Fragment`, `Event`, `Ending`이고, 리소스 계열은 후속 타입 후보로 둔다.

| ItemType | 설명 | 주요 사용처 | Row에 담을 대표 메타데이터 |
|---|---|---|---|
| `Character` | VN 등장인물의 기준 ID. 화자와 화면 표시 캐릭터를 모두 이 타입으로 참조한다. | `SpeakerID`, `CharacterID`, `CharMap` key | 표시 이름, 이름 색상, 기본 스프라이트, 기본 음성/대사 스타일 |
| `CharacterSprite` | 캐릭터의 스탠딩 이미지/표정/의상/포즈 조합 ID. `PoseID`를 별도로 두지 않고 스프라이트 Row가 표현한다. | `CharacterSpriteID` | 소유 캐릭터, 스프라이트 에셋, 표정/의상/포즈 태그, 기본 표시 위치 |
| `Background` | 배경/장소 이미지 또는 배경 연출 ID. | `BackgroundID` | 배경 에셋, 장소명, 시간대, 전환 효과 기본값 |
| `BGM` | 배경 음악 ID. | `BgmID` | 사운드 에셋, 루프 여부, 볼륨 기본값, 분위기 태그 |
| `SFX` | 효과음 ID. | `SfxID` | 사운드 에셋, 볼륨 기본값, 중복 재생 정책 |
| `Event` | EventHub가 관리하는 이벤트 식별 ID. 실제 시작 위치는 `StartSceneID`로 분리한다. | `EventID`, `SeenEvents` | 표시명, 요약, 정렬/분류 태그, 개발 상태 |
| `Fragment` | 기억 조각/단서/수집형 메타 진행 ID. | `Fragments`, 조건/상태 변경의 Fragment 도메인 | 표시명, 설명, 아이콘, 관련 캐릭터, 관련 엔딩 |
| `Ending` | 엔딩 기록과 갤러리 해금 기준 ID. | `EndingMap`, 조건/상태 변경의 Ending 도메인 | 엔딩명, 엔딩 타입, 설명, 대표 이미지, 갤러리 표시 여부 |

> 현재 구현 메모: `CharacterSprite`, `Background`, `BGM`, `SFX`는 아직 `ItemCore`의 전용 `EItemType`으로 확정하지 않는다. 해당 참조는 전용 타입을 추가하기 전까지 `FVNItemID` 메타 필드로 보관하고, 검증 규칙은 후속 ItemType 확정 시 강화한다.

### 2.2 구현된 VN ItemTableRow

파일: `Plugins/VisualNovel/Source/VisualNovel/Public/VNItemTableRows.h`

| Row 타입 | 상속 | 기본 ItemType | 대표 필드 |
|---|---|---|---|
| `FVNCharacterTableRow` | `FItemTableRow` | `Character` | `Description`, `NameColor`, `DefaultSpriteID`, `DialogueStyleID` |
| `FVNFragmentTableRow` | `FItemTableRow` | `Fragment` | `Description`, `IconID`, `RelatedCharacterID`, `RelatedEndingID` |
| `FVNEventTableRow` | `FItemTableRow` | `Event` | `Summary`, `CategoryTags`, `DefaultDayID`, `DefaultSlot` |
| `FVNEndingTableRow` | `FItemTableRow` | `Ending` | `Description`, `EndingTypeID`, `RepresentativeImageID`, `ShouldShowInGallery` |

각 Row 생성자는 `FItemTableRow(EItemType::...)`를 호출해 기본 `ItemID` 타입을 고정한다. Row에 들어가는 관련 캐릭터/단서/엔딩 참조는 VN wrapper 타입을 사용하고, 아직 전용 ItemType이 없는 이미지/사운드 리소스 참조는 `FVNItemID`로 둔다.

### 2.3 ItemID 적용 원칙

- SaveGame에는 Row 포인터나 에셋을 저장하지 않고 `FItemID` 값만 저장한다. 코드 입력면에서는 가능한 VN wrapper 타입을 사용한다.
- 런타임에서 자주 쓰는 캐릭터/리소스 Row는 VN 전용 캐시 Subsystem에서 `FItemID -> Row`로 보관해 조회 비용을 줄인다.
- `FItemID::Zero`는 미지정 값으로 사용한다.
- `FItemID` 필드는 에디터 검증에서 기대 `ItemType`과 Registry 존재 여부를 확인한다.
- `FStorySceneID`는 StoryFlow 그래프 ID이므로 `EventID`와 합치지 않는다.

## 3. enum 1차 후보

### EVNDaySlot

하루 행동 시스템의 슬롯을 코드에서 안정적으로 다루기 위한 enum이다.

```cpp
UENUM(BlueprintType)
enum class EVNDaySlot : uint8
{
	None,
	Morning,
	Lunch,
	AfterSchool,
	Evening,
	Night,
};
```

| 값 | 용도 |
|---|---|
| `None` | 슬롯 없음, 자동 컷신, 프롤로그/엔딩 |
| `Morning` | 등교/오전 자동 이벤트 |
| `Lunch` | 점심 이벤트 |
| `AfterSchool` | 방과 후 핵심 선택 |
| `Evening` | 저녁 정산/자동 이벤트 |
| `Night` | 특수 컷신, 폐장 후, 루프 전환 |

### EVNEventRunMode

이벤트 재생 방식을 나타낸다.

```cpp
UENUM(BlueprintType)
enum class EVNEventRunMode : uint8
{
	Normal,
	Auto,
	Once,
	Repeatable,
	Summary,
};
```

| 값 | 용도 |
|---|---|
| `Normal` | 일반 선택 이벤트 |
| `Auto` | 조건 충족 시 선택 없이 바로 실행 |
| `Once` | 1회만 실행 |
| `Repeatable` | 반복 실행 가능 |
| `Summary` | 루프 이후 압축 재방문 후보 |

### EVNStateOp

상태 변경 연산이다.

```cpp
UENUM(BlueprintType)
enum class EVNStateOp : uint8
{
	Set,
	Add,
	Remove,
	Max,
	Min,
};
```

| 값 | bool | int | set |
|---|---|---|---|
| `Set` | true/false 설정 | 값 대입 | 전체 대체는 1차 비권장 |
| `Add` | true 처리 | 값 증가 | 항목 추가 |
| `Remove` | false 처리 | 값 감소는 비권장 | 항목 제거 |
| `Max` | 사용 안 함 | 큰 값으로 갱신 | 사용 안 함 |
| `Min` | 사용 안 함 | 작은 값으로 갱신 | 사용 안 함 |

### EVNCompareOp

조건 비교 연산이다.

```cpp
UENUM(BlueprintType)
enum class EVNCompareOp : uint8
{
	Equal,
	NotEqual,
	Greater,
	GreaterEqual,
	Less,
	LessEqual,
	Exists,
	NotExists,
};
```

### EVNStateDomain

상태/조건이 조회하거나 변경할 영역이다. 실제 헤더에서는 `FVNStateChange`, `FVNCondition`보다 먼저 선언한다.

```cpp
UENUM(BlueprintType)
enum class EVNStateDomain : uint8
{
	Bool,
	Int,
	Name,
	Fragment,
	SeenEvent,
	Ending,
};
```

| 값 | 대상 |
|---|---|
| `Bool` | `BoolMap` |
| `Int` | `IntMap` |
| `Name` | `NameMap` |
| `Fragment` | `Fragments` (`TSet<FItemID>`) |
| `SeenEvent` | `SeenEvents` (`TSet<FItemID>`) |
| `Ending` | `EndingMap` (`TMap<FItemID, FVNEndingState>`) |

## 4. FVNStoryState

`FVNStoryState`는 저장 가능한 진행 상태의 루트 구조다.

### 4.1 C++ 구조 후보

```cpp
USTRUCT(BlueprintType)
struct FVNStoryState
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName CurrentDay = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName CurrentWeek = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EVNDaySlot CurrentSlot = EVNDaySlot::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 LoopCount = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FStoryFlowRef CurrentFlowRef;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FName, bool> BoolMap;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FName, int32> IntMap;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FName, FName> NameMap;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FItemID, FVNCharState> CharMap;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSet<FItemID> Fragments;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FItemID, FVNEndingState> EndingMap;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSet<FItemID> SeenEvents;
};
```

### 4.2 루트 필드 상세

| 필드 | 타입 | 기본값 | 저장 | 설명 |
|---|---|---|---|---|
| `CurrentDay` | `FName` | `None` | 필수 | 현재 날짜 ID. 예: `D25`, `D1`, `DDay`, `DPlus2` |
| `CurrentWeek` | `FName` | `None` | 필수 | 주차 ID. 예: `Week1`, `FinalWeek`, `TrueEnding` |
| `CurrentSlot` | `EVNDaySlot` | `None` | 필수 | 현재 하루 슬롯 |
| `LoopCount` | `int32` | `0` | 필수 | 실패 루프 횟수 |
| `CurrentFlowRef` | `FStoryFlowRef` | invalid | 필수 | 저장/로드 복원 위치 |
| `BoolMap` | `TMap<FName, bool>` | empty | 필수 | 플래그 상태 |
| `IntMap` | `TMap<FName, int32>` | empty | 필수 | 호감도/압박/회피 같은 수치 상태 |
| `NameMap` | `TMap<FName, FName>` | empty | 선택 | `DDayResult`, 마지막 선택 등 이름형 상태 |
| `CharMap` | `TMap<FItemID, FVNCharState>` | empty | 필수 | `Character` ItemID별 진행도 |
| `Fragments` | `TSet<FItemID>` | empty | 필수 | 보유한 `Fragment` ItemID 목록 |
| `EndingMap` | `TMap<FItemID, FVNEndingState>` | empty | 필수 | `Ending` ItemID별 관람/반복 기록 |
| `SeenEvents` | `TSet<FItemID>` | empty | 필수 | 이미 본 `Event` ItemID 목록 |

### 4.3 날짜 ID 표기

파일명과 문서에서는 `D-25`, `D+2`를 사용하지만, 코드/키에서는 특수문자를 줄인다.

| 문서 표기 | 코드 ID | 설명 |
|---|---|---|
| D-25 | `D25` | 루프 시작 월요일 |
| D-1 | `D1` | 축제 1일차 |
| D-Day | `DDay` | 축제 마지막 날 |
| D+1 | `DPlus1` | 토요일 암전 |
| D+2 | `DPlus2` | 일요일 병실 기상 |
| D+3 | `DPlus3` | 월요일 방학식 |

게임 UI와 대사에서는 이 ID를 노출하지 않고 자연어를 사용한다.

## 5. FVNCharState

캐릭터별 진행 상태다. `FVNStoryState::CharMap`의 키는 `Character` 타입 `FItemID`를 사용한다.

```cpp
USTRUCT(BlueprintType)
struct FVNCharState
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Affinity = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Trust = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Phase = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Strain = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EVNDaySlot LastSlot = EVNDaySlot::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName LastDay = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsResolved = false;
};
```

| 필드 | 타입 | 기본값 | 설명 |
|---|---|---|---|
| `Affinity` | `int32` | 0 | 일반 호감/친밀도. 진엔딩 조건 자체는 아님 |
| `Trust` | `int32` | 0 | 핵심 신뢰도. 하연은 `HayeonTrust`와 연동 가능 |
| `Phase` | `int32` | 0 | 루트 이벤트 단계 |
| `Strain` | `int32` | 0 | 관계 균열/압박/의존 누적. 캐릭터별 의미는 프로젝트 데이터에서 해석 |
| `LastSlot` | `EVNDaySlot` | `None` | 마지막으로 선택한 슬롯 |
| `LastDay` | `FName` | `None` | 마지막으로 선택한 날짜 |
| `IsResolved` | `bool` | false | 해당 캐릭터 클라이맥스 해결 여부 |

캐릭터별 특수 수치가 필요한 경우 `IntMap`에 짧은 키로 둔다.

| 키 | 의미 |
|---|---|
| `HayeonTrust` | 하연 신뢰 |
| `HayeonPace` | 하연의 속도 이해 |
| `SohaPressure` | 소하 과거 미련 압박 |
| `SeorinControl` | 서린에게 정답을 맡기는 경향 |
| `MiruDepend` | 미루 의존 누적 |
| `Avoid` | 회피 누적 |

## 6. FVNEndingState

엔딩 기록이다.

```cpp
USTRUCT(BlueprintType)
struct FVNEndingState
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool HasBeenSeen = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 SeenCount = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName LastSeenDay = NAME_None;
};
```

| 필드 | 타입 | 기본값 | 설명 |
|---|---|---|---|
| `HasBeenSeen` | `bool` | false | 한 번이라도 본 엔딩인지 |
| `SeenCount` | `int32` | 0 | 반복 관람 횟수 |
| `LastSeenDay` | `FName` | `None` | 마지막 관람 시점 |

1차 엔딩 ItemID 라벨 예시:

```text
Ending:TrueEnding
Ending:HayeonMiss
Ending:SohaSad
Ending:SeorinSad
Ending:MiruSad
Ending:AccFail
Ending:AvoidLoop
Ending:HiddenCollapse
```

## 7. 상태 키 초기값

Miyeansi 1차 프로토타입의 초기 상태다.

### BoolMap

| 키 | 기본값 | 설명 |
|---|---|---|
| `ComaStart` | false | 현실 사고 이후 혼수상태 내면세계 진입 |
| `MetHayeon` | false | 하연 전학 이벤트 완료 |
| `IsHayeonBoothD1Complete` | false | 축제 1일차 하연 부스 운영 완료 |
| `HasHayeonD2Premonition` | false | D-2 말하지 못한 예감에서 하연의 조용한 동선을 이해 |
| `DidChooseHayeonOnDDay` | false | D-Day 낮 하연 보정 선택 성공 |
| `HasConfirmedHayeonMutualFeelings` | false | 하연과 상호 마음 확인 조건 충족 |
| `IsSohaResolved` | false | 소하 관계 정리 완료 |
| `IsSeorinResolved` | false | 서린 관계 정리 완료 |
| `IsMiruResolved` | false | 미루 관계 정리 완료 |
| `CanEnterDDayTrueEnding` | false | D-Day 진엔딩 선택 가능 |
| `IsDDayTrueEnding` | false | D-Day 진엔딩 분기 통과 |
| `HasAcceptedAccidentMemory` | false | 사고 기억을 현실의 기억으로 받아들임 |
| `CanEnterTrueEndingEpilogue` | false | 진엔딩 후일담 진입 가능 |
| `HasSeenTrueEndingWake` | false | 일요일 병실 기상 완료 |
| `HasReachedTrueEndingMonday` | false | 월요일 방학식 도달 |
| `CanEnterTrueEndingCredit` | false | 진엔딩 크레딧 진입 가능 |
| `IsMissingPhysicalClue` | false | 사고 물리 단서 부족 보조 플래그 |
| `DidAvoidHeartOnDDay` | false | 하연 감정 단서 부족 보조 플래그 |
| `HiddenCollapse` | false | 히든 붕괴 엔딩 조건 충족 |
| `HasSeenCeremony` | false | 방학식 참석 완료 |

### IntMap

| 키 | 기본값 | 설명 |
|---|---|---|
| `HayeonTrust` | 0 | 하연 신뢰 |
| `HayeonPace` | 0 | 하연의 속도 이해 |
| `SohaPressure` | 0 | 소하 과거 미련 압박 |
| `SeorinControl` | 0 | 서린에게 답을 맡기는 정도 |
| `MiruDepend` | 0 | 미루 의존 누적 |
| `Avoid` | 0 | 회피 누적 |

### Fragments

`Fragments`는 `Fragment` 타입 ItemID의 집합이다. 아래 값은 Row 라벨 예시다.

| ItemID 라벨 | 의미 |
|---|---|
| `Fragment:HasSohaClue` | 하연을 좋아했다는 감정 단서 |
| `Fragment:HasSeorinClue` | 소무대 임시 구조물 위험 단서 |
| `Fragment:HasMiruClue` | 출입금지 표지/고정끈 문제 단서 |
| `Fragment:HasHayeonClue` | 하연과 함께 사고 기억에 도달할 감정 단서 |

### NameMap

| 키 | 기본값 | 설명 |
|---|---|---|
| `DDayChoice` | `None` | D-Day 낮 선택 대상 |
| `DDayResult` | `None` | 최종 결과. `True`, `HayeonMiss`, `AccFail`, `HiddenCollapse` 등 |
| `ReturnDay` | `D25` | 실패 루프 복귀 날짜 |

## 8. 상태 변경 구조

이벤트나 Shot 완료 시 상태를 변경하기 위한 공통 구조다.

```cpp
USTRUCT(BlueprintType)
struct FVNStateChange
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EVNStateDomain Domain = EVNStateDomain::Bool;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Key = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FItemID ItemID = FItemID::Zero;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EVNStateOp Op = EVNStateOp::Set;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool BoolValue = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 IntValue = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName NameValue = NAME_None;
};
```

| 필드 | 용도 |
|---|---|
| `Domain` | 상태가 들어 있는 영역. `Bool`, `Int`, `Name`, `Fragment`, `SeenEvent`, `Ending` |
| `Key` | `Bool`, `Int`, `Name` 도메인에서 바꿀 상태 키 |
| `ItemID` | `Fragment`, `SeenEvent`, `Ending` 도메인에서 바꿀 ItemID |
| `Op` | 변경 방식 |
| `BoolValue` | bool 상태에 사용 |
| `IntValue` | int 상태에 사용 |
| `NameValue` | name 상태나 set 추가에 사용 |

`Domain`으로 `BoolMap`, `IntMap`, `NameMap`, `Fragments`, `SeenEvents`, `EndingMap` 중 어디에 적용할지 명시한다. `Bool`/`Int`/`Name`은 `Key`를 사용하고, `Fragment`/`SeenEvent`/`Ending`은 `ItemID`를 사용한다. 키 접두어만 보고 타입을 추론하지 않는다.

## 9. 조건 구조

`UVNConditionEvaluator`와 `UVNEventSetAsset`이 함께 사용하는 조건 구조다.

```cpp
USTRUCT(BlueprintType)
struct FVNCondition
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EVNStateDomain Domain = EVNStateDomain::Bool;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Key = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FItemID ItemID = FItemID::Zero;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EVNCompareOp Op = EVNCompareOp::Equal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool BoolValue = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 IntValue = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName NameValue = NAME_None;
};

USTRUCT(BlueprintType)
struct FVNConditionSet
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FVNCondition> All;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FVNCondition> Any;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FVNCondition> None;
};
```

평가 규칙:

1. `All`은 모두 참이어야 한다.
2. `Any`는 비어 있으면 통과, 값이 있으면 하나 이상 참이어야 한다.
3. `None`은 모두 거짓이어야 한다.
4. 세 배열을 모두 통과해야 조건 통과다.
5. `Exists`/`NotExists`는 `Bool`/`Int`/`Name` 도메인에서 값의 참거짓이 아니라 키 존재 여부를 본다. bool 값 자체를 비교할 때는 `Equal`/`NotEqual`을 사용한다.

예시:

| 의도 | 조건 |
|---|---|
| 하연 신뢰 6 이상 | `Domain=Int`, `Key=HayeonTrust`, `Op=GreaterEqual`, `IntValue=6` |
| 소하 정리 완료 | `Domain=Bool`, `Key=IsSohaResolved`, `Op=Equal`, `BoolValue=true` |
| 서린 단서 보유 | `Domain=Fragment`, `ItemID=Fragment:HasSeorinClue`, `Op=Exists` |
| 회피 루프 아님 | `Domain=Int`, `Key=Avoid`, `Op=Less`, `IntValue=3` |

### 9.1 StoryState Subsystem과 조건 Branch

`UVNStoryStateSubsystem`은 현재 GameInstance의 `FVNStoryState`를 보관한다. `UVNConditionBranch`는 StoryFlow의 `UStoryBranchBase`를 상속하며, 실행 시 이 Subsystem의 StoryState를 읽어 출력 인덱스를 고른다.

`FVNConditionBranchCase`는 하나의 Branch 출력 후보를 나타낸다.

| 필드 | 타입 | 설명 |
|---|---|---|
| `DisplayName` | `FText` | Branch 출력 핀 표시명 |
| `ConditionSet` | `FVNConditionSet` | 이 출력이 선택되기 위해 만족해야 하는 조건 묶음 |

`UVNConditionBranch` 출력 핀은 항상 아래 순서를 따른다.

```text
Cases[0]
Cases[1]
Cases[2]
...
Default
```

평가 규칙:

1. `Cases`를 위에서부터 순서대로 평가한다.
2. 처음 통과한 Case의 배열 인덱스를 StoryFlow 출력 인덱스로 반환한다.
3. 어떤 Case도 통과하지 못하면 마지막 `Default` 출력으로 보낸다.
4. `UVNStoryStateSubsystem`을 찾지 못한 경우도 `Default`로 보낸다.
5. `Cases`나 `DefaultOutputName`이 바뀌면 Branch 출력 목록을 다시 구성한다.

## 10. VN Shot 입력 필드

1차 구현에서는 `UVNDialogueShot`과 `UVNChoiceShot`만 확정한다. `UVNFlagShot`은 별도 클래스로 늘리지 않고, 두 Shot의 `OnEnter` / `OnComplete` / `OnSelect` 상태 변경으로 흡수한다.

중요한 연결 규칙:

- `UVNDialogueShot`은 StoryFlow의 기본 Shot처럼 단일 `Next` 핀으로 진행한다.
- `UVNChoiceShot`도 1차에서는 단일 `Next` 핀만 사용한다.
- 선택지별로 바로 다른 Scene에 점프하지 않는다.
- 선택 결과는 `NameMap`이나 `BoolMap`에 저장하고, 다음 노드의 `UVNConditionBranch`가 분기를 고른다.
- 선택지별 직접 출력 핀은 에디터 커스텀 노드가 필요하므로 1차 범위에서 제외한다.

이 규칙을 지키면 StoryFlow 코어를 바꾸지 않고도 선택지 분기를 만들 수 있다.

### 10.1 공통 입력 원칙

| 구분 | 원칙 |
|---|---|
| UPROPERTY 필드명 | 짧은 PascalCase |
| 런타임 내부 멤버 | 기존 코드처럼 `_` + PascalCase |
| 캐릭터/스프라이트/배경/BGM/SFX 참조 | `FVNCharacterID` 또는 `FVNItemID` 래퍼 사용. 실제 Row/에셋은 ItemCore Registry와 VN 캐시 Subsystem에서 해석 |
| 상태 변경 | `FVNStateChange` 배열 재사용 |
| 조건 | `FVNConditionSet` 재사용 |
| 긴 문장 | `FText`, `meta=(MultiLine=true)` |
| 분기 | ChoiceShot 직접 점프가 아니라 `UVNConditionBranch`에서 처리 |

### 10.2 EVNDialogueLineKind

```cpp
UENUM(BlueprintType)
enum class EVNDialogueLineKind : uint8
{
	Dialogue,
	Narration,
	Monologue,
	System,
};
```

| 값 | 용도 |
|---|---|
| `Dialogue` | 캐릭터 이름이 표시되는 일반 대사 |
| `Narration` | 이름 없이 출력되는 지문/내레이션 |
| `Monologue` | 재윤의 독백 |
| `System` | 날짜 전환, 짧은 안내, 디버그용 시스템 문구 |

### 10.3 FVNCharacterCue

한 줄의 대사 동안 화면에 표시할 캐릭터 상태다.

```cpp
USTRUCT(BlueprintType)
struct FVNCharacterCue
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FItemID CharacterID = FItemID::Zero;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FItemID CharacterSpriteID = FItemID::Zero;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName PositionID = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool HasFocus = false;
};
```

| 필드 | 필수 | 설명 |
|---|---|---|
| `CharacterID` | 필수 | `Character` 타입 ItemID. 예: `Character:Hayeon` |
| `CharacterSpriteID` | 선택 | `CharacterSprite` 타입 ItemID. 표정/의상/포즈 조합은 Row에서 해석 |
| `PositionID` | 선택 | 화면 위치. 예: `Left`, `Center`, `Right` |
| `HasFocus` | 선택 | 대사 화자 강조 여부 |

### 10.4 FVNDialogueLine

`UVNDialogueShot` 안에 들어가는 실제 대사 한 줄이다. 한 Shot에 여러 줄을 담아 그래프 노드 수를 줄인다.

```cpp
USTRUCT(BlueprintType)
struct FVNDialogueLine
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EVNDialogueLineKind Kind = EVNDialogueLineKind::Dialogue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FItemID SpeakerID = FItemID::Zero;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText SpeakerName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(MultiLine=true))
	FText Text;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FItemID BackgroundID = FItemID::Zero;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FItemID BgmID = FItemID::Zero;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FItemID SfxID = FItemID::Zero;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FVNCharacterCue> Characters;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FVNStateChange> OnShow;
};
```

| 필드 | 필수 | 설명 |
|---|---|---|
| `Kind` | 필수 | 대사/내레이션/독백/시스템 문구 구분 |
| `SpeakerID` | 조건부 | 일반 대사면 `Character` 타입 ItemID 권장. 내레이션은 비워도 됨 |
| `SpeakerName` | 선택 | 표시 이름을 직접 덮어쓸 때 사용. 비우면 `SpeakerID`의 Character Row에서 해석 |
| `Text` | 필수 | 실제 출력 문장 |
| `BackgroundID` | 선택 | `Background` 타입 ItemID. 비우면 유지 |
| `BgmID` | 선택 | `BGM` 타입 ItemID. 비우면 유지 |
| `SfxID` | 선택 | `SFX` 타입 ItemID. 이 줄에서 재생할 효과음 |
| `Characters` | 선택 | 화면에 표시할 캐릭터 큐 |
| `OnShow` | 선택 | 이 줄이 표시되는 순간 적용할 상태 변경 |

### 10.5 UVNDialogueShot

```cpp
UCLASS(BlueprintType, EditInlineNew)
class VISUALNOVEL_API UVNDialogueShot : public UStoryShotBase
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="VisualNovel")
	TArray<FVNDialogueLine> Lines;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="VisualNovel")
	TArray<FVNStateChange> OnEnter;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="VisualNovel")
	TArray<FVNStateChange> OnComplete;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="VisualNovel")
	bool ShouldWaitForInput = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="VisualNovel")
	bool ShouldAddToBacklog = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="VisualNovel")
	float AutoAdvanceDelay = 0.0f;
};
```

| 필드 | 기본값 | 설명 |
|---|---|---|
| `Lines` | empty | 출력할 대사 줄 목록. 최소 1개 필요 |
| `OnEnter` | empty | Shot 시작 시 1회 적용할 상태 변경 |
| `OnComplete` | empty | 모든 줄을 넘긴 뒤 적용할 상태 변경 |
| `ShouldWaitForInput` | true | true면 플레이어 입력으로 다음 줄 진행 |
| `ShouldAddToBacklog` | true | true면 대사 로그에 기록 |
| `AutoAdvanceDelay` | 0.0 | 0보다 크면 입력 없이 지정 시간 뒤 진행. 암전/짧은 연출에 사용 |

### 10.6 FVNChoiceOption

```cpp
USTRUCT(BlueprintType)
struct FVNChoiceOption
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ChoiceID = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Text;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVNConditionSet ShowCond;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVNConditionSet EnableCond;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText DisabledText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FVNStateChange> OnSelect;
};
```

| 필드 | 필수 | 설명 |
|---|---|---|
| `ChoiceID` | 필수 | 선택 결과 ID. 예: `Hayeon`, `Soha`, `Alone`, `Stay`, `Avoid` |
| `Text` | 필수 | 플레이어에게 보이는 선택지 문구 |
| `ShowCond` | 선택 | 선택지가 목록에 보일 조건 |
| `EnableCond` | 선택 | 선택 가능 조건. 실패하면 비활성 표시 |
| `DisabledText` | 선택 | 비활성 이유 문구 |
| `OnSelect` | 선택 | 선택 직후 적용할 상태 변경 |

### 10.7 UVNChoiceShot

```cpp
UCLASS(BlueprintType, EditInlineNew)
class VISUALNOVEL_API UVNChoiceShot : public UStoryShotBase
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="VisualNovel")
	FText PromptText;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="VisualNovel")
	FName ResultKey = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="VisualNovel")
	TArray<FVNChoiceOption> Options;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="VisualNovel")
	TArray<FVNStateChange> OnEnter;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="VisualNovel")
	TArray<FVNStateChange> OnComplete;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="VisualNovel")
	bool ShouldAutoSelectSingleOption = false;
};
```

| 필드 | 기본값 | 설명 |
|---|---|---|
| `PromptText` | empty | 선택지 위에 표시할 질문/상황 문구 |
| `ResultKey` | None | 선택한 `ChoiceID`를 `NameMap`에 저장할 키. 예: `DDayChoice` |
| `Options` | empty | 선택지 목록. 최소 1개 필요 |
| `OnEnter` | empty | 선택지 표시 전에 적용할 상태 변경 |
| `OnComplete` | empty | 선택 완료 뒤 공통 적용할 상태 변경 |
| `ShouldAutoSelectSingleOption` | false | 표시 가능한 선택지가 1개뿐일 때 자동 선택할지 여부 |

선택 처리 순서:

1. `OnEnter`를 적용한다.
2. `ShowCond`를 통과한 선택지만 목록에 만든다.
3. `EnableCond`를 통과하지 못한 선택지는 비활성으로 보여준다.
4. 플레이어가 선택하면 `ResultKey`가 비어 있지 않은 경우 `NameMap[ResultKey] = ChoiceID`를 저장한다.
5. 선택지의 `OnSelect`를 적용한다.
6. Shot의 `OnComplete`를 적용한다.
7. StoryFlow 기본 `Next` 핀으로 진행한다.

예시:

| 필드 | 값 |
|---|---|
| `PromptText` | 오늘 축제를 누구와 둘러볼까? |
| `ResultKey` | `DDayChoice` |
| `Options[0].ChoiceID` | `Hayeon` |
| `Options[0].Text` | 하연을 찾는다 |
| `Options[0].ShowCond` | `IsHayeonBoothD1Complete == true` |
| `Options[0].OnSelect` | `DidChooseHayeonOnDDay = true`, `HayeonTrust += 1` |
| `Options[1].ChoiceID` | `Alone` |
| `Options[1].Text` | 혼자 소무대를 확인한다 |
| 다음 노드 | `UVNConditionBranch` 또는 `DDay_03Assess` |


### 10.8 1차 구현 메모

현재 C++ 구현은 UI 위젯 표시 전 단계의 Shot 구조를 먼저 고정한다.

- `UVNDialogueShot`은 `BeginDialogueInStoryState`, `AdvanceLineInStoryState`, `CompleteDialogueInStoryState`로 상태 변경 흐름을 검증할 수 있다.
- `UVNChoiceShot`은 `GetVisibleOptionsFromStoryState`, `SelectOptionInStoryState`, `SelectOptionByChoiceIDInStoryState`로 조건 평가와 선택 결과 저장을 검증할 수 있다.
- `FVNCharacterCue.CharacterID`와 `FVNDialogueLine.SpeakerID`는 `FVNCharacterID`를 사용한다.
- 스프라이트/배경/BGM/SFX는 아직 전용 ItemType이 확정되지 않았으므로 `FVNItemID`로 보관한다.
- 실제 대사창/선택지 위젯 연결은 다음 UI 브리지 단계에서 처리한다.
- VN 콘텐츠 DataTable Row는 `FVNCharacterTableRow`, `FVNFragmentTableRow`, `FVNEventTableRow`, `FVNEndingTableRow`로 먼저 고정한다.
## 11. UVNEventSetAsset

`UVNEventSetAsset`은 날짜/슬롯/조건별 이벤트 목록을 담는 데이터 에셋이다. 현재 C++ 1차 구현은 이 에셋과 `UVNEventHubSubsystem`으로 이벤트 표시 필터, Auto 선택, 시작/완료 상태 변경까지 검증한다.

### 11.1 C++ 구조 후보

```cpp
UCLASS(BlueprintType)
class VISUALNOVEL_API UVNEventSetAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FVNEventDef> Events;
};
```

`UVNEventHubSubsystem`은 이 에셋을 읽어 현재 상태에서 실행 가능한 이벤트를 고른다. 1차 구현은 `GetVisibleEventsFromStoryState`, `TryFindAutoEventFromStoryState`, `BeginEventInStoryState`, `CompleteEventInStoryState` API를 제공한다.

## 12. FVNEventDef

이벤트 하나의 정의다.

```cpp
USTRUCT(BlueprintType)
struct FVNEventDef
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FItemID EventID = FItemID::Zero;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName DayID = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EVNDaySlot Slot = EVNDaySlot::None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Priority = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EVNEventRunMode RunMode = EVNEventRunMode::Normal;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FVNConditionSet ShowCond;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FVNConditionSet StartCond;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FStorySceneID StartSceneID;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FVNStateChange> OnStart;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FVNStateChange> OnComplete;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName CompleteFlag = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName NextDay = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EVNDaySlot NextSlot = EVNDaySlot::None;
};
```

### 12.1 필드 상세

| 필드 | 타입 | 필수 | 설명 |
|---|---|---|---|
| `EventID` | `FItemID` | 필수 | `Event` 타입 ItemID. `StartSceneID`와 별개이며 반드시 유일해야 함 |
| `DisplayName` | `FText` | 선택 | 허브 선택지에 표시할 이름. 자동 이벤트는 비워도 됨 |
| `DayID` | `FName` | 필수 | 실행 날짜. 예: `D1`, `DDay`, `DPlus2` |
| `Slot` | `EVNDaySlot` | 필수 | 실행 슬롯 |
| `Priority` | `int32` | 필수 | 높을수록 먼저 평가. 자동 이벤트 충돌 해결에 사용 |
| `RunMode` | `EVNEventRunMode` | 필수 | 일반/자동/1회/반복/압축 재생 여부 |
| `ShowCond` | `FVNConditionSet` | 선택 | 허브 목록에 보일 조건 |
| `StartCond` | `FVNConditionSet` | 선택 | 실제 시작 가능 조건. ShowCond보다 엄격할 수 있음 |
| `StartSceneID` | `FStorySceneID` | 필수 | 시작 StoryFlow SceneID |
| `OnStart` | `TArray<FVNStateChange>` | 선택 | 이벤트 시작 즉시 적용할 상태 변경 |
| `OnComplete` | `TArray<FVNStateChange>` | 선택 | 이벤트 완료 후 적용할 상태 변경 |
| `CompleteFlag` | `FName` | 선택 | 완료 시 true로 켤 bool 키. `SeenEvents`와 별개 |
| `NextDay` | `FName` | 선택 | 완료 후 날짜 이동. 비우면 유지 |
| `NextSlot` | `EVNDaySlot` | 선택 | 완료 후 슬롯 이동. `None`이면 EventHub 기본 진행 |

### 12.2 EventHub 선택 규칙

`UVNEventHubSubsystem`의 기본 선택 순서다.

1. `DayID == CurrentDay`인 이벤트만 고른다.
2. `Slot == CurrentSlot` 또는 `Slot == None`인 이벤트를 고른다.
3. `RunMode == Once`이고 `SeenEvents`에 있으면 제외한다.
4. `ShowCond`를 통과한 이벤트만 허브 목록에 표시한다.
5. `RunMode == Auto`인 이벤트가 있으면 우선순위가 가장 높은 1개를 바로 실행한다.
6. 플레이어가 이벤트를 선택하면 `StartCond`를 다시 확인한다.
7. 시작 시 `OnStart`를 적용하고 `StartSceneID`로 전환한다.
8. 완료 시 `SeenEvents`에 `EventID` ItemID를 추가하고 `OnComplete`, `CompleteFlag`, `NextDay`, `NextSlot`을 적용한다.

현재 구현 메모:

- `GetVisibleEventsFromStoryState`는 `DayID`, `Slot`, `RunMode == Once`의 SeenEvents 제외, `ShowCond`, 유효한 `EventID`/`StartSceneID`를 확인한다.
- `TryFindAutoEventFromStoryState`는 표시 가능하고 `StartCond`까지 통과한 Auto 이벤트 중 가장 높은 `Priority`를 고른다.
- `BeginEventInStoryState`는 `StartCond`를 통과한 이벤트에 한해 `OnStart`를 적용한다.
- `CompleteEventInStoryState`는 `SeenEvents`, `OnComplete`, `CompleteFlag`, `NextDay`, `NextSlot`을 적용한다.
- 실제 StoryFlow Scene 전환 호출은 후속 UI/플로우 브리지 단계에서 처리한다.

## 13. Miyeansi 1차 이벤트 예시

### Cafe_04DayOne

| 필드 | 값 |
|---|---|
| `EventID` | `Event:Cafe_04DayOne` |
| `DisplayName` | 쉬어가 카페 운영 돕기 |
| `DayID` | `D1` |
| `Slot` | `AfterSchool` |
| `Priority` | 80 |
| `RunMode` | `Once` |
| `ShowCond` | `MetHayeon == true` |
| `StartSceneID` | `Cafe_04DayOne` |
| `OnComplete` | `IsHayeonBoothD1Complete=true`, `HayeonTrust+1`, `HayeonPace+1` |
| `NextSlot` | `Evening` |

### DDay_00Entry

| 필드 | 값 |
|---|---|
| `EventID` | `Event:DDay_00Entry` |
| `DisplayName` | 축제 마지막 날 시작 |
| `DayID` | `DDay` |
| `Slot` | `Morning` |
| `Priority` | 100 |
| `RunMode` | `Auto` |
| `StartSceneID` | `DDay_00Entry` |
| `NextSlot` | `AfterSchool` |

### DDay_04Gate

| 필드 | 값 |
|---|---|
| `EventID` | `Event:DDay_04Gate` |
| `DisplayName` | 폐장 후 최종 분기 |
| `DayID` | `DDay` |
| `Slot` | `Night` |
| `Priority` | 100 |
| `RunMode` | `Auto` |
| `StartCond` | `DDayResult != None` |
| `StartSceneID` | `DDay_04Gate` |

### TE_00_TimeSkip

| 필드 | 값 |
|---|---|
| `EventID` | `Event:TE_00_TimeSkip` |
| `DisplayName` | 토요일 암전 |
| `DayID` | `DPlus1` |
| `Slot` | `None` |
| `Priority` | 100 |
| `RunMode` | `Auto` |
| `StartCond` | `CanEnterTrueEndingEpilogue == true` |
| `StartSceneID` | `TE_00_TimeSkip` |
| `NextDay` | `DPlus2` |
| `NextSlot` | `None` |

## 14. 데이터 검증 규칙

에디터 저장 또는 빌드 검증에서 확인할 항목이다.

| 검증 | 오류 조건 |
|---|---|
| EventID 중복 | 같은 `UVNEventSetAsset` 안에 동일 `EventID` ItemID가 2개 이상 |
| StartSceneID 누락 | `StartSceneID`가 비어 있음 |
| DayID 누락 | 일반 이벤트인데 `DayID`가 비어 있음 |
| 조건 키/ItemID 공백 | `Bool`/`Int`/`Name` 조건의 `Key`가 비어 있거나, `Fragment`/`SeenEvent`/`Ending` 조건의 `ItemID`가 비어 있음 |
| 상태 변경 키/ItemID 공백 | `Bool`/`Int`/`Name` 변경의 `Key`가 비어 있거나, `Fragment`/`SeenEvent`/`Ending` 변경의 `ItemID`가 비어 있음 |
| DialogueShot 빈 줄 | `UVNDialogueShot.Lines`가 비어 있거나, `Text`와 연출/상태 변경이 모두 비어 있는 줄이 있음 |
| ChoiceShot 선택지 없음 | `UVNChoiceShot.Options`가 비어 있거나 `ShowCond` 통과 후 표시 가능한 선택지가 없음 |
| ChoiceID 중복 | 같은 `UVNChoiceShot` 안에 동일 `ChoiceID`가 2개 이상 |
| Choice 결과 키 누락 | 선택 결과로 분기해야 하는 `UVNChoiceShot`인데 `ResultKey`가 비어 있음 |
| 자동 이벤트 충돌 | 같은 날짜/슬롯에 `Auto`가 여러 개이고 Priority도 같음 |
| 완료 후 이동 없음 | 허브로 돌아갈 수 없는 이벤트가 `NextSlot`/Transition을 모두 제공하지 않음 |
| D-Day 결과 누락 | `DDay_04Gate`가 처리할 결과 SceneID가 없음 |
| ItemID 타입 불일치 | `SpeakerID`, `CharacterID`, `CharacterSpriteID`, `BackgroundID`, `BgmID`, `SfxID`, `EventID`, `Fragment`, `Ending` 필드가 기대 ItemType과 다름 |
| ItemID Registry 누락 | ItemID가 `FItemID::Zero`가 아닌데 ItemCore Registry에서 Row를 찾을 수 없음 |

## 15. 1차 완료 기준

- `FVNStoryState`의 루트 필드와 하위 구조가 C++ USTRUCT로 바로 옮길 수 있을 만큼 확정됨.
- VN 콘텐츠 DataTable의 1차 Row 타입(`Character/Fragment/Event/Ending`)이 ItemCore 기반으로 구현됨.
- `UVNDialogueShot` / `UVNChoiceShot` 입력 필드가 StoryFlow 기본 Shot/Branch 구조를 바꾸지 않는 범위에서 확정됨.
- `UVNEventSetAsset`이 날짜/슬롯/조건/SceneID/상태 변경을 모두 표현할 수 있음.
- `UVNEventHubSubsystem`이 현재 상태 기준 이벤트 표시, Auto 선택, 시작/완료 상태 변경을 자동화 테스트로 검증함.
- D-Day 진엔딩, 실패 루프 1종, 진엔딩 후일담 시작 이벤트를 같은 구조로 표현할 수 있음.
- 데이터 검증 규칙으로 중복 EventID, 빈 SceneID, 자동 이벤트 충돌, ItemID 타입/Registry 누락을 잡을 수 있음.
- Miyeansi 전용 키는 데이터에만 있고, `VisualNovelPlugin` 코드에 하드코딩하지 않는 경계가 유지됨.

## 16. 후속 작업

`DDay_03Assess`~`DDay_04TrueChoice`~`DDay_05True`와 실패 루프 검증 기준은 [D-Day 최소 프로토타입 테스트 시나리오](./D-Day_최소_프로토타입_테스트_시나리오.md)에 정리했다.

다음 작업은 이 데이터 구조를 실제 `VisualNovelPlugin` 코드와 에디터 Validator로 옮기는 것이다.
