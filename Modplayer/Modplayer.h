#pragma once
typedef void *ModplayerHandle;

#ifdef _DEBUG
inline ModplayerHandle Modplayer_Create(char* path) { return 0; }
inline void Modplayer_Start(ModplayerHandle handle) {}
inline void Modplayer_Stop(ModplayerHandle handle) {}
inline void Modplayer_SetOrder(ModplayerHandle handle, unsigned int order) {}

#else
extern "C" {
	ModplayerHandle Modplayer_Create(char* path);
	void Modplayer_Start(ModplayerHandle handle);
	void Modplayer_Stop(ModplayerHandle handle);
	void Modplayer_SetOrder(ModplayerHandle handle, unsigned int order);
}
#endif // DEBUG
