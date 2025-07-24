# Majora's Mask: Recompiled Modern Controller Overhaul Mod

__IMPORTANT: All the textures included in this pack are designed for a control scheme with the Action button on the right and the Attack button on the left. If you use a different control scheme, you'll have to use a pre-configured texture pack or modify the texture pack manually. [Read more here](https://github.com/t0mtee/MMModernControllerOverhaul/blob/main/TEXTURES.md).__

A Zelda64Recomp mod, built with cross-mod compatibility in mind, that aims to overhaul the HUD and UI surrounding the game's control scheme in order to align it as closely as possible with modern controllers.

It does this in many ways - the most obvious being repositioning HUD elements. The positions of the buttons are configurable, allowing you to match with your personal control scheme. Empty button glyphs remain consistent no matter the button placements, and the Shoulder button's position can be moved depending on your preference:

![Position movement demonstration](readme/positions.gif)

Modern Controller Overhaul also aims to make the buttons more consistent. Why is it that only the C-buttons have empty button glyphs? Why not the Action and Attack buttons too? With MCO, glyphs can be added to the Action and Attack buttons, or removed altogether:

![Glyph options demonstration](readme/glyphs.gif)

Text relating to the control scheme can been changed too. Instead of referencing some elusive Z Targeting, text can reference ZL, LT, or L2 Targeting, all dependent on your personal configuration.

This has been combined with textures, with there being different texture packs available to reflect your controller (Xbox, Playstation, Steam Deck, and L2 + R2 schemes are included in this repo). [Read more here](https://github.com/t0mtee/MMModernControllerOverhaul/blob/main/TEXTURES.md). Here's an example of text and texture replacements in action together:

![Text and textures demonstration](readme/text(ures).gif)

Requires [Interface Helper](), and optionally [EZ Text Replacer API](https://thunderstore.io/c/zelda-64-recompiled/p/LT_Schmiddy/EZ_Text_Replacer_API/) for text changes.

Credits:
- Wiseguy helped me out with moving the C-Up button - couldn't have done it without you and your... wisdom.
- Nerrel and GhostlyDark provided permission for me to use their textures as a base when modifying them.
- pizza1398 created the pause menu switcher button textures.
- LT_Schmiddy helped me set up a symbol file so that ammo count positions can still be accessed in more recent decomp commits.