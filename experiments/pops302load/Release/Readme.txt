This plugin is to load 3.02 psx emulator in 3.03 OE.

Although a lot of games work better in 3.03 emulator than in 3.02, in some cases
it is the contrary, so you may want to load 3.02 psx emulator in certain cases.

Instructions:

- Copy seplugins to the root of your memory stick.
- Using some of the latest psardumpers, extract the 3.02 firmware with the option of the button X.
- Once done, copy the following files from /F0/kd to /seplugins/pops302: meaudio.prx, mesg_led.prx,
pops.prx and popsman.prx.

- Enter in recovery menu, and enable pops302ld.prx plugin if you want to load 3.02 emulator.
- Run the game. It will run with 3.02 psx emulator.
- Whenever you want to use again the 3.03 emulator, just disable the plugin from recovery menu.

Known problems:

* During tests i tried castlevania sotn compressed. During the game i noticed anormal high activity
in the memory stick (note: the fact that the prx's are in memory stick is not related to this issue,
since they are quickly loaded to ram). 
It didn't happen with the game not compressed (and it doesn't happen in 3.03 compressed). 
I don't know if this issue affects other games.
In such a case, it may be a problem of the mixing of modules, or maybe just a 3.02 problem.
If you experience that problem, you may want to use uncompressed games if it slowdowns the game.
