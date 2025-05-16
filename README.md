# Majora's Mask: Recompiled Modern Controller Overhaul Mod

__IMPORTANT: If you have issues with the Attack button glyph showing, it is due to a recomp bug itself where mod priority doesn't affect hook execution order. I know that Arrow Tweaks has this issue when combined with this mod. To fix it, you need to open the Arrow Tweaks (or whichever other mod) nrm file, rename a file in the archive, then rename it back to the original name. This'll be fixed in Z64Recomp 1.2.1.__

A Zelda64Recomp mod, built with cross-mod compatibility in mind, that aims to overhaul the HUD and UI surrounding the game's control scheme in order to align it as closely as possible with modern controllers.

It does this in many ways - the most obvious being repositioning HUD elements. The positions of the buttons are configurable, allowing you to match with your personal control scheme. Empty button glyphs remain consistent no matter the button placements, and the Shoulder button's position can be moved depending on your preference:

![Position movement demonstration](readme/positions.gif)

Modern Controller Overhaul also aims to make the buttons more consistent. Why is it that only the C-buttons have empty button glyphs? Why not the Action and Attack buttons too? With MCO, glyphs can be added to the Action and Attack buttons, or removed altogether:

![Glyph options demonstration](readme/glyphs.gif)

Text relating to the control scheme has been changed too. Instead of referencing some elusive Z Targeting, text can reference ZL, RT, or R2 Targeting, all dependent on your personal configuration. This has been combined with textures, with there being different texture packs available to reflect your controller (Xbox, Playstation, Steam Deck, and 8BitDo schemes are included in this download). [Read more here](https://github.com/t0mtee/MMModernControllerOverhaul/blob/main/TEXTURES.md). Here's an example of text and texture replacements in action together:

![Text and textures demonstration](readme/text(ures).gif)

Bugs and inconsistencies with the original game's UI have been fixed as well:
- Deku Link's empty C-buttons being disabled while other forms aren't has been fixed.
- The green ammo bug before obtaining the magic meter has been fixed.

Requires [EZ Text Replacer API](https://thunderstore.io/c/zelda-64-recompiled/p/LT_Schmiddy/EZ_Text_Replacer_API/).

Credits:
- Wiseguy helped me out with moving the C-Up button - couldn't have done it without you and your... wisdom.
- Nerrel and GhostlyDark provided permission for me to use their textures as a base when modifying them.
- pizza1398 created the pause menu switcher button textures.
- LT_Schmiddy helped me set up a symbol file so that ammo count positions can still be accessed in more recent decomp commits.