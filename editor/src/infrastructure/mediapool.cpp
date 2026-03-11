#include "mediapool.h"

MediaSource* Mediapool::add_file(const char* filepath){
  pool_.push_back(std::make_unique<VideoSource>(filepath));

  return pool_.back().get();
};