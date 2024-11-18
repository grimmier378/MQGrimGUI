# MQ2GrimGUI

Converting MyUI Lua into a Plugin. 

This will give you a Main Window with toggles for enable/disabling other windows. 

Current Windows include: 

* Main Window (This containes toggles to all of the other windows as a button to get to config)
* Player Window (Target window is included if not split out)
* Target Window (You can split this out from the Player Window and either use both or as a standalone)
* Group Window (Shows party members and allows you drop items onto their bars to trade)
* Buff Window (Shows Buffs and Debuffs)
* Song Window (Shows Bard Songs, and Short Duration Buffs)
* Spell Window (Shows your spells and allows you to click to cast, right click to forget and Alt Click to Inspect, Right clicking an empty spell gem will bring up a spell picker)
* Pet Window (Shows your pet You can customize which buttons you want on the window. Drop an item onto the pet health bar to trade.)
* Config Window For Setting your Color Selections Bar/Gem scaling and flash settings for pusling icons or combat indicator.

Theme Support built in:
 
* Each window can be themed and these will persist between sessions.
* Currently there are 6 themes included beyond Default.
* Custom themes may be added at a future date.


## Getting Started

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
Command  | Option   | Description
---------|----------|---------------------
/grimgui | [show]   | Toggles Main Window
/grimgui | [lock]   | Toggles Window Locking]
/grimgui | [player] | Toggles Player Window
/grimgui | [target] | Toggles Target Window
/grimgui | [pet]    | Toggles Pet Window
/grimgui | [group]  | Toggles Group Window
/grimgui | [spells] | Toggles Spells Window
/grimgui | [buffs]  | Toggles Buffs Window
/grimgui | [songs]  | Toggles Songs Window
/grimgui | [config] | Opens Configuration Window
/grimgui | [help]   | Displays this help message
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
LockWindows=0
BuffIconSize=24
FlashBuffInterval=40
[PlayerTarg]
SplitTarget=0
ShowPlayerWindow=0
CombatFlashInterval=100
PlayerBarHeight=15
TargetBarHeight=15
AggroBarHeight=10
Theme=Default
[Pet]
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
GroupBarHeight=15
Theme=Default
[Spells]
ShowSpellsWindow=0
SpellGemHeight=32
Theme=Default
[Buffs]
ShowBuffWindow=0
BuffTimerThreshold=0
Theme=Default
[Songs]
ShowSongWindow=0
Theme=Default
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

