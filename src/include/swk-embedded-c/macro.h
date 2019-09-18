#pragma once

#ifdef __cplusplus
#define SWK_EXTERNC_BEGIN extern "C" { 
#define SWK_EXTERNC_END }
#else 
#define SWK_EXTERNC_BEGIN
#define SWK_EXTERNC_END
#endif
