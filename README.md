# No Bug Company: 탑다운 로그라이크 슈터

No Bug Company 프로젝트 리포지토리 입니다. 소스 코드 구조 및 클래스에 대한 설명입니다.

## 프로젝트 개요

No Bug Company는 언리얼 엔진5로 제작된  2개의 스테이지로 이루어진 탑다운 생존 로그라이크 게임입니다. 
플레이어는 곤충과 같은 적("해충") 무리와 싸우는 캐릭터 역할을 맡습니다. 핵심 게임 플레이 루프는 정해진 시간 동안 생존하고, 적을 물리쳐 경험치와 재료를 얻고, 레벨을 올리고, 다양한 "해충 방제" 테마의 무기를 업그레이드하는 것입니다. 

## 핵심 C++ 클래스 및 컴포넌트

### 주요 클래스

#### `APeCoGameMode`
- **주요 기능(책임):**
    - 스테이지 및 라운드 시작/종료를 포함한 전반적인 게임 흐름을 관리합니다.
    - `APeCoEnemySpawner`를 통해 적의 스폰을 제어합니다.
    - 스테이지 타이머, 현재 라운드, 활성화된 적의 수와 같은 게임의 중요 데이터를 추적합니다.
    - 게임 내 상점 기능을 관리하는 `UStoreComponent`를 포함합니다.
    - 캐릭터 간의 최종 데미지를 계산합니다.

#### `APeCoPlayerCharacter`
플레이어가 직접 조종하는 메인 캐릭터입니다.
- **주요 기능(책임):**
    - 탑다운 시점의 카메라를 구현합니다.
    - `UInventoryComponent`를 통해 플레이어의 아이템 소지 현황을 관리합니다.
    - `UEquipmentComponent`를 통해 무기 장착 및 해제를 처리합니다.
    - 공격을 회피하기 위한 구르기 스킬을 사용 할 수 있습니다.
    - 적과 충돌하여 피해를 입은 후 일시적인 무적 상태가 되는 기능이 있습니다.
  
#### `APeCoEnemyCharacter`
게임에 등장하는 모든 "해충" 적의 기반이 되는 클래스입니다.
- **주요 기능(책임):**
    - 표준화된 데미지 처리를 위해 `ICombatInterface`를 구현합니다.
    - 고유한 `EnemyID` (FName)와 `EnemyTag` (FGameplayTag)로 식별됩니다.
    - 체력, 데미지 같은 스탯을 가지며, 지상(Ground) 또는 비행(Flying) 타입입니다.
    - 둔화, 기절, 약화, 화상 등 다양한 상태 이상의 영향을 받을 수 있습니다.
    - 데이터 에셋에 정의된 드롭 테이블에 따라 죽을 때 아이템을 드롭합니다.

#### `AWeapon`
모든 플레이어 무기의 기반 클래스입니다.
- **주요 기능(책임):**
    - `BaseDamage`, `CriticalChance`, `Cooldown`, `Ammo`, `Range` 등 포괄적인 스탯 세트를 가집니다.
    - 스탯 수정 및 밸런싱이 용이하도록 `UDataTable`을 사용했습니다.
    - 다양한 무기 타입을 수용하기 위해 여러 발사 패턴(`Projectile`, `Shotgun`, `Conical`)을 사용합니다.
    - **강화**와 **진화** 두 단계의 견고한 업그레이드 시스템을 통해 무기를 성장시킬 수 있습니다.
    - 델리게이트 콜백을 통해 발사, 재장전 등 자신의 상태를 UI에 전달합니다.

#### `UInventoryComponent`
플레이어의 아이템과 재화를 관리하는 컴포넌트입니다.
- **주요 기능(책임):**
    - 아이템 추가, 제거, 쿼리 기능을 담당합니다.
    - 플레이어의 재화(돈)를 관리하며, 거래나 제작 시 재료 및 재화가 충분한지 확인합니다.

#### `UEquipmentComponent`
플레이어가 장착한 아이템을 관리합니다.
- **주요 기능(책임):**
    - `FGameplayTag`를 사용하여 장비 슬롯을 식별하고, 해당 슬롯의 아이템 장착, 해제, 사용을 처리합니다.
    - 아이템 타입별 재사용 대기시간(쿨다운)을 관리합니다.

#### `UStoreComponent`
`GameMode`에 부착되어 게임 내 상점의 로직을 관리합니다.
- **주요 기능(책임):**
    - 아이템의 구매 및 판매 가격을 정의합니다.
    - 아이템과 재료의 구매 및 판매를 처리합니다.
    - 플레이어에게 무작위 보상을 제공하는 로직을 포함합니다.
## 주요 개발 내용
제가 직접 설계하고 구현한 핵심 시스템들입니다. 각 시스템은 확장성과 유지보수성을 고려하여 컴포넌트 기반으로 설계되었습니다.

#### 1. 아이템 시스템 (Item Component)
아이템 자체의 데이터와 기능 로직을 PeCoItemComponent로 분리하여, 아이템 액터가 인벤토리 시스템이나 장비 시스템에 종속되지 않도록 설계했습니다. 이를 통해 각 아이템은 고유한 속성(중첩 가능 여부, 수량 등)과 사용 효과를 가질 수 있으며, 시스템 간의 결합도를 낮춰 유지보수 및 확장이 용이하도록 만들었습니다.

관련 소스 코드:

Source/PestControl/09_Items/Components/PeCoItemComponent.h
Source/PestControl/09_Items/Components/PeCoItemComponent.cpp

#### 2. 상점 및 재화 시스템 (Store & Currency System)
플레이어가 게임 내에서 획득한 재화로 아이템이나 능력치를 구매하고, 소지한 아이템을 판매할 수 있는 상점 시스템을 구현했습니다. UStoreComponent는 APeCoGameMode에 부착되어 게임의 전반적인 상점 로직을 관리하며, UInventoryComponent와 상호작용하여 플레이어의 재화를 직접 관리합니다. 데이터 테이블을 활용하여 아이템 가격 및 능력치 업그레이드 비용을 유연하게 조절할 수 있도록 설계했습니다.

관련 소스 코드:
[StoreComponent.h](https://github.com/Seo-BJ/No-Bug-Company/blob/develop/Source/PestControl/00_GameModes/Components/StoreComponent.h)
[StoreComponent.cpp](https://github.com/Seo-BJ/No-Bug-Company/blob/develop/Source/PestControl/00_GameModes/Components/StoreComponent.cpp)
[InventoryComponent.h](https://github.com/Seo-BJ/No-Bug-Company/blob/develop/Source/PestControl/01_Character/Components/InventoryComponent.h) (재화 관리 부분)

#### 3. 인벤토리 시스템 (Inventory System)
UInventoryComponent를 통해 플레이어가 소지한 모든 아이템과 재화를 관리합니다. 아이템의 추가, 제거, 수량 확인 등의 기능을 제공하며, UEquipmentComponent와 연동하여 아이템 장착 및 해제 로직을 처리합니다. 아이템 데이터는 FGameplayTag을 통해 효율적으로 관리됩니다.

관련 소스 코드:

Source/PestControl/01_Character/Components/InventoryComponent.h
Source/PestControl/01_Character/Components/InventoryComponent.cpp

#### 4. 플레이어 상태 및 능력치 시스템 (Player State & Stat System)
APeCoPlayerState를 활용하여 플레이어의 레벨, 킬 카운트, 체력, 이동 속도 등 핵심 데이터를 관리합니다. 능력치는 FPeCoStatData 구조체로 정의되어 기본 값, 추가 보너스, 배율 보너스 등을 종합적으로 계산할 수 있습니다. UPlayerStatPresenterComponent는 APeCoPlayerState의 데이터 변경을 감지하고, UI에 이를 알리는 역할을 하여 데이터와 표현을 분리했습니다.

관련 소스 코드:

[PeCoPlayerState.h](https://github.com/Seo-BJ/No-Bug-Company/blob/develop/Source/PestControl/02_Player/PeCoPlayerState.h)
[PeCoPlayerState.h](https://github.com/Seo-BJ/No-Bug-Company/blob/develop/Source/PestControl/02_Player/PeCoPlayerState.cpp)
[PlayerStats.h](https://github.com/Seo-BJ/No-Bug-Company/blob/develop/Source/PestControl/02_Player/PlayerStats.h)
[PlayerStatPresenterComponent.h](https://github.com/Seo-BJ/No-Bug-Company/blob/develop/Source/PestControl/02_Player/Components/PlayerStatPresenterComponent.h)

#### 5. UI 시스템 (User Interface)
게임의 전체적인 UI를 전담하여 개발했습니다. 플레이어의 상태(체력, 경험치), 게임 시간, 인벤토리, 상점, 게임 오버 화면 등 사용자와 상호작용하는 모든 시각적 요소를 UMG를 활용하여 구현했습니다. C++ 클래스와 블루프린트 위젯을 연동하여 데이터 바인딩 및 이벤트 처리를 효율적으로 관리했습니다.

관련 소스 코드:

[PeCoHUD.h](https://github.com/Seo-BJ/No-Bug-Company/blob/develop/Source/PestControl/04_UI/PeCoHUD.h)
[PeCoHUD.h](https://github.com/Seo-BJ/No-Bug-Company/blob/develop/Source/PestControl/04_UI/PeCoHUD.cpp)
[PeCoUserWidget.h](https://github.com/Seo-BJ/No-Bug-Company/blob/develop/Source/PestControl/04_UI/PeCoUserWidget.h)
[PlayerOverlay.h](https://github.com/Seo-BJ/No-Bug-Company/blob/develop/Source/PestControl/04_UI/PlayerOverlay.h)

## 디렉토리 구조

### C++ 소스 (`Source/PestControl`)
C++ 코드는 게임 플레이 기능별로 다음 디렉토리로 구성됩니다:
- `00_GameModes`: 게임 모드 및 게임 스테이트 클래스를 포함합니다.
- `01_Character`: `InventoryComponent`와 같은 핵심 컴포넌트와 함께 플레이어 및 적 캐릭터의 기본 클래스를 포함합니다.
- `02_Player`: `PlayerController`, `PlayerState`와 같은 플레이어 전용 클래스를 포함합니다.
- `03_Input`: `InputActions` 및 `InputComponent`를 포함한 플레이어 입력을 처리합니다.
- `04_UI`: HUD 및 다양한 위젯을 포함한 사용자 인터페이스의 C++ 백엔드를 담당합니다.
- `05_Animation`: `AnimInstance`와 같은 애니메이션 관련 클래스를 포함합니다.
- `07_Weapon`: 기본 `Weapon` 클래스 및 다양한 구현을 포함한 모든 무기 관련 클래스를 포함합니다.
- `09_Items`: 소모품 및 전투 아이템에 대한 로직을 포함합니다.
- `10_Enemy`: AI 컨트롤러, 비헤이비어 트리, 태스크 및 적 전용 캐릭터 클래스를 포함합니다.
- `20_System`: 범용 시스템 클래스 및 함수 라이브러리를 포함합니다.
- `21_Data`: 데이터 테이블의 구조를 정의하는 C++ 구조체 및 열거형을 포함합니다.
- `22_Level`: 레벨별 오브젝트 및 상호작용과 관련된 클래스를 포함합니다.
