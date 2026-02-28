#include "trackalloc.h"

uint32_t TrackAlloc::currentAlloc(){
    return alloc - dealloc;
}