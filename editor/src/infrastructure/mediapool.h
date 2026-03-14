#pragma once
#include "ffmpeg.h"
#include <vector>
#include <memory>
class MediaSource{
  public:
  const char* filepath;
  MediaSource(const char* filepath) : filepath(filepath){};
};
class VideoSource : public MediaSource{
  public:
  int w, h;
  float duration;
  VideoSource(const char * filepath) : MediaSource(filepath){};
};
class ImageSource : public MediaSource{
  int w, h;

};

class Mediapool{
  public:
  MediaSource* add_file(const char* filepath); // escolhe ref concreta baseado no filepath, adiciona ref na pool, retorna ref
  const std::vector<std::unique_ptr<MediaSource>>& get_pool();
  private:
  std::vector<std::unique_ptr<MediaSource>> pool_;
};


