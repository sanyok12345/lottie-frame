#include "animation.h"

std::unique_ptr<rlottie::Animation> load_animation_from_data(const std::string& data) {
    return rlottie::Animation::loadFromData(data, "");
}

size_t get_total_frames(std::unique_ptr<rlottie::Animation>& animation) {
    return animation->totalFrame();
}

void render_frame_sync(std::unique_ptr<rlottie::Animation>& animation, size_t frame, rlottie::Surface& surface) {
    animation->renderSync(frame, surface, true);
}