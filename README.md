# No Bug Company

언리얼 엔진으로 제작한 3D 탑다운 생존 로그라이크 슈팅 게임입니다.

플레이어는 몰려오는 해충과 싸우며 경험치와 재료를 획득하고, 다양한 해충 방제 테마의 무기와 능력치를 강화하여 정해진 시간 동안 생존해야 합니다.


## 프로젝트 정보

- 개발 기간: 2024.09.05 ~ 2024.12.20
- 개발 인원: 5인 팀 프로젝트
- 담당 역할: 팀장, 게임플레이 및 UI 개발
- 사용 기술: Unreal Engine, C++, Blueprint, Gameplay Tag, Data Table, UMG, Material
- 플랫폼: Windows

## 담당한 주요 시스템

아래 시스템은 제가 직접 설계하고 구현한 부분입니다. 기능 추가와 밸런싱이 쉽도록 데이터와 로직을 분리하고, 시스템 간 의존성을 낮추는 방향으로 설계했습니다.

### 1. 데이터 기반 아이템 및 보상 시스템

다양한 무기, 아이템, 보상 로직으로 인해 테스트와 밸런싱 작업이 복잡해지는 문제를 해결하기 위해 데이터 테이블과 게임플레이 태그를 사용했습니다.

아이템 자체의 데이터와 기능은 `UPeCoItemComponent`로 분리하여 아이템 액터가 인벤토리나 장비 시스템에 직접 종속되지 않도록 했습니다. 새로운 보상 아이템을 추가할 때 기존 확률을 다시 계산하지 않아도 되도록 가중치 기반 랜덤 보상 방식도 적용했습니다.

관련 소스 코드:

- [PeCoItemComponent.h](Source/PestControl/09_Items/Components/PeCoItemComponent.h)
- [PeCoItemComponent.cpp](Source/PestControl/09_Items/Components/PeCoItemComponent.cpp)
- [PeCoDataRow.h](Source/PestControl/21_Data/PeCoDataRow.h)

### 2. 인벤토리 및 장비 시스템

`UInventoryComponent`에서 플레이어가 소지한 아이템과 재화를 관리하고, 아이템 추가, 제거, 수량 확인 기능을 제공하도록 했습니다.

장착과 해제, 아이템 사용과 쿨다운 처리는 `UEquipmentComponent`로 분리했습니다. 장비 슬롯은 게임플레이 태그로 구분하여 새로운 장비 타입을 추가하기 쉽도록 구성했습니다.

관련 소스 코드:

- [InventoryComponent.h](Source/PestControl/01_Character/Components/InventoryComponent.h)
- [InventoryComponent.cpp](Source/PestControl/01_Character/Components/InventoryComponent.cpp)
- [EquipmentComponent.h](Source/PestControl/01_Character/Components/EquipmentComponent.h)
- [EquipmentComponent.cpp](Source/PestControl/01_Character/Components/EquipmentComponent.cpp)

### 3. 상점 및 재화 시스템

플레이어가 획득한 재화로 아이템이나 능력치를 구매하고, 소지한 아이템을 판매할 수 있는 상점 시스템을 구현했습니다.

`UStoreComponent`는 게임 모드에 부착되어 상점의 주요 로직을 관리하고, `UInventoryComponent`와 상호작용하여 재화와 아이템을 처리합니다. 아이템 가격과 능력치 업그레이드 비용은 데이터 테이블로 관리하여 코드 수정 없이 값을 조정할 수 있도록 했습니다.

관련 소스 코드:

- [StoreComponent.h](Source/PestControl/00_GameModes/Components/StoreComponent.h)
- [StoreComponent.cpp](Source/PestControl/00_GameModes/Components/StoreComponent.cpp)
- [PeCoGameMode.h](Source/PestControl/00_GameModes/PeCoGameMode.h)

### 4. 플레이어 능력치 시스템과 MVP 패턴

초기에는 플레이어의 능력치, 아이템, 버프 시스템이 서로 강하게 결합되어 있었습니다. 각 클래스의 책임과 의존성 방향을 명확하게 만들기 위해 플레이어 능력치 시스템에 MVP 패턴을 적용했습니다.

- `APeCoPlayerState`는 Model로서 스탯 데이터를 소유하고 데이터 변경을 담당합니다.
- `UPlayerStatPresenterComponent`는 스탯 변경을 요청하고 변경 이벤트를 구독하여 View에 전달합니다.
- `UPlayerOverlay`는 View로서 전달받은 데이터로 UI를 갱신합니다.

관련 소스 코드:

- [PeCoPlayerState.h](Source/PestControl/02_Player/PeCoPlayerState.h)
- [PeCoPlayerState.cpp](Source/PestControl/02_Player/PeCoPlayerState.cpp)
- [PlayerStats.h](Source/PestControl/02_Player/PlayerStats.h)
- [PlayerStatPresenterComponent.h](Source/PestControl/02_Player/Components/PlayerStatPresenterComponent.h)
- [PlayerOverlay.h](Source/PestControl/04_UI/PlayerOverlay.h)

### 5. UI 제작 및 Tick 연산 최적화

플레이어의 체력과 경험치, 게임 시간, 인벤토리, 상점, 게임 오버 화면을 포함한 전체적인 인게임 및 로비 UI를 담당했습니다.

C++ 클래스와 블루프린트 위젯을 연결하고, 값이 변경될 때만 UI가 반응하도록 이벤트 기반으로 구성하여 불필요한 Tick 사용을 줄였습니다. 머티리얼을 사용하여 마우스 입력에 반응하는 원형 인벤토리 UI도 제작했습니다.

적 애니메이션에서는 Render 시 Tick Pose 활성화와 Blueprint Thread Safe Update를 적용하여 게임 스레드의 Tick 연산을 줄였습니다.

관련 소스 코드:

- [PeCoHUD.h](Source/PestControl/04_UI/PeCoHUD.h)
- [PeCoHUD.cpp](Source/PestControl/04_UI/PeCoHUD.cpp)
- [PeCoUserWidget.h](Source/PestControl/04_UI/PeCoUserWidget.h)
- [PlayerOverlay.h](Source/PestControl/04_UI/PlayerOverlay.h)
