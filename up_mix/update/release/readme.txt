3.03/3.10 OE mix for UP.

This is an OE update to let UP users have the vsh of 3.03 (which is more customizable)
and the kernel of 3.10 to launch psp and psx games.

I have also done this thing to fix some annoying things for up users (pressing L at boot, and at
recoveryng from sleep).

Instructions.

- On the PSP original nand, install 3.10 OE-A (or A', doesn't mattter because the patches of A' will be applied)
- On the UP nand, install 3.03 OE-C
- Copy 303_10 mix to GAME150, and run it from the UP nand.
- Done.

Now you will have the following benefits:

- 3.03 vsh, which you can customize.
- When you execute a psp or psx game, or a homebrew, OE will switch the nand to the psp one (containing 3.10 OE) automatically
so you will be able to play psx and psp games in the higher firmware, and since it will run on the psp nand, you
won't have problems with psx games or with 333 Mhz.
- When you recover from sleep mode, OE will also be back to the nand you were, so you don't need to press L
 (in fact, you must not press it :p)
- You can access both flashes via usb in the recovery 