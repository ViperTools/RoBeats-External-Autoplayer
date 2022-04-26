# Using the Autoplayer
First make sure to download the autoplayer in the releases tab.

## Manual Keys
I **highly recommend** that you enable the `Manual Keys` option in the UI. This makes the autoplayer use the keys specified in the UI and works on every stage. If Manual Keys is disabled, it will attempt to find the keys automatically and will only work in the default stage. Setting the keys is fairly straightforward. Type in your keybinds in the order they appear in game. If you have a special keybind such as a numpad key, grab the name from here https://developer.roblox.com/en-us/api-reference/enum/KeyCode and enter it instead. Otherwise just enter the one character key name.


## Attaching
Although I called it *attaching*, the auto player does not actually inject anything into Roblox and is fully external. Attaching is very easy. Click the attach button at the top of the UI after loading into RoBeats and wait for it to attach. After it attaches it will change the text to `detach`. If it says anything else it's probably an error. Use the reference below to determine a possible solution:
<br>`NO RBLX` Means that Roblox is not open or the autoplayer could not find it.
<br>`NO WS` Means it could not find the workspace and is most likely not updated.
<br>`NO DM` Means it could not find the datamodel and is most likely not updated.
<br>`ERROR: n` Specifies an unknown error and should be sent in the Discord server for further inspection.

## Notes

* RoBeats External **does** auto update, but Roblox occasionally changes things that will break the auto player. If it stops working or an error code comes up suggesting it is not updated, let me know and I will look into it.
* If the auto player breaks for some reason, try opening the esc menu and closing it. This "resets" the autoplayer. If that doesn't work, try detaching and re-attaching. If that still doesn't work, let me know and I will try to debug it.
* You must be focused on the Roblox application and the esc menu must not be open. If you click off of Roblox or open the esc menu the autoplayer will stop playing. This is because it works by simulating keyboard input and you don't want it spamming keys when you aren't playing the game.
* **As specified in the license, all distribution of any portion of this work must be released along with its full source and the same license attached. All modifications of the source must be documented.**
