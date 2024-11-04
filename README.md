# MQ2GrimGUI

Converting MyUI Lua into a Plugin. 

This will give you a Main Window with toggles for enable/disabling other windows. 

Current Windows include: 

* Player Window (Target window is included if not split out)
* Target Window (You can split this out from the Player Window and either use both or as a standalone)
* Config Window For Setting your Color Selections and More Settings to Come.

## Getting Started

Quick start instructions to get users up and going

```txt
/plugin MQ2GrimGUI
```

### Commands

Describe the commands available and how to use them.

```txt
/grimgui -- Toggles the MainWindow (with the window toggles on it)
```

### Configuration File

Describe the configuration file and what the settings do

Your Settings will default to MQ2GrimGUI.ini while you are not logged into a character.

Once you Log in we will Load MQ2GrimGUI_Server name_CharName.ini file or create one from the defaults if it is missing.

This will revert back after you camp to charecter select. 

```yaml
[Settings]
ShowMainGui=1 -- Show the Main Window
[PlayerTarg]
SplitTarget=1  -- Split Target window out of the PlayerWindow (Or only target window if ShowPlayerWindow is set to 0)
ShowPlayerWindow=1  -- Show the Player Window
[Colors]
-- Min and Max Color's for progress bars. the bar will transition from One to the other as they fill/empty
MinColorHP=0.876,0.341,1,1
MaxColorHP=0.845,0.151,0.151,1
MinColorEnd=0.56962,0.322716,0.139401,1
MaxColorEnd=1,0.900141,0.400844,0.966825
MinColorMP=0.259,0.114,0.514,1
MaxColorMP=0.079,0.468,0.848,1
```

## Other Notes

Add additional notes

## Authors

* **Grimmier** - *Initial work*

See also the list of [contributors](https://github.com/grimmier378/MQ2GrimGUI/contributors) who participated in this project.

## Acknowledgments

