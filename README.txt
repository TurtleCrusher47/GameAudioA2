Name: Wong Kwok Yeu Ray
Admin: 222569R

1. Music and Sounds(in Music and Sound folders respectively)
(Music)
- Music_Level.ogg

(Sounds)
- Sound_Damaged.ogg
- Sound_Dash.ogg
- Sound_DashPad.ogg
- Sound_Jump.ogg
- Sound_Landing.ogg
- Sound_Money.ogg
- Sound_Portal.ogg
- Sound_Potion.ogg
- Sound_Step.ogg

2.
- Music_Level is played as background music throughout the game

- Sound_Damaged.ogg is played when the player takes damage
- Sound_Dash.ogg is played when the player presses shift to dash
- Sound_DashPad.ogg is played when the player goes through a dash pad
- Sound_Jump.ogg is played when the player presses space to jump
- Sound_Landing.ogg is played when the player lands on the ground
- Sound_Money.ogg is played when the player collects a coin
- Sound_Portal.ogg is played when the player enters a portal to go to the next level
- Sound_Potion.ogg is played when the player picks up a jump boost potion
- Sound_Step.ogg is played when the player is walking

3.
For the Music Player and SoundController, I made a getter and setter for the master volume as I
found the increase and decrease volume functions inconvenient when adjusting volume.

I also made a getter and setter that changed the volume for individual sounds as I wanted to
be able to change the player sfx and world sounds individually as compared to just changing 
everything at once using the master volume.

4.
I used the current PauseState and modified it using imgui to allow the modification of various sounds 
using sliders. These include, Music Master Volume, SFX Master Volume, Player SFX Volume and World SFX Volume.

I also normalized all the sounds and music beforehand to ensure that they were at their desired volume. 

I had varied levels of normalization between the different sound effects. One example of this is the player's footsteps 
being normalized to a lower db or the dash sound being louder than the jump sound. 

I also tested out changing the pitch on the dash as I found that it was too sharp at the beginning.

Lastly, I cut some of the audio as there were periods of silence and looped the background music.