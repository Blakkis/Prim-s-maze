#pragma once

#include <vector>
#include <stdint.h>

//#define ENABLE_FPS_CAP
#define W_LOGIC_FRAMERATE 120.0 // Logic framerate
#define W_MIN_FRAMERATE 30.0    // Minimum allowed framerate
#define W_MAX_FRAMERATE 4096.0  // Maximum allowed framerate


struct W_FrameTimerPacket
{
    public:
        static bool ft_exhaustAccumulator(void);

        static double ft_getFramerate(void){return ft_fps;}
        static double ft_getLogicDelta(void){return ft_logic_dt;}
        static double ft_getSeconds(void){return ft_seconds;}
        static double ft_getDelta(void){return ft_delta;}
        static double ft_getFrameBlend(void){return ft_alpha;}
        static double ft_getTimeScale(void){return ft_timescale;}

        static void setLogicFramerate(double logic_framerate){
            ft_logic_dt = 1000.0 / logic_framerate / 1000.0;
        }

    private:
        static double ft_seconds;     // Program seconds
        static double ft_fps;         // Last calculated framerate from samples
        static double ft_delta;       // Last delta tick
        static double ft_alpha;       // Time left in accumulator represented between 0.0 - 1.0;
        static double ft_logic_dt;    // Internal logic framerate
        static double ft_accumulator; // Accumulator for deltas
        static double ft_timescale;   // Default 1.0

        static constexpr
        double ft_min_fps = 1000.0 / W_MIN_FRAMERATE / 1000.0;

    friend struct W_FramerateManager;
};


struct W_FramerateManager final: W_FrameTimerPacket
{
    explicit W_FramerateManager(const std::size_t num_of_samples);
    void tick(void);

    private:
        std::vector<uint32_t> frame_samples;
        unsigned int frame_count;   // Frame counter
        unsigned int frame_limit;   // Max number of samples before framerate is calculated
        uint32_t frame_last_tick;   // Store last tick

        #ifdef ENABLE_FPS_CAP
            double frame_max_framerate; // Framerate cap *Optional
        #endif // ENABLE_FPS_CAP

};
