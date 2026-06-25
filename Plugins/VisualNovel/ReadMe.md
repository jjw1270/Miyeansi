# VisualNovel

`VisualNovel`은 Unreal Engine 5 프로젝트에서 `StoryFlow` 위에 **미연시 진행 상태**, **조건 평가**, **상태 변경**, **조건 Branch**를 얹기 위한 런타임 플러그인이다.

이 문서는 `Plugins/VisualNovel`의 **현재 코드 기준**으로 유지되는 기술 README다. 기획 기준과 1차 구현 범위는 프로젝트 기획 문서의 `VisualNovelPlugin_1차_구현범위.md`, `VisualNovelPlugin_데이터_상세설계.md`를 따른다.

---

## 핵심 요약

- `FVNStoryState`는 날짜/슬롯/루프/StoryFlow 위치와 Bool/Int/Name/ItemID 기반 진행 상태를 보관한다.
- `FVNCondition`과 `FVNConditionSet`은 StoryState를 기준으로 조건을 평가한다.
- `FVNStateChange`는 StoryState에 Set/Add/Remove/Max/Min 변경을 적용한다.
- `UVNConditionEvaluator`는 조건 평가와 상태 변경 적용을 담당하는 Blueprint Function Library다.
- `UVNStoryStateSubsystem`은 현재 GameInstance의 VN 진행 상태를 보관한다.
- `UVNConditionBranch`는 StoryFlow Branch로 동작하며, 조건 Case를 순서대로 평가해 다음 출력 핀을 선택한다.
- `UVNDialogueShot`은 대사 라인 진행과 OnEnter/OnShow/OnComplete 상태 변경 적용 구조를 제공한다.
- `UVNChoiceShot`은 선택지 표시 조건, 활성 조건, 선택 결과 저장, OnSelect/OnComplete 적용 구조를 제공한다.
- `UVNEventSetAsset`과 `UVNEventHubSubsystem`은 날짜/슬롯/조건 기준으로 시작 가능한 이벤트를 고르고 시작/완료 상태 변경을 적용한다.
- VN 콘텐츠 ID는 `FItemID`를 직접 노출하지 않고 `FVNCharacterID`, `FVNFragmentID`, `FVNEventID`, `FVNEndingID` 같은 용도별 래퍼를 사용한다.
- `FVNCharacterTableRow`, `FVNFragmentTableRow`, `FVNEventTableRow`, `FVNEndingTableRow`는 ItemCore Row를 상속해 각 ItemType의 기본 메타데이터를 제공한다.

---

## 플러그인 정보

| 항목 | 값 |
| --- | --- |
| 경로 | `Plugins/VisualNovel` |
| 플러그인 이름 | `VisualNovel` |
| 모듈 | `VisualNovel` |
| Engine version | 5.7 |
| Content 포함 | 있음 (`CanContainContent: true`) |
| 주요 의존 플러그인 | `StoryFlow`, `ItemCore`, `SaveGame`, `CustomUI`, `CommonLibrary` |

---

## 모듈 구성

| 모듈 | 타입 | 역할 |
| --- | --- | --- |
| `VisualNovel` | Runtime | VN 상태 구조, ItemID 래퍼, ItemTableRow, 조건 평가, 상태 변경, StoryState Subsystem, 조건 Branch, Dialogue/Choice Shot, EventHub |

### 주요 의존성

`VisualNovel`:

- `Core`, `CoreUObject`, `Engine`
- `StoryFlow`
- `ItemCore`
- `SaveGame`
- `CustomUI`
- `CommonLibrary`
- `Slate`, `SlateCore`

---

## 주요 런타임 타입

### ItemID 래퍼

파일: `Source/VisualNovel/Public/VNItemIDs.h`

| 타입 | 용도 |
| --- | --- |
| `FVNItemID` | 조건 Domain에 따라 해석되는 범용 VN ItemID |
| `FVNCharacterID` | 캐릭터 진행 상태 키 |
| `FVNFragmentID` | 기억 조각/사고 단서 보유 목록 |
| `FVNEventID` | 이미 본 이벤트 기록 |
| `FVNEndingID` | 엔딩 관람 기록 |

`FItemID`는 그대로 노출하지 않고, VN 도메인에 맞는 wrapper를 사용한다. 내부 값과 해시는 `FItemID`와 동일하게 동작한다.

### ItemTableRow 계열

파일: `Source/VisualNovel/Public/VNItemTableRows.h`

ItemCore의 `FItemTableRow`를 상속한 VN 전용 데이터 테이블 Row다. 생성자에서 기대 `EItemType`을 고정하므로, 새 Row를 만들 때 기본 `ItemID` 타입이 자동으로 맞춰진다.

| 타입 | 고정 ItemType | 대표 메타데이터 |
| --- | --- | --- |
| `FVNCharacterTableRow` | `Character` | 설명, 이름 색상, 기본 스프라이트 ID, 대사 스타일 ID |
| `FVNFragmentTableRow` | `Fragment` | 설명, 아이콘 ID, 관련 캐릭터/엔딩 ID |
| `FVNEventTableRow` | `Event` | 요약, 분류 태그, 기본 날짜/슬롯 |
| `FVNEndingTableRow` | `Ending` | 설명, 엔딩 분류 ID, 대표 이미지 ID, 갤러리 표시 여부 |

스프라이트/배경/BGM/SFX는 아직 `ItemCore`의 전용 `EItemType`으로 확정하지 않았으므로, 현재 Row 메타데이터에서는 `FVNItemID`로만 보관한다.

### `FVNStoryState`

파일: `Source/VisualNovel/Public/VNStoryState.h`

저장 가능한 VN 진행 상태의 루트 구조다.

주요 필드:

| 필드 | 의미 |
| --- | --- |
| `CurrentDay` | 현재 날짜 ID |
| `CurrentWeek` | 현재 주차/구간 ID |
| `CurrentSlot` | 현재 하루 행동 슬롯 |
| `LoopCount` | 실패 루프 횟수 |
| `CurrentFlowRef` | 저장/로드 시 복원할 StoryFlow 위치 |
| `BoolMap` | 플래그형 상태 저장소 |
| `IntMap` | 호감도/신뢰/회피 같은 정수 상태 저장소 |
| `NameMap` | 선택 결과/분기 결과 같은 이름형 상태 저장소 |
| `CharMap` | 캐릭터 ItemID별 진행 상태 |
| `Fragments` | 기억 조각/사고 단서 보유 목록 |
| `EndingMap` | 엔딩 관람/반복 기록 |
| `SeenEvents` | 이미 본 이벤트 목록 |

### `FVNCondition` / `FVNConditionSet`

파일: `Source/VisualNovel/Public/VNCondition.h`

`FVNCondition`은 단일 조건이고, `FVNConditionSet`은 All/Any/None 조합 조건이다.

규칙:

1. `All`은 모두 참이어야 통과한다.
2. `Any`는 비어 있으면 통과, 값이 있으면 하나 이상 참이어야 한다.
3. `None`은 모두 거짓이어야 통과한다.
4. 세 배열을 모두 통과해야 조건 묶음이 참이다.
5. `Exists`/`NotExists`는 `Bool`/`Int`/`Name` 도메인에서 값의 참거짓이 아니라 키 존재 여부를 본다.

### `FVNStateChange`

파일: `Source/VisualNovel/Public/VNStateChange.h`

StoryState에 적용할 상태 변경 명령이다.

| Domain | 주요 대상 |
| --- | --- |
| `Bool` | `BoolMap` |
| `Int` | `IntMap` |
| `Name` | `NameMap` |
| `Fragment` | `Fragments` |
| `SeenEvent` | `SeenEvents` |
| `Ending` | `EndingMap` |

| Op | 의미 |
| --- | --- |
| `Set` | 대상 값을 입력 값으로 교체 |
| `Add` | 정수 상태 또는 엔딩 관람 횟수 증가 |
| `Remove` | 대상 키/ItemID 기록 제거 |
| `Max` | 현재 정수 값과 입력 값 중 큰 값으로 갱신 |
| `Min` | 현재 정수 값과 입력 값 중 작은 값으로 갱신 |

### `UVNConditionEvaluator`

파일: `Source/VisualNovel/Public/VNConditionEvaluator.h`

Blueprint/C++에서 조건 평가와 상태 변경 적용을 호출하는 함수 모음이다.

| 함수 | 역할 |
| --- | --- |
| `EvaluateCondition` | 단일 조건 평가 |
| `EvaluateConditionSet` | All/Any/None 조건 묶음 평가 |
| `ApplyStateChange` | 상태 변경 하나 적용 |
| `ApplyStateChanges` | 상태 변경 배열을 순서대로 적용 |

### `UVNStoryStateSubsystem`

파일: `Source/VisualNovel/Public/VNStoryStateSubsystem.h`

`UGameInstanceSubsystem`으로 현재 플레이 세션의 VN 진행 상태를 보관한다.

주요 API:

| 함수 | 역할 |
| --- | --- |
| `GetStoryState` | 현재 StoryState 반환 |
| `SetStoryState` | StoryState 교체 |
| `ResetStoryState` | StoryState 초기화 |
| `ApplyStateChange` | 현재 StoryState에 변경 하나 적용 |
| `ApplyStateChanges` | 현재 StoryState에 변경 배열 적용 |

SaveGame 브리지가 구현되면 이 Subsystem의 `FVNStoryState`와 저장 객체를 동기화하는 방식으로 연결한다.

### `UVNConditionBranch`

파일: `Source/VisualNovel/Public/VNConditionBranch.h`

`UStoryBranchBase`를 상속한 VN 전용 조건 Branch다.

동작:

1. `UVNStoryStateSubsystem`에서 현재 `FVNStoryState`를 읽는다.
2. `Cases`를 위에서부터 평가한다.
3. 처음 통과한 Case의 출력 인덱스를 반환한다.
4. 아무 Case도 통과하지 못하거나 Subsystem을 찾지 못하면 마지막 `Default` 출력으로 보낸다.

출력 핀 구성:

```text
Cases[0]
Cases[1]
Cases[2]
...
Default
```

`Cases`나 `DefaultOutputName`이 바뀌면 StoryFlow Branch 출력 목록도 자동으로 다시 구성된다.

### `UVNDialogueShot`

파일: `Source/VisualNovel/Public/VNDialogueShot.h`

`UStoryShotBase`를 상속한 VN 대사 Shot이다. 한 Shot 안에 여러 `FVNDialogueLine`을 담고, 진행 시 상태 변경을 적용한다.

| 타입/필드 | 역할 |
| --- | --- |
| `EVNDialogueLineKind` | Dialogue/Narration/Monologue/System 구분 |
| `FVNCharacterCue` | 캐릭터 ID, 스프라이트 ID, 위치, 포커스 정보 |
| `FVNDialogueLine` | 한 줄의 텍스트, 리소스 ID, 캐릭터 큐, `OnShow` |
| `Lines` | 출력할 대사 줄 목록 |
| `OnEnter` | Shot 시작 시 적용할 상태 변경 |
| `OnComplete` | 모든 줄이 끝난 뒤 적용할 상태 변경 |

1차 구현은 UI 표시 대신 라인 진행과 상태 변경 적용 API를 제공한다.

### `UVNChoiceShot`

파일: `Source/VisualNovel/Public/VNChoiceShot.h`

`UStoryShotBase`를 상속한 VN 선택지 Shot이다. 선택지 조건을 평가하고 선택 결과를 StoryState에 저장한다.

| 타입/필드 | 역할 |
| --- | --- |
| `FVNChoiceOption` | ChoiceID, 표시 문구, ShowCond, EnableCond, OnSelect |
| `FVNChoiceOptionState` | 평가된 선택지 표시/활성 상태 |
| `ResultKey` | 선택한 ChoiceID를 저장할 `NameMap` 키 |
| `OnEnter` | 선택지 표시 전 적용할 상태 변경 |
| `OnComplete` | 선택 완료 뒤 공통 적용할 상태 변경 |

1차 구현은 실제 선택지 위젯 표시 대신, 표시 가능한 선택지 계산과 선택 적용 API를 제공한다.

### `UVNEventSetAsset` / `UVNEventHubSubsystem`

파일: `Source/VisualNovel/Public/VNEventSetAsset.h`, `Source/VisualNovel/Public/VNEventHubSubsystem.h`

날짜/슬롯/조건 기준으로 허브에서 보이거나 자동 실행할 이벤트를 고르는 1차 구조다.

| 타입/함수 | 역할 |
| --- | --- |
| `FVNEventDef` | 이벤트 ID, 날짜/슬롯, 우선순위, 실행 방식, 조건, 시작 Scene, 시작/완료 상태 변경 |
| `UVNEventSetAsset` | 제작 데이터로 관리할 이벤트 정의 배열 |
| `GetVisibleEventsFromStoryState` | 현재 StoryState에서 허브 목록에 보이는 이벤트 계산 |
| `TryFindAutoEventFromStoryState` | 시작 가능한 Auto 이벤트 중 우선순위가 가장 높은 이벤트 선택 |
| `BeginEventInStoryState` | StartCond 확인 후 OnStart 적용 |
| `CompleteEventInStoryState` | SeenEvents 기록, OnComplete, CompleteFlag, NextDay/NextSlot 적용 |

1차 구현은 실제 StoryFlow 전환 호출 전 단계로, 어떤 `StartSceneID`를 실행해야 하는지와 상태 변화만 결정한다.

---

## 자동화 테스트

현재 테스트 경로:

```text
Source/VisualNovel/Private/Tests
```

등록된 테스트:

| 테스트 | 확인 내용 |
| --- | --- |
| `VisualNovel.ConditionEvaluator.*` | 조건 평가, 조건 묶음, 상태 변경, batch 적용 |
| `VisualNovel.ConditionBranch.*` | Branch 출력 구성, 조건 선택, Default fallback |
| `VisualNovel.ItemTableRows.*` | VN Row의 기본 ItemType과 메타데이터 저장 |
| `VisualNovel.EventHub.*` | 이벤트 표시 필터, Auto 선택, 시작/완료 상태 변경 |
| `VisualNovel.Shot.Dialogue.StateFlow` | DialogueShot 라인 진행과 상태 변경 적용 |
| `VisualNovel.Shot.Choice.*` | ChoiceShot 선택지 조건 평가와 선택 결과 저장 |
| `VisualNovel.StoryStateSubsystem.StateAccess` | StoryState 보관, 상태 변경 적용, 초기화 |

명령줄 실행 예:

```powershell
& 'F:\UnrealEngine\UE_5.7\Engine\Binaries\Win64\UnrealEditor-Cmd.exe' `
  'F:\UnrealProjects\Project\Miyeansi\Miyeansi.uproject' `
  -Unattended `
  -NoSplash `
  -NoSound `
  -NullRHI `
  -ExecCmds='Automation RunTests VisualNovel;Quit' `
  -TestExit='Automation Test Queue Empty' `
  -Log="VisualNovelTests.log"
```

---

## 현재 구현 범위

완료:

- VN 전용 ItemID 래퍼
- VN 전용 ItemTableRow 계열
- StoryState 구조
- 조건/상태 변경 구조
- 조건 평가기
- StoryState Subsystem
- Condition Branch
- 조건/Branch 자동화 테스트
- `UVNDialogueShot` 1차 구조
- `UVNChoiceShot` 1차 구조
- `UVNEventSetAsset` / `UVNEventHubSubsystem` 1차 구조
- Shot/EventHub 자동화 테스트

진행 예정:

- SaveGame 브리지
- D-Day 최소 프로토타입 플레이 검증

---

## 현재 한계 / 주의사항

- SaveGame 브리지는 아직 구현 전이다.
- EventHub는 아직 StoryFlow 실제 Scene 전환 호출을 직접 수행하지 않고, 시작할 `StartSceneID`와 상태 변경까지만 결정한다.
- Dialogue/Choice Shot은 UI 위젯 브리지 전 단계이며, 현재는 라인/선택 상태 흐름 API까지만 제공한다.
- `UVNConditionBranch`는 현재 `UVNStoryStateSubsystem`의 메모리 상태만 읽는다.
- Branch는 StoryFlow 정책에 따라 순간 판단 단계이며, Branch 자체를 저장 대상으로 보지 않는다.
- Miyeansi 전용 키나 루트 조건은 플러그인 코드에 하드코딩하지 않고 DataAsset/Blueprint/프로젝트 코드에서 주입해야 한다.

---

## 추천 코드 읽기 순서

1. `VisualNovel.uplugin`
2. `Source/VisualNovel/VisualNovel.Build.cs`
3. `Source/VisualNovel/Public/VNDefines.h`
4. `Source/VisualNovel/Public/VNItemIDs.h`
5. `Source/VisualNovel/Public/VNItemTableRows.h`
6. `Source/VisualNovel/Private/VNItemTableRows.cpp`
7. `Source/VisualNovel/Public/VNStoryState.h`
8. `Source/VisualNovel/Public/VNCondition.h`
9. `Source/VisualNovel/Public/VNStateChange.h`
10. `Source/VisualNovel/Public/VNConditionEvaluator.h`
11. `Source/VisualNovel/Private/VNConditionEvaluator.cpp`
12. `Source/VisualNovel/Public/VNStoryStateSubsystem.h`
13. `Source/VisualNovel/Private/VNStoryStateSubsystem.cpp`
14. `Source/VisualNovel/Public/VNConditionBranch.h`
15. `Source/VisualNovel/Private/VNConditionBranch.cpp`
16. `Source/VisualNovel/Public/VNDialogueShot.h`
17. `Source/VisualNovel/Private/VNDialogueShot.cpp`
18. `Source/VisualNovel/Public/VNChoiceShot.h`
19. `Source/VisualNovel/Private/VNChoiceShot.cpp`
20. `Source/VisualNovel/Public/VNEventSetAsset.h`
21. `Source/VisualNovel/Private/VNEventSetAsset.cpp`
22. `Source/VisualNovel/Public/VNEventHubSubsystem.h`
23. `Source/VisualNovel/Private/VNEventHubSubsystem.cpp`
24. `Source/VisualNovel/Private/Tests/*.cpp`

---

## 관련 문서

- `기획/시스템/VisualNovelPlugin_1차_구현범위.md`
- `기획/시스템/VisualNovelPlugin_데이터_상세설계.md`
- `기획/시스템/D-Day_최소_프로토타입_테스트_시나리오.md`
