idf_component_register(
    SRCS "main.c"
         "ui_pixel.c"
         "ui_pixel_math.c"
         "demo_display.c"
         "demo_button.c"
         "demo_audio.c"
         "demo_battery.c"
         "demo_leader.c"
    INCLUDE_DIRS "."
    REQUIRES bsp
    EMBED_FILES "../assets/leader_frame_0.rgb565"
                "../assets/leader_frame_1.rgb565"
                "../assets/leader_frame_2.rgb565"
                "../assets/leader_frame_3.rgb565"
                "../assets/leader_apology.pcm"
)
