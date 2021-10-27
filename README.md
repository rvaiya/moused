# What?

Mouse configuration daemon. This is an initial code dump of a personal tool. YMMV.
I will revisit and properly publish this later/maybe :P.

# Install

	make && sudo make install && sudo systemctl --now enable moused

# Config

All mice are configured in /etc/moused.conf. The file has the following format

	[<mouse name>]
	
	
	<LHS> = <Action>

`sudo moused -m`
## Example	
	[Logitech M570]

	scroll_swap_axes=1
	scrollmode_sensitivity=1.5
	scroll_inhibit_x = 1
	btn9 = btn1t
	btn8 = sensitivity(.25)
	scrolldown = scrollon
	scrollup = scrolloff

# Options

## LHS

 - btn[0-9]
 - scroll(up|down|left|right)
 - btn[0-9]t - toggle variant of buttons 0-9

## Actions

 - scrollon
 - scrolloff
 - scrollt 
 - sensitivity(<num>)
 - btn[0-9]

E.G

	scrollup=scrolloff
	scrolldown=scrollon

Allows you to activate scroll mode by moving the scroll wheel one or more
notches down and deactivate it by scrolling up. This is particularly useful for
trackballs with a limited number of buttons.
