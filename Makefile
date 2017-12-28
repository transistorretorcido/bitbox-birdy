#birdy makefile
NO_SDCARD=1
NO_SOUND=1
#DEFINES = VGA_BPP=16 
#DEFINES += VGA_MODE=320 VGA_BPP=16 
NAME = birdy


GAME_C_FILES = assets.c $(NAME).c
GAME_C_OPTS = -DVGA_MODE=640
include $(BITBOX)/kernel/bitbox.mk

assets.c: back_green_top.png back_orange_top.png \
		  back_2.png bird_1.png bird_2.png bird_3.png \
		  bird_1_shad.png bird_2_shad.png bird_3_shad.png \
		  pipe_up.png tube.png \
		  tube_down_short.png tube_down_med.png tube_down_long.png \
		  tube_up_short.png tube_up_med.png tube_up_long.png \
		  tube_up_short_shad.png default_tube_shad.png \
		  n_zero.png n_one.png n_two.png n_three.png n_four.png n_five.png \
		  n_six.png n_seven.png n_eight.png n_nine.png \
		  floor_1.png back_1.png \
		  start_1.png go_1.png game_over_1.png
	python rle_encode2.py $^ > $@
	

	

	
	
    
   
	


