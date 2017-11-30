# Wallman

A simple tool for managing wallpaper profiles with feh. Designed for use in i3.

Usage: 

wallman [option(s)] [profile]

Options:

-l --list        List all profiles

-c --current     Apply currently set profile, generally used on login

-s --set         Takes either two or three arguments:

		 wallman -s monitor_num wallpaper_path - this temporarily changes a wallpaper

		 wallman -s profile_name monitor_num wallpaper_path - this permanently sets a wallpaper within a profile

-d --display	 Set a display name for a profile. Usage:

		 wallman -d profile_name display_name

-C --category	 Set the category of a profile. Usage:
		 
		 wallman -C category_name profile_name


Profiles should be set up in ~/.config/wallman

Example config:
---------------

	Profile_Name:

		monitors: 2

		category: none
	
		/path/to/wallpaper1.jpg

		/path/to/wallpaper2.png


	Profile_Name_2: DisplayName

		monitors: 2

		category: CategoryName

		/path/to/wallpaper1.jpg

		/path/to/wallpaper2.png

	Hidden_Profile: ignore
		
		monitors: 1

		category: none

		/path/to/wallpaper1.jpg
