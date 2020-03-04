
#ifndef rainbowvalue
#define rainbowvalue
#define MAX_PW_LEN 64
    struct s_rainbowvalue256 {
    char pw[MAX_PW_LEN];
    char hash[32]; // 32 * 8bit = 256 bit
};

#endif