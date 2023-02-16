#include "GLViewFinalProject.h"

#include "WorldList.h" //This is where we place all of our WOs
#include "ManagerOpenGLState.h" //We can change OpenGL State attributes with this
#include "Axes.h" //We can set Axes to on/off with this
#include "PhysicsEngineODE.h"

//Different WO used by this module
#include "WO.h"
#include "WOStatic.h"
#include "WOStaticPlane.h"
#include "WOStaticTrimesh.h"
#include "WOTrimesh.h"
#include "WOHumanCyborg.h"
#include "WOHumanCal3DPaladin.h"
#include "WOWayPointSpherical.h"
#include "WOLight.h"
#include "WOSkyBox.h"
#include "WOCar1970sBeater.h"
#include "Camera.h"
#include "CameraStandard.h"
#include "CameraChaseActorSmooth.h"
#include "CameraChaseActorAbsNormal.h"
#include "CameraChaseActorRelNormal.h"
#include "Model.h"
#include "ModelDataShared.h"
#include "ModelMesh.h"
#include "ModelMeshDataShared.h"
#include "ModelMeshSkin.h"
#include "WONVStaticPlane.h"
#include "WONVPhysX.h"
#include "WONVDynSphere.h"
#include "WOImGui.h" //GUI Demos also need to #include "AftrImGuiIncludes.h"
#include "AftrImGuiIncludes.h"
#include "AftrGLRendererBase.h"
#include "NetMessengerClient.h"
#include "GLSLShader.h"
#include "GLSLShaderPerVertexColorGL32.h"
#include "AftrUtilities.h"
#include <string>
#include "WOGridECEFElevation.h"
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <WOFTGLString.h>
#include "WOGUILabel.h"
#include <MGLFTGLString.h>
#include <boost/lexical_cast.hpp>
#include "GLViewFinalProject.h"
#include <cstdlib>


//#include <iomanip>

using namespace Aftr;
using namespace std;



std::string comicSans(ManagerEnvironmentConfiguration::getSMM() + "/fonts/COMIC.ttf");

GLViewFinalProject* GLViewFinalProject::New(const std::vector< std::string >& args)
{
	GLViewFinalProject* glv = new GLViewFinalProject(args);
	glv->init(Aftr::GRAVITY, Vector(0, 0, -1.0f), "aftr.conf", PHYSICS_ENGINE_TYPE::petODE);
	glv->onCreate();
	return glv;
}

GLViewFinalProject::GLViewFinalProject(const std::vector< std::string >& args) : GLView(args)
{
	//Initialize any member variables that need to be used inside of LoadMap() here.
	//Note: At this point, the Managers are not yet initialized. The Engine initialization
	//occurs immediately after this method returns (see GLViewFinalProject::New() for
	//reference). Then the engine invoke's GLView::loadMap() for this module.
	//After loadMap() returns, GLView::onCreate is finally invoked.

	//The order of execution of a module startup:
	//GLView::New() is invoked:
	//    calls GLView::init()
	//       calls GLView::loadMap() (as well as initializing the engine's Managers)
	//    calls GLView::onCreate()

	//GLViewFinalProject::onCreate() is invoked after this module's LoadMap() is completed.

	temp = false;
}


void GLViewFinalProject::onCreate()
{
	//GLViewFinalProject::onCreate() is invoked after this module's LoadMap() is completed.
	//At this point, all the managers are initialized. That is, the engine is fully initialized.

	if (this->pe != NULL)
	{
		//optionally, change gravity direction and magnitude here
		//The user could load these values from the module's aftr.conf
		this->pe->setGravityNormalizedVector(Vector(0, 0, -1.0f));
		this->pe->setGravityScalar(Aftr::GRAVITY);
	}
	this->setActorChaseType(STANDARDEZNAV); //Default is STANDARDEZNAV mode
	//this->setNumPhysicsStepsPerRender( 0 ); //pause physics engine on start up; will remain paused till set to 1
}


GLViewFinalProject::~GLViewFinalProject()
{
	//Implicitly calls GLView::~GLView()
}


void GLViewFinalProject::updateWorld()
{
	GLView::updateWorld(); //Just call the parent's update world first.
	//If you want to add additional functionality, do it after
	//this call.
}


void GLViewFinalProject::onResizeWindow(GLsizei width, GLsizei height)
{
	GLView::onResizeWindow(width, height); //call parent's resize method.
}


void GLViewFinalProject::onMouseDown(const SDL_MouseButtonEvent& e)
{
	GLView::onMouseDown(e);
}


void GLViewFinalProject::onMouseUp(const SDL_MouseButtonEvent& e)
{
	GLView::onMouseUp(e);
}


void GLViewFinalProject::onMouseMove(const SDL_MouseMotionEvent& e)
{
	GLView::onMouseMove(e);
}


void GLViewFinalProject::onKeyDown(const SDL_KeyboardEvent& key)
{
	GLView::onKeyDown(key);
	if (key.keysym.sym == SDLK_0)
		this->setNumPhysicsStepsPerRender(1);
}


void GLViewFinalProject::onKeyUp(const SDL_KeyboardEvent& key)
{
	GLView::onKeyUp(key);
}


void Aftr::GLViewFinalProject::loadMap()
{
	this->worldLst = new WorldList(); //WorldList is a 'smart' vector that is used to store WO*'s
	this->actorLst = new WorldList();
	this->netLst = new WorldList();

	ManagerOpenGLState::GL_CLIPPING_PLANE = 1000.0;
	ManagerOpenGLState::GL_NEAR_PLANE = 0.1f;
	ManagerOpenGLState::enableFrustumCulling = false;
	Axes::isVisible = true;
	this->glRenderer->isUsingShadowMapping(false); //set to TRUE to enable shadow mapping, must be using GL 3.2+

	this->cam->setPosition(15, 15, 10);

	std::string shinyRedPlasticCube(ManagerEnvironmentConfiguration::getSMM() + "/models/cube4x4x4redShinyPlastic_pp.wrl");
	std::string wheeledCar(ManagerEnvironmentConfiguration::getSMM() + "/models/rcx_treads.wrl");
	std::string grass(ManagerEnvironmentConfiguration::getSMM() + "/models/grassFloor400x400_pp.wrl");
	std::string human(ManagerEnvironmentConfiguration::getSMM() + "/models/human_chest.wrl");
	/*std::string jet(ManagerEnvironmentConfiguration::getSMM() + "/models/jet_wheels_down_PP.wrl");*/

	//SkyBox Textures readily available
	std::vector< std::string > skyBoxImageNames; //vector to store texture paths
	skyBoxImageNames.push_back(ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_mountains+6.jpg");

	{
		//Create a light
		float ga = 0.1f; //Global Ambient Light level for this module
		ManagerLight::setGlobalAmbientLight(aftrColor4f(ga, ga, ga, 1.0f));
		WOLight* light = WOLight::New();
		light->isDirectionalLight(true);
		light->setPosition(Vector(0, 0, 100));
		//Set the light's display matrix such that it casts light in a direction parallel to the -z axis (ie, downwards as though it was "high noon")
		//for shadow mapping to work, this->glRenderer->isUsingShadowMapping( true ), must be invoked.
		light->getModel()->setDisplayMatrix(Mat4::rotateIdentityMat({ 0, 1, 0 }, 90.0f * Aftr::DEGtoRAD));
		light->setLabel("Light");
		worldLst->push_back(light);
	}

	{
		//Create the SkyBox
		WO* wo = WOSkyBox::New(skyBoxImageNames.at(0), this->getCameraPtrPtr());
		wo->setPosition(Vector(0, 0, 0));
		wo->setLabel("Sky Box");
		wo->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;
		worldLst->push_back(wo);
	}

	createFinalProjectWayPoints();

	WOImGui* gui = WOImGui::New(nullptr);
	gui->setLabel("My Gui");
	gui->subscribe_drawImGuiWidget(
		[this, gui]() //this is a lambda, the capture clause is in [], the input argument list is in (), and the body is in {}
		{
			ImGui::Begin("Visualization");//creates a new window 

			//ImGui::Text("Enter Latitude");
			ImGui::InputDouble("Enter Latitude", &lat, 0.0, 0.0, "%.06f", 0);
			//ImGui::Text("Enter Longitude");
			ImGui::InputDouble("Enter Longitude", &lon, 0.0, 0.0, "%.06f", 0);

			ImGui::Text("Select Parameter");
			const char* items[] = { "--Select Parmeter--", "Visibility", "Temperature", "Surface Pressure"};
			static int item_current = 0;
			string value;
			if (ImGui::Combo(" ", &item_current, items, IM_ARRAYSIZE(items))) {
				value = items[item_current];
			}

			if (item_current == 1) {
				ImGui::Text("Visibility Visualization");
				if (ImGui::Button("Get Visibility (In Miles)")) {
					temp = true;
					paramretval = openfiles("C:/Users/msrup/latitude.csv", "C:/Users/msrup/longitude.csv", "C:/Users/msrup/vis.csv", lat, lon);
				}
			}

			if (item_current == 2) {
				ImGui::Text("Temperature Visualization");
				if (ImGui::Button("Get Temperature (In Kelvin)")) {
					temp = true;
					paramretval = openfiles("C:/Users/msrup/latitude.csv", "C:/Users/msrup/longitude.csv", "C:/Users/msrup/t.csv", lat, lon);
				}
			}

		/*	if (item_current == 3) {
				ImGui::Text("Surface Pressure Visualization");
				if (ImGui::Button("Get Surface Pressure (In kPa)")) {
					temp = true;
					paramretval = openfiles("C:/Users/msrup/latitude.csv", "C:/Users/msrup/longitude.csv", "C:/Users/msrup/sp.csv", lat, lon);
				}
			}*/

			if (temp == true) {
				ImGui::Text(paramretval.c_str());
			}

			if (ImGui::Button("Clear Values")) {
				temp = false;
			}

			if (ImGui::Button("Reset")) {
				temp = false;
				lat = 0.0;
				lon = 0.0;
				deleteGrid();
			}

			ImGui::End(); //finalizes a window/

			/*ImPlot::BeginPlot("Test");
			ImPlot::EndPlot();*/
			//ImPlot::PlotHeatmap();

		});

	this->worldLst->push_back(gui);
}


void GLViewFinalProject::createFinalProjectWayPoints()
{
	// Create a waypoint with a radius of 3, a frequency of 5 seconds, activated by GLView's camera, and is visible.
	WayPointParametersBase params(this);
	params.frequency = 5000;
	params.useCamera = true;
	params.visible = true;
	WOWayPointSpherical* wayPt = WOWayPointSpherical::New(params, 3);
	wayPt->setPosition(Vector(-20, -20, 3));
	worldLst->push_back(wayPt);
}

string GLViewFinalProject::openfiles(const std::string& path1, const std::string& path2, const std::string& path3, double latitude, double longitude) {

	std::ifstream fin1;
	std::ifstream fin2;
	std::ifstream fin3;

	//cout << "my values***************************************************************" << endl;

	fin1.open(path1, ios::in);
	fin2.open(path2, ios::in);
	fin3.open(path3, ios::in);

	int row_index = 0;
	int line_count = 0;
	if (fin1.is_open() && fin2.is_open() && fin3.is_open())
	{
		//cout << "my values***************************************************************8open" << endl;
		while (getline(fin1, linelat) && getline(fin2, linelon) && getline(fin3, lineparam))
		{
			/*line_count++;
			cout << "line count" << line_count << endl;*/
			rowlat.clear();
			rowlon.clear();
			rowparam.clear();

			stringstream str1(linelat);
			stringstream str2(linelon);
			stringstream str3(lineparam);

			//cout << "line " << linelat << endl;

			while (getline(str1, word1, ','))
				rowlat.push_back(word1);
			while (getline(str2, word2, ','))
				rowlon.push_back(word2);
			while (getline(str3, word3, ','))
				rowparam.push_back(word3);

			//for (int i = 0; i < 20; i++)
			//{
			//	/*cout << row_index << " row values:" << endl;
			//	cout << rowlat[i] << "," << rowlon[i] << "," << rowparam[i] << endl;*/
			//	arr[row_index][0] = rowlat[i];
			//	arr[row_index][1] = rowlon[i];
			//	arr[row_index][2] = rowparam[i];
			//	row_index++;
			//}
			
			for (int i = 0; i < rowlat.size(); i++) {
				matrix.push_back({ index, rowlat[i],rowlon[i], rowparam[i] });
				index++;
			}
			
		}
		/*int k = 0;
		for (int i = 0; i < index; i++) {
			cout << matrix[i].rowindex << "------" << matrix[i].lat1d << "------" << matrix[i].lon1d << "------" << matrix[i].param1d << endl;
			k++;
			cout << k << "----------count" << endl;
		}*/
	}

	else {
		cout << "Could not open the file\n";
	}

	/*cout << rowlat.size() << "---------column size" << endl;
	cout << index << "---------row size" << endl;*/

	/*rowsize = rowlat.size();
	colsize = index / rowlat.size();*/

	rowsize = 1059;
	colsize = 1799;


	displayGrid(matrix);

	double mindis = 2147483647;
	string lonfin, latfin;


	string nearestparam = "";
	string gridparam = "";

	int flag = 0;
	double x = latitude;
	string latval = to_string(x);
	double y = longitude;
	string lonval = to_string(y);

	for (int z = 0; z < index; z++) {
		latval.clear();
		lonval.clear();
		//cout << arr[z][0] << "," << arr[z][1] << "," << arr[z][2] << endl;
		if (latval == matrix[z].lat1d && lonval == matrix[z].lon1d) {
			flag = 1;
			gridparam = matrix[z].param1d;
			break;
		}

		//find nearest point
		double latdiff = stod(matrix[z].lat1d) - x;
		double londiff = stod(matrix[z].lon1d) - y;
		double distance = sqrt(latdiff * latdiff + londiff * londiff);
		if (distance <= mindis)
		{
			lonfin.clear();
			latfin.clear();
			mindis = distance;
			nearestparam = matrix[z].param1d;
			lonfin = matrix[z].lon1d;
			latfin = matrix[z].lat1d;

			/*cout << distance << endl;
			cout << latfin << " " << lonfin << " " << nearestparam << endl;*/
		}
	}
	//cout << latfin << " " << lonfin << " " << nearestparam << " ********************" << endl;

	if (flag == 0) {
		return nearestparam;
	}
	else {
		return gridparam;
	}
}

void GLViewFinalProject::displayGrid(vector<onedvals> matrix) {
	/*for (int z = 0; z < 400; z++) {
		cout << arr[z][0] << "," << arr[z][1] << "," << arr[z][2] << endl;
	}*/

	std::vector<std::vector<Aftr::VectorD>> gridpt;
	std::vector<std::vector<Aftr::aftrColor4ub>> color;

	gridpt.clear();
	color.clear();

	double minpv = minParamValue(matrix);
	double maxpv = maxParamValue(matrix);

	int z = 0;

	for (int i = 0; i < rowsize; ++i) {
		gridpt.push_back(std::vector<Aftr::VectorD>(0));
		color.push_back(std::vector<Aftr::aftrColor4ub>(0));
		gridpt[i].clear();
		color[i].clear();
		gridpt[i].resize(colsize);
		color[i].resize(colsize);

		for (int j = 0; j < colsize; ++j) {
			//cout << matrix[z].param1d << endl;
			gridpt[i][j] = VectorD(i, j, stod(matrix[z].param1d));
			z++;
			//cout << gridpt[i][j].z << endl;
			double c = gridpt[i][j].z;
			float value = 0;
			if (c < minpv) {
				value = 0;
			}
			else if (c > maxpv) {
				value = 1;
			}
			else {
				value = (c - minpv) / (maxpv - minpv);
			}
			Vector hsv = { value * (5.0f / 6.0f), 1, 1 };
			color[i][j] = AftrUtilities::convertHSVtoRGB(hsv);
		}
	}

	WOGrid* heightField = WOGrid::New(gridpt, Vector(1, 1, 1), color);

	ModelMeshSkin& hfSkin = heightField->getModel()->getModelDataShared()->getModelMeshes().at(0)->getSkins().at(0);
	hfSkin.getMultiTextureSet().at(0)->setTextureRepeats(5.0f);
	hfSkin.setAmbient(aftrColor4f(0.4f, 0.4f, 0.4f, 1.0f)); //Color of object when it is not in any light
	hfSkin.setDiffuse(aftrColor4f(1.0f, 1.0f, 1.0f, 1.0f)); //Diffuse color components (ie, matte shading color of this object)
	hfSkin.setSpecular(aftrColor4f(0.4f, 0.4f, 0.4f, 1.0f)); //Specular color component (ie, how "shiney" it is)
	hfSkin.setSpecularCoefficient(10);
	std::string vertexShader = ManagerEnvironmentConfiguration::getSMM() + "/shaders/defaultGL32.vert";
	std::string fragmentShader = ManagerEnvironmentConfiguration::getSMM() + "/shaders/defaultGL32PerVertexColor.frag";
	hfSkin.setShader(GLSLShaderPerVertexColorGL32::New());

	worldLst->push_back(heightField);
}

double GLViewFinalProject::minParamValue(std::vector<onedvals> matrix) {
	double minval = 2147483647;
	double pval = 2147483647;
	for (int z = 0; z < index ; z++) {

		pval = stod(matrix[z].param1d);
		if (pval <= minval) {
			minval = pval;
		}
	}
	//cout << minval << "minimum value" << endl;
	return minval;
}

double GLViewFinalProject::maxParamValue(std::vector<onedvals> matrix) {
	double maxval = -2147483647;
	double pval = -2147483647;
	for (int z = 0; z < index; z++) {

		pval = stod(matrix[z].param1d);
		if (pval >= maxval) {
			maxval = pval;
		}
	}
	//cout << maxval << "maximum value" << endl;
	return maxval;
}

void GLViewFinalProject::deleteGrid() {
	if (worldLst->size() > 2) {
		worldLst->eraseViaWOIndex(worldLst->size() - 1);
	}
}