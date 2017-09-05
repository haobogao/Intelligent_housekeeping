#include "system_UI_control.h"

void system_UI_control(unsigned int ui_command) {
	switch (ui_command) {
		case main_UI_cmd:
			status_system =  G_MAIN;
			printf("\n system UI jump to main_UI\n\n" );
			break;
		case musicplayer_UI_cmd:
			status_system =  G_MUSIC;
			printf("\n system UI jump to musicplayer UI\n\n");
			break;
		case picturedisplayer_UI_cmd:
			status_system =  G_PICTURE_DISP;
			printf("\n system UI jump to picturedisplayer UI\n\n" );
			break;
		case TH_value_UI_cmd:
			status_system =  G_T_SENSOR;
			printf("\n system UI jump to temperature and humidity value display UI\n\n");
			break;
		default :
			printf("\n Wrong UI command !\n\n");
			break;
	}
	
}