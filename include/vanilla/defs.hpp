#pragma once
#define _XOPEN_SOURCE 700

#define _XOPEN_REALTIME 1
#define _POSIX_ASYNC_IO 1
#define _POSIX_PRIO_IO 1
#define _POSIX_SYNC_IO 1
#define _XOPEN_SHM 1
#define _POSIX_PRIORITIZED_IO 1
#define _POSIX_REGEXP 1
#undef _FLT_ROUNDS
#define _FLT_ROUNDS 1
// #pragma pack(4)
#pragma fenv_access(on)
#pragma STDC FP_CONTRACT ON
#pragma STDC CX_LIMITED_RANGE ON

