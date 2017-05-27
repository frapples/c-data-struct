#ifndef __DATA__H__
#define __DATA__H__

#include<stdbool.h>
/* 支持使用void * 储存Long和bool类型的数据。 */
static long   voidp2long   (void*  voidp);
static bool   voidp2bool   (void*  voidp);
static void*  long2voidp   (long);
static void*  bool2voidp   (bool);

union __cast {
    void* voidp;
    long vlong;
    bool vbool;
};

static inline long voidp2long(void*  voidp)
{
    union __cast cast;
    cast.voidp = voidp;
    return cast.vlong;
}

static inline bool voidp2bool(void*  voidp)
{
    union __cast cast;
    cast.voidp = voidp;
    return cast.vbool;
}


static inline void*  long2voidp(long long_data)
{
    union __cast cast;
    cast.vlong = long_data;
    return cast.voidp;
}

static inline void*  bool2voidp(bool bool_data)
{
    union __cast cast;
    cast.vbool = bool_data;
    return cast.voidp;
}

#endif
