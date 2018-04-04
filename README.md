# Wallman

A simple tool for managing wallpaper profiles with feh. Designed for use in i3.

Optional dependency of jgmenu can be used for displaying a menu.

## Usage

wallman [option(s)]

-l --list - list profiles

-a --apply [profile] - apply a profile (Does not set as current)

-s --set [profile] - apply a profile and set as current

-c --current - apply currently set profile. Primarily used to re-apply wallpapers on login.

-f --temp-config [config file] - set the config file to use for the command

-F --set-config [config file] - set the default config file to use

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

## Anatomy of a Profile

The following is an example profile that will be used to explain the options for a profile

~~~~
Profile: ArchLinux
    Title: Arch Wallpapers
    Category: Linux
    Hidden: False
    Paths:
        /path/to/wallpaper_1.jpg
        /path/to/wallpaper_2.jpg
        /path/to/wallpaper_3.jpg
~~~~

1. `Profile:` - This is the main name of the profile, and what it is referred to as when applying it

2. `Title:` - Optional - The display name of the profile. This is primarily used for jgmenu.

3. `Category:` - Optional - The category the profile is in. This is primarily used for jgmenu.

4. `Hidden:` - Optional - Whether or not the profile should be shown in the main jgmenu config.

5. `Paths:` - This is followed by a list of paths.

## Examples

1. `wallman -a ArchLinux`

    * Apply the profile "ArchLinux", but do not set it in the main config file. This will not persist on a reboot, unlike "-s"

2. `wallman -s ArchLinux`

    * Apply the profile "ArchLinux" and set it as current in the main config file

3. `wallman -m 2 -w /path/to/wallpaper`

    * Set the wallpaper on the second wallpaper to "/path/to/wallpaper"

    * This changes the current profile in the main config file, but not modify the original profile

4. `wallman -S NewArchWallpaper -t "New Arch Set" -C Linux -H true`
    
    * Save the current profile (which has been modified) to a new profile called NewArchWallpaper

    * Set the display title of the new profile to "New Arch Set"

    * Set the category to Linux

    * Set the new profile to hidden

5. `wallman -f random -s FluffyCats` - Apply the profile "FluffyCats" from the file "random.profile" without permanently switching to that profile list.

## Config Files

Wallman uses two config files. The first of these is `~/.config/wallman/config` which stores the currently applied profile and the location of the current profiles file.

The first line of the main config file contains something similar to `Active Profile: LinuxPropoganda`. This tells Wallman to pull the full list of profiles from a file `LinuxPropoganda.profile`, located in the `~/.config/wallman` folder.

Example configs are located below

- Main Config File `~/.config/wallman/config`
~~~~
Active Profile: LinuxPropoganda
Profile: ArchLinux
    Title: Arch Wallpapers
    Category: Linux
    Paths:
        /path/to/wallpaper_1.jpg
        /path/to/wallpaper_2.jpg
        /path/to/wallpaper_3.jpg
~~~~

- Profile File `~/.config/wallman/LinuxPropoganda.profile`

~~~~
Profile: ArchLinux
    Title: Arch Wallpapers
    Category: Linux
    Paths:
        /path/to/wallpaper_1.jpg
        /path/to/wallpaper_2.jpg
        /path/to/wallpaper_3.jpg

Profile: NSFWArchLinux
    Title: Arch NSFW
    Category: Linux
    Hidden: True
    Paths:
        /path/to/wallpaper_4.jpg
        /path/to/wallpaper_5.jpg
        /path/to/wallpaper_6.jpg
~~~~

## JGMenu

JGMenu integration is entirely optional, but can provide a quick and easy way to manage wallpapers from a gui.

Wallman generates two JGMenu config files that are stored in the wallman config directory.

- ~/.config/wallman/jgmenu - Configs for non-hidden wallpapers

- ~/.config/wallman/jgmenu_hidden - Configs for hidden wallpapers

Wallman also generates scripts for showing the menus. These scripts can be modified and will not be overridden by the program after they are originally generated.

- ~/.config/wallman/jgmenu_run - Prints menu for non-hidden wallpapers

- ~/.config/wallman/jgmenu_run_hidden - Prints menu for hidden wallpapers

## Polybar Integration

These scripts can be easily integrated into any gui that supports running commands. Polybar integration is shown as an example. In this example, left clicking shows the non-hidden wallpapers, while right clicking shows a menu of hidden wallpapers.

~~~~
[module/menu-wallman]
type = custom/text
content = "Wallpapers"
click-left = ~/.config/wallman/jgmenu_run
click-right = ~/.config/wallman/jgmenu_run_hidden
~~~~
