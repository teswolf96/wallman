# Wallman

A simple tool for managing wallpaper profiles with feh. Designed for use in i3.

Optional dependency of jgmenu can be used for displaying a menu.

Usage: 

wallman [option(s)] [profile]

-l --list - list profiles

-a --apply - apply a profile (Does not set as current)

-s --set - apply a profile and set as current

-c --current - apply currently set profile

-f --temp-config - set the config file to use for the command

-F --set-config - set the default config file to use

-d --delete - delete a profile

-p --profile - set the profile to modify

-n --new - Create a new profile

-t --title - set the profile to modify

-C --category - set the profile to modify

-H --hidden - set the monitor to modify

-w --wallpaper - set the path to a wallpaper
	If no profile is set, this will change only your current profile

-S --save - save the current profile as a new profile

-O --overwrite - when saving, overwrite an existing profile if it exists

-j --jgmenu - force regenerate the jgmenu

