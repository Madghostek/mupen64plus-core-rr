#include "backends/api/encoder.h"
#include "SDL_mutex.h"
#include "api/callbacks.h"
#include "api/m64p_types.h"
#include "m64p_encoder.h"
#include "encoder.h"

#include <SDL2/SDL_mutex.h>

static void* backend_obj = NULL;
static struct encoder_backend_interface* ibackend;
static struct SDL_mutex* enc_mutex;

extern struct encoder_backend_interface g_iffmpeg_encoder_backend;

BOOL Encoder_IsActive() {
    return backend_obj != NULL;
}

EXPORT m64p_error CALL Encoder_Start(const char* path, m64p_encoder_format format) {
    m64p_error res;
    SDL_LockMutex(enc_mutex);
    res = ibackend->init(&backend_obj, path, format);
    SDL_UnlockMutex(enc_mutex);
    return res;
}

EXPORT m64p_error CALL Encoder_Stop(int discard) {
    SDL_LockMutex(enc_mutex);
    m64p_error res = ibackend->free(backend_obj, discard);
    backend_obj = NULL;
    SDL_UnlockMutex(enc_mutex);
    return res;
}

void encoder_startup() {
    ibackend = &g_iffmpeg_encoder_backend;
    enc_mutex = SDL_CreateMutex();
}
void encoder_shutdown() {
    SDL_LockMutex(enc_mutex);
    
    ibackend->free(backend_obj, false);
    backend_obj = NULL;
    SDL_UnlockMutex(enc_mutex);
    
    SDL_DestroyMutex(enc_mutex);
}
void encoder_push_video() {
    m64p_error res;
    SDL_LockMutex(enc_mutex);
    
    res = ibackend->push_video(backend_obj);
    if (res != M64ERR_SUCCESS) {
        DebugMessage(M64MSG_ERROR, "Backend audio push failed");
    } 
    SDL_UnlockMutex(enc_mutex);
}
void encoder_set_sample_rate(unsigned int rate) {
    m64p_error res;
    SDL_LockMutex(enc_mutex);
    
    res = ibackend->set_sample_rate(backend_obj, rate);
    if (res != M64ERR_SUCCESS) {
        DebugMessage(M64MSG_ERROR, "Backend audio push failed");
    } 
    SDL_UnlockMutex(enc_mutex);
}
void encoder_push_audio(void *data, size_t len) {
    m64p_error res;
    SDL_LockMutex(enc_mutex);
    
    res = ibackend->push_audio(backend_obj, data, len);
    if (res != M64ERR_SUCCESS) {
        DebugMessage(M64MSG_ERROR, "Backend audio push failed");
    } 
    SDL_UnlockMutex(enc_mutex);
}