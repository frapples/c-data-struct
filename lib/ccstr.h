#ifndef __CCSTR_H__RAND494660157516882943
#define __CCSTR_H__RAND494660157516882943

/* ccstr, Compatibility c string */

typedef const char* ccstr_t;

ccstr_t ccstr_create(const char* c_str);
void ccstr_destory(ccstr_t* str);

ccstr_t ccstr_concat(ccstr_t str1, ...);
ccstr_t ccstr_len(ccstr_t str);



#endif /* __CCSTR_H__RAND494660157516882943 */
