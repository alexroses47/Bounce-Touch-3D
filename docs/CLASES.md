# Mapa de clases de Bounce.exe

Generado con `tools/classes_doc.py` a partir de la RTTI y las vtables del binario (`tools/rtti_scan.py`). **Antes de reimplementar un método, mira aquí qué versión se ejecuta para ese objeto:** la entrada de la vtable de su propia clase.

- *Entrada*: índice en la vtable (las 0 y 1 son los destructores en los descendientes de `lang::Object`).
- *Implementa*: la clase que aporta esa versión. **Negrita** = la sobrescribe esta clase.
- Los nombres `Clase::vNN` de Ghidra siguen esta misma regla.

## Jerarquía (clases del juego)

```
lang::Object
  Action  [2 entradas]
lang::Object
  ActionBase
    ActionActivateRandomTrigger  [3 entradas]
    ActionAddImpulse  [3 entradas]
    ActionAddLife  [3 entradas]
    ActionAddScore  [3 entradas]
    ActionAddSpeed  [3 entradas]
    ActionBreak  [3 entradas]
    ActionChangeBall  [3 entradas]
    ActionCheckPoint  [3 entradas]
    ActionCounter  [3 entradas]
    ActionDelay  [3 entradas]
    ActionJumpToAction  [3 entradas]
    ActionJumpToRandomAction  [3 entradas]
    ActionLevelComplete  [3 entradas]
    ActionLevelFailed  [3 entradas]
    ActionLinkNode  [3 entradas]
    ActionLoadLevel  [3 entradas]
    ActionModifyVariable  [3 entradas]
    ActionMoveObject  [3 entradas]
    ActionNop  [3 entradas]
    ActionParticleFX  [3 entradas]
    ActionPlayAnimation  [3 entradas]
    ActionPlaySound  [3 entradas]
    ActionPressure  [3 entradas]
    ActionRandomDelay  [3 entradas]
    ActionSetBallDirection  [3 entradas]
    ActionSetBallMaxSpeed  [3 entradas]
    ActionSetCameraSpringProperties  [3 entradas]
    ActionSetCameraView  [3 entradas]
    ActionSetGameMode  [3 entradas]
    ActionSetGameObjectPosition  [3 entradas]
    ActionSetGravity  [3 entradas]
    ActionSetGuidingObject  [3 entradas]
    ActionSetObjectState  [3 entradas]
    ActionSetTriggerState  [3 entradas]
    ActionSetVariable  [3 entradas]
    ActionShowText  [3 entradas]
    ActionSpeedMultiply  [3 entradas]
    ActionStopSound  [3 entradas]
    ActionSwitchObjectState  [3 entradas]
    ActionTestVariable  [3 entradas]
    ActionTimer  [3 entradas]
    ActionUnLinkNode  [3 entradas]
    ActionWaitForIdle  [3 entradas]
    ActionWaitForKeyHold  [3 entradas]
    ActionWaitForKeyPress  [3 entradas]
    ActionWaitForKeyRelease  [3 entradas]
lang::Object
  AnimationItem  [2 entradas]
lang::Object
  AnimationProperty  [2 entradas]
lang::Object
  AudioItem  [2 entradas]
hgr::Node
  AudioObject  [4 entradas]
framework::App
  BounceApp  [9 entradas]
lang::Object
  CollisionGeometry  [2 entradas]
hgr::Node
  CollisionObject  [3 entradas]
lang::Object
  CollisionSpace  [2 entradas]
lang::Object
  GameFlowManager  [2 entradas]
lang::Object
  GameObject  [16 entradas]
    ActiveGameObject  [17 entradas]
      Ball  [20 entradas]
        PlayerBall  [20 entradas]
    GameCamera  [16 entradas]
    Trigger  [16 entradas]
lang::Object
  GameScene  [17 entradas]
    AttractionScene  [17 entradas]
    CutsceneScene  [17 entradas]
    DeathScene  [17 entradas]
    Game  [17 entradas]
    LoadingScreenScene  [17 entradas]
    Menu  [17 entradas]
    PopupScene  [17 entradas]
    WorldMap  [17 entradas]
lang::Object
  GameSettings  [2 entradas]
(sin base)
  IAchievements
    Achievements  [3 entradas]
(sin base)
  IAudioManager  [8 entradas]
    AudioManagerS60  [11 entradas]
(sin base)
  IGameSaves
    GameSaves  [4 entradas]
(sin base)
  IResources  [43 entradas]
    Resources  [43 entradas]
lang::Object
  MenuItem  [31 entradas]
    ContainerItem  [31 entradas]
    ImageItem  [31 entradas]
    KeyConfigurator  [31 entradas]
    SliderController  [31 entradas]
    TextItem  [31 entradas]
lang::Object
  MenuPage  [2 entradas]
framework::OSInterface
  OSSS60  [4 entradas]
lang::Object
  ResourceManager  [2 entradas]
(sin base)
  ResourceObject
    IGameFont
      CustomBitmapFontResource  [6 entradas]
    SpriteSheet  [2 entradas]
    TextGroup  [2 entradas]
lang::Object
  animationData  [2 entradas]
lang::Object
  moveAnimationData  [2 entradas]
```

## Vtables

### Achievements

Origen: juego · base: IAchievements · 3 entradas, 3 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0xf624` | **Achievements** |
| 1 | `0x149998` | **Achievements** |
| 2 | `0x14998c` | **Achievements** |

### Action

Origen: juego · base: lang::Object · 2 entradas, 2 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x14df08` | **Action** |
| 1 | `0x14deac` | **Action** |

### ActionActivateRandomTrigger

Origen: juego · base: ActionBase → lang::Object · 3 entradas, 3 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x14bec8` | **ActionActivateRandomTrigger** |
| 1 | `0x14be80` | **ActionActivateRandomTrigger** |
| 2 | `0x1490d4` | **ActionActivateRandomTrigger** |

### ActionAddImpulse

Origen: juego · base: ActionBase → lang::Object · 3 entradas, 3 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x14aae0` | **ActionAddImpulse** |
| 1 | `0x14aaa0` | **ActionAddImpulse** |
| 2 | `0x1490d4` | **ActionAddImpulse** |

### ActionAddLife

Origen: juego · base: ActionBase → lang::Object · 3 entradas, 3 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x149a44` | **ActionAddLife** |
| 1 | `0x149a2c` | **ActionAddLife** |
| 2 | `0x1490d4` | **ActionAddLife** |

### ActionAddScore

Origen: juego · base: ActionBase → lang::Object · 3 entradas, 3 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x14a128` | **ActionAddScore** |
| 1 | `0x14a110` | **ActionAddScore** |
| 2 | `0x1490d4` | **ActionAddScore** |

### ActionAddSpeed

Origen: juego · base: ActionBase → lang::Object · 3 entradas, 3 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x14a174` | **ActionAddSpeed** |
| 1 | `0x14a134` | **ActionAddSpeed** |
| 2 | `0x1490d4` | **ActionAddSpeed** |

### ActionBreak

Origen: juego · base: ActionBase → lang::Object · 3 entradas, 3 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x1497f4` | **ActionBreak** |
| 1 | `0x1497dc` | **ActionBreak** |
| 2 | `0x1490d4` | **ActionBreak** |

### ActionChangeBall

Origen: juego · base: ActionBase → lang::Object · 3 entradas, 3 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x14ab2c` | **ActionChangeBall** |
| 1 | `0x14ab14` | **ActionChangeBall** |
| 2 | `0x1490d4` | **ActionChangeBall** |

### ActionCheckPoint

Origen: juego · base: ActionBase → lang::Object · 3 entradas, 3 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x14ab78` | **ActionCheckPoint** |
| 1 | `0x14ab38` | **ActionCheckPoint** |
| 2 | `0x1490d4` | **ActionCheckPoint** |

### ActionCounter

Origen: juego · base: ActionBase → lang::Object · 3 entradas, 3 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x149a68` | **ActionCounter** |
| 1 | `0x149a50` | **ActionCounter** |
| 2 | `0x1490d4` | **ActionCounter** |

### ActionDelay

Origen: juego · base: ActionBase → lang::Object · 3 entradas, 3 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x149854` | **ActionDelay** |
| 1 | `0x149814` | **ActionDelay** |
| 2 | `0x149800` | **ActionDelay** |

### ActionJumpToAction

Origen: juego · base: ActionBase → lang::Object · 3 entradas, 3 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x14b2f0` | **ActionJumpToAction** |
| 1 | `0x14b2d8` | **ActionJumpToAction** |
| 2 | `0x14b2cc` | **ActionJumpToAction** |

### ActionJumpToRandomAction

Origen: juego · base: ActionBase → lang::Object · 3 entradas, 3 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x14bdc8` | **ActionJumpToRandomAction** |
| 1 | `0x14bd94` | **ActionJumpToRandomAction** |
| 2 | `0x1490d4` | **ActionJumpToRandomAction** |

### ActionLevelComplete

Origen: juego · base: ActionBase → lang::Object · 3 entradas, 3 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x14b5e0` | **ActionLevelComplete** |
| 1 | `0x14b5c8` | **ActionLevelComplete** |
| 2 | `0x1490d4` | **ActionLevelComplete** |

### ActionLevelFailed

Origen: juego · base: ActionBase → lang::Object · 3 entradas, 3 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x14af4c` | **ActionLevelFailed** |
| 1 | `0x14af34` | **ActionLevelFailed** |
| 2 | `0x1490d4` | **ActionLevelFailed** |

### ActionLinkNode

Origen: juego · base: ActionBase → lang::Object · 3 entradas, 3 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x14a20c` | **ActionLinkNode** |
| 1 | `0x14a1a8` | **ActionLinkNode** |
| 2 | `0x1490d4` | **ActionLinkNode** |

### ActionLoadLevel

Origen: juego · base: ActionBase → lang::Object · 3 entradas, 3 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x14a6a0` | **ActionLoadLevel** |
| 1 | `0x14a660` | **ActionLoadLevel** |
| 2 | `0x1490d4` | **ActionLoadLevel** |

### ActionModifyVariable

Origen: juego · base: ActionBase → lang::Object · 3 entradas, 3 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x14b918` | **ActionModifyVariable** |
| 1 | `0x14b8a8` | **ActionModifyVariable** |
| 2 | `0x1490d4` | **ActionModifyVariable** |

### ActionMoveObject

Origen: juego · base: ActionBase → lang::Object · 3 entradas, 3 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x14ac30` | **ActionMoveObject** |
| 1 | `0x14abc0` | **ActionMoveObject** |
| 2 | `0x14abac` | **ActionMoveObject** |

### ActionNop

Origen: juego · base: ActionBase → lang::Object · 3 entradas, 3 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x14e644` | **ActionNop** |
| 1 | `0x14e62c` | **ActionNop** |
| 2 | `0x1490d4` | **ActionNop** |

### ActionParticleFX

Origen: juego · base: ActionBase → lang::Object · 3 entradas, 3 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x14ad04` | **ActionParticleFX** |
| 1 | `0x14ac94` | **ActionParticleFX** |
| 2 | `0x1490d4` | **ActionParticleFX** |

### ActionPlayAnimation

Origen: juego · base: ActionBase → lang::Object · 3 entradas, 3 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x14b684` | **ActionPlayAnimation** |
| 1 | `0x14b614` | **ActionPlayAnimation** |
| 2 | `0x14b5ec` | **ActionPlayAnimation** |

### ActionPlaySound

Origen: juego · base: ActionBase → lang::Object · 3 entradas, 3 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x14a6ec` | **ActionPlaySound** |
| 1 | `0x14a6d4` | **ActionPlaySound** |
| 2 | `0x1490d4` | **ActionPlaySound** |

### ActionPressure

Origen: juego · base: ActionBase → lang::Object · 3 entradas, 3 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x14a27c` | **ActionPressure** |
| 1 | `0x14a264` | **ActionPressure** |
| 2 | `0x1490d4` | **ActionPressure** |

### ActionRandomDelay

Origen: juego · base: ActionBase → lang::Object · 3 entradas, 3 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x14af8c` | **ActionRandomDelay** |
| 1 | `0x14af74` | **ActionRandomDelay** |
| 2 | `0x14af58` | **ActionRandomDelay** |

### ActionSetBallDirection

Origen: juego · base: ActionBase → lang::Object · 3 entradas, 3 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x14bbec` | **ActionSetBallDirection** |
| 1 | `0x14bbac` | **ActionSetBallDirection** |
| 2 | `0x1490d4` | **ActionSetBallDirection** |

### ActionSetBallMaxSpeed

Origen: juego · base: ActionBase → lang::Object · 3 entradas, 3 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x14ba84` | **ActionSetBallMaxSpeed** |
| 1 | `0x14ba44` | **ActionSetBallMaxSpeed** |
| 2 | `0x1490d4` | **ActionSetBallMaxSpeed** |

### ActionSetCameraSpringProperties

Origen: juego · base: ActionBase → lang::Object · 3 entradas, 3 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x14ccec` | **ActionSetCameraSpringProperties** |
| 1 | `0x14ccd4` | **ActionSetCameraSpringProperties** |
| 2 | `0x1490d4` | **ActionSetCameraSpringProperties** |

### ActionSetCameraView

Origen: juego · base: ActionBase → lang::Object · 3 entradas, 3 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x14b740` | **ActionSetCameraView** |
| 1 | `0x14b6e8` | **ActionSetCameraView** |
| 2 | `0x1490d4` | **ActionSetCameraView** |

### ActionSetGameMode

Origen: juego · base: ActionBase → lang::Object · 3 entradas, 3 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x14afb0` | **ActionSetGameMode** |
| 1 | `0x14af98` | **ActionSetGameMode** |
| 2 | `0x1490d4` | **ActionSetGameMode** |

### ActionSetGameObjectPosition

Origen: juego · base: ActionBase → lang::Object · 3 entradas, 3 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x14bf60` | **ActionSetGameObjectPosition** |
| 1 | `0x14bf08` | **ActionSetGameObjectPosition** |
| 2 | `0x1490d4` | **ActionSetGameObjectPosition** |

### ActionSetGravity

Origen: juego · base: ActionBase → lang::Object · 3 entradas, 3 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x14ad80` | **ActionSetGravity** |
| 1 | `0x14ad68` | **ActionSetGravity** |
| 2 | `0x1490d4` | **ActionSetGravity** |

### ActionSetGuidingObject

Origen: juego · base: ActionBase → lang::Object · 3 entradas, 3 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x14bc60` | **ActionSetGuidingObject** |
| 1 | `0x14bc20` | **ActionSetGuidingObject** |
| 2 | `0x1490d4` | **ActionSetGuidingObject** |

### ActionSetObjectState

Origen: juego · base: ActionBase → lang::Object · 3 entradas, 3 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x14b9d4` | **ActionSetObjectState** |
| 1 | `0x14b97c` | **ActionSetObjectState** |
| 2 | `0x1490d4` | **ActionSetObjectState** |

### ActionSetTriggerState

Origen: juego · base: ActionBase → lang::Object · 3 entradas, 3 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x14baf8` | **ActionSetTriggerState** |
| 1 | `0x14bab8` | **ActionSetTriggerState** |
| 2 | `0x1490d4` | **ActionSetTriggerState** |

### ActionSetVariable

Origen: juego · base: ActionBase → lang::Object · 3 entradas, 3 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x14affc` | **ActionSetVariable** |
| 1 | `0x14afbc` | **ActionSetVariable** |
| 2 | `0x1490d4` | **ActionSetVariable** |

### ActionShowText

Origen: juego · base: ActionBase → lang::Object · 3 entradas, 3 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x14a2c8` | **ActionShowText** |
| 1 | `0x14a288` | **ActionShowText** |
| 2 | `0x1490d4` | **ActionShowText** |

### ActionSpeedMultiply

Origen: juego · base: ActionBase → lang::Object · 3 entradas, 3 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x14b7cc` | **ActionSpeedMultiply** |
| 1 | `0x14b78c` | **ActionSpeedMultiply** |
| 2 | `0x1490d4` | **ActionSpeedMultiply** |

### ActionStopSound

Origen: juego · base: ActionBase → lang::Object · 3 entradas, 3 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x14a738` | **ActionStopSound** |
| 1 | `0x14a6f8` | **ActionStopSound** |
| 2 | `0x1490d4` | **ActionStopSound** |

### ActionSwitchObjectState

Origen: juego · base: ActionBase → lang::Object · 3 entradas, 3 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x14bcec` | **ActionSwitchObjectState** |
| 1 | `0x14bc94` | **ActionSwitchObjectState** |
| 2 | `0x1490d4` | **ActionSwitchObjectState** |

### ActionTestVariable

Origen: juego · base: ActionBase → lang::Object · 3 entradas, 3 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x14b36c` | **ActionTestVariable** |
| 1 | `0x14b2fc` | **ActionTestVariable** |
| 2 | `0x1490d4` | **ActionTestVariable** |

### ActionTimer

Origen: juego · base: ActionBase → lang::Object · 3 entradas, 3 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x1498d4` | **ActionTimer** |
| 1 | `0x149894` | **ActionTimer** |
| 2 | `0x149888` | **ActionTimer** |

### ActionUnLinkNode

Origen: juego · base: ActionBase → lang::Object · 3 entradas, 3 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x14add4` | **ActionUnLinkNode** |
| 1 | `0x14ad8c` | **ActionUnLinkNode** |
| 2 | `0x1490d4` | **ActionUnLinkNode** |

### ActionWaitForIdle

Origen: juego · base: ActionBase → lang::Object · 3 entradas, 3 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x14b054` | **ActionWaitForIdle** |
| 1 | `0x14b03c` | **ActionWaitForIdle** |
| 2 | `0x14b030` | **ActionWaitForIdle** |

### ActionWaitForKeyHold

Origen: juego · base: ActionBase → lang::Object · 3 entradas, 3 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x14ba38` | **ActionWaitForKeyHold** |
| 1 | `0x14ba20` | **ActionWaitForKeyHold** |
| 2 | `0x1490d4` | **ActionWaitForKeyHold** |

### ActionWaitForKeyPress

Origen: juego · base: ActionBase → lang::Object · 3 entradas, 3 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x14bb60` | **ActionWaitForKeyPress** |
| 1 | `0x14bb2c` | **ActionWaitForKeyPress** |
| 2 | `0x1490d4` | **ActionWaitForKeyPress** |

### ActionWaitForKeyRelease

Origen: juego · base: ActionBase → lang::Object · 3 entradas, 3 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x14bd6c` | **ActionWaitForKeyRelease** |
| 1 | `0x14bd38` | **ActionWaitForKeyRelease** |
| 2 | `0x1490d4` | **ActionWaitForKeyRelease** |

### ActiveGameObject

Origen: juego · base: GameObject → lang::Object · 17 entradas, 7 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x14aebc` | **ActiveGameObject** |
| 1 | `0x14aea4` | **ActiveGameObject** |
| 2 | `0x18244` | **ActiveGameObject** |
| 3 | `0x182d8` | **ActiveGameObject** |
| 4 | `0x148f6c` | **ActiveGameObject** |
| 5 | `0x148f6c` | **ActiveGameObject** |
| 6 | `0x3a82c` | GameObject |
| 7 | `0x14ea2c` | GameObject |
| 8 | `0x14ea38` | GameObject |
| 9 | `0x1490f8` | GameObject |
| 10 | `0x14ea40` | GameObject |
| 11 | `0x14ea48` | GameObject |
| 12 | `0x14e9ac` | GameObject |
| 13 | `0x14e92c` | GameObject |
| 14 | `0x3a908` | GameObject |
| 15 | `0x3a978` | GameObject |
| 16 | `0x18248` | **ActiveGameObject** |

### AnimationItem

Origen: juego · base: lang::Object · 2 entradas, 2 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x149da8` | **AnimationItem** |
| 1 | `0x149d90` | **AnimationItem** |

### AnimationProperty

Origen: juego · base: lang::Object · 2 entradas, 2 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x14b1f4` | **AnimationProperty** |
| 1 | `0x14b1dc` | **AnimationProperty** |

### AttractionScene

Origen: juego · base: GameScene → lang::Object · 17 entradas, 9 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x14a93c` | **AttractionScene** |
| 1 | `0x14a88c` | **AttractionScene** |
| 2 | `0x19f7c` | **AttractionScene** |
| 3 | `0x1a09c` | **AttractionScene** |
| 4 | `0x3b658` | GameScene |
| 5 | `0x1a0e8` | **AttractionScene** |
| 6 | `0x1a1a0` | **AttractionScene** |
| 7 | `0x1a1b4` | **AttractionScene** |
| 8 | `0x1a434` | **AttractionScene** |
| 9 | `0x1a430` | **AttractionScene** |
| 10 | `0x3b9fc` | GameScene |
| 11 | `0x3ba00` | GameScene |
| 12 | `0x3ba48` | GameScene |
| 13 | `0x3ba50` | GameScene |
| 14 | `0x3ba58` | GameScene |
| 15 | `0x3ba90` | GameScene |
| 16 | `0x3ba98` | GameScene |

### AudioItem

Origen: juego · base: lang::Object · 2 entradas, 2 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x14e6c0` | **AudioItem** |
| 1 | `0x14e668` | **AudioItem** |

### AudioManagerS60

Origen: juego · base: IAudioManager · 11 entradas, 8 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x11068` | **AudioManagerS60** |
| 1 | `0x1110c` | **AudioManagerS60** |
| 2 | `0xf968` | IAudioManager |
| 3 | `0x11180` | **AudioManagerS60** |
| 4 | `0x14a0d8` | IAudioManager |
| 5 | `0x14a0d4` | IAudioManager |
| 6 | `0x11494` | **AudioManagerS60** |
| 7 | `0x11538` | **AudioManagerS60** |
| 8 | `0x11240` | **AudioManagerS60** |
| 9 | `0x112e0` | **AudioManagerS60** |
| 10 | `0x11368` | **AudioManagerS60** |

### AudioObject

Origen: juego · base: hgr::Node → lang::Object · 4 entradas, 4 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x1d18c` | **AudioObject** |
| 1 | `0x1d1b0` | **AudioObject** |
| 2 | `0x1d25c` | **AudioObject** |
| 3 | `0x1d320` | **AudioObject** |

### Ball

Origen: juego · base: ActiveGameObject → GameObject → lang::Object · 20 entradas, 15 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x14d484` | **Ball** |
| 1 | `0x14d3c8` | **Ball** |
| 2 | `0x1da64` | **Ball** |
| 3 | `0x1daa4` | **Ball** |
| 4 | `0x1de94` | **Ball** |
| 5 | `0x1df9c` | **Ball** |
| 6 | `0x3a82c` | GameObject |
| 7 | `0x14ea2c` | GameObject |
| 8 | `0x14ea38` | GameObject |
| 9 | `0x14d3c0` | **Ball** |
| 10 | `0x14edb4` | **Ball** |
| 11 | `0x14ea48` | GameObject |
| 12 | `0x1eee4` | **Ball** |
| 13 | `0x1ef68` | **Ball** |
| 14 | `0x1f080` | **Ball** |
| 15 | `0x1f248` | **Ball** |
| 16 | `0x18248` | ActiveGameObject |
| 17 | `0x14eda8` | **Ball** |
| 18 | `0x14eda0` | **Ball** |
| 19 | `0x14d368` | **Ball** |

### BounceApp

Origen: juego · base: framework::App · 9 entradas, 9 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x1fa90` | **BounceApp** |
| 1 | `0x1fc24` | **BounceApp** |
| 2 | `0x1ff8c` | **BounceApp** |
| 3 | `0x2003c` | **BounceApp** |
| 4 | `0x2004c` | **BounceApp** |
| 5 | `0x20130` | **BounceApp** |
| 6 | `0x14e70c` | **BounceApp** |
| 7 | `0x2005c` | **BounceApp** |
| 8 | `0x200ec` | **BounceApp** |

### CollisionGeometry

Origen: juego · base: lang::Object · 2 entradas, 2 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x20b5c` | **CollisionGeometry** |
| 1 | `0x20c68` | **CollisionGeometry** |

### CollisionObject

Origen: juego · base: hgr::Node → lang::Object · 3 entradas, 3 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x21d68` | **CollisionObject** |
| 1 | `0x21d8c` | **CollisionObject** |
| 2 | `0x21f30` | **CollisionObject** |

### CollisionSpace

Origen: juego · base: lang::Object · 2 entradas, 2 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x14a534` | **CollisionSpace** |
| 1 | `0x14a4b4` | **CollisionSpace** |

### ContainerItem

Origen: juego · base: MenuItem → lang::Object · 31 entradas, 5 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x14a074` | **ContainerItem** |
| 1 | `0x14a00c` | **ContainerItem** |
| 2 | `0x29020` | **ContainerItem** |
| 3 | `0x298d8` | **ContainerItem** |
| 4 | `0x4e448` | MenuItem |
| 5 | `0x4e4d4` | MenuItem |
| 6 | `0x14f760` | MenuItem |
| 7 | `0x14f78c` | MenuItem |
| 8 | `0x14f928` | MenuItem |
| 9 | `0x14f940` | MenuItem |
| 10 | `0x14f930` | MenuItem |
| 11 | `0x14e278` | MenuItem |
| 12 | `0x14f904` | MenuItem |
| 13 | `0x14f774` | MenuItem |
| 14 | `0x14f784` | MenuItem |
| 15 | `0x14f79c` | MenuItem |
| 16 | `0x14f794` | MenuItem |
| 17 | `0x14f938` | MenuItem |
| 18 | `0x14f77c` | MenuItem |
| 19 | `0x14e2c4` | MenuItem |
| 20 | `0x14e2a0` | MenuItem |
| 21 | `0x14e28c` | MenuItem |
| 22 | `0x14e298` | MenuItem |
| 23 | `0x14e2bc` | MenuItem |
| 24 | `0x14e2e8` | MenuItem |
| 25 | `0x14e2f0` | MenuItem |
| 26 | `0x14e280` | MenuItem |
| 27 | `0x14e2b4` | MenuItem |
| 28 | `0x14e2a8` | MenuItem |
| 29 | `0x29bbc` | **ContainerItem** |
| 30 | `0x4e578` | MenuItem |

### CustomBitmapFontResource

Origen: juego · base: IGameFont → ResourceObject · 6 entradas, 6 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x14be48` | **CustomBitmapFontResource** |
| 1 | `0x14be04` | **CustomBitmapFontResource** |
| 2 | `0x2c230` | **CustomBitmapFontResource** |
| 3 | `0x2c3f4` | **CustomBitmapFontResource** |
| 4 | `0x14bdf4` | **CustomBitmapFontResource** |
| 5 | `0x14bdfc` | **CustomBitmapFontResource** |

### CutsceneScene

Origen: juego · base: GameScene → lang::Object · 17 entradas, 9 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x276a8` | **CutsceneScene** |
| 1 | `0x277a0` | **CutsceneScene** |
| 2 | `0x277b8` | **CutsceneScene** |
| 3 | `0x277c0` | **CutsceneScene** |
| 4 | `0x3b658` | GameScene |
| 5 | `0x278d8` | **CutsceneScene** |
| 6 | `0x27e04` | **CutsceneScene** |
| 7 | `0x27e78` | **CutsceneScene** |
| 8 | `0x2723c` | **CutsceneScene** |
| 9 | `0x28dd0` | **CutsceneScene** |
| 10 | `0x3b9fc` | GameScene |
| 11 | `0x3ba00` | GameScene |
| 12 | `0x3ba48` | GameScene |
| 13 | `0x3ba50` | GameScene |
| 14 | `0x3ba58` | GameScene |
| 15 | `0x3ba90` | GameScene |
| 16 | `0x3ba98` | GameScene |

### DeathScene

Origen: juego · base: GameScene → lang::Object · 17 entradas, 9 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x2a218` | **DeathScene** |
| 1 | `0x2a2b0` | **DeathScene** |
| 2 | `0x2a348` | **DeathScene** |
| 3 | `0x2a350` | **DeathScene** |
| 4 | `0x3b658` | GameScene |
| 5 | `0x2a604` | **DeathScene** |
| 6 | `0x2a750` | **DeathScene** |
| 7 | `0x2a8dc` | **DeathScene** |
| 8 | `0x2abbc` | **DeathScene** |
| 9 | `0x2ab94` | **DeathScene** |
| 10 | `0x3b9fc` | GameScene |
| 11 | `0x3ba00` | GameScene |
| 12 | `0x3ba48` | GameScene |
| 13 | `0x3ba50` | GameScene |
| 14 | `0x3ba58` | GameScene |
| 15 | `0x3ba90` | GameScene |
| 16 | `0x3ba98` | GameScene |

### Game

Origen: juego · base: GameScene → lang::Object · 17 entradas, 10 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x2d2a4` | **Game** |
| 1 | `0x2d508` | **Game** |
| 2 | `0x2d520` | **Game** |
| 3 | `0x2e810` | **Game** |
| 4 | `0x3b658` | GameScene |
| 5 | `0x30410` | **Game** |
| 6 | `0x30aa8` | **Game** |
| 7 | `0x33fe4` | **Game** |
| 8 | `0x2d118` | **Game** |
| 9 | `0x34a48` | **Game** |
| 10 | `0x2fd08` | **Game** |
| 11 | `0x3ba00` | GameScene |
| 12 | `0x3ba48` | GameScene |
| 13 | `0x3ba50` | GameScene |
| 14 | `0x3ba58` | GameScene |
| 15 | `0x3ba90` | GameScene |
| 16 | `0x3ba98` | GameScene |

### GameCamera

Origen: juego · base: GameObject → lang::Object · 16 entradas, 4 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x1490f4` | **GameCamera** |
| 1 | `0x1490dc` | **GameCamera** |
| 2 | `0x35470` | **GameCamera** |
| 3 | `0x3a038` | GameObject |
| 4 | `0x3504c` | **GameCamera** |
| 5 | `0x3a828` | GameObject |
| 6 | `0x3a82c` | GameObject |
| 7 | `0x14ea2c` | GameObject |
| 8 | `0x14ea38` | GameObject |
| 9 | `0x1490f8` | GameObject |
| 10 | `0x14ea40` | GameObject |
| 11 | `0x14ea48` | GameObject |
| 12 | `0x14e9ac` | GameObject |
| 13 | `0x14e92c` | GameObject |
| 14 | `0x3a908` | GameObject |
| 15 | `0x3a978` | GameObject |

### GameFlowManager

Origen: juego · base: lang::Object · 2 entradas, 2 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x361ac` | **GameFlowManager** |
| 1 | `0x365dc` | **GameFlowManager** |

### GameObject

Origen: juego · base: lang::Object · 16 entradas, 16 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x149114` | **GameObject** |
| 1 | `0x1490fc` | **GameObject** |
| 2 | `0x39528` | **GameObject** |
| 3 | `0x3a038` | **GameObject** |
| 4 | `0x3a1b4` | **GameObject** |
| 5 | `0x3a828` | **GameObject** |
| 6 | `0x3a82c` | **GameObject** |
| 7 | `0x14ea2c` | **GameObject** |
| 8 | `0x14ea38` | **GameObject** |
| 9 | `0x1490f8` | **GameObject** |
| 10 | `0x14ea40` | **GameObject** |
| 11 | `0x14ea48` | **GameObject** |
| 12 | `0x14e9ac` | **GameObject** |
| 13 | `0x14e92c` | **GameObject** |
| 14 | `0x3a908` | **GameObject** |
| 15 | `0x3a978` | **GameObject** |

### GameSaves

Origen: juego · base: IGameSaves · 4 entradas, 4 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0xf360` | **GameSaves** |
| 1 | `0xf470` | **GameSaves** |
| 2 | `0x14e724` | **GameSaves** |
| 3 | `0x14e718` | **GameSaves** |

### GameScene

Origen: juego · base: lang::Object · 17 entradas, 17 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x14e79c` | **GameScene** |
| 1 | `0x14e728` | **GameScene** |
| 2 | `0x148f6c` | **GameScene** |
| 3 | `0x148f6c` | **GameScene** |
| 4 | `0x3b658` | **GameScene** |
| 5 | `0x148f6c` | **GameScene** |
| 6 | `0x3b778` | **GameScene** |
| 7 | `0x148f6c` | **GameScene** |
| 8 | `0x148f6c` | **GameScene** |
| 9 | `0x148f6c` | **GameScene** |
| 10 | `0x3b9fc` | **GameScene** |
| 11 | `0x3ba00` | **GameScene** |
| 12 | `0x3ba48` | **GameScene** |
| 13 | `0x3ba50` | **GameScene** |
| 14 | `0x3ba58` | **GameScene** |
| 15 | `0x3ba90` | **GameScene** |
| 16 | `0x3ba98` | **GameScene** |

### GameSettings

Origen: juego · base: lang::Object · 2 entradas, 2 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x1499e0` | **GameSettings** |
| 1 | `0x1499a0` | **GameSettings** |

### IAudioManager

Origen: juego · base: — · 8 entradas, 8 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x148f6c` | **IAudioManager** |
| 1 | `0x148f6c` | **IAudioManager** |
| 2 | `0xf968` | **IAudioManager** |
| 3 | `0xf96c` | **IAudioManager** |
| 4 | `0x14a0d8` | **IAudioManager** |
| 5 | `0x14a0d4` | **IAudioManager** |
| 6 | `0x10d80` | **IAudioManager** |
| 7 | `0x10e4c` | **IAudioManager** |

### IResources

Origen: juego · base: — · 43 entradas, 43 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x148f6c` | **IResources** |
| 1 | `0x148f6c` | **IResources** |
| 2 | `0x148f6c` | **IResources** |
| 3 | `0x148f6c` | **IResources** |
| 4 | `0x148f6c` | **IResources** |
| 5 | `0x148f6c` | **IResources** |
| 6 | `0x148f6c` | **IResources** |
| 7 | `0x148f6c` | **IResources** |
| 8 | `0x148f6c` | **IResources** |
| 9 | `0x148f6c` | **IResources** |
| 10 | `0x148f6c` | **IResources** |
| 11 | `0x148f6c` | **IResources** |
| 12 | `0x148f6c` | **IResources** |
| 13 | `0x148f6c` | **IResources** |
| 14 | `0x148f6c` | **IResources** |
| 15 | `0x148f6c` | **IResources** |
| 16 | `0x148f6c` | **IResources** |
| 17 | `0x148f6c` | **IResources** |
| 18 | `0x148f6c` | **IResources** |
| 19 | `0x148f6c` | **IResources** |
| 20 | `0x148f6c` | **IResources** |
| 21 | `0x148f6c` | **IResources** |
| 22 | `0x148f6c` | **IResources** |
| 23 | `0x148f6c` | **IResources** |
| 24 | `0x148f6c` | **IResources** |
| 25 | `0x148f6c` | **IResources** |
| 26 | `0x148f6c` | **IResources** |
| 27 | `0x148f6c` | **IResources** |
| 28 | `0x148f6c` | **IResources** |
| 29 | `0x148f6c` | **IResources** |
| 30 | `0x148f6c` | **IResources** |
| 31 | `0x1492e8` | **IResources** |
| 32 | `0x149280` | **IResources** |
| 33 | `0x40038` | **IResources** |
| 34 | `0x148f6c` | **IResources** |
| 35 | `0x148f6c` | **IResources** |
| 36 | `0x148f6c` | **IResources** |
| 37 | `0x149340` | **IResources** |
| 38 | `0x149338` | **IResources** |
| 39 | `0x401a0` | **IResources** |
| 40 | `0x401fc` | **IResources** |
| 41 | `0x148f6c` | **IResources** |
| 42 | `0x148f6c` | **IResources** |

### ImageItem

Origen: juego · base: MenuItem → lang::Object · 31 entradas, 4 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x14e848` | **ImageItem** |
| 1 | `0x14e808` | **ImageItem** |
| 2 | `0x44eb8` | **ImageItem** |
| 3 | `0x4e2ec` | MenuItem |
| 4 | `0x4e448` | MenuItem |
| 5 | `0x4e4d4` | MenuItem |
| 6 | `0x14f760` | MenuItem |
| 7 | `0x14f78c` | MenuItem |
| 8 | `0x14f928` | MenuItem |
| 9 | `0x14f940` | MenuItem |
| 10 | `0x14f930` | MenuItem |
| 11 | `0x14e278` | MenuItem |
| 12 | `0x14f904` | MenuItem |
| 13 | `0x14f774` | MenuItem |
| 14 | `0x14f784` | MenuItem |
| 15 | `0x14f79c` | MenuItem |
| 16 | `0x14f794` | MenuItem |
| 17 | `0x14f938` | MenuItem |
| 18 | `0x14f77c` | MenuItem |
| 19 | `0x14e2c4` | MenuItem |
| 20 | `0x14e2a0` | MenuItem |
| 21 | `0x14e28c` | MenuItem |
| 22 | `0x14e298` | MenuItem |
| 23 | `0x14e2bc` | MenuItem |
| 24 | `0x14e2e8` | MenuItem |
| 25 | `0x14e2f0` | MenuItem |
| 26 | `0x14e280` | MenuItem |
| 27 | `0x14e2b4` | MenuItem |
| 28 | `0x14e2a8` | MenuItem |
| 29 | `0x44f5c` | **ImageItem** |
| 30 | `0x4e578` | MenuItem |

### KeyConfigurator

Origen: juego · base: MenuItem → lang::Object · 31 entradas, 6 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x14aa48` | **KeyConfigurator** |
| 1 | `0x14a9e4` | **KeyConfigurator** |
| 2 | `0x402d8` | **KeyConfigurator** |
| 3 | `0x4e2ec` | MenuItem |
| 4 | `0x4060c` | **KeyConfigurator** |
| 5 | `0x40380` | **KeyConfigurator** |
| 6 | `0x14f760` | MenuItem |
| 7 | `0x14f78c` | MenuItem |
| 8 | `0x14f928` | MenuItem |
| 9 | `0x14f940` | MenuItem |
| 10 | `0x14f930` | MenuItem |
| 11 | `0x14e278` | MenuItem |
| 12 | `0x14f904` | MenuItem |
| 13 | `0x14f774` | MenuItem |
| 14 | `0x14f784` | MenuItem |
| 15 | `0x14f79c` | MenuItem |
| 16 | `0x14f794` | MenuItem |
| 17 | `0x14f938` | MenuItem |
| 18 | `0x14f77c` | MenuItem |
| 19 | `0x14e2c4` | MenuItem |
| 20 | `0x14e2a0` | MenuItem |
| 21 | `0x14e28c` | MenuItem |
| 22 | `0x14e298` | MenuItem |
| 23 | `0x14e2bc` | MenuItem |
| 24 | `0x14e2e8` | MenuItem |
| 25 | `0x14e2f0` | MenuItem |
| 26 | `0x14e280` | MenuItem |
| 27 | `0x14e2b4` | MenuItem |
| 28 | `0x14e2a8` | MenuItem |
| 29 | `0x40618` | **KeyConfigurator** |
| 30 | `0x4e578` | MenuItem |

### LoadingScreenScene

Origen: juego · base: GameScene → lang::Object · 17 entradas, 9 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x14b400` | **LoadingScreenScene** |
| 1 | `0x14b3e8` | **LoadingScreenScene** |
| 2 | `0x14b3d0` | **LoadingScreenScene** |
| 3 | `0x408e0` | **LoadingScreenScene** |
| 4 | `0x3b658` | GameScene |
| 5 | `0x409c4` | **LoadingScreenScene** |
| 6 | `0x40b1c` | **LoadingScreenScene** |
| 7 | `0x41e84` | **LoadingScreenScene** |
| 8 | `0x14b3e0` | **LoadingScreenScene** |
| 9 | `0x14b3e4` | **LoadingScreenScene** |
| 10 | `0x3b9fc` | GameScene |
| 11 | `0x3ba00` | GameScene |
| 12 | `0x3ba48` | GameScene |
| 13 | `0x3ba50` | GameScene |
| 14 | `0x3ba58` | GameScene |
| 15 | `0x3ba90` | GameScene |
| 16 | `0x3ba98` | GameScene |

### Menu

Origen: juego · base: GameScene → lang::Object · 17 entradas, 9 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x14d628` | **Menu** |
| 1 | `0x14d548` | **Menu** |
| 2 | `0x45130` | **Menu** |
| 3 | `0x45138` | **Menu** |
| 4 | `0x3b658` | GameScene |
| 5 | `0x4516c` | **Menu** |
| 6 | `0x4525c` | **Menu** |
| 7 | `0x4b1c4` | **Menu** |
| 8 | `0x4b1c8` | **Menu** |
| 9 | `0x4b1cc` | **Menu** |
| 10 | `0x3b9fc` | GameScene |
| 11 | `0x3ba00` | GameScene |
| 12 | `0x3ba48` | GameScene |
| 13 | `0x3ba50` | GameScene |
| 14 | `0x3ba58` | GameScene |
| 15 | `0x3ba90` | GameScene |
| 16 | `0x3ba98` | GameScene |

### MenuItem

Origen: juego · base: lang::Object · 31 entradas, 31 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x14e33c` | **MenuItem** |
| 1 | `0x14e2fc` | **MenuItem** |
| 2 | `0x4e200` | **MenuItem** |
| 3 | `0x4e2ec` | **MenuItem** |
| 4 | `0x4e448` | **MenuItem** |
| 5 | `0x4e4d4` | **MenuItem** |
| 6 | `0x14f760` | **MenuItem** |
| 7 | `0x14f78c` | **MenuItem** |
| 8 | `0x14f928` | **MenuItem** |
| 9 | `0x14f940` | **MenuItem** |
| 10 | `0x14f930` | **MenuItem** |
| 11 | `0x14e278` | **MenuItem** |
| 12 | `0x14f904` | **MenuItem** |
| 13 | `0x14f774` | **MenuItem** |
| 14 | `0x14f784` | **MenuItem** |
| 15 | `0x14f79c` | **MenuItem** |
| 16 | `0x14f794` | **MenuItem** |
| 17 | `0x14f938` | **MenuItem** |
| 18 | `0x14f77c` | **MenuItem** |
| 19 | `0x14e2c4` | **MenuItem** |
| 20 | `0x14e2a0` | **MenuItem** |
| 21 | `0x14e28c` | **MenuItem** |
| 22 | `0x14e298` | **MenuItem** |
| 23 | `0x14e2bc` | **MenuItem** |
| 24 | `0x14e2e8` | **MenuItem** |
| 25 | `0x14e2f0` | **MenuItem** |
| 26 | `0x14e280` | **MenuItem** |
| 27 | `0x14e2b4` | **MenuItem** |
| 28 | `0x14e2a8` | **MenuItem** |
| 29 | `0x14e294` | **MenuItem** |
| 30 | `0x4e578` | **MenuItem** |

### MenuPage

Origen: juego · base: lang::Object · 2 entradas, 2 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x14e3c0` | **MenuPage** |
| 1 | `0x14e370` | **MenuPage** |

### OSSS60

Origen: juego · base: framework::OSInterface · 4 entradas, 4 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x14df9c` | **OSSS60** |
| 1 | `0x14df90` | **OSSS60** |
| 2 | `0x14df5c` | **OSSS60** |
| 3 | `0x14df60` | **OSSS60** |

### PlayerBall

Origen: juego · base: Ball → ActiveGameObject → GameObject → lang::Object · 20 entradas, 12 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x149360` | **PlayerBall** |
| 1 | `0x149348` | **PlayerBall** |
| 2 | `0x4e808` | **PlayerBall** |
| 3 | `0x4ee68` | **PlayerBall** |
| 4 | `0x4f200` | **PlayerBall** |
| 5 | `0x4f2c8` | **PlayerBall** |
| 6 | `0x50630` | **PlayerBall** |
| 7 | `0x14ea2c` | GameObject |
| 8 | `0x14ea38` | GameObject |
| 9 | `0x14d3c0` | Ball |
| 10 | `0x14edb4` | Ball |
| 11 | `0x14ed7c` | **PlayerBall** |
| 12 | `0x14ec50` | **PlayerBall** |
| 13 | `0x14ea68` | **PlayerBall** |
| 14 | `0x1f080` | Ball |
| 15 | `0x1f248` | Ball |
| 16 | `0x18248` | ActiveGameObject |
| 17 | `0x14ed70` | **PlayerBall** |
| 18 | `0x14ed4c` | **PlayerBall** |
| 19 | `0x14d368` | Ball |

### PopupScene

Origen: juego · base: GameScene → lang::Object · 17 entradas, 9 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x149720` | **PopupScene** |
| 1 | `0x149658` | **PopupScene** |
| 2 | `0x525c4` | **PopupScene** |
| 3 | `0x525cc` | **PopupScene** |
| 4 | `0x3b658` | GameScene |
| 5 | `0x52750` | **PopupScene** |
| 6 | `0x528e4` | **PopupScene** |
| 7 | `0x52c5c` | **PopupScene** |
| 8 | `0x53104` | **PopupScene** |
| 9 | `0x52fe8` | **PopupScene** |
| 10 | `0x3b9fc` | GameScene |
| 11 | `0x3ba00` | GameScene |
| 12 | `0x3ba48` | GameScene |
| 13 | `0x3ba50` | GameScene |
| 14 | `0x3ba58` | GameScene |
| 15 | `0x3ba90` | GameScene |
| 16 | `0x3ba98` | GameScene |

### ResourceManager

Origen: juego · base: lang::Object · 2 entradas, 2 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x540d0` | **ResourceManager** |
| 1 | `0x54168` | **ResourceManager** |

### Resources

Origen: juego · base: IResources · 43 entradas, 38 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x547c4` | **Resources** |
| 1 | `0x54804` | **Resources** |
| 2 | `0x5482c` | **Resources** |
| 3 | `0x54854` | **Resources** |
| 4 | `0x5485c` | **Resources** |
| 5 | `0x54864` | **Resources** |
| 6 | `0x5486c` | **Resources** |
| 7 | `0x54874` | **Resources** |
| 8 | `0x5487c` | **Resources** |
| 9 | `0x54884` | **Resources** |
| 10 | `0x54894` | **Resources** |
| 11 | `0x5489c` | **Resources** |
| 12 | `0x548f0` | **Resources** |
| 13 | `0x5490c` | **Resources** |
| 14 | `0x548d8` | **Resources** |
| 15 | `0x54900` | **Resources** |
| 16 | `0x54914` | **Resources** |
| 17 | `0x5491c` | **Resources** |
| 18 | `0x5492c` | **Resources** |
| 19 | `0x54924` | **Resources** |
| 20 | `0x54934` | **Resources** |
| 21 | `0x5498c` | **Resources** |
| 22 | `0x549bc` | **Resources** |
| 23 | `0x549e0` | **Resources** |
| 24 | `0x14e88c` | **Resources** |
| 25 | `0x14e884` | **Resources** |
| 26 | `0x549fc` | **Resources** |
| 27 | `0x14e87c` | **Resources** |
| 28 | `0x54964` | **Resources** |
| 29 | `0x54a04` | **Resources** |
| 30 | `0x54b7c` | **Resources** |
| 31 | `0x1492e8` | IResources |
| 32 | `0x149280` | IResources |
| 33 | `0x40038` | IResources |
| 34 | `0x54c90` | **Resources** |
| 35 | `0x54d1c` | **Resources** |
| 36 | `0x54d50` | **Resources** |
| 37 | `0x149340` | IResources |
| 38 | `0x149338` | IResources |
| 39 | `0x54718` | **Resources** |
| 40 | `0x5476c` | **Resources** |
| 41 | `0x544bc` | **Resources** |
| 42 | `0x54608` | **Resources** |

### SliderController

Origen: juego · base: MenuItem → lang::Object · 31 entradas, 7 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x14af00` | **SliderController** |
| 1 | `0x14aec0` | **SliderController** |
| 2 | `0x54dd0` | **SliderController** |
| 3 | `0x5505c` | **SliderController** |
| 4 | `0x552ec` | **SliderController** |
| 5 | `0x552f0` | **SliderController** |
| 6 | `0x14f760` | MenuItem |
| 7 | `0x14f78c` | MenuItem |
| 8 | `0x14f928` | MenuItem |
| 9 | `0x14f940` | MenuItem |
| 10 | `0x14f930` | MenuItem |
| 11 | `0x14e278` | MenuItem |
| 12 | `0x14f904` | MenuItem |
| 13 | `0x14f774` | MenuItem |
| 14 | `0x14f784` | MenuItem |
| 15 | `0x14f79c` | MenuItem |
| 16 | `0x14f794` | MenuItem |
| 17 | `0x14f938` | MenuItem |
| 18 | `0x14f77c` | MenuItem |
| 19 | `0x14e2c4` | MenuItem |
| 20 | `0x14e2a0` | MenuItem |
| 21 | `0x14e28c` | MenuItem |
| 22 | `0x14e298` | MenuItem |
| 23 | `0x14e2bc` | MenuItem |
| 24 | `0x14e2e8` | MenuItem |
| 25 | `0x14e2f0` | MenuItem |
| 26 | `0x14e280` | MenuItem |
| 27 | `0x14e2b4` | MenuItem |
| 28 | `0x14e2a8` | MenuItem |
| 29 | `0x55358` | **SliderController** |
| 30 | `0x4e578` | MenuItem |

### SpriteSheet

Origen: juego · base: ResourceObject · 2 entradas, 2 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x149950` | **SpriteSheet** |
| 1 | `0x149908` | **SpriteSheet** |

### TextGroup

Origen: juego · base: ResourceObject · 2 entradas, 2 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x14e8e4` | **TextGroup** |
| 1 | `0x14e894` | **TextGroup** |

### TextItem

Origen: juego · base: MenuItem → lang::Object · 31 entradas, 4 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x14e48c` | **TextItem** |
| 1 | `0x14e404` | **TextItem** |
| 2 | `0x55c90` | **TextItem** |
| 3 | `0x4e2ec` | MenuItem |
| 4 | `0x4e448` | MenuItem |
| 5 | `0x4e4d4` | MenuItem |
| 6 | `0x14f760` | MenuItem |
| 7 | `0x14f78c` | MenuItem |
| 8 | `0x14f928` | MenuItem |
| 9 | `0x14f940` | MenuItem |
| 10 | `0x14f930` | MenuItem |
| 11 | `0x14e278` | MenuItem |
| 12 | `0x14f904` | MenuItem |
| 13 | `0x14f774` | MenuItem |
| 14 | `0x14f784` | MenuItem |
| 15 | `0x14f79c` | MenuItem |
| 16 | `0x14f794` | MenuItem |
| 17 | `0x14f938` | MenuItem |
| 18 | `0x14f77c` | MenuItem |
| 19 | `0x14e2c4` | MenuItem |
| 20 | `0x14e2a0` | MenuItem |
| 21 | `0x14e28c` | MenuItem |
| 22 | `0x14e298` | MenuItem |
| 23 | `0x14e2bc` | MenuItem |
| 24 | `0x14e2e8` | MenuItem |
| 25 | `0x14e2f0` | MenuItem |
| 26 | `0x14e280` | MenuItem |
| 27 | `0x14e2b4` | MenuItem |
| 28 | `0x14e2a8` | MenuItem |
| 29 | `0x55ef8` | **TextItem** |
| 30 | `0x4e578` | MenuItem |

### Trigger

Origen: juego · base: GameObject → lang::Object · 16 entradas, 6 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x14e1e4` | **Trigger** |
| 1 | `0x14e15c` | **Trigger** |
| 2 | `0x56f28` | **Trigger** |
| 3 | `0x3a038` | GameObject |
| 4 | `0x3a1b4` | GameObject |
| 5 | `0x5734c` | **Trigger** |
| 6 | `0x3a82c` | GameObject |
| 7 | `0x14ea2c` | GameObject |
| 8 | `0x14ea38` | GameObject |
| 9 | `0x1490f8` | GameObject |
| 10 | `0x14ea40` | GameObject |
| 11 | `0x14f744` | **Trigger** |
| 12 | `0x14e9ac` | GameObject |
| 13 | `0x14e92c` | GameObject |
| 14 | `0x57948` | **Trigger** |
| 15 | `0x3a978` | GameObject |

### WorldMap

Origen: juego · base: GameScene → lang::Object · 17 entradas, 10 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x14e520` | **WorldMap** |
| 1 | `0x14e508` | **WorldMap** |
| 2 | `0x5839c` | **WorldMap** |
| 3 | `0x58ec4` | **WorldMap** |
| 4 | `0x3b658` | GameScene |
| 5 | `0x59f48` | **WorldMap** |
| 6 | `0x5b8bc` | **WorldMap** |
| 7 | `0x5bbc0` | **WorldMap** |
| 8 | `0x5ca5c` | **WorldMap** |
| 9 | `0x5ca58` | **WorldMap** |
| 10 | `0x59988` | **WorldMap** |
| 11 | `0x3ba00` | GameScene |
| 12 | `0x3ba48` | GameScene |
| 13 | `0x3ba50` | GameScene |
| 14 | `0x3ba58` | GameScene |
| 15 | `0x3ba90` | GameScene |
| 16 | `0x3ba98` | GameScene |

### animationData

Origen: juego · base: lang::Object · 2 entradas, 2 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x14a10c` | **animationData** |
| 1 | `0x14a0f4` | **animationData** |

### moveAnimationData

Origen: juego · base: lang::Object · 2 entradas, 2 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x14b2c8` | **moveAnimationData** |
| 1 | `0x14b2b0` | **moveAnimationData** |

### CBackBuffer

Origen: S60 · base: 0x854 · 5 entradas, 5 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x5e0b0` | **CBackBuffer** |
| 1 | `0x5e170` | **CBackBuffer** |
| 2 | `0x1485c4` | **CBackBuffer** |
| 3 | `0x5e714` | **CBackBuffer** |
| 4 | `0x5e770` | **CBackBuffer** |

### CFrameworkAppUi

Origen: S60 · base: 0xe28 · 46 entradas, 46 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x5cbb8` | **CFrameworkAppUi** |
| 1 | `0x5cc60` | **CFrameworkAppUi** |
| 2 | `0x1485c4` | **CFrameworkAppUi** |
| 3 | `0x148754` | **CFrameworkAppUi** |
| 4 | `0x148ab4` | **CFrameworkAppUi** |
| 5 | `0x148aa4` | **CFrameworkAppUi** |
| 6 | `0x148a6c` | **CFrameworkAppUi** |
| 7 | `0x1486c4` | **CFrameworkAppUi** |
| 8 | `0x5cd50` | **CFrameworkAppUi** |
| 9 | `0x1486dc` | **CFrameworkAppUi** |
| 10 | `0x148acc` | **CFrameworkAppUi** |
| 11 | `0x148a74` | **CFrameworkAppUi** |
| 12 | `0x1486d4` | **CFrameworkAppUi** |
| 13 | `0x14874c` | **CFrameworkAppUi** |
| 14 | `0x148854` | **CFrameworkAppUi** |
| 15 | `0x14885c` | **CFrameworkAppUi** |
| 16 | `0x5cf18` | **CFrameworkAppUi** |
| 17 | `0x1488ac` | **CFrameworkAppUi** |
| 18 | `0x1488c4` | **CFrameworkAppUi** |
| 19 | `0x14899c` | **CFrameworkAppUi** |
| 20 | `0x148b0c` | **CFrameworkAppUi** |
| 21 | `0x1488bc` | **CFrameworkAppUi** |
| 22 | `0x148a84` | **CFrameworkAppUi** |
| 23 | `0x1488cc` | **CFrameworkAppUi** |
| 24 | `0x5cdec` | **CFrameworkAppUi** |
| 25 | `0x1488a4` | **CFrameworkAppUi** |
| 26 | `0x1488d4` | **CFrameworkAppUi** |
| 27 | `0x148884` | **CFrameworkAppUi** |
| 28 | `0x148a9c` | **CFrameworkAppUi** |
| 29 | `0x5cd18` | **CFrameworkAppUi** |
| 30 | `0x148a7c` | **CFrameworkAppUi** |
| 31 | `0x1488b4` | **CFrameworkAppUi** |
| 32 | `0x148abc` | **CFrameworkAppUi** |
| 33 | `0x14888c` | **CFrameworkAppUi** |
| 34 | `0x14889c` | **CFrameworkAppUi** |
| 35 | `0x148a5c` | **CFrameworkAppUi** |
| 36 | `0x14898c` | **CFrameworkAppUi** |
| 37 | `0x148964` | **CFrameworkAppUi** |
| 38 | `0x148874` | **CFrameworkAppUi** |
| 39 | `0x14887c` | **CFrameworkAppUi** |
| 40 | `0x148aec` | **CFrameworkAppUi** |
| 41 | `0x148af4` | **CFrameworkAppUi** |
| 42 | `0x148afc` | **CFrameworkAppUi** |
| 43 | `0x148adc` | **CFrameworkAppUi** |
| 44 | `0x148ad4` | **CFrameworkAppUi** |
| 45 | `0x5ce2c` | **CFrameworkAppUi** |

### CFrameworkAppView

Origen: S60 · base: 0x169 · 44 entradas, 44 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x5d314` | **CFrameworkAppView** |
| 1 | `0x5d368` | **CFrameworkAppView** |
| 2 | `0x1485c4` | **CFrameworkAppView** |
| 3 | `0x5dba8` | **CFrameworkAppView** |
| 4 | `0x148674` | **CFrameworkAppView** |
| 5 | `0x148824` | **CFrameworkAppView** |
| 6 | `0x148764` | **CFrameworkAppView** |
| 7 | `0x1487bc` | **CFrameworkAppView** |
| 8 | `0x14881c` | **CFrameworkAppView** |
| 9 | `0x14878c` | **CFrameworkAppView** |
| 10 | `0x148784` | **CFrameworkAppView** |
| 11 | `0x148694` | **CFrameworkAppView** |
| 12 | `0x14869c` | **CFrameworkAppView** |
| 13 | `0x14872c` | **CFrameworkAppView** |
| 14 | `0x14867c` | **CFrameworkAppView** |
| 15 | `0x5dc44` | **CFrameworkAppView** |
| 16 | `0x1486fc` | **CFrameworkAppView** |
| 17 | `0x1486ec` | **CFrameworkAppView** |
| 18 | `0x148704` | **CFrameworkAppView** |
| 19 | `0x148794` | **CFrameworkAppView** |
| 20 | `0x14879c` | **CFrameworkAppView** |
| 21 | `0x1487c4` | **CFrameworkAppView** |
| 22 | `0x1487e4` | **CFrameworkAppView** |
| 23 | `0x1487d4` | **CFrameworkAppView** |
| 24 | `0x5d784` | **CFrameworkAppView** |
| 25 | `0x1487dc` | **CFrameworkAppView** |
| 26 | `0x1486b4` | **CFrameworkAppView** |
| 27 | `0x1486a4` | **CFrameworkAppView** |
| 28 | `0x14873c` | **CFrameworkAppView** |
| 29 | `0x148714` | **CFrameworkAppView** |
| 30 | `0x1486f4` | **CFrameworkAppView** |
| 31 | `0x148814` | **CFrameworkAppView** |
| 32 | `0x1487f4` | **CFrameworkAppView** |
| 33 | `0x1487fc` | **CFrameworkAppView** |
| 34 | `0x148804` | **CFrameworkAppView** |
| 35 | `0x14880c` | **CFrameworkAppView** |
| 36 | `0x1487a4` | **CFrameworkAppView** |
| 37 | `0x1487ac` | **CFrameworkAppView** |
| 38 | `0x1487b4` | **CFrameworkAppView** |
| 39 | `0x148734` | **CFrameworkAppView** |
| 40 | `0x1487ec` | **CFrameworkAppView** |
| 41 | `0x5dc98` | **CFrameworkAppView** |
| 42 | `0x14870c` | **CFrameworkAppView** |
| 43 | `0x148864` | **CFrameworkAppView** |

### CFrameworkApplication

Origen: S60 · base: 0xd80 · 18 entradas, 18 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x14bba0` | **CFrameworkApplication** |
| 1 | `0x14bb88` | **CFrameworkApplication** |
| 2 | `0x1485c4` | **CFrameworkApplication** |
| 3 | `0x148b5c` | **CFrameworkApplication** |
| 4 | `0x1489dc` | **CFrameworkApplication** |
| 5 | `0x5cb50` | **CFrameworkApplication** |
| 6 | `0x148b1c` | **CFrameworkApplication** |
| 7 | `0x14863c` | **CFrameworkApplication** |
| 8 | `0x148934` | **CFrameworkApplication** |
| 9 | `0x148b54` | **CFrameworkApplication** |
| 10 | `0x14864c` | **CFrameworkApplication** |
| 11 | `0x148654` | **CFrameworkApplication** |
| 12 | `0x148954` | **CFrameworkApplication** |
| 13 | `0x14893c` | **CFrameworkApplication** |
| 14 | `0x148944` | **CFrameworkApplication** |
| 15 | `0x1489a4` | **CFrameworkApplication** |
| 16 | `0x1489ac` | **CFrameworkApplication** |
| 17 | `0x5cb54` | **CFrameworkApplication** |

### CFrameworkDocument

Origen: S60 · base: 0xd6c · 23 entradas, 23 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x5dd34` | **CFrameworkDocument** |
| 1 | `0x5dd40` | **CFrameworkDocument** |
| 2 | `0x1485c4` | **CFrameworkDocument** |
| 3 | `0x1488e4` | **CFrameworkDocument** |
| 4 | `0x1488f4` | **CFrameworkDocument** |
| 5 | `0x14865c` | **CFrameworkDocument** |
| 6 | `0x1488fc` | **CFrameworkDocument** |
| 7 | `0x14894c` | **CFrameworkDocument** |
| 8 | `0x14890c` | **CFrameworkDocument** |
| 9 | `0x148924` | **CFrameworkDocument** |
| 10 | `0x14895c` | **CFrameworkDocument** |
| 11 | `0x14999c` | **CFrameworkDocument** |
| 12 | `0x14891c` | **CFrameworkDocument** |
| 13 | `0x14892c` | **CFrameworkDocument** |
| 14 | `0x1489d4` | **CFrameworkDocument** |
| 15 | `0x148634` | **CFrameworkDocument** |
| 16 | `0x148914` | **CFrameworkDocument** |
| 17 | `0x148a44` | **CFrameworkDocument** |
| 18 | `0x1488dc` | **CFrameworkDocument** |
| 19 | `0x5dd58` | **CFrameworkDocument** |
| 20 | `0x148a4c` | **CFrameworkDocument** |
| 21 | `0x1488ec` | **CFrameworkDocument** |
| 22 | `0x148904` | **CFrameworkDocument** |

### MEikStatusPaneObserver

Origen: S60 · base: — · 1 entradas, 1 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x148f6c` | **MEikStatusPaneObserver** |

### framework::App

Origen: KA3D · base: — · 9 entradas, 9 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x5e88c` | **framework::App** |
| 1 | `0x5e8a4` | **framework::App** |
| 2 | `0x148f6c` | **framework::App** |
| 3 | `0x5e8c4` | **framework::App** |
| 4 | `0x5e8c8` | **framework::App** |
| 5 | `0x5e8cc` | **framework::App** |
| 6 | `0x5e8d0` | **framework::App** |
| 7 | `0x5e8e0` | **framework::App** |
| 8 | `0x5e900` | **framework::App** |

### gr::BaseTexture

Origen: KA3D · base: gr::ContextObject → lang::Object · 8 entradas, 5 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x14bff0` | **gr::BaseTexture** |
| 1 | `0x14bfd8` | **gr::BaseTexture** |
| 2 | `0x5ec24` | gr::ContextObject |
| 3 | `0x5ec28` | gr::ContextObject |
| 4 | `0x5ec2c` | gr::ContextObject |
| 5 | `0x148f6c` | **gr::BaseTexture** |
| 6 | `0x148f6c` | **gr::BaseTexture** |
| 7 | `0x148f6c` | **gr::BaseTexture** |

### gr::Context

Origen: KA3D · base: lang::Object · 34 entradas, 34 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x5eac0` | **gr::Context** |
| 1 | `0x5eac4` | **gr::Context** |
| 2 | `0x148f6c` | **gr::Context** |
| 3 | `0x148f6c` | **gr::Context** |
| 4 | `0x148f6c` | **gr::Context** |
| 5 | `0x148f6c` | **gr::Context** |
| 6 | `0x148f6c` | **gr::Context** |
| 7 | `0x148f6c` | **gr::Context** |
| 8 | `0x148f6c` | **gr::Context** |
| 9 | `0x148f6c` | **gr::Context** |
| 10 | `0x148f6c` | **gr::Context** |
| 11 | `0x148f6c` | **gr::Context** |
| 12 | `0x148f6c` | **gr::Context** |
| 13 | `0x148f6c` | **gr::Context** |
| 14 | `0x148f6c` | **gr::Context** |
| 15 | `0x148f6c` | **gr::Context** |
| 16 | `0x148f6c` | **gr::Context** |
| 17 | `0x148f6c` | **gr::Context** |
| 18 | `0x148f6c` | **gr::Context** |
| 19 | `0x148f6c` | **gr::Context** |
| 20 | `0x148f6c` | **gr::Context** |
| 21 | `0x148f6c` | **gr::Context** |
| 22 | `0x148f6c` | **gr::Context** |
| 23 | `0x148f6c` | **gr::Context** |
| 24 | `0x148f6c` | **gr::Context** |
| 25 | `0x148f6c` | **gr::Context** |
| 26 | `0x148f6c` | **gr::Context** |
| 27 | `0x148f6c` | **gr::Context** |
| 28 | `0x148f6c` | **gr::Context** |
| 29 | `0x148f6c` | **gr::Context** |
| 30 | `0x14c190` | **gr::Context** |
| 31 | `0x14c1bc` | **gr::Context** |
| 32 | `0x14ed90` | **gr::Context** |
| 33 | `0x14ed98` | **gr::Context** |

### gr::ContextObject

Origen: KA3D · base: lang::Object · 5 entradas, 5 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x14c038` | **gr::ContextObject** |
| 1 | `0x14c020` | **gr::ContextObject** |
| 2 | `0x5ec24` | **gr::ContextObject** |
| 3 | `0x5ec28` | **gr::ContextObject** |
| 4 | `0x5ec2c` | **gr::ContextObject** |

### gr::CubeTexture

Origen: KA3D · base: gr::BaseTexture → gr::ContextObject → lang::Object · 8 entradas, 2 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x14c014` | **gr::CubeTexture** |
| 1 | `0x14bffc` | **gr::CubeTexture** |
| 2 | `0x5ec24` | gr::ContextObject |
| 3 | `0x5ec28` | gr::ContextObject |
| 4 | `0x5ec2c` | gr::ContextObject |
| 5 | `0x148f6c` | gr::BaseTexture |
| 6 | `0x148f6c` | gr::BaseTexture |
| 7 | `0x148f6c` | gr::BaseTexture |

### gr::DIPrimitive

Origen: KA3D · base: gr::Primitive → gr::ContextObject → lang::Object · 61 entradas, 51 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x5edcc` | **gr::DIPrimitive** |
| 1 | `0x5ee48` | **gr::DIPrimitive** |
| 2 | `0x5ec24` | gr::ContextObject |
| 3 | `0x5ec28` | gr::ContextObject |
| 4 | `0x5ec2c` | gr::ContextObject |
| 5 | `0x148f6c` | gr::Primitive |
| 6 | `0x620d8` | gr::Primitive |
| 7 | `0x620dc` | gr::Primitive |
| 8 | `0x5efd0` | **gr::DIPrimitive** |
| 9 | `0x5f1bc` | **gr::DIPrimitive** |
| 10 | `0x5f004` | **gr::DIPrimitive** |
| 11 | `0x5f038` | **gr::DIPrimitive** |
| 12 | `0x5f06c` | **gr::DIPrimitive** |
| 13 | `0x5f0a0` | **gr::DIPrimitive** |
| 14 | `0x5f0d4` | **gr::DIPrimitive** |
| 15 | `0x5f108` | **gr::DIPrimitive** |
| 16 | `0x5f13c` | **gr::DIPrimitive** |
| 17 | `0x5f170` | **gr::DIPrimitive** |
| 18 | `0x5f1ac` | **gr::DIPrimitive** |
| 19 | `0x61288` | **gr::DIPrimitive** |
| 20 | `0x61184` | **gr::DIPrimitive** |
| 21 | `0x5f1cc` | **gr::DIPrimitive** |
| 22 | `0x5f240` | **gr::DIPrimitive** |
| 23 | `0x5f484` | **gr::DIPrimitive** |
| 24 | `0x148f6c` | gr::Primitive |
| 25 | `0x620e0` | gr::Primitive |
| 26 | `0x61114` | **gr::DIPrimitive** |
| 27 | `0x6113c` | **gr::DIPrimitive** |
| 28 | `0x61154` | **gr::DIPrimitive** |
| 29 | `0x6115c` | **gr::DIPrimitive** |
| 30 | `0x61164` | **gr::DIPrimitive** |
| 31 | `0x61174` | **gr::DIPrimitive** |
| 32 | `0x61310` | **gr::DIPrimitive** |
| 33 | `0x613c8` | **gr::DIPrimitive** |
| 34 | `0x613d0` | **gr::DIPrimitive** |
| 35 | `0x613d8` | **gr::DIPrimitive** |
| 36 | `0x613e0` | **gr::DIPrimitive** |
| 37 | `0x613f0` | **gr::DIPrimitive** |
| 38 | `0x148f6c` | gr::Primitive |
| 39 | `0x613f8` | **gr::DIPrimitive** |
| 40 | `0x148f6c` | gr::Primitive |
| 41 | `0x61440` | **gr::DIPrimitive** |
| 42 | `0x61448` | **gr::DIPrimitive** |
| 43 | `0x613e8` | **gr::DIPrimitive** |
| 44 | `0x614e0` | **gr::DIPrimitive** |
| 45 | `0x61504` | **gr::DIPrimitive** |
| 46 | `0x61528` | **gr::DIPrimitive** |
| 47 | `0x6154c` | **gr::DIPrimitive** |
| 48 | `0x61570` | **gr::DIPrimitive** |
| 49 | `0x61594` | **gr::DIPrimitive** |
| 50 | `0x615b8` | **gr::DIPrimitive** |
| 51 | `0x615dc` | **gr::DIPrimitive** |
| 52 | `0x61344` | **gr::DIPrimitive** |
| 53 | `0x61618` | **gr::DIPrimitive** |
| 54 | `0x6168c` | **gr::DIPrimitive** |
| 55 | `0x61694` | **gr::DIPrimitive** |
| 56 | `0x61f9c` | **gr::DIPrimitive** |
| 57 | `0x61fa4` | **gr::DIPrimitive** |
| 58 | `0x61fac` | **gr::DIPrimitive** |
| 59 | `0x61fb4` | **gr::DIPrimitive** |
| 60 | `0x62080` | **gr::DIPrimitive** |

### gr::N3D_Context

Origen: N3D · base: gr::Context → lang::Object · 34 entradas, 30 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x7cf40` | **gr::N3D_Context** |
| 1 | `0x7cfd0` | **gr::N3D_Context** |
| 2 | `0x7db94` | **gr::N3D_Context** |
| 3 | `0x7de04` | **gr::N3D_Context** |
| 4 | `0x7d334` | **gr::N3D_Context** |
| 5 | `0x7d340` | **gr::N3D_Context** |
| 6 | `0x7d398` | **gr::N3D_Context** |
| 7 | `0x7d6a8` | **gr::N3D_Context** |
| 8 | `0x7d704` | **gr::N3D_Context** |
| 9 | `0x7db20` | **gr::N3D_Context** |
| 10 | `0x7d144` | **gr::N3D_Context** |
| 11 | `0x7d16c` | **gr::N3D_Context** |
| 12 | `0x7d1d8` | **gr::N3D_Context** |
| 13 | `0x7d254` | **gr::N3D_Context** |
| 14 | `0x7d33c` | **gr::N3D_Context** |
| 15 | `0x7e4e8` | **gr::N3D_Context** |
| 16 | `0x7e04c` | **gr::N3D_Context** |
| 17 | `0x7e078` | **gr::N3D_Context** |
| 18 | `0x7d2bc` | **gr::N3D_Context** |
| 19 | `0x7e07c` | **gr::N3D_Context** |
| 20 | `0x7e098` | **gr::N3D_Context** |
| 21 | `0x7e0a0` | **gr::N3D_Context** |
| 22 | `0x7e5b0` | **gr::N3D_Context** |
| 23 | `0x7e5b8` | **gr::N3D_Context** |
| 24 | `0x7e5a8` | **gr::N3D_Context** |
| 25 | `0x7e5cc` | **gr::N3D_Context** |
| 26 | `0x7e5dc` | **gr::N3D_Context** |
| 27 | `0x7e5d4` | **gr::N3D_Context** |
| 28 | `0x7e5c0` | **gr::N3D_Context** |
| 29 | `0x7e5c8` | **gr::N3D_Context** |
| 30 | `0x14c190` | gr::Context |
| 31 | `0x14c1bc` | gr::Context |
| 32 | `0x14ed90` | gr::Context |
| 33 | `0x14ed98` | gr::Context |

### gr::N3D_Primitive

Origen: N3D · base: gr::DIPrimitive → gr::Primitive → gr::ContextObject → lang::Object · 61 entradas, 16 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x7f718` | **gr::N3D_Primitive** |
| 1 | `0x7f7bc` | **gr::N3D_Primitive** |
| 2 | `0x7fff4` | **gr::N3D_Primitive** |
| 3 | `0x8006c` | **gr::N3D_Primitive** |
| 4 | `0x80104` | **gr::N3D_Primitive** |
| 5 | `0x7f870` | **gr::N3D_Primitive** |
| 6 | `0x7ffcc` | **gr::N3D_Primitive** |
| 7 | `0x7ffd4` | **gr::N3D_Primitive** |
| 8 | `0x5efd0` | gr::DIPrimitive |
| 9 | `0x5f1bc` | gr::DIPrimitive |
| 10 | `0x5f004` | gr::DIPrimitive |
| 11 | `0x5f038` | gr::DIPrimitive |
| 12 | `0x5f06c` | gr::DIPrimitive |
| 13 | `0x5f0a0` | gr::DIPrimitive |
| 14 | `0x5f0d4` | gr::DIPrimitive |
| 15 | `0x5f108` | gr::DIPrimitive |
| 16 | `0x5f13c` | gr::DIPrimitive |
| 17 | `0x5f170` | gr::DIPrimitive |
| 18 | `0x5f1ac` | gr::DIPrimitive |
| 19 | `0x61288` | gr::DIPrimitive |
| 20 | `0x61184` | gr::DIPrimitive |
| 21 | `0x5f1cc` | gr::DIPrimitive |
| 22 | `0x5f240` | gr::DIPrimitive |
| 23 | `0x5f484` | gr::DIPrimitive |
| 24 | `0x7f8ec` | **gr::N3D_Primitive** |
| 25 | `0x7ffdc` | **gr::N3D_Primitive** |
| 26 | `0x80278` | **gr::N3D_Primitive** |
| 27 | `0x80414` | **gr::N3D_Primitive** |
| 28 | `0x61154` | gr::DIPrimitive |
| 29 | `0x6115c` | gr::DIPrimitive |
| 30 | `0x61164` | gr::DIPrimitive |
| 31 | `0x61174` | gr::DIPrimitive |
| 32 | `0x61310` | gr::DIPrimitive |
| 33 | `0x613c8` | gr::DIPrimitive |
| 34 | `0x613d0` | gr::DIPrimitive |
| 35 | `0x613d8` | gr::DIPrimitive |
| 36 | `0x613e0` | gr::DIPrimitive |
| 37 | `0x613f0` | gr::DIPrimitive |
| 38 | `0x7ffec` | **gr::N3D_Primitive** |
| 39 | `0x613f8` | gr::DIPrimitive |
| 40 | `0x7ffe4` | **gr::N3D_Primitive** |
| 41 | `0x61440` | gr::DIPrimitive |
| 42 | `0x61448` | gr::DIPrimitive |
| 43 | `0x613e8` | gr::DIPrimitive |
| 44 | `0x614e0` | gr::DIPrimitive |
| 45 | `0x61504` | gr::DIPrimitive |
| 46 | `0x61528` | gr::DIPrimitive |
| 47 | `0x6154c` | gr::DIPrimitive |
| 48 | `0x61570` | gr::DIPrimitive |
| 49 | `0x61594` | gr::DIPrimitive |
| 50 | `0x615b8` | gr::DIPrimitive |
| 51 | `0x615dc` | gr::DIPrimitive |
| 52 | `0x61344` | gr::DIPrimitive |
| 53 | `0x61618` | gr::DIPrimitive |
| 54 | `0x6168c` | gr::DIPrimitive |
| 55 | `0x61694` | gr::DIPrimitive |
| 56 | `0x61f9c` | gr::DIPrimitive |
| 57 | `0x61fa4` | gr::DIPrimitive |
| 58 | `0x61fac` | gr::DIPrimitive |
| 59 | `0x8010c` | **gr::N3D_Primitive** |
| 60 | `0x7f6ac` | **gr::N3D_Primitive** |

### gr::N3D_Shader

Origen: N3D · base: gr::Shader → gr::ContextObject → lang::Object · 32 entradas, 18 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x14bfcc` | **gr::N3D_Shader** |
| 1 | `0x14bfb4` | **gr::N3D_Shader** |
| 2 | `0x14bfac` | **gr::N3D_Shader** |
| 3 | `0x14bfb0` | **gr::N3D_Shader** |
| 4 | `0x5ec2c` | gr::ContextObject |
| 5 | `0x148f6c` | gr::Shader |
| 6 | `0x806b8` | **gr::N3D_Shader** |
| 7 | `0x806bc` | **gr::N3D_Shader** |
| 8 | `0x806c4` | **gr::N3D_Shader** |
| 9 | `0x148f6c` | gr::Shader |
| 10 | `0x148f6c` | gr::Shader |
| 11 | `0x806f4` | **gr::N3D_Shader** |
| 12 | `0x806f8` | **gr::N3D_Shader** |
| 13 | `0x806fc` | **gr::N3D_Shader** |
| 14 | `0x8072c` | **gr::N3D_Shader** |
| 15 | `0x80730` | **gr::N3D_Shader** |
| 16 | `0x80760` | **gr::N3D_Shader** |
| 17 | `0x148f6c` | gr::Shader |
| 18 | `0x80764` | **gr::N3D_Shader** |
| 19 | `0x807a0` | **gr::N3D_Shader** |
| 20 | `0x807cc` | **gr::N3D_Shader** |
| 21 | `0x148f6c` | gr::Shader |
| 22 | `0x148f6c` | gr::Shader |
| 23 | `0x148f6c` | gr::Shader |
| 24 | `0x148f6c` | gr::Shader |
| 25 | `0x148f6c` | gr::Shader |
| 26 | `0x148f6c` | gr::Shader |
| 27 | `0x148f6c` | gr::Shader |
| 28 | `0x148f6c` | gr::Shader |
| 29 | `0x807d4` | **gr::N3D_Shader** |
| 30 | `0x148f6c` | gr::Shader |
| 31 | `0x148f6c` | **gr::N3D_Shader** |

### gr::N3D_Shader_Default

Origen: N3D · base: gr::N3D_Shader → gr::Shader → gr::ContextObject → lang::Object · 32 entradas, 19 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x808d8` | **gr::N3D_Shader_Default** |
| 1 | `0x80954` | **gr::N3D_Shader_Default** |
| 2 | `0x14bfac` | gr::N3D_Shader |
| 3 | `0x14bfb0` | gr::N3D_Shader |
| 4 | `0x5ec2c` | gr::ContextObject |
| 5 | `0x809dc` | **gr::N3D_Shader_Default** |
| 6 | `0x806b8` | gr::N3D_Shader |
| 7 | `0x806bc` | gr::N3D_Shader |
| 8 | `0x806c4` | gr::N3D_Shader |
| 9 | `0x80cfc` | **gr::N3D_Shader_Default** |
| 10 | `0x80dac` | **gr::N3D_Shader_Default** |
| 11 | `0x806f4` | gr::N3D_Shader |
| 12 | `0x80dd0` | **gr::N3D_Shader_Default** |
| 13 | `0x81128` | **gr::N3D_Shader_Default** |
| 14 | `0x81154` | **gr::N3D_Shader_Default** |
| 15 | `0x80730` | gr::N3D_Shader |
| 16 | `0x80760` | gr::N3D_Shader |
| 17 | `0x81640` | **gr::N3D_Shader_Default** |
| 18 | `0x80764` | gr::N3D_Shader |
| 19 | `0x807a0` | gr::N3D_Shader |
| 20 | `0x807cc` | gr::N3D_Shader |
| 21 | `0x80ab0` | **gr::N3D_Shader_Default** |
| 22 | `0x81224` | **gr::N3D_Shader_Default** |
| 23 | `0x81624` | **gr::N3D_Shader_Default** |
| 24 | `0x80c90` | **gr::N3D_Shader_Default** |
| 25 | `0x80ce8` | **gr::N3D_Shader_Default** |
| 26 | `0x80cf0` | **gr::N3D_Shader_Default** |
| 27 | `0x81634` | **gr::N3D_Shader_Default** |
| 28 | `0x81618` | **gr::N3D_Shader_Default** |
| 29 | `0x807d4` | gr::N3D_Shader |
| 30 | `0x8167c` | **gr::N3D_Shader_Default** |
| 31 | `0x81628` | **gr::N3D_Shader_Default** |

### gr::N3D_Shader_Default::Shared

Origen: N3D · base: lang::Object · 2 entradas, 2 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x14c104` | **gr::N3D_Shader_Default::Shared** |
| 1 | `0x14c0d0` | **gr::N3D_Shader_Default::Shared** |

### gr::N3D_Texture

Origen: N3D · base: gr::Texture → gr::BaseTexture → gr::ContextObject → lang::Object · 12 entradas, 11 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x804f4` | **gr::N3D_Texture** |
| 1 | `0x80530` | **gr::N3D_Texture** |
| 2 | `0x806ac` | **gr::N3D_Texture** |
| 3 | `0x806b0` | **gr::N3D_Texture** |
| 4 | `0x5ec2c` | gr::ContextObject |
| 5 | `0x80678` | **gr::N3D_Texture** |
| 6 | `0x80680` | **gr::N3D_Texture** |
| 7 | `0x80690` | **gr::N3D_Texture** |
| 8 | `0x8057c` | **gr::N3D_Texture** |
| 9 | `0x80604` | **gr::N3D_Texture** |
| 10 | `0x80620` | **gr::N3D_Texture** |
| 11 | `0x80698` | **gr::N3D_Texture** |

### gr::Palette

Origen: KA3D · base: gr::ContextObject → lang::Object · 8 entradas, 5 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x14c1dc` | **gr::Palette** |
| 1 | `0x14c1c4` | **gr::Palette** |
| 2 | `0x5ec24` | gr::ContextObject |
| 3 | `0x5ec28` | gr::ContextObject |
| 4 | `0x5ec2c` | gr::ContextObject |
| 5 | `0x148f6c` | **gr::Palette** |
| 6 | `0x148f6c` | **gr::Palette** |
| 7 | `0x148f6c` | **gr::Palette** |

### gr::Primitive

Origen: KA3D · base: gr::ContextObject → lang::Object · 59 entradas, 56 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x14c224` | **gr::Primitive** |
| 1 | `0x14c20c` | **gr::Primitive** |
| 2 | `0x5ec24` | gr::ContextObject |
| 3 | `0x5ec28` | gr::ContextObject |
| 4 | `0x5ec2c` | gr::ContextObject |
| 5 | `0x148f6c` | **gr::Primitive** |
| 6 | `0x620d8` | **gr::Primitive** |
| 7 | `0x620dc` | **gr::Primitive** |
| 8 | `0x148f6c` | **gr::Primitive** |
| 9 | `0x148f6c` | **gr::Primitive** |
| 10 | `0x148f6c` | **gr::Primitive** |
| 11 | `0x148f6c` | **gr::Primitive** |
| 12 | `0x148f6c` | **gr::Primitive** |
| 13 | `0x148f6c` | **gr::Primitive** |
| 14 | `0x148f6c` | **gr::Primitive** |
| 15 | `0x148f6c` | **gr::Primitive** |
| 16 | `0x148f6c` | **gr::Primitive** |
| 17 | `0x148f6c` | **gr::Primitive** |
| 18 | `0x148f6c` | **gr::Primitive** |
| 19 | `0x148f6c` | **gr::Primitive** |
| 20 | `0x148f6c` | **gr::Primitive** |
| 21 | `0x148f6c` | **gr::Primitive** |
| 22 | `0x148f6c` | **gr::Primitive** |
| 23 | `0x148f6c` | **gr::Primitive** |
| 24 | `0x148f6c` | **gr::Primitive** |
| 25 | `0x620e0` | **gr::Primitive** |
| 26 | `0x148f6c` | **gr::Primitive** |
| 27 | `0x148f6c` | **gr::Primitive** |
| 28 | `0x148f6c` | **gr::Primitive** |
| 29 | `0x148f6c` | **gr::Primitive** |
| 30 | `0x148f6c` | **gr::Primitive** |
| 31 | `0x148f6c` | **gr::Primitive** |
| 32 | `0x148f6c` | **gr::Primitive** |
| 33 | `0x148f6c` | **gr::Primitive** |
| 34 | `0x148f6c` | **gr::Primitive** |
| 35 | `0x148f6c` | **gr::Primitive** |
| 36 | `0x148f6c` | **gr::Primitive** |
| 37 | `0x148f6c` | **gr::Primitive** |
| 38 | `0x148f6c` | **gr::Primitive** |
| 39 | `0x148f6c` | **gr::Primitive** |
| 40 | `0x148f6c` | **gr::Primitive** |
| 41 | `0x148f6c` | **gr::Primitive** |
| 42 | `0x148f6c` | **gr::Primitive** |
| 43 | `0x148f6c` | **gr::Primitive** |
| 44 | `0x148f6c` | **gr::Primitive** |
| 45 | `0x148f6c` | **gr::Primitive** |
| 46 | `0x148f6c` | **gr::Primitive** |
| 47 | `0x148f6c` | **gr::Primitive** |
| 48 | `0x148f6c` | **gr::Primitive** |
| 49 | `0x148f6c` | **gr::Primitive** |
| 50 | `0x148f6c` | **gr::Primitive** |
| 51 | `0x148f6c` | **gr::Primitive** |
| 52 | `0x148f6c` | **gr::Primitive** |
| 53 | `0x148f6c` | **gr::Primitive** |
| 54 | `0x148f6c` | **gr::Primitive** |
| 55 | `0x148f6c` | **gr::Primitive** |
| 56 | `0x148f6c` | **gr::Primitive** |
| 57 | `0x148f6c` | **gr::Primitive** |
| 58 | `0x148f6c` | **gr::Primitive** |

### gr::Shader

Origen: KA3D · base: gr::ContextObject → lang::Object · 31 entradas, 28 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x14c184` | **gr::Shader** |
| 1 | `0x14c16c` | **gr::Shader** |
| 2 | `0x5ec24` | gr::ContextObject |
| 3 | `0x5ec28` | gr::ContextObject |
| 4 | `0x5ec2c` | gr::ContextObject |
| 5 | `0x148f6c` | **gr::Shader** |
| 6 | `0x148f6c` | **gr::Shader** |
| 7 | `0x148f6c` | **gr::Shader** |
| 8 | `0x148f6c` | **gr::Shader** |
| 9 | `0x148f6c` | **gr::Shader** |
| 10 | `0x148f6c` | **gr::Shader** |
| 11 | `0x148f6c` | **gr::Shader** |
| 12 | `0x148f6c` | **gr::Shader** |
| 13 | `0x148f6c` | **gr::Shader** |
| 14 | `0x148f6c` | **gr::Shader** |
| 15 | `0x148f6c` | **gr::Shader** |
| 16 | `0x148f6c` | **gr::Shader** |
| 17 | `0x148f6c` | **gr::Shader** |
| 18 | `0x148f6c` | **gr::Shader** |
| 19 | `0x148f6c` | **gr::Shader** |
| 20 | `0x148f6c` | **gr::Shader** |
| 21 | `0x148f6c` | **gr::Shader** |
| 22 | `0x148f6c` | **gr::Shader** |
| 23 | `0x148f6c` | **gr::Shader** |
| 24 | `0x148f6c` | **gr::Shader** |
| 25 | `0x148f6c` | **gr::Shader** |
| 26 | `0x148f6c` | **gr::Shader** |
| 27 | `0x148f6c` | **gr::Shader** |
| 28 | `0x148f6c` | **gr::Shader** |
| 29 | `0x148f6c` | **gr::Shader** |
| 30 | `0x148f6c` | **gr::Shader** |

### gr::Texture

Origen: KA3D · base: gr::BaseTexture → gr::ContextObject → lang::Object · 12 entradas, 6 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x14c200` | **gr::Texture** |
| 1 | `0x14c1e8` | **gr::Texture** |
| 2 | `0x5ec24` | gr::ContextObject |
| 3 | `0x5ec28` | gr::ContextObject |
| 4 | `0x5ec2c` | gr::ContextObject |
| 5 | `0x148f6c` | gr::BaseTexture |
| 6 | `0x148f6c` | gr::BaseTexture |
| 7 | `0x148f6c` | gr::BaseTexture |
| 8 | `0x148f6c` | **gr::Texture** |
| 9 | `0x148f6c` | **gr::Texture** |
| 10 | `0x148f6c` | **gr::Texture** |
| 11 | `0x148f6c` | **gr::Texture** |

### hgr::Camera

Origen: KA3D · base: hgr::Node → lang::Object · 3 entradas, 3 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x82b24` | **hgr::Camera** |
| 1 | `0x82bc8` | **hgr::Camera** |
| 2 | `0x82be0` | **hgr::Camera** |

### hgr::DefaultPipe

Origen: KA3D · base: hgr::Pipe → lang::Object · 3 entradas, 3 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x14cd10` | **hgr::DefaultPipe** |
| 1 | `0x14ccf8` | **hgr::DefaultPipe** |
| 2 | `0x83900` | **hgr::DefaultPipe** |

### hgr::DefaultResourceManager

Origen: KA3D · base: hgr::ResourceManager → lang::Object · 6 entradas, 6 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x83c94` | **hgr::DefaultResourceManager** |
| 1 | `0x83e28` | **hgr::DefaultResourceManager** |
| 2 | `0x83e40` | **hgr::DefaultResourceManager** |
| 3 | `0x83ef4` | **hgr::DefaultResourceManager** |
| 4 | `0x841a0` | **hgr::DefaultResourceManager** |
| 5 | `0x84268` | **hgr::DefaultResourceManager** |

### hgr::Dummy

Origen: KA3D · base: hgr::Node → lang::Object · 3 entradas, 3 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x84978` | **hgr::Dummy** |
| 1 | `0x8497c` | **hgr::Dummy** |
| 2 | `0x84994` | **hgr::Dummy** |

### hgr::GlowPipe

Origen: KA3D · base: hgr::Pipe → lang::Object · 3 entradas, 3 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x14d0b8` | **hgr::GlowPipe** |
| 1 | `0x14d0a0` | **hgr::GlowPipe** |
| 2 | `0x852a8` | **hgr::GlowPipe** |

### hgr::KeyframeSequence

Origen: KA3D · base: lang::Object · 2 entradas, 2 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x14cf88` | **hgr::KeyframeSequence** |
| 1 | `0x14cf54` | **hgr::KeyframeSequence** |

### hgr::Light

Origen: KA3D · base: hgr::Node → lang::Object · 3 entradas, 3 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x86238` | **hgr::Light** |
| 1 | `0x8623c` | **hgr::Light** |
| 2 | `0x86254` | **hgr::Light** |

### hgr::Lines

Origen: KA3D · base: hgr::Visual → hgr::Node → lang::Object · 6 entradas, 6 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x88a5c` | **hgr::Lines** |
| 1 | `0x88aac` | **hgr::Lines** |
| 2 | `0x88b08` | **hgr::Lines** |
| 3 | `0x88b30` | **hgr::Lines** |
| 4 | `0x88ee8` | **hgr::Lines** |
| 5 | `0x88f98` | **hgr::Lines** |

### hgr::Mesh

Origen: KA3D · base: hgr::Visual → hgr::Node → lang::Object · 6 entradas, 6 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x8729c` | **hgr::Mesh** |
| 1 | `0x872d8` | **hgr::Mesh** |
| 2 | `0x87320` | **hgr::Mesh** |
| 3 | `0x8746c` | **hgr::Mesh** |
| 4 | `0x87f00` | **hgr::Mesh** |
| 5 | `0x87fe0` | **hgr::Mesh** |

### hgr::Node

Origen: KA3D · base: lang::Object · 3 entradas, 3 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x89ef4` | **hgr::Node** |
| 1 | `0x89f98` | **hgr::Node** |
| 2 | `0x8a334` | **hgr::Node** |

### hgr::ParticleSystem

Origen: KA3D · base: hgr::Visual → hgr::Node → lang::Object · 6 entradas, 6 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x8cf78` | **hgr::ParticleSystem** |
| 1 | `0x8cffc` | **hgr::ParticleSystem** |
| 2 | `0x8d078` | **hgr::ParticleSystem** |
| 3 | `0x8eec8` | **hgr::ParticleSystem** |
| 4 | `0x8ef40` | **hgr::ParticleSystem** |
| 5 | `0x8efec` | **hgr::ParticleSystem** |

### hgr::ParticleSystem::Description

Origen: KA3D · base: lang::Object · 2 entradas, 2 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x14ce50` | **hgr::ParticleSystem::Description** |
| 1 | `0x14cd90` | **hgr::ParticleSystem::Description** |

### hgr::Pipe

Origen: KA3D · base: lang::Object · 3 entradas, 3 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x90f14` | **hgr::Pipe** |
| 1 | `0x90f58` | **hgr::Pipe** |
| 2 | `0x148f6c` | **hgr::Pipe** |

### hgr::PipeSetup

Origen: KA3D · base: lang::Object · 3 entradas, 3 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x913a4` | **hgr::PipeSetup** |
| 1 | `0x9142c` | **hgr::PipeSetup** |
| 2 | `0x91d1c` | **hgr::PipeSetup** |

### hgr::Scene

Origen: KA3D · base: hgr::Node → lang::Object · 3 entradas, 3 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x95940` | **hgr::Scene** |
| 1 | `0x959b0` | **hgr::Scene** |
| 2 | `0x95a20` | **hgr::Scene** |

### hgr::SceneInputStream

Origen: KA3D · base: io::DataInputStream → io::FilterInputStream → io::InputStream → lang::Object · 17 entradas, 2 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x969dc` | **hgr::SceneInputStream** |
| 1 | `0x969e0` | **hgr::SceneInputStream** |
| 2 | `0x6e1b4` | io::DataInputStream |
| 3 | `0x6e1a0` | io::DataInputStream |
| 4 | `0x6f4e8` | io::FilterInputStream |
| 5 | `0x6f500` | io::FilterInputStream |
| 6 | `0x6e1c8` | io::DataInputStream |
| 7 | `0x6e318` | io::DataInputStream |
| 8 | `0x6e368` | io::DataInputStream |
| 9 | `0x6e3a8` | io::DataInputStream |
| 10 | `0x6e3e8` | io::DataInputStream |
| 11 | `0x6e57c` | io::DataInputStream |
| 12 | `0x6e64c` | io::DataInputStream |
| 13 | `0x6e71c` | io::DataInputStream |
| 14 | `0x6e7ec` | io::DataInputStream |
| 15 | `0x6e8bc` | io::DataInputStream |
| 16 | `0x6eac4` | io::DataInputStream |

### hgr::SceneOutputStream

Origen: KA3D · base: io::DataOutputStream → io::FilterOutputStream → io::OutputStream → lang::Object · 13 entradas, 2 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x99174` | **hgr::SceneOutputStream** |
| 1 | `0x99180` | **hgr::SceneOutputStream** |
| 2 | `0x6f538` | io::FilterOutputStream |
| 3 | `0x6f568` | io::FilterOutputStream |
| 4 | `0x6ee08` | io::DataOutputStream |
| 5 | `0x6ee48` | io::DataOutputStream |
| 6 | `0x6ee88` | io::DataOutputStream |
| 7 | `0x6eec8` | io::DataOutputStream |
| 8 | `0x6efc4` | io::DataOutputStream |
| 9 | `0x6f0a0` | io::DataOutputStream |
| 10 | `0x6f174` | io::DataOutputStream |
| 11 | `0x6f248` | io::DataOutputStream |
| 12 | `0x6f31c` | io::DataOutputStream |

### hgr::TransformAnimation

Origen: KA3D · base: lang::Object · 2 entradas, 2 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x9b3c4` | **hgr::TransformAnimation** |
| 1 | `0x9b4b8` | **hgr::TransformAnimation** |

### hgr::TransformAnimation::Float3Anim

Origen: KA3D · base: lang::Object · 2 entradas, 2 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x14cfe4` | **hgr::TransformAnimation::Float3Anim** |
| 1 | `0x14cfb0` | **hgr::TransformAnimation::Float3Anim** |

### hgr::TransformAnimationSet

Origen: KA3D · base: lang::Hashtable<lang::String · 2 entradas, 2 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x14d024` | **hgr::TransformAnimationSet** |
| 1 | `0x14d00c` | **hgr::TransformAnimationSet** |

### hgr::UserPropertySet

Origen: KA3D · base: lang::Hashtable<lang::String · 2 entradas, 2 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x14cf50` | **hgr::UserPropertySet** |
| 1 | `0x14cf38` | **hgr::UserPropertySet** |

### hgr::Visual

Origen: KA3D · base: hgr::Node → lang::Object · 6 entradas, 6 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x9feb0` | **hgr::Visual** |
| 1 | `0x9feb4` | **hgr::Visual** |
| 2 | `0x9fecc` | **hgr::Visual** |
| 3 | `0x148f6c` | **hgr::Visual** |
| 4 | `0x148f6c` | **hgr::Visual** |
| 5 | `0x148f6c` | **hgr::Visual** |

### img::ImageReader

Origen: KA3D · base: lang::Object · 2 entradas, 2 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x70fe4` | **img::ImageReader** |
| 1 | `0x71044` | **img::ImageReader** |

### io::ByteArrayInputStream

Origen: KA3D · base: io::InputStream → lang::Object · 6 entradas, 6 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x6e038` | **io::ByteArrayInputStream** |
| 1 | `0x6e05c` | **io::ByteArrayInputStream** |
| 2 | `0x6e08c` | **io::ByteArrayInputStream** |
| 3 | `0x6dd90` | **io::ByteArrayInputStream** |
| 4 | `0x6e0dc` | **io::ByteArrayInputStream** |
| 5 | `0x6e0ec` | **io::ByteArrayInputStream** |

### io::DataInputStream

Origen: KA3D · base: io::FilterInputStream → io::InputStream → lang::Object · 17 entradas, 15 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x6e13c` | **io::DataInputStream** |
| 1 | `0x6e168` | **io::DataInputStream** |
| 2 | `0x6e1b4` | **io::DataInputStream** |
| 3 | `0x6e1a0` | **io::DataInputStream** |
| 4 | `0x6f4e8` | io::FilterInputStream |
| 5 | `0x6f500` | io::FilterInputStream |
| 6 | `0x6e1c8` | **io::DataInputStream** |
| 7 | `0x6e318` | **io::DataInputStream** |
| 8 | `0x6e368` | **io::DataInputStream** |
| 9 | `0x6e3a8` | **io::DataInputStream** |
| 10 | `0x6e3e8` | **io::DataInputStream** |
| 11 | `0x6e57c` | **io::DataInputStream** |
| 12 | `0x6e64c` | **io::DataInputStream** |
| 13 | `0x6e71c` | **io::DataInputStream** |
| 14 | `0x6e7ec` | **io::DataInputStream** |
| 15 | `0x6e8bc` | **io::DataInputStream** |
| 16 | `0x6eac4` | **io::DataInputStream** |

### io::DataOutputStream

Origen: KA3D · base: io::FilterOutputStream → io::OutputStream → lang::Object · 13 entradas, 11 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x14c3e8` | **io::DataOutputStream** |
| 1 | `0x14c3d0` | **io::DataOutputStream** |
| 2 | `0x6f538` | io::FilterOutputStream |
| 3 | `0x6f568` | io::FilterOutputStream |
| 4 | `0x6ee08` | **io::DataOutputStream** |
| 5 | `0x6ee48` | **io::DataOutputStream** |
| 6 | `0x6ee88` | **io::DataOutputStream** |
| 7 | `0x6eec8` | **io::DataOutputStream** |
| 8 | `0x6efc4` | **io::DataOutputStream** |
| 9 | `0x6f0a0` | **io::DataOutputStream** |
| 10 | `0x6f174` | **io::DataOutputStream** |
| 11 | `0x6f248` | **io::DataOutputStream** |
| 12 | `0x6f31c` | **io::DataOutputStream** |

### io::FileInputStream

Origen: KA3D · base: io::InputStream → lang::Object · 6 entradas, 6 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x6d130` | **io::FileInputStream** |
| 1 | `0x6d160` | **io::FileInputStream** |
| 2 | `0x6d19c` | **io::FileInputStream** |
| 3 | `0x6dd90` | **io::FileInputStream** |
| 4 | `0x6d2b8` | **io::FileInputStream** |
| 5 | `0x6d3c4` | **io::FileInputStream** |

### io::FileOutputStream

Origen: KA3D · base: io::OutputStream → lang::Object · 4 entradas, 4 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x6d4c8` | **io::FileOutputStream** |
| 1 | `0x6d4f8` | **io::FileOutputStream** |
| 2 | `0x6d534` | **io::FileOutputStream** |
| 3 | `0x6d650` | **io::FileOutputStream** |

### io::FilterInputStream

Origen: KA3D · base: io::InputStream → lang::Object · 6 entradas, 6 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x14c40c` | **io::FilterInputStream** |
| 1 | `0x14c3f4` | **io::FilterInputStream** |
| 2 | `0x6f498` | **io::FilterInputStream** |
| 3 | `0x6f4c0` | **io::FilterInputStream** |
| 4 | `0x6f4e8` | **io::FilterInputStream** |
| 5 | `0x6f500` | **io::FilterInputStream** |

### io::FilterOutputStream

Origen: KA3D · base: io::OutputStream → lang::Object · 4 entradas, 4 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x14c430` | **io::FilterOutputStream** |
| 1 | `0x14c418` | **io::FilterOutputStream** |
| 2 | `0x6f538` | **io::FilterOutputStream** |
| 3 | `0x6f568` | **io::FilterOutputStream** |

### io::FindFile

Origen: KA3D · base: lang::Object · 2 entradas, 2 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x6ce30` | **io::FindFile** |
| 1 | `0x6ce74` | **io::FindFile** |

### io::FindFile::Impl

Origen: KA3D · base: lang::Object · 2 entradas, 2 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x14cc4c` | **io::FindFile::Impl** |
| 1 | `0x14cbc0` | **io::FindFile::Impl** |

### io::PropertyParser

Origen: KA3D · base: lang::Object · 2 entradas, 2 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x14c34c` | **io::PropertyParser** |
| 1 | `0x14c334` | **io::PropertyParser** |

### lang::Hashtable<lang::String, hgr::DefaultResourceManager::TextureResource, lang::Hash<lang::String>>

Origen: KA3D · base: lang::Object · 2 entradas, 2 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x157650` | **lang::Hashtable<lang::String, hgr::DefaultResourceManager::TextureResource, lang::Hash<lang::String>>** |
| 1 | `0x14da94` | **lang::Hashtable<lang::String, hgr::DefaultResourceManager::TextureResource, lang::Hash<lang::String>>** |

### lang::Hashtable<lang::String, int, lang::Hash<lang::String>>

Origen: KA3D · base: lang::Object · 2 entradas, 2 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x158b10` | **lang::Hashtable<lang::String, int, lang::Hash<lang::String>>** |
| 1 | `0x14db0c` | **lang::Hashtable<lang::String, int, lang::Hash<lang::String>>** |

### lang::Hashtable<lang::String, lang::Ptr<gr::CubeTexture>, lang::Hash<lang::String>>

Origen: KA3D · base: lang::Object · 2 entradas, 2 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x157a0c` | **lang::Hashtable<lang::String, lang::Ptr<gr::CubeTexture>, lang::Hash<lang::String>>** |
| 1 | `0x14daac` | **lang::Hashtable<lang::String, lang::Ptr<gr::CubeTexture>, lang::Hash<lang::String>>** |

### lang::Hashtable<lang::String, lang::Ptr<hgr::ParticleSystem>, lang::Hash<lang::String>>

Origen: KA3D · base: lang::Object · 2 entradas, 2 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x157ddc` | **lang::Hashtable<lang::String, lang::Ptr<hgr::ParticleSystem>, lang::Hash<lang::String>>** |
| 1 | `0x14dac4` | **lang::Hashtable<lang::String, lang::Ptr<hgr::ParticleSystem>, lang::Hash<lang::String>>** |

### lang::Hashtable<lang::String, lang::Ptr<hgr::TransformAnimation>, lang::Hash<lang::String>>

Origen: KA3D · base: lang::Object · 2 entradas, 2 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x1586b4` | **lang::Hashtable<lang::String, lang::Ptr<hgr::TransformAnimation>, lang::Hash<lang::String>>** |
| 1 | `0x14dadc` | **lang::Hashtable<lang::String, lang::Ptr<hgr::TransformAnimation>, lang::Hash<lang::String>>** |

### lang::Hashtable<lang::String, lang::String, lang::Hash<lang::String>>

Origen: KA3D · base: lang::Object · 2 entradas, 2 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x158a04` | **lang::Hashtable<lang::String, lang::String, lang::Hash<lang::String>>** |
| 1 | `0x14daf4` | **lang::Hashtable<lang::String, lang::String, lang::Hash<lang::String>>** |

### lang::MemoryPool

Origen: KA3D · base: lang::Object · 2 entradas, 2 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x677d4` | **lang::MemoryPool** |
| 1 | `0x67810` | **lang::MemoryPool** |

### lang::Object

Origen: KA3D · base: — · 2 entradas, 2 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x67cc0` | **lang::Object** |
| 1 | `0x67ccc` | **lang::Object** |

### lang::UTFConverter

Origen: KA3D · base: lang::Converter → lang::Object · 4 entradas, 4 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x14d768` | **lang::UTFConverter** |
| 1 | `0x14d750` | **lang::UTFConverter** |
| 2 | `0x6a3c8` | **lang::UTFConverter** |
| 3 | `0x6a674` | **lang::UTFConverter** |

### nlibs::CIN3dEngine

Origen: N3D · base: nlibs::CN3dEngine · 46 entradas, 46 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0xa07f8` | **nlibs::CIN3dEngine** |
| 1 | `0xa0334` | **nlibs::CIN3dEngine** |
| 2 | `0xa03f8` | **nlibs::CIN3dEngine** |
| 3 | `0xa0dd0` | **nlibs::CIN3dEngine** |
| 4 | `0xa0e04` | **nlibs::CIN3dEngine** |
| 5 | `0xa0e40` | **nlibs::CIN3dEngine** |
| 6 | `0xa0eb4` | **nlibs::CIN3dEngine** |
| 7 | `0xa0ec8` | **nlibs::CIN3dEngine** |
| 8 | `0xa0ef4` | **nlibs::CIN3dEngine** |
| 9 | `0xa0f60` | **nlibs::CIN3dEngine** |
| 10 | `0xa0f8c` | **nlibs::CIN3dEngine** |
| 11 | `0xa0fd4` | **nlibs::CIN3dEngine** |
| 12 | `0xa0fdc` | **nlibs::CIN3dEngine** |
| 13 | `0xa1024` | **nlibs::CIN3dEngine** |
| 14 | `0xa1050` | **nlibs::CIN3dEngine** |
| 15 | `0xa1090` | **nlibs::CIN3dEngine** |
| 16 | `0xa10bc` | **nlibs::CIN3dEngine** |
| 17 | `0xa1104` | **nlibs::CIN3dEngine** |
| 18 | `0xa110c` | **nlibs::CIN3dEngine** |
| 19 | `0xa1178` | **nlibs::CIN3dEngine** |
| 20 | `0xa1180` | **nlibs::CIN3dEngine** |
| 21 | `0xa1268` | **nlibs::CIN3dEngine** |
| 22 | `0xa1270` | **nlibs::CIN3dEngine** |
| 23 | `0xa1280` | **nlibs::CIN3dEngine** |
| 24 | `0xa128c` | **nlibs::CIN3dEngine** |
| 25 | `0xa12c8` | **nlibs::CIN3dEngine** |
| 26 | `0xa1308` | **nlibs::CIN3dEngine** |
| 27 | `0xa13b8` | **nlibs::CIN3dEngine** |
| 28 | `0xa13cc` | **nlibs::CIN3dEngine** |
| 29 | `0xa13e0` | **nlibs::CIN3dEngine** |
| 30 | `0xa1748` | **nlibs::CIN3dEngine** |
| 31 | `0xa1a08` | **nlibs::CIN3dEngine** |
| 32 | `0xa1c28` | **nlibs::CIN3dEngine** |
| 33 | `0xa1eb0` | **nlibs::CIN3dEngine** |
| 34 | `0xa1fa0` | **nlibs::CIN3dEngine** |
| 35 | `0xa1fac` | **nlibs::CIN3dEngine** |
| 36 | `0xa1fb8` | **nlibs::CIN3dEngine** |
| 37 | `0xa1fc4` | **nlibs::CIN3dEngine** |
| 38 | `0xa1fd0` | **nlibs::CIN3dEngine** |
| 39 | `0xa1fd8` | **nlibs::CIN3dEngine** |
| 40 | `0xa1fe0` | **nlibs::CIN3dEngine** |
| 41 | `0xa1fe8` | **nlibs::CIN3dEngine** |
| 42 | `0xa1ff0` | **nlibs::CIN3dEngine** |
| 43 | `0xa1ff8` | **nlibs::CIN3dEngine** |
| 44 | `0xac210` | **nlibs::CIN3dEngine** |
| 45 | `0xb3460` | **nlibs::CIN3dEngine** |

### nlibs::CIN3dFaceBuffer

Origen: N3D · base: nlibs::CN3dFaceBuffer · 11 entradas, 11 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0xc1320` | **nlibs::CIN3dFaceBuffer** |
| 1 | `0xc129c` | **nlibs::CIN3dFaceBuffer** |
| 2 | `0xc12d8` | **nlibs::CIN3dFaceBuffer** |
| 3 | `0xc13d4` | **nlibs::CIN3dFaceBuffer** |
| 4 | `0xc13dc` | **nlibs::CIN3dFaceBuffer** |
| 5 | `0xc13e4` | **nlibs::CIN3dFaceBuffer** |
| 6 | `0xc13ec` | **nlibs::CIN3dFaceBuffer** |
| 7 | `0xc13f4` | **nlibs::CIN3dFaceBuffer** |
| 8 | `0xc13fc` | **nlibs::CIN3dFaceBuffer** |
| 9 | `0xc1408` | **nlibs::CIN3dFaceBuffer** |
| 10 | `0xc1414` | **nlibs::CIN3dFaceBuffer** |

### nlibs::CIN3dTexture

Origen: N3D · base: nlibs::CN3dTexture · 10 entradas, 10 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0xc1814` | **nlibs::CIN3dTexture** |
| 1 | `0xc1788` | **nlibs::CIN3dTexture** |
| 2 | `0xc17b8` | **nlibs::CIN3dTexture** |
| 3 | `0xc196c` | **nlibs::CIN3dTexture** |
| 4 | `0xc1974` | **nlibs::CIN3dTexture** |
| 5 | `0xc197c` | **nlibs::CIN3dTexture** |
| 6 | `0xc1984` | **nlibs::CIN3dTexture** |
| 7 | `0xc1990` | **nlibs::CIN3dTexture** |
| 8 | `0xc1998` | **nlibs::CIN3dTexture** |
| 9 | `0xc19a0` | **nlibs::CIN3dTexture** |

### nlibs::CIN3dVertexBuffer

Origen: N3D · base: nlibs::CN3dVertexBuffer · 11 entradas, 11 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0xc0bd0` | **nlibs::CIN3dVertexBuffer** |
| 1 | `0xc0b64` | **nlibs::CIN3dVertexBuffer** |
| 2 | `0xc0b94` | **nlibs::CIN3dVertexBuffer** |
| 3 | `0xc11a8` | **nlibs::CIN3dVertexBuffer** |
| 4 | `0xc11b0` | **nlibs::CIN3dVertexBuffer** |
| 5 | `0xc11b8` | **nlibs::CIN3dVertexBuffer** |
| 6 | `0xc11c0` | **nlibs::CIN3dVertexBuffer** |
| 7 | `0xc11c8` | **nlibs::CIN3dVertexBuffer** |
| 8 | `0xc11d0` | **nlibs::CIN3dVertexBuffer** |
| 9 | `0xc11d8` | **nlibs::CIN3dVertexBuffer** |
| 10 | `0xc11e0` | **nlibs::CIN3dVertexBuffer** |

### nlibs::CNRenderer

Origen: N3D · base: — · 14 entradas, 14 propias

| Entrada | Función | Implementa |
|---|---|---|
| 0 | `0x131034` | **nlibs::CNRenderer** |
| 1 | `0x131054` | **nlibs::CNRenderer** |
| 2 | `0x131080` | **nlibs::CNRenderer** |
| 3 | `0x131214` | **nlibs::CNRenderer** |
| 4 | `0x131224` | **nlibs::CNRenderer** |
| 5 | `0x13137c` | **nlibs::CNRenderer** |
| 6 | `0x131400` | **nlibs::CNRenderer** |
| 7 | `0x131428` | **nlibs::CNRenderer** |
| 8 | `0x1316f8` | **nlibs::CNRenderer** |
| 9 | `0x131738` | **nlibs::CNRenderer** |
| 10 | `0x131888` | **nlibs::CNRenderer** |
| 11 | `0x131964` | **nlibs::CNRenderer** |
| 12 | `0x131abc` | **nlibs::CNRenderer** |
| 13 | `0x134790` | **nlibs::CNRenderer** |

