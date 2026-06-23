# VisualNovelPlugin 1차 구현 범위

이 문서는 StoryFlow 위에 얹을 재사용 가능한 미연시 전용 플러그인 `VisualNovelPlugin`의 1차 구현 범위를 확정한다.

`VisualNovelPlugin`은 Miyeansi 전용 스토리를 직접 알지 않는다. 플러그인은 날짜, 상태, 조건, 선택지, 저장 브리지를 제공하고, Miyeansi 프로젝트는 캐릭터, 루트, 장면, 배경, 대사 데이터를 별도 에셋으로 넣는다.

## 1. 1차 목표

1차 목표는 전체 게임 완성이 아니라, 다음 흐름을 실제 게임에서 재생하고 저장/분기할 수 있는 최소 기반을 만드는 것이다.

```text
StoryFlow Scene 실행
-> VN용 대사/선택지 Shot 재생
-> StoryState 변경
-> 조건 Branch 평가
-> 다음 Scene 전환
-> SaveGame에 StoryFlowRef + StoryState 저장
-> 루프/엔딩 결과 기록
```

이 범위가 완성되면 `17_전체_스토리_제작용_시나리오.md`의 SceneID를 실제 StoryFlow 에셋으로 옮기는 작업을 시작할 수 있다.

## 2. 플러그인 역할 경계

| 영역 | StoryFlowPlugin | VisualNovelPlugin | Miyeansi 프로젝트 |
|---|---|---|---|
| Scene/Shot/Branch 실행 | 담당 | 사용 | 데이터 제작 |
| 대사창, 선택지, 캐릭터 표시 | 비관여 | 기본 Shot/UI 브리지 제공 | UI 스킨 적용 |
| 날짜/슬롯/플래그 | 비관여 | 범용 상태 계층 제공 | Miyeansi 상태 키 정의 |
| 루트/히로인/엔딩 내용 | 비관여 | 비관여 | 담당 |
| 저장/복원 | `FStoryFlowRef` 제공 | `StoryState`와 묶어 저장 | 저장 슬롯 정책 적용 |
| 갤러리/크레딧 연출 | 비관여 | 1차 제외 | 2차 제작 |

원칙:

- StoryFlow 코어는 미연시 장르를 몰라도 된다.
- `VisualNovelPlugin`은 StoryFlow에 의존해도 되지만, StoryFlow는 `VisualNovelPlugin`에 의존하지 않는다.
- 플러그인 코드에 `Hayeon`, `Soha`, `DDayTrue` 같은 Miyeansi 전용 키를 하드코딩하지 않는다.
- Miyeansi 전용 조건은 DataAsset, DataTable, Blueprint, 프로젝트 코드에서 주입한다.

## 3. 1차 포함 범위

### 3.1 StoryState

`StoryState`는 미연시 진행 상태의 단일 기준이다.

필수 데이터:

| 데이터 | 예시 | 용도 |
|---|---|---|
| 현재 날짜/주차 | `D-25`, `FinalWeek` | 이벤트 개방, 루프 복귀 |
| 현재 슬롯 | `Morning`, `Lunch`, `AfterSchool`, `Evening` | 하루 행동 선택 |
| 회차 | `LoopCount` | 루프 연출, 반복 이벤트 압축 |
| 범용 플래그 | `HayeonBoothD1`, `DDayTrue` | 조건 분기 |
| 정수 상태 | `HayeonTrust`, `HayeonPace`, `Avoid` | 점수 조건 |
| 캐릭터 진행 | `Hayeon`, `Soha`, `Seorin`, `Miru` | 루트 단계, 마지막 선택 슬롯 |
| 기억 조각 | `ClueSoha`, `ClueSeorin`, `ClueMiru`, `ClueHayeon` | 진엔딩 조건 |
| 엔딩 기록 | `TrueEnding`, `SohaSad` | 루프/수집/재진입 제어 |
| 현재 StoryFlow 위치 | `FStoryFlowRef` | 저장/로드 후 장면 복원 |

1차 구현에서는 상태 타입을 과하게 늘리지 않는다.

필수 타입:

- bool
- int
- name/string ID
- tag 배열 또는 name set

2차 범위:

- 복잡한 수식 언어
- 상태 변경 히스토리 전체 로그
- 도감/갤러리용 세부 수집 데이터

### 3.2 조건 평가

조건 평가는 `UVNConditionEvaluator`가 담당한다.

1차 조건 형식:

| 조건 | 예시 |
|---|---|
| bool 일치 | `SohaDone == true` |
| int 비교 | `HayeonTrust >= 6` |
| name 일치 | `CurrentDay == DDay` |
| 플래그 모두 충족 | `SohaDone`, `SeorinDone`, `MiruDone` |
| 플래그 하나 이상 충족 | `ClueSoha` 또는 `ClueSeorin` |
| 부정 조건 | `Avoid < 3` |

1차에서는 조건을 DataAsset/Blueprint에서 조합 가능한 구조로 두고, 문자열 수식 파서는 만들지 않는다.

### 3.3 VN용 Shot

| 클래스 | 1차 역할 | 비고 |
|---|---|---|
| `UVNDialogueShot` | 대사, 나레이션, 캐릭터 스프라이트, 배경, BGM/SFX, 간단한 상태 변경 | 입력 필드 확정 |
| `UVNChoiceShot` | 선택지 표시, 선택 가능 조건, 선택 결과 상태 변경, 선택 결과 저장 | 입력 필드 확정 |
| `UVNFlagShot` | 대사 없이 상태만 변경하는 짧은 처리 Shot | 필요하면 DialogueShot의 옵션으로 흡수 가능 |
| `UVNWaitShot` | 짧은 대기, 암전, 전환 타이밍 | 프롤로그/진엔딩 후일담에 필요 |

입력 필드 기준은 [VisualNovelPlugin 데이터 상세 설계](./VisualNovelPlugin_데이터_상세설계.md)의 `VN Shot 입력 필드` 섹션을 따른다.

1차 선택지 분기 원칙:

- `UVNChoiceShot`은 StoryFlow 기본 Shot의 단일 `Next` 핀으로만 진행한다.
- 선택지별 직접 Scene 점프는 하지 않는다.
- 선택한 `ChoiceID`를 `ResultKey`로 `NameMap`에 저장하고, 다음 `UVNConditionBranch`가 실제 분기를 고른다.
- 선택지별 출력 핀을 가진 커스텀 그래프 노드는 2차 범위로 둔다.

1차 UI는 완성형 연출보다 기능 확인을 우선한다.

필수 UI:

- 대사창
- 이름 표시
- 다음 진행 입력
- 선택지 목록
- 간단한 백로그 후보

2차 UI:

- 고급 텍스트 효과
- 음성 재생 표시
- 캐릭터별 대사창 스킨
- 갤러리/도감/크레딧 UI

### 3.4 VN용 Branch

`UVNConditionBranch`는 StoryState를 읽어 StoryFlow 출력 인덱스를 선택한다.

1차 사용처:

- D-Day 최종 결과: `DDay_04Gate`
- 루프 복귀 여부
- 진엔딩 후일담 진입 여부
- 조건 미충족 선택지의 대체 흐름

D-Day 결과 평가는 별도 함수 또는 프로젝트 전용 평가 에셋으로 분리한다.

```text
EvaluateDDayResult()
-> HiddenCollapse / True / HayeonMiss / SohaSad / SeorinSad / MiruSad / AccFail / AvoidLoop
-> UVNConditionBranch가 출력 링크 선택
```

### 3.5 EventHub

`UVNEventHubSubsystem`은 날짜와 슬롯에 따라 선택 가능한 이벤트를 만든다.

1차 역할:

- 현재 날짜/슬롯 조회
- 조건을 만족하는 이벤트 목록 구성
- 선택한 이벤트의 시작 SceneID 반환
- 이벤트 종료 후 다음 슬롯 또는 다음 날짜로 진행
- 자동 이벤트가 있으면 선택 목록보다 우선 실행

이벤트 데이터는 1차에서 `UVNEventSetAsset` 같은 DataAsset 기준을 우선한다. DataTable은 대량 입력 보조 수단으로 추후 확장한다.

이유:

- StoryFlow SceneID, 조건 묶음, 상태 변경, 우선순위가 구조화되어야 한다.
- Blueprint/에디터에서 조건 묶음을 확인하기 쉽다.
- CSV/DataTable만으로는 Branch/조건/후속 Scene 연결이 길어질수록 검수성이 떨어진다.

### 3.6 저장 브리지

`UVNSaveGame` 또는 저장 브리지 계층은 아래 두 값을 반드시 함께 저장한다.

```text
FStoryFlowRef CurrentStoryRef
FVNStoryState StoryState
```

필수 저장 항목:

- 현재 SceneID / ShotID
- StoryState 전체
- 관람한 이벤트 ID
- 엔딩 기록
- 루프 횟수
- 설정값은 1차 범위에서 제외 가능

로드 기준:

1. SaveGame에서 `StoryState` 복원
2. `FStoryFlowRef`로 StoryFlow 위치 복원
3. 복원 실패 시 EventHub가 현재 날짜/슬롯 허브 Scene으로 복구

### 3.7 루프/엔딩 최소 처리

`UVNLoopManager`와 `UVNEndingManager`는 1차에서 작게 시작한다.

필수 기능:

- 엔딩 결과 기록
- 루프 시작 날짜 결정
- 루프 시 유지할 상태와 초기화할 상태 구분
- 진엔딩이면 루프하지 않고 `TE_00_Sat`로 전환

1차 루프 정책:

| 상태 | 실패 루프 시 처리 |
|---|---|
| `LoopCount` | 증가 |
| 엔딩 기록 | 유지 |
| 핵심 기억 조각 | 일부 유지 가능 |
| 하루 슬롯/현재 날짜 | D-25 월요일로 복귀 |
| 루트 진행도 | 기본 초기화 |
| 히든 복구 조각 | 2차 범위 |

## 4. 1차 제외 범위

아래 항목은 중요하지만 지금 확정하지 않는다.

| 제외 항목 | 이유 |
|---|---|
| 진엔딩 크레딧 세부 콘티 | 음악/원화/애니 분량이 후반 제작에서 바뀔 가능성이 큼 |
| 엔딩 수집 UI | 엔딩 기록 저장 이후 붙이면 됨 |
| 기억 조각 도감 | 핵심 분기 구현 후 제작 가능 |
| 고급 백로그/스킵/오토 | 대사 재생이 안정된 뒤 확장 |
| 캐릭터별 UI 스킨 | 공통 대사창 검증 후 적용 |
| 복잡한 조건 수식 파서 | 1차는 DataAsset 조합 조건으로 충분 |
| 자동 추천/알림 | 플레이 감각 검증 후 판단 |
| 히든 붕괴 전용 UI | 기본 루프/엔딩 처리 후 확장 |

## 5. 개발 순서

1. `VisualNovelPlugin` 모듈 생성
   - StoryFlow 의존성 연결
   - Miyeansi 전용 데이터 하드코딩 금지 확인
2. `FVNStoryState`와 저장용 구조 작성
   - bool/int/name 상태 저장
   - 캐릭터 진행도, 기억 조각, 엔딩 기록 포함
3. `UVNConditionEvaluator` 작성
   - bool/int/name 조건
   - all/any/not 조합
4. VN용 Shot 1차 작성
   - `UVNDialogueShot`
   - `UVNChoiceShot`
   - 필요 시 `UVNWaitShot`
5. `UVNConditionBranch` 작성
   - StoryState 기반 출력 인덱스 선택
6. `UVNEventHubSubsystem` 작성
   - 날짜/슬롯 이벤트 선택
   - 이벤트 완료 후 슬롯 진행
7. SaveGame 브리지 작성
   - `FStoryFlowRef + FVNStoryState` 저장/복원
8. Miyeansi 검증용 최소 데이터 연결
   - `DDay_03Assess`
   - `DDay_04Gate`
   - `DDay_05True`
   - `TE_00_Sat`~`TE_01_Wake`
9. 루프/엔딩 기록 최소 연결
   - 실패 루프 D-25 복귀
   - 진엔딩 후일담 진입

## 6. 1차 완료 기준

- StoryFlow Scene에서 `UVNDialogueShot`이 대사를 표시하고 다음 Shot으로 진행된다.
- `UVNChoiceShot`이 조건에 맞는 선택지를 표시하고 선택 결과를 StoryState에 반영한다.
- `UVNConditionBranch`가 StoryState를 읽어 서로 다른 다음 Scene으로 보낸다.
- EventHub가 날짜/슬롯 기준으로 시작 SceneID를 고른다.
- 저장 후 재실행해도 `FStoryFlowRef`와 StoryState가 함께 복원된다.
- `DDay_04Gate`에서 진엔딩/실패 루프/새드엔딩 중 최소 2개 이상의 결과가 실제로 갈라진다.
- 진엔딩 성공 시 `DDay_05True` 이후 `TE_00_Sat`로 이동한다.
- 실패 루프 시 D-25 월요일 허브로 복귀한다.

## 7. Miyeansi 데이터 연결 기준

Miyeansi 전용 키는 프로젝트 데이터에서만 정의한다.

1차 필수 키:

```text
ComaStart
HayeonTrust
HayeonPace
HayeonBoothD1
HayeonD2
DDayHayeon
HayeonMutual
SohaDone
SeorinDone
MiruDone
ClueSoha
ClueSeorin
ClueMiru
ClueHayeon
Avoid
MissPhysClue
DDayNoHeart
HiddenCollapse
DDayTrueReady
DDayTrue
AccMemOk
TE_Unlocked
TE_Wake
TE_Monday
TE_Credit
```

1차 필수 SceneID:

```text
DDay_03Assess
DDay_04Gate
DDay_05True
DDay_06HayeonMiss
DDay_10AccFail
DDay_11Avoid
TE_00_Sat
TE_01_Wake
```

확장 SceneID는 `17_전체_스토리_제작용_시나리오.md`와 `16_D+2_D+3_진엔딩_후일담_컷신.md`를 따른다.

## 8. 테스트 기준

최소 자동/수동 테스트 후보:

| 테스트 | 확인 내용 |
|---|---|
| 상태 저장 테스트 | bool/int/name 상태가 저장 후 복원되는지 확인 |
| 조건 평가 테스트 | `HayeonTrust >= 6`, `SohaDone == true` 같은 조건 결과 확인 |
| 선택지 테스트 | 조건 충족 선택지만 표시되고 선택 시 상태가 바뀌는지 확인 |
| Branch 테스트 | 같은 Scene에서 상태에 따라 다른 NextSceneID가 선택되는지 확인 |
| 저장 복원 테스트 | SceneID/ShotID와 StoryState가 같이 복원되는지 확인 |
| D-Day 분기 테스트 | 진엔딩 조건 충족/미충족 케이스가 갈라지는지 확인 |
| 루프 복귀 테스트 | 실패 결과 후 D-25 월요일 허브로 돌아가는지 확인 |

## 9. 후속 작업

`FVNStoryState`와 `UVNEventSetAsset` 상세 필드는 [VisualNovelPlugin 데이터 상세 설계](./VisualNovelPlugin_데이터_상세설계.md)에서 확정한다.

구현 직전 Shot 입력 필드는 [VisualNovelPlugin 데이터 상세 설계](./VisualNovelPlugin_데이터_상세설계.md)에 확정했다.

구현 착수 전 기준 문서는 [D-Day 최소 프로토타입 테스트 시나리오](./D-Day_최소_프로토타입_테스트_시나리오.md)까지 준비되었다.

다음 작업은 1차 구현 착수와 에디터 Validator 작업 목록 분해다.

1. `VisualNovelPlugin` 모듈 생성과 StoryFlow 의존성 연결
2. `FVNStoryState`, `FVNStateChange`, `FVNConditionSet` 구현
3. `UVNDialogueShot`, `UVNChoiceShot`, `UVNConditionBranch` 구현
4. 데이터 검증 규칙을 실제 에디터 Validator 작업 목록으로 분해
