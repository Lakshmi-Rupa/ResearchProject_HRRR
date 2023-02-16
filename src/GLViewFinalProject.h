#pragma once

#include "GLView.h"
#include <vector>
#include "NetMessengerClient.h"
#include <WOFTGLString.h>
#include "WOGUILabel.h"

using namespace std;

namespace Aftr
{
	class Camera;

	/**
	   \class GLViewFinalProject
	   \author Scott Nykl
	   \brief A child of an abstract GLView. This class is the top-most manager of the module.

	   Read \see GLView for important constructor and init information.

	   \see GLView

		\{
	*/

	class GLViewFinalProject : public GLView
	{
	public:
		//declaring structure to create 
		struct onedvals {
			int rowindex;
			string lat1d;
			string lon1d;
			string param1d;
		};
		static GLViewFinalProject* New(const std::vector< std::string >& outArgs);
		virtual ~GLViewFinalProject();
		virtual void updateWorld(); ///< Called once per frame
		virtual void loadMap(); ///< Called once at startup to build this module's scene
		virtual void createFinalProjectWayPoints();
		virtual void onResizeWindow(GLsizei width, GLsizei height);
		virtual void onMouseDown(const SDL_MouseButtonEvent& e);
		virtual void onMouseUp(const SDL_MouseButtonEvent& e);
		virtual void onMouseMove(const SDL_MouseMotionEvent& e);
		virtual void onKeyDown(const SDL_KeyboardEvent& key);
		virtual void onKeyUp(const SDL_KeyboardEvent& key);
		virtual void displayGrid(std::vector<onedvals> matrix);
		virtual double minParamValue(std::vector<onedvals> matrix);
		virtual double maxParamValue(std::vector<onedvals> matrix);
		virtual std::string openfiles(const std::string& path1, const std::string& path2, const std::string& path3, double latitude, double longitude);
		virtual void deleteGrid();

		//virtual void displayGrid(std::string arr[1905141][3]);
		//virtual double minParamValue(std::string arr[1905141][3]);
		//virtual double maxParamValue(std::string arr[1905141][3]);
	
		bool temp;
		std::string paramretval;
		double lat = 0;
		double lon = 0;

		//declaring index for vector matrix
		int index = 0;

		vector<string> rowlat;
		vector<string> rowlon;
		vector<string> rowparam;
		std::string linelat, linelon, lineparam, word1, word2, word3;
		std::vector<std::vector<Aftr::VectorD>> gridpt;
		std::vector<std::vector<Aftr::aftrColor4ub>> color;
		std::vector<onedvals> matrix;

		int rowsize = 0;
		int colsize = 0;
		

	protected:
		GLViewFinalProject(const std::vector< std::string >& args);
		virtual void onCreate();
	};

} //namespace Aftr
