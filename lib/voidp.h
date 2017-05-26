#ifndef __DATA__H__
#define __DATA__H__

#include<stdbool.h>
/* 支持使用void * 储存Long和bool类型的数据。 */
static long   voidp2long   (void*  voidp);
static bool   voidp2bool   (void*  voidp);
static void*  voidp2pointer(void*  voidp);
static void*  long2voidp   (long);
static void*  bool2voidp   (bool);


static inline long voidp2long(void*  voidp)
{
    return (long)voidp;
}

static inline bool voidp2bool(void*  voidp)
{
    return (bool)voidp;
}

static inline void* voidp2pointer(void*  voidp)
{
    return voidp;
}



static inline void*  long2voidp(long long_data)
{
    return (void* )long_data;
}

static inline void*  bool2voidp(bool bool_data)
{
    return (void* )bool_data;
}

#endif
