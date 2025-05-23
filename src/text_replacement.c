#include "eztr_api.h"

RECOMP_IMPORT("*", u32 recomp_get_config_u32(const char* key));

void Mod_GetButton(char* array) {
    switch (recomp_get_config_u32("text_replacement")) {
        case 0:
            array[0] = 'Z';
            array[1] = '\0';
            break;
        case 1:
            array[0] = 'Z';
            array[1] = 'L';
            break;
        case 2:
            array[0] = 'L';
            array[1] = 'T';
            break;
        case 3:
            array[0] = 'L';
            array[1] = '2';
            break;
    }
    array[2] = '\0';
}

char button[3];

EZTR_MSG_CALLBACK(c0227) {
    Mod_GetButton(button);
    EZTR_MsgSContent_Sprintf(buf->data.content, "|05|17There you go! See? You can do it|11if you try!|18|11|13|12When I fly over to people or|11objects, use |B5 to look at them|11so you can talk to them.|11|12Use |01%s Targeting|05 to talk to people|11even if they're far away.|11 |11|12Or, if there's no one to target,|11you can use |B5 to look |01straight|11ahead|05. |11|12But enough about that for now...|11Come over to this |01tree |05and |01check|11|05it out!|00|BF", button);
}

EZTR_MSG_CALLBACK(c1788) {
    Mod_GetButton(button);
    EZTR_MsgSContent_Sprintf(buf->data.content, "|04Hero's Shield|11|00Use with |B4. Press |B4 while |01%s|11Targeting|00 to defend while moving.|BF", button);
}

EZTR_MSG_CALLBACK(c1912) {
    Mod_GetButton(button);
    EZTR_MsgSContent_Sprintf(buf->data.content, "|05What?! Don't you know about|11the |00Keese|05? Use %s Targeting when|11it gets close. Even if it flies off,|11I'll follow it for you.|00|BF", button);
}

EZTR_MSG_CALLBACK(c191D) {
    Mod_GetButton(button);
    EZTR_MsgSContent_Sprintf(buf->data.content, "|05Use %s Targeting methods while|11defending...The \"Unthinkable?\"|11What could that possibly be|11in a place as dark as this?|00|BF", button);
}

EZTR_MSG_CALLBACK(c1944) {
    Mod_GetButton(button);
    EZTR_MsgSContent_Sprintf(buf->data.content, "|05What?! You don't know about the|11|00Poe|05? If you try to use %s|11Targeting, it vanishes.|00|BF", button);
}

EZTR_MSG_CALLBACK(c1946) {
    Mod_GetButton(button);
    EZTR_MsgSContent_Sprintf(buf->data.content, "|05Don't you know about the |00Blue|11Tektite|05? Use %s Targeting to follow|11it as it bounces!|00|BF", button);
}

EZTR_MSG_CALLBACK(c1954) {
    Mod_GetButton(button);
    EZTR_MsgSContent_Sprintf(buf->data.content, "|05The |00pirate|05 found you!|11Use %s targeting and look for your|11chance. You're caught if you don't|11win.|00|BF", button);
}

EZTR_MSG_CALLBACK(c271A) {
    Mod_GetButton(button);
    EZTR_MsgSContent_Sprintf(buf->data.content, "Now then, before you draw your|11sword, I shall teach you the|11basics of |01unarmed|00 combat.|11|12I'll start with |01sideways jumps|00.|11After pressing |B5 to lock onto|11that log with %s Targeting, press|11|B0 while pushing |BB left or right.|10Try it yourself.|E0|BF", button);
}

EZTR_MSG_CALLBACK(c271C) {
    Mod_GetButton(button);
    EZTR_MsgSContent_Sprintf(buf->data.content, "Next is the |01roll attack|00.|11After pressing |B5 to %s Target,|11press |B0 while pushing |BB|11forward to roll.|10Begin!|E0|BF", button);
}

EZTR_MSG_CALLBACK(c271D) {
    Mod_GetButton(button);
    EZTR_MsgSContent_Sprintf(buf->data.content, "After %s Targeting with |B5, press|11|B0 while pressing |BB up to move|11and perform a |01roll attack|00.|E0|BF", button);
}

EZTR_MSG_CALLBACK(c271E) {
    Mod_GetButton(button);
    EZTR_MsgSContent_Sprintf(buf->data.content, "Next is the |01backflip|00.|11Lock on and %s Target the log with|11|B5, then press |B0 while pushing|11|BB down to jump.|10Begin!|E0|BF", button);
}

EZTR_MSG_CALLBACK(c2722) {
    Mod_GetButton(button);
    EZTR_MsgSContent_Sprintf(buf->data.content, "Next is the |01vertical cut|00.|11Press |B1 after %s Targeting with|11|B5.|11Now then...Begin!|E0|BF", button);
}

EZTR_MSG_CALLBACK(c2723) {
    Mod_GetButton(button);
    EZTR_MsgSContent_Sprintf(buf->data.content, "Press |B1 after %s Targeting with|11|B5 to execute a |01vertical cut|00.|E0|BF", button);
}

EZTR_MSG_CALLBACK(c2724) {
    Mod_GetButton(button);
    EZTR_MsgSContent_Sprintf(buf->data.content, "Next is the |01thrust|00.|11After pressing |B5 to %s Target,|11push |BB forward and press |B1.|E0|BF", button);
}

EZTR_MSG_CALLBACK(c2725) {
    Mod_GetButton(button);
    EZTR_MsgSContent_Sprintf(buf->data.content, "After %s Targeting with |B5, push|11|BB forward and press |B1 to|11perform a |01thrust|00.|E0|BF", button);
}

EZTR_MSG_CALLBACK(c2726) {
    Mod_GetButton(button);
    EZTR_MsgSContent_Sprintf(buf->data.content, "And finally, the |01jump attack|00.|11Press |B0 after drawing your|11sword and %s Targeting with |B5.|11Begin.|E0|BF", button);
}

EZTR_MSG_CALLBACK(c2727) {
    Mod_GetButton(button);
    EZTR_MsgSContent_Sprintf(buf->data.content, "Draw your sword, press |B5 to|11%s Target, then press |B0 to|11perform the |01jump attack|00.|E0|BF", button);
}

EZTR_MSG_CALLBACK(c272B) {
    Mod_GetButton(button);
    EZTR_MsgSContent_Sprintf(buf->data.content, "Do not %s Target!|E0|BF|00", button);
}

EZTR_ON_INIT void Mod_MessageReplacement() {
    EZTR_Basic_ReplaceText(
        0x0227,
        EZTR_STANDARD_TEXT_BOX_I,
        1,
        EZTR_ICON_NO_ICON,
        EZTR_NO_VALUE,
        EZTR_NO_VALUE,
        EZTR_NO_VALUE,
        true,
        "|05|17There you go! See? You can do it|11if you try!|18|11|13|12When I fly over to people or|11objects, use |B5 to look at them|11so you can talk to them.|11|12Use |01ZR Targeting|05 to talk to people|11even if they're far away.|11 |11|12Or, if there's no one to target,|11you can use |B5 to look |01straight|11ahead|05. |11|12But enough about that for now...|11Come over to this |01tree |05and |01check|11|05it out!|00|BF",
        c0227
    );

    EZTR_Basic_ReplaceText(
        0x1788,
        EZTR_STANDARD_TEXT_BOX_II,
        1,
        EZTR_ICON_HEROS_SHIELD,
        EZTR_NO_VALUE,
        EZTR_NO_VALUE,
        EZTR_NO_VALUE,
        true,
        "|04Hero's Shield|11|00Use with |B4. Press |B4 while |01ZL|11Targeting|00 to defend while moving.|BF",
        c1788
    );
    
    EZTR_Basic_ReplaceText(
        0x1912,
        EZTR_BLUE_TEXT_BOX,
        0,
        EZTR_ICON_NO_ICON,
        EZTR_NO_VALUE,
        EZTR_NO_VALUE,
        EZTR_NO_VALUE,
        true,
        "|05What?! Don't you know about|11the |00Keese|05? Use ZL Targeting when|11it gets close. Even if it flies off,|11I'll follow it for you.|00|BF",
        c1912
    );
    
    EZTR_Basic_ReplaceText(
        0x191D,
        EZTR_BLUE_TEXT_BOX,
        0,
        EZTR_ICON_NO_ICON,
        EZTR_NO_VALUE,
        EZTR_NO_VALUE,
        EZTR_NO_VALUE,
        true,
        "|05Use ZL Targeting methods while|11defending...The \"Unthinkable?\"|11What could that possibly be|11in a place as dark as this?|00|BF",
        c191D
    );
    
    EZTR_Basic_ReplaceText(
        0x1944,
        EZTR_BLUE_TEXT_BOX,
        0,
        EZTR_ICON_NO_ICON,
        EZTR_NO_VALUE,
        EZTR_NO_VALUE,
        EZTR_NO_VALUE,
        true,
        "|05What?! You don't know about the|11|00Poe|05? If you try to use ZL|11Targeting, it vanishes.|00|BF",
        c1944
    );
    
    EZTR_Basic_ReplaceText(
        0x1946,
        EZTR_BLUE_TEXT_BOX,
        0,
        EZTR_ICON_NO_ICON,
        EZTR_NO_VALUE,
        EZTR_NO_VALUE,
        EZTR_NO_VALUE,
        true,
        "|05Don't you know about the |00Blue|11Tektite|05? Use ZL Targeting to follow|11it as it bounces!|00|BF",
        c1946
    );

    EZTR_Basic_ReplaceText(
        0x1954,
        EZTR_BLUE_TEXT_BOX,
        0,
        EZTR_ICON_NO_ICON,
        EZTR_NO_VALUE,
        EZTR_NO_VALUE,
        EZTR_NO_VALUE,
        true,
        "|05The |00pirate|05 found you!|11Use ZL targeting and look for your|11chance. You're caught if you don't|11win.|00|BF",
        c1954
    );
    
    EZTR_Basic_ReplaceText(
        0x271A,
        EZTR_STANDARD_TEXT_BOX_I,
        0,
        EZTR_ICON_NO_ICON,
        EZTR_NO_VALUE,
        EZTR_NO_VALUE,
        EZTR_NO_VALUE,
        true,
        "Now then, before you draw your|11sword, I shall teach you the|11basics of |01unarmed|00 combat.|11|12I'll start with |01sideways jumps|00.|11After pressing |B5 to lock onto|11that log with ZL Targeting, press|11|B0 while pushing |BB left or right.|10Try it yourself.|E0|BF",
        c271A
    );

    EZTR_Basic_ReplaceText(
        0x271C,
        EZTR_STANDARD_TEXT_BOX_I,
        0,
        EZTR_ICON_NO_ICON,
        EZTR_NO_VALUE,
        EZTR_NO_VALUE,
        EZTR_NO_VALUE,
        true,
        "Next is the |01roll attack|00.|11After pressing |B5 to ZL Target,|11press |B0 while pushing |BB|11forward to roll.|10Begin!|E0|BF",
        c271C
    );

    EZTR_Basic_ReplaceText(
        0x271D,
        EZTR_STANDARD_TEXT_BOX_II,
        0,
        EZTR_ICON_NO_ICON,
        EZTR_NO_VALUE,
        EZTR_NO_VALUE,
        EZTR_NO_VALUE,
        true,
        "After ZL Targeting with |B5, press|11|B0 while pressing |BB up to move|11and perform a |01roll attack|00.|E0|BF",
        c271D
    );

    EZTR_Basic_ReplaceText(
        0x271E,
        EZTR_STANDARD_TEXT_BOX_I,
        0,
        EZTR_ICON_NO_ICON,
        EZTR_NO_VALUE,
        EZTR_NO_VALUE,
        EZTR_NO_VALUE,
        true,
        "Next is the |01backflip|00.|11Lock on and ZL Target the log with|11|B5, then press |B0 while pushing|11|BB down to jump.|10Begin!|E0|BF",
        c271E
    );

    EZTR_Basic_ReplaceText(
        0x2722,
        EZTR_STANDARD_TEXT_BOX_I,
        0,
        EZTR_ICON_NO_ICON,
        EZTR_NO_VALUE,
        EZTR_NO_VALUE,
        EZTR_NO_VALUE,
        true,
        "Next is the |01vertical cut|00.|11Press |B1 after ZL Targeting with|11|B5.|11Now then...Begin!|E0|BF",
        c2722
    );

    EZTR_Basic_ReplaceText(
        0x2723,
        EZTR_STANDARD_TEXT_BOX_II,
        0,
        EZTR_ICON_NO_ICON,
        EZTR_NO_VALUE,
        EZTR_NO_VALUE,
        EZTR_NO_VALUE,
        true,
        "Press |B1 after ZL Targeting with|11|B5 to execute a |01vertical cut|00.|E0|BF",
        c2723
    );

    EZTR_Basic_ReplaceText(
        0x2724,
        EZTR_STANDARD_TEXT_BOX_I,
        0,
        EZTR_ICON_NO_ICON,
        EZTR_NO_VALUE,
        EZTR_NO_VALUE,
        EZTR_NO_VALUE,
        true,
        "Next is the |01thrust|00.|11After pressing |B5 to ZL Target,|11push |BB forward and press |B1.|E0|BF",
        c2724
    );

    EZTR_Basic_ReplaceText(
        0x2725,
        EZTR_STANDARD_TEXT_BOX_II,
        0,
        EZTR_ICON_NO_ICON,
        EZTR_NO_VALUE,
        EZTR_NO_VALUE,
        EZTR_NO_VALUE,
        true,
        "After ZL Targeting with |B5, push|11|BB forward and press |B1 to|11perform a |01thrust|00.|E0|BF",
        c2725
    );

    EZTR_Basic_ReplaceText(
        0x2726,
        EZTR_STANDARD_TEXT_BOX_II,
        0,
        EZTR_ICON_NO_ICON,
        EZTR_NO_VALUE,
        EZTR_NO_VALUE,
        EZTR_NO_VALUE,
        true,
        "And finally, the |01jump attack|00.|11Press |B0 after drawing your|11sword and ZL Targeting with |B5.|11Begin.|E0|BF",
        c2726
    );

    EZTR_Basic_ReplaceText(
        0x2727,
        EZTR_STANDARD_TEXT_BOX_II,
        0,
        EZTR_ICON_NO_ICON,
        EZTR_NO_VALUE,
        EZTR_NO_VALUE,
        EZTR_NO_VALUE,
        true,
        "Draw your sword, press |B5 to|11ZL Target, then press |B0 to|11perform the |01jump attack|00.|E0|BF",
        c2727
    );

    EZTR_Basic_ReplaceText(
        0x272B,
        EZTR_STANDARD_TEXT_BOX_II,
        0,
        EZTR_ICON_NO_ICON,
        EZTR_NO_VALUE,
        EZTR_NO_VALUE,
        EZTR_NO_VALUE,
        true,
        "Do not ZL Target!|E0|BF|00",
        c272B
    );
}