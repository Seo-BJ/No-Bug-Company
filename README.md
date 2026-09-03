# No Bug Company

Unreal Engine 5로 제작한 3D 탑다운 생존 로그라이크 슈팅 게임입니다.

플레이어는 몰려오는 해충과 싸우며 경험치와 재료를 획득하고, 해충 방제 테마의 무기와 능력치를 강화해 정해진 시간 동안 생존해야 합니다.

## 프로젝트 정보

| 항목 | 내용 |
| --- | --- |
| 개발 기간 | 2024.09.05 ~ 2024.12.20 |
| 개발 인원 | 5인 팀 프로젝트 |
| 담당 역할 | 팀장, 게임플레이 및 UI 개발 |
| 개발 환경 | Unreal Engine 5.5, C++, Blueprint |
| 주요 기술 | Data Table, Gameplay Tag, UMG, Object Pooling, Unreal Insights |
| 플랫폼 | Windows |

## 담당 업무와 핵심 성과

기능을 빠르게 추가하는 것뿐 아니라, 기획 데이터와 로직의 결합도를 낮추고 반복 생성 비용과 불필요한 Tick을 줄이는 방향으로 시스템을 설계했습니다.

| 문제 | 해결 | 결과 |
| --- | --- | --- |
| 보상·아이템이 코드에 결합되어 밸런싱과 테스트가 복잡함 | Data Table과 Gameplay Tag 기반 데이터 구조, 가중치 랜덤 선택 적용 | 코드 수정 없이 데이터만으로 보상 구간과 수량 조정 |
| Enemy·Projectile 반복 생성/제거에서 초기화와 할당 비용이 집중됨 | 월드 단위 오브젝트 풀, 레벨별 PreWarm 정책, 재사용 수명주기 구현 | P99 프레임 34.3% 감소, 50ms 초과 프레임 97.4% 감소 |
| 플레이어 스탯 변경 로직과 UI 갱신이 강하게 결합됨 | Model–Presenter–View로 책임 분리, 델리게이트 기반 갱신 | 계층별 책임과 의존성 방향을 명확히 해 기능 변경의 영향 범위 축소 |

## 1. 데이터 기반 게임플레이

### 문제

무기, 아이템, 능력치, 보상 규칙이 늘어나면서 값을 변경할 때마다 코드 수정과 재검증이 필요했습니다. 특히 레벨업 보상은 아이템별 수량과 확률을 함께 관리해야 해 기획 변경에 따른 밸런싱 비용이 커졌습니다.

### 해결

- 보상 구간과 아이템별 수량 가중치를 `Data Table` 행 구조로 분리했습니다.
- 아이템과 능력치의 식별에는 `Gameplay Tag`를 사용해 시스템 간 구체 클래스 의존을 줄였습니다.
- 확률의 합을 100으로 맞추는 방식 대신, 전체 가중치 합에서 난수를 뽑는 누적 가중치 선택을 적용했습니다. 새 항목을 추가해도 기존 값을 다시 정규화할 필요가 없습니다.
- 확률 배열과 수량 배열의 길이가 다르거나 전체 가중치가 0 이하인 경우 보상 계산을 중단하거나 해당 항목을 건너뛰도록 방어했습니다.

```text
레벨업
  → 현재 레벨에 맞는 보상 행 선택
  → 각 아이템의 가중치 총합 계산
  → 누적 가중치 구간에서 수량 선택
  → InventoryComponent에 지급
  → 결과 UI 표시
```

### 핵심 클래스

- [`FSupplyData`](Source/PestControl/21_Data/PeCoDataRow.h#L95-L122): 레벨 구간과 보상 아이템의 수량별 가중치를 정의합니다.
- [`APeCoPlayerState`](Source/PestControl/02_Player/PeCoPlayerState.cpp#L338-L448): 현재 레벨에 맞는 보상 규칙을 선택하고 가중치 계산과 지급을 처리합니다.
- [`UInventoryComponent`](Source/PestControl/01_Character/Components/InventoryComponent.cpp#L28-L115): 선택된 아이템을 플레이어 인벤토리에 추가하고 수량 변경 이벤트를 전달합니다.

## 2. 오브젝트 풀링과 Tick 연산 최적화

### 문제

Enemy와 Projectile이 짧은 주기로 생성·제거되면서 `SpawnActor`/`Destroy`에 따른 초기화와 메모리 할당 비용이 특정 프레임에 집중됐습니다.

초기 Enemy 풀링 적용에서는 평균 FPS가 **43.25 → 33.51**로 22.5% 감소하고, Hitch가 **111회 → 174회**로 증가했습니다. 풀의 존재만으로 성능이 개선되는 것이 아니라, 비활성 객체의 실행 상태까지 함께 관리해야 한다는 점을 확인했습니다.

### 분석과 해결

Unreal Insights로 비활성 Enemy를 추적한 결과, 화면에서 숨긴 뒤에도 AI·Movement 컴포넌트 Tick이 계속 실행되고 재사용 시 AIController가 다시 생성되는 경로가 있었습니다.

- `UWorldSubsystem`이 클래스별 활성/비활성 인스턴스를 관리하도록 했습니다.
- `IPoolable` 인터페이스로 획득과 반환 시점의 초기화 계약을 통일했습니다.
- 반환 시 Actor, CharacterMovement, SkeletalMesh, Brain, PathFollowing Tick과 충돌을 중지했습니다.
- AIController와 Pawn의 Possess 관계는 유지해 재획득 시 Controller가 누적 생성되지 않도록 했습니다.
- 레벨별 `Pool Profile`에서 클래스별 PreWarm 수량, 확장 허용 여부, 비활성 보관 상한을 설정하도록 분리했습니다.
- Projectile은 이동 속도, 충돌, Niagara 이펙트와 내부 타이머를 재설정해 같은 인스턴스를 안전하게 재사용하도록 했습니다.

### 측정 결과

동일한 프레임 테스트 시나리오에서 일반 생성·제거 방식과 풀링 방식을 비교한 결과입니다. 수치는 실행 환경에 따라 달라질 수 있습니다.

| 항목 | 일반 방식 | Pooling | 변화 |
| --- | ---: | ---: | ---: |
| Enemy 활성화 1회 비용 | 938 μs | 302 μs | **67.7% 감소** |
| Enemy 비활성화 1회 비용 | 138 μs | 68.1 μs | **50.5% 감소** |
| 평균 FPS | 41.4 | 42.5 | **2.6% 증가** |
| P99 프레임 | 56.6 ms | 37.2 ms | **34.3% 감소** |
| 50 ms 초과 프레임 | 59.5회 | 1.5회 | **97.4% 감소** |

평균 FPS 상승보다 생성 비용이 몰리는 프레임의 스파이크와 긴 프레임을 줄이는 데 더 큰 효과가 있었습니다.

### 핵심 클래스

- [`UPeCoPoolSubsystem`](Source/PestControl/20_System/Pool/PeCoPoolSubsystem.cpp#L160-L329): 객체의 획득, 반환, 중복 반환 방어와 PreWarm 정책을 관리합니다.
- [`APeCoEnemyCharacter`](Source/PestControl/01_Character/PeCoEnemyCharacter.cpp#L432-L574): Enemy의 이동, 충돌, 애니메이션과 AI 실행 상태를 복원·정리합니다.
- [`AProjectile`](Source/PestControl/07_Weapon/Projectile.cpp#L84-L170): Projectile의 이동, 충돌, Niagara 이펙트와 내부 타이머를 재설정합니다.

## 3. MVP 패턴을 이용한 플레이어 스탯 리팩터링

### 문제

초기에는 플레이어 능력치 변경, 게임플레이 처리, UI 갱신이 서로 직접 연결되어 있었습니다. 기능을 추가할수록 한 시스템의 수정이 다른 시스템에 영향을 주었고, UI가 게임플레이 데이터를 반복 조회하는 구조도 생겼습니다.

### 해결

플레이어 능력치 시스템을 Model–Presenter–View로 분리했습니다.

| 역할 | 클래스 | 책임 |
| --- | --- | --- |
| Model | `APeCoPlayerState`, `FPeCoStatData` | 스탯 데이터 소유, 계산과 변경 이벤트 발행 |
| Presenter | `UPlayerStatPresenterComponent` | 외부 변경 요청을 Model에 전달하고 변경 이벤트를 View용 이벤트로 변환 |
| View | `UPlayerOverlay`와 UMG Blueprint | Presenter의 이벤트를 구독해 화면 표시만 갱신 |

```text
게임플레이 요청 → Presenter → PlayerState(Model)
                              ↓ OnStatChanged
                    Presenter의 View 이벤트
                              ↓
                      UMG Blueprint(View)
```

Model은 데이터와 규칙, Presenter는 변경 요청과 이벤트 전달, View는 표현만 담당하도록 분리했습니다. 그 결과 스탯이나 UI 기능을 변경할 때 다른 계층까지 수정하던 범위를 줄이고 각 계층을 독립적으로 확장할 수 있게 됐습니다.

### 핵심 클래스

- [`APeCoPlayerState`](Source/PestControl/02_Player/PeCoPlayerState.h#L51-L87): Model로서 플레이어 스탯을 소유하고 값 변경을 담당합니다.
- [`UPlayerStatPresenterComponent`](Source/PestControl/02_Player/Components/PlayerStatPresenterComponent.cpp#L24-L125): Model의 변경 이벤트를 View 이벤트로 변환하고 외부 요청을 전달합니다.
- [`UPlayerOverlay`](Source/PestControl/04_UI/PlayerOverlay.h#L21-L49): View로서 Presenter가 제공한 데이터를 UMG Blueprint에 표시합니다.

## 그 외 담당 시스템

### 인벤토리와 장비

- `UInventoryComponent`에서 아이템과 재화를 관리하고 추가·제거·수량 조회 기능을 구현했습니다.
- `UEquipmentComponent`에서 장착, 사용, 슬롯별 쿨다운을 분리했습니다.
- 슬롯과 아이템 타입을 Gameplay Tag로 비교해 새로운 장비 타입을 확장할 수 있도록 구성했습니다.

### 상점과 재화

- 아이템 구매·판매와 능력치 업그레이드 흐름을 구현했습니다.
- 가격과 업그레이드 수치는 Data Table과 Curve Table로 관리했습니다.
- 상점 아이템은 `TSoftClassPtr`과 비동기 로딩을 사용하고, 로딩 핸들을 보관해 완료 전 해제되지 않도록 했습니다.

### UI

체력, 경험치, 게임 시간, 인벤토리, 상점, 보상, 게임 오버 화면을 포함한 인게임 UI를 담당했습니다. C++에서 게임플레이 데이터를 제공하고 UMG Blueprint에서 표현과 애니메이션을 담당하도록 역할을 나눴습니다.

## 저장소 구성

이 저장소는 포트폴리오 코드 검토를 위해 C++ 소스와 최소 프로젝트 설정만 공개하며, 용량이 큰 `Content` 에셋은 포함하지 않습니다. 따라서 저장소만으로 게임을 실행할 수는 없으며, 구현은 Unreal Engine 5.5 기준의 [`Source/PestControl`](Source/PestControl)에서 확인할 수 있습니다.
