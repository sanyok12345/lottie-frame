#ifndef ANIMATION_H
#define ANIMATION_H

#include <memory>
#include <string>
#include <rlottie.h>

std::unique_ptr<rlottie::Animation> load_animation_from_data(const std::string& data);
size_t get_total_frames(std::unique_ptr<rlottie::Animation>& animation);
void render_frame_sync(std::unique_ptr<rlottie::Animation>& animation, size_t frame, rlottie::Surface& surface);

#endif // ANIMATION_H