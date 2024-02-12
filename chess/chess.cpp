/**
* The program is a chessboard with pieces that you can move with the selctor.
*  You can also move the camera around the board and zoom in and out.
*
* @File chess.cpp
* @Author Philip A. Sundt with parts taken from Rafael Palomar and Yan Chernikov (The Cherno)
*/
#include "chessApplication.h"


int main(int argc, char* argv[])
{
	chessApp application("chess", "1.0");

	application.ParseArguments(argc, argv);
	application.Init();
	
	return application.Run();
}
