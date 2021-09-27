#pragma once
//#include <windef.h>

typedef struct _AIK_READ {
    wchar_t name[32];
    unsigned char level;
    bool gravity;
    float speed;
    float attack_speed;

    /* Target */
    float x;
    float y;
    float z;
} AIK_RERAD, * PAIK_READ;

typedef struct _AIK_WRITE {
    bool gravity;
    float speed;
    float attack_speed;

    /* Target */
    float x;
    float y;
    float z;
} AIK_WRITE, * PAIK_WRITE;
