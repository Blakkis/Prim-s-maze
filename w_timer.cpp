#include "w_timer.hpp"

#include <algorithm>
#include <SDL.h>


double W_FrameTimerPacket::ft_seconds     = 0.0;
double W_FrameTimerPacket::ft_fps         = 0.0;
double W_FrameTimerPacket::ft_delta       = 0.0;
double W_FrameTimerPacket::ft_alpha       = 0.0;
double W_FrameTimerPacket::ft_logic_dt    = 0.0;
double W_FrameTimerPacket::ft_accumulator = 0.0;
double W_FrameTimerPacket::ft_timescale   = 1.0;


bool W_FrameTimerPacket::ft_exhaustAccumulator(void)
{
    bool exhausted = false;
    if (ft_accumulator >= ft_logic_dt)
    {
        exhausted = true;
        ft_accumulator -= ft_logic_dt;
    }
    // Calculate time left
    ft_alpha = ft_accumulator / ft_logic_dt;
    return exhausted;
}


W_FramerateManager::W_FramerateManager(const std::size_t num_of_samples)
{
    // Min sample is 8 (Remove this in the future)
    frame_samples.reserve((num_of_samples < 8) ? 8: num_of_samples);
    frame_limit = num_of_samples;
    frame_count = 0;
    frame_last_tick = 0;

    setLogicFramerate(W_LOGIC_FRAMERATE);

    #ifdef ENABLE_FPS_CAP
    frame_max_framerate = 1000.0 / 60.0;
    #endif // ENABLE_FPS_CAP
}

void W_FramerateManager::tick(void)
{
    if(++frame_count >= frame_limit)
    {
        uint32_t average = std::accumulate(frame_samples.begin(), frame_samples.end(), 0);

        ft_fps = std::min(1000.0 / (static_cast<double>(average) / frame_limit), W_MAX_FRAMERATE);

        frame_samples.clear();
        frame_count = 0;
    }
    uint32_t frame_tick = SDL_GetTicks();
    uint32_t delta = frame_tick - frame_last_tick;

    ft_delta = std::min(ft_min_fps, static_cast<double>(delta) / 1000.0);
    ft_seconds = static_cast<double>(frame_tick / 1000.0);

    frame_samples.push_back(delta);
    frame_last_tick = frame_tick;

    ft_accumulator += ft_delta;
    #ifdef ENABLE_FPS_CAP
    if (delta < frame_max_framerate) SDL_Delay(frame_max_framerate - delta);
    #endif // ENABLE_FPS_CAP
}
