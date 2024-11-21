# MQ2GrimGUI

Converting MyUI Lua into a Plugin. 

Giving you a feature filled ImGui based UI for the game.

_Current Windows:_ 

* Main Window (This containes toggles to all of the other windows as a button to get to config)
* Player Window (Target window is included if not split out)
* Target Window (You can split this out from the Player Window and either use both or as a standalone)
* Group Window (Shows party members and allows you drop items onto their bars to trade)
* Casting Window (Shows the spell Casting Bar when you are casting a Spell, AA, Item...)
* Buff Window (Shows Buffs and Debuffs)
* Song Window (Shows Bard Songs, and Short Duration Buffs)
* Spell Window (Shows your spells and allows you to click to cast, right click to forget and Alt Click to Inspect, Right clicking an empty spell gem will bring up a spell picker)
* Pet Window (Shows your pet You can customize which buttons you want on the window. Drop an item onto the pet health bar to trade.)
* Config Window For Setting All the Things.

_Window Settings:_

* Window locking per window, also a Lock ALL override toggle.
* Right click Context for access to per window Settings and Locking.
* See the config Window for setting up your colors and other settings.

_Theme Support built-in:_
 
* Each window can be themed and these will persist between sessions.
* Currently there are 6 themes included beyond Default.
* Custom themes may be added at a future date.

### Getting Started

Quick start instructions to get users up and going

```txt
/plugin MQ2GrimGUI
The first time you load this you should get a Main Window with all the toggles on it on the screen.
If this does not show up try `/grimgui show` to toggle it.
From this windos you can toggle the other windows on or off and set up your settings and colors.
```

### Commands

Describe the commands available and how to use them.

```txt

Command  | Option		  | Description
---------|----------------|---------------------
/grimgui | [show]		  | Toggles Main Window
/grimgui | [lock]		  | Toggles Lock All Windows
/grimgui | [player]		  | Toggles Player Window
/grimgui | [target]		  | Toggles Target Window
/grimgui | [pet]		  | Toggles Pet Window
/grimgui | [group]		  | Toggles Group Window
/grimgui | [spells]		  | Toggles Spells Window
/grimgui | [buffs]		  | Toggles Buffs Window
/grimgui | [songs]		  | Toggles Songs Window
/grimgui | [config]		  | Opens Configuration Window
/grimgui | [clickthrough] | Toggles Hud Click Through
/grimgui | [help]		  | Displays this help message
```

### Configuration File

Describe the configuration file and what the settings do

Your Settings will default to MQ2GrimGUI.ini while you are not logged into a character.

Once you Log in we will Load MQ2GrimGUI_Server name_CharName.ini file or create one from the defaults if it is missing.

This will revert back after you camp to charecter select. 

```yaml
[Settings]
ShowMainGui=1
ShowTitleBars=1
BuffIconSize=20
FlashBuffInterval=4
[LockWindows]
LockAllWindows=0
LockPlayerWin=0
LockTargetWin=0
LockPetWin=0
LockGroupWin=0
LockSpellsWin=0
LockBuffsWin=0
LockSongWin=0
LockHudWin=0
[PlayerTarg]
SplitTarget=0
ShowPlayerWindow=0
ShowTargetBuffs=1
ShowAggroMeter=1
CombatFlashInterval=15
PlayerBarHeight=12
TargetBarHeight=12
AggroBarHeight=12
Theme=Default
[Pet]
ShowPetButtons=1
ShowPetWindow=0
Theme=Default
Attack=1
Sit=1
Follow=1
Hold=1
Taunt=1
Guard=1
Back=1
Focus=1
Stop=1
Leave=1
Regroup=1
Report=1
Swarm=1
Kill=1
[Group]
ShowGroupWindow=0
ShowSelfOnGroup=0
ShowEmptyGroup=1
GroupBarHeight=12
Theme=Default
[Songs]
ShowSongWindow=0
Theme=Default
[Spells]
ShowSpellsWindow=0
SpellGemHeight=32
Theme=Default
[Buffs]
ShowBuffWindow=0
BuffTimerThreshold=0
Theme=Default
[Hud]
ShowHud=0
HudClickThrough=0
HudAlpha=120
[Colors]
MinColorHP=4292827135
MaxColorHP=4292355879
MinColorMP=4282523011
MaxColorMP=4279531480
MinColorEnd=4294930181
MaxColorEnd=2998049050
MinColorCast=4292355879
MaxColorCast=4279531480
```

## Other Notes

https://www.youtube.com/watch?v=2xn2amsY28A

## Authors

* **Grimmier aka GrimGUI** - *Initial work*

See also the list of [contributors](https://github.com/grimmier378/MQ2GrimGUI/contributors) who participated in this project.

## Acknowledgments

