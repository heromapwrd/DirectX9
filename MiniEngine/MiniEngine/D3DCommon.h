#ifndef _D3DCOMMON_H__
#define _D3DCOMMON_H__

#define DELETE(p) {if(p) { delete (p); (p)=NULL;}}
#define DELETEARRAY(p) {if(p) {delete [](p);(p)=NULL;}}
#define RELEASE(p) {if(p) {(p)->Release();(p)=NULL;}}
#define GETTIMESTAMP timeGetTime

#endif //_D3DCOOMMON_H__