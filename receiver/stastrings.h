// NAME:  stastrings.h
// Desc:  Implementation of Read only Static Strings / String View
// Auth:  KillingWhales (Nur Hary)
// TODO:  Implementation Function for anystrings

#ifndef ___LYSTASTRINGS_VERSION___
#define ___LYSTASTRINGS_VERSION___ "0.0.1"

#include <stddef.h>
#include <stdlib.h>

// EXPAND FROM lyutils.h
#ifndef ___LYUTILS_VERSION___
#if (__STDC_VERSION__ >= 202311l)
#define LYNULL nullptr
#else
#define LYNULL NULL
#endif
// EXPAND FROM lyutils.h

#endif

typedef struct {
  size_t count;
  const char* arr;
} StaStrings;

void
StaStringsNew(StaStrings* st, const char* s);

void
StaStringsTrim(StaStrings* st);

void
StaStringsSlices(StaStrings* st, int l, int r);

StaStrings
StaStringsSplit(StaStrings* st, char sep);

char**
StaStringsSplitMulti(StaStrings st, char sep);

#ifdef ___LYSTASTRINGS_IMP___

#include <ctype.h>
#include <string.h>

// EXPAND FROM lycontainer/dynlist.h
#ifndef ___LYDYNLIST_VERSION___
#define ___LYDYNLIST_VERSION___ "0.0.1"

#define LYM_DYNLIST(type) type*
#define ___LYMCDYNLIST_BASECAPACITY___ 2

typedef struct {
  size_t capacity;
  size_t count;
} ___LyDynlistHeader;

#define lymDynlistPush(arr, val)                                               \
  {                                                                            \
    if (arr == LYNULL) {                                                       \
      ___LyDynlistHeader* header =                                             \
          malloc(sizeof(*arr) * ___LYMCDYNLIST_BASECAPACITY___ +               \
                 sizeof(___LyDynlistHeader));                                  \
      header->capacity = ___LYMCDYNLIST_BASECAPACITY___;                       \
      header->count = 0;                                                       \
      arr = (void*)(header + 1);                                               \
    };                                                                         \
    ___LyDynlistHeader* header = (___LyDynlistHeader*)(arr) - 1;               \
    if (header->count >= header->capacity) {                                   \
      header->capacity *= 1.5;                                                 \
      header = realloc(header, sizeof(*arr) * header->capacity +               \
                                   sizeof(___LyDynlistHeader));                \
      arr = (void*)(header + 1);                                               \
    }                                                                          \
    (arr)[header->count++] = (val);                                            \
  }

#define lymDynlistLen(arr) ((___LyDynlistHeader*)(arr) - 1)->count

#define lymDynlistFree(arr)                                                    \
  free((___LyDynlistHeader*)(arr) - 1);                                        \
  arr = LYNULL
#endif
// EXPAND FROM lycontainer/dynlist.h

void
StaStringsNew(StaStrings* st, const char* s) {
  st->count = strlen(s);
  st->arr = s;
}

void
StaStringsTrim(StaStrings* st) {
  while (st->count > 0 && isspace(st->arr[0])) { // THANK YOU TSODING
    st->arr++;
    st->count -= 1;
  }
  while (st->count > 0 &&
         isspace(st->arr[st->count - 1])) { // THANK YOU TSODING
    st->count -= 1;
  }
}

void
StaStringsSlices(StaStrings* st, int l, int r) {
  st->count -= r + l;
  st->arr += l;
}

StaStrings
StaStringsSplit(StaStrings* st, char sep) { // THANKS TSODING, AGAIN
  size_t i = 0;
  while (i < st->count && st->arr[i] != sep) {
    i += 1;
  }
  if (i < st->count) {
    StaStrings r = {
        .count = i,
        .arr = st->arr,
    }; // ada masalah: kalau yang utama di free maka ini juga akan free
    StaStringsSlices(st, i + 1, 0);
    return r;
  }
  StaStrings r = *st;
  StaStringsSlices(st, st->count, 0);
  return r;
}

[[maybe_unused]]
char**
StaStringsSplitMulti(StaStrings st, char sep) {
  int* bufid = LYNULL;
  int i = 0;
  while (i < st.count) {
    i += 1;
    if (st.arr[i] == sep) {
      lymDynlistPush(bufid, i);
    }
  }
  lymDynlistPush(bufid, st.count);
  size_t buflen = lymDynlistLen(bufid);
  int templast = 0;

  const char* curref = st.arr;
  char** curhead = (char**)malloc(sizeof(char*) * buflen);
  if (curhead == LYNULL) {
    return LYNULL;
  }
  for (int i = 0; i < buflen; i++) {
    st.arr = curref;
    if (isspace(st.arr[templast])) {
      templast += 1;
    }
    int bufsize = bufid[i] - templast;
    char* curptr = (char*)malloc(bufsize + 1);
    curptr[bufsize] = 0;
    StaStringsSlices(&st, templast, buflen - bufid[i]);
    StaStringsTrim(&st);
    memcpy(curptr, st.arr, bufsize);

    curhead[i] = curptr;
    templast = bufid[i] + 1;
  }
  lymDynlistFree(bufid);
  return curhead;
}
#endif

#endif
