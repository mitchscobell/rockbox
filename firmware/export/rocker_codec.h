#ifndef __ROCKER_CODEC__
#define __ROCKER_CODEC__

#define AUDIOHW_CAPS 0
AUDIOHW_SETTING(VOLUME, "dB", 0, 1, -127,  0, -30)
#endif

#define AUDIOHW_MUTE_ON_PAUSE

void audiohw_mute(int mute);
