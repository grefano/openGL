#pragma once

#include <iostream>
#include <cstdint>


struct TrackAlloc{
    uint32_t alloc = 0;
    uint32_t dealloc = 0;
    uint32_t currentAlloc();
};