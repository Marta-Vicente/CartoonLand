////////////////////////////////////////////////////////////////////////////////
//
//  Loading meshes from external files
//
//
//
// INTRODUCES:
// MODEL DATA, ASSIMP, mglMesh.hpp
//
////////////////////////////////////////////////////////////////////////////////


#include <iostream>
#include <iomanip>
#include <string>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>
#include "mgl/mgl.hpp"

#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/string_cast.hpp>	

#include <irrKlang.h>
#include <FreeImage.h>

////////////////////////////////////////////////////////////////////////// WINDOW
int windowWidth, windowHeight;
////////////////////////////////////////////////////////////////////////// WINDOW
int snapNum = 1;
#define TRANSF_MESHES 7
////////////////////////////////////////////////////////////////////////// SOUND

irrklang::ISoundEngine* SoundEngine = irrklang::createIrrKlangDevice();
const float soundVolume = 0.3f;

////////////////////////////////////////////////////////////////////////// MYAPP
glm::mat4 ModelMatrix(1.0f);
const glm::mat4 ChangingModelMatrix = ModelMatrix;

struct Mesh_obj
{
	mgl::Mesh* Mesh = nullptr;
	//mgl::ShaderProgram* Shaders = nullptr;
	glm::vec3 color;
	glm::mat4 transformation;
} ;


class MyApp : public mgl::App {

public:
	void initCallback(GLFWwindow* win) override;
	void displayCallback(GLFWwindow* win, double elapsed) override;
	void windowSizeCallback(GLFWwindow* win, int width, int height) override;
	void windowCloseCallback(GLFWwindow* win) override;
	void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) override;
	void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) override;
private:
	const GLuint POSITION = 0, COLOR = 1, UBO_BP = 0;

	glm::vec3 axis_x = { 1.0f, 0.0f, 0.0f };
	glm::vec3 axis_y = { 0.0f, 1.0f, 0.0f };
	glm::vec3 axis_z = { 0.0f, 0.0f, 1.0f };

	// CAMERA1
	glm::quat initial_postion_c1 = { 0.0f, 0.178f, 5.422f, 9.928f };
	float alfa = 0.0f;
	float beta = 0.0f;
	int accelaration_x = 0;
	int accelaration_y = 0;
	float zoom = 2.f;
	bool zooming = true;
	bool projection_camera1 = true;

	// CAMERA2
	glm::quat initial_position_c2 = { 0.f, 0.317f, 0.167f, 11.308f };
	float alfa2 = 0.0f;
	float beta2 = 0.0f;
	int accelaration_x2 = 0;
	int accelaration_y2 = 0;
	float zoom2 = 2.f;
	bool projection_camera2 = true;

	bool OLD_P_CLICKED = false;
	bool OLD_C_CLICKED = false;
	bool OLD_S_CLICKED = false;

	bool camera1_on = true;

	glm::mat4 c1_ChangingViewMatrix;
	glm::mat4 c2_ChangingViewMatrix;

	double xpos, ypos = 0;
	double old_xpos, old_ypos = 0;
	int button, action, mods;

	mgl::ShaderProgram* Shaders = nullptr;

	//Camera
	mgl::Camera* Camera = nullptr;
	mgl::Camera* Camera2 = nullptr;
	GLint ModelMatrixId;

	//CAMERA TYPES
	// Orthographic LeftRight(-2,2) BottomTop(-2,2) NearFar(1,10)
	glm::mat4 ProjectionMatrix1 =
		glm::ortho(-2.0f, 2.0f, -2.0f, 2.0f, 1.0f, 15.0f);

	// Perspective Fovy(30) Aspect(640/480) NearZ(1) FarZ(30)
	glm::mat4 ProjectionMatrix2 =
		glm::perspective(glm::radians(30.0f), 4.0f / 3.0f, 1.0f, 30.0f);

	std::vector<Mesh_obj> meshes;

	//Movement var
	float parametric_movement = 0.0f;
	const float param_sensitivity = 0.011f;
	const float max_param = 1.0f;
	const float min_param = 0.0f;

	//init
	void createMeshes();
	void createShaderPrograms();
	void createCamera();
	//update
	void update(GLFWwindow* win);
	void updateTransformationMatrices();
	void processMouseMovement(GLFWwindow* win);
	//render
	void render();
	//window size
	void updateProjMatrices(float ratio);
	//
	void snapshot(GLFWwindow* win, int width, int height);
};



///////////////////////////////////////////////////////////////////////// MESHES

void MyApp::createMeshes() {

	std::string mesh_dir = "../assets/";

	std::vector<std::string> meshesNames;
	std::vector<glm::vec3> colors;
	std::vector<glm::mat4> transformations;

	glm::mat4 M;
	glm::mat4 rotationBetweenPlanes = glm::rotate(glm::radians(parametric_movement * 90.f), glm::vec3(1.f, 0.f, 0.f));


	meshesNames.push_back("blue_triangle.obj");
	colors.push_back({ 0.1f, 0.9f, 0.9f });
	//--------------------------------------------------------------------------

	meshesNames.push_back("pink_triangle.obj");
	colors.push_back({ 0.9f, 0.45f, 0.5f });
	//--------------------------------------------------------------------------

	meshesNames.push_back("orange_triangle.obj");
	colors.push_back({ 0.9f, 0.5f, 0.1f });
	//--------------------------------------------------------------------------

	meshesNames.push_back("paralelogram.obj");
	colors.push_back({ 0.9f, 0.7f, 0.1f });
	//--------------------------------------------------------------------------

	meshesNames.push_back("purple_triangle.obj");
	colors.push_back({ 0.7f, 0.1f, 0.9f });
	//--------------------------------------------------------------------------

	meshesNames.push_back("red_triangle.obj");
	colors.push_back({ 0.9f, 0.1f, 0.1f });
	//--------------------------------------------------------------------------

	meshesNames.push_back("green_cube.obj");
	//meshesNames.push_back("pantheon.obj");
	colors.push_back({ 0.1f, 0.9f, 0.1f });
	//--------------------------------------------------------------------------

	for (int i = 0; i < meshesNames.size(); i++) {
		Mesh_obj meshSingle;
		meshSingle.Mesh = new mgl::Mesh();
		meshSingle.Mesh->joinIdenticalVertices();
		meshSingle.Mesh->create(mesh_dir + meshesNames[i]);
		meshSingle.color = colors[i];
		meshes.push_back(meshSingle);
	}
	updateTransformationMatrices();

}


///////////////////////////////////////////////////////////////////////// SHADER

void MyApp::createShaderPrograms() {
	Shaders = new mgl::ShaderProgram();
	Shaders->addShader(GL_VERTEX_SHADER, "cube-vs.glsl");
	Shaders->addShader(GL_FRAGMENT_SHADER, "cube-fs.glsl");

	Shaders->addAttribute(mgl::POSITION_ATTRIBUTE, mgl::Mesh::POSITION);
	if (meshes[0].Mesh->hasNormals()) {
		Shaders->addAttribute(mgl::NORMAL_ATTRIBUTE, mgl::Mesh::NORMAL);
	}
	if (meshes[0].Mesh->hasTexcoords()) {
		Shaders->addAttribute(mgl::TEXCOORD_ATTRIBUTE, mgl::Mesh::TEXCOORD);
	}
	if (meshes[0].Mesh->hasTangentsAndBitangents()) {
		Shaders->addAttribute(mgl::TANGENT_ATTRIBUTE, mgl::Mesh::TANGENT);
	}

	Shaders->addUniform(mgl::MODEL_MATRIX);
	Shaders->addUniformBlock(mgl::CAMERA_BLOCK, UBO_BP);
	Shaders->addUniform("Color");
	Shaders->create();

	ModelMatrixId = Shaders->Uniforms[mgl::MODEL_MATRIX].index;
}

///////////////////////////////////////////////////////////////////////// CAMERA

void MyApp::updateProjMatrices(float ratio) {
	ProjectionMatrix1 = glm::ortho(-2.f * ratio, 2.f * ratio, -2.0f * ratio, 2.0f* ratio, 1.0f, 15.0f);
	ProjectionMatrix2 = glm::perspective(glm::radians(30.0f), ratio, 1.0f, 30.0f);
}

void MyApp::createCamera() {

	Camera2 = new mgl::Camera(UBO_BP);
	Camera = new mgl::Camera(UBO_BP);
	Camera->setViewMatrix(glm::lookAt({ initial_postion_c1.x,initial_postion_c1.y,initial_postion_c1.z },
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f)));
	Camera->setProjectionMatrix(ProjectionMatrix2);

}

/////////////////////////////////////////////////////////////////////////// DRAW

void MyApp::updateTransformationMatrices() {
	std::vector<glm::mat4> tTemp;

	glm::mat4 M;
	glm::mat4 rotationBetweenPlanes = glm::rotate(glm::radians(parametric_movement * 90.f), glm::vec3(1.f, 0.f, 0.f));

	M = ChangingModelMatrix
		* glm::translate(glm::vec3((-0.9f * glm::cos(glm::radians(45.f)) * parametric_movement), (0.9f * glm::cos(glm::radians(45.f)) * parametric_movement), 0))
		* rotationBetweenPlanes
		* glm::rotate(glm::radians(parametric_movement * -45.f), glm::vec3(0.f, 1.f, 0.f));
	tTemp.push_back(M);

	M = ChangingModelMatrix
		* glm::translate(glm::vec3(0, (-0.9f * glm::cos(glm::radians(45.f)) * parametric_movement), 0))
		* rotationBetweenPlanes
		* glm::rotate(glm::radians(parametric_movement * 45.f), glm::vec3(0.f, 1.f, 0.f));
	tTemp.push_back(M);

	M = ChangingModelMatrix
		* glm::translate(glm::vec3(-0.9f * glm::cos(glm::radians(45.f)) * parametric_movement, 0.9f * glm::cos(glm::radians(45.f)) * parametric_movement, 0))
		* rotationBetweenPlanes
		* glm::rotate(glm::radians(parametric_movement * 135.f), glm::vec3(0.f, 1.f, 0.f));
	tTemp.push_back(M);

	M = ChangingModelMatrix
		* glm::translate(glm::vec3(-0.9f * glm::cos(glm::radians(45.f)) * parametric_movement, -0.9f * glm::cos(glm::radians(45.f)) * parametric_movement, 0))
		* rotationBetweenPlanes
		* glm::rotate(glm::radians(parametric_movement * -45.f), glm::vec3(0.f, 1.f, 0.f))
		* glm::rotate(glm::radians(parametric_movement * 180.f), glm::vec3(1.f, 0.f, 0.f));
	tTemp.push_back(M);

	M = ChangingModelMatrix
		* glm::translate(glm::vec3((0.45f * glm::cos(glm::radians(45.f)) * parametric_movement), (-0.45f * glm::cos(glm::radians(45.f)) * parametric_movement), 0))
		* rotationBetweenPlanes
		* glm::rotate(glm::radians(parametric_movement * -45.f), glm::vec3(0.f, 1.f, 0.f))
		* glm::translate(glm::vec3(0.f, 0.f, -0.45f * parametric_movement));
	tTemp.push_back(M);

	M = ChangingModelMatrix
		* glm::translate(glm::vec3((0.45f * glm::cos(glm::radians(45.f)) * parametric_movement), (0.45f * glm::cos(glm::radians(45.f)) * parametric_movement), 0))
		* rotationBetweenPlanes
		* glm::rotate(glm::radians(parametric_movement * 135.f), glm::vec3(0.f, 1.f, 0.f))
		* glm::translate(glm::vec3(0.45f * parametric_movement, 0.f, 0.f));
	tTemp.push_back(M);

	M = ChangingModelMatrix
		* rotationBetweenPlanes
		* glm::rotate(glm::radians(parametric_movement * 135.f), glm::vec3(0.f, 1.f, 0.f));
	tTemp.push_back(M);

	for (int i = 0; i < TRANSF_MESHES; i++) {
		meshes[i].transformation = tTemp[i];
	}
}

void MyApp::update(GLFWwindow* win) {
	//INPUT
	processMouseMovement(win);
	updateTransformationMatrices();

	//CAMERAS
	if (camera1_on) {

		glm::quat qy = glm::angleAxis(glm::radians(-alfa), axis_y);
		glm::quat qx = glm::angleAxis(glm::radians(beta), axis_x);
		glm::quat q1 = qx * initial_postion_c1 * glm::inverse(qx);
		glm::quat q2 = qy * q1 * glm::inverse(qy);
		glm::vec3 vf = { q2.x, q2.y, q2.z };

		const glm::mat4 ChangingViewMatrix =
			glm::lookAt(vf, glm::vec3(0.0f, 0.0f, 0.0f),
				glm::vec3(0.0f, 1.0f, 0.0f));

		Camera->setViewMatrix(ChangingViewMatrix * glm::scale(glm::vec3(1.0f * zoom)));
		if (projection_camera1)
			Camera->setProjectionMatrix(ProjectionMatrix2);
		else
			Camera->setProjectionMatrix(ProjectionMatrix1);

	}
	else {

		glm::quat qy = glm::angleAxis(glm::radians(-alfa2), axis_y);
		glm::quat qx = glm::angleAxis(glm::radians(beta2), axis_x);
		glm::quat q1 = qx * initial_position_c2 * glm::inverse(qx);
		glm::quat q2 = qy * q1 * glm::inverse(qy);
		glm::vec3 vf = { q2.x, q2.y, q2.z };

		const glm::mat4 ChangingViewMatrix =
			glm::lookAt(vf, glm::vec3(0.0f, 0.0f, 0.0f),
				glm::vec3(0.0f, 1.0f, 0.0f));

		Camera2->setViewMatrix(ChangingViewMatrix * glm::scale(glm::vec3(1.0f * zoom2)));
		if (projection_camera2)
			Camera2->setProjectionMatrix(ProjectionMatrix2);
		else
			Camera2->setProjectionMatrix(ProjectionMatrix1);
	}
}


void MyApp::render() {
	Shaders->bind();
	
	for (int i = 0; i < meshes.size(); i++) {

		glUniform3f(Shaders->Uniforms["Color"].index, meshes[i].color.x, meshes[i].color.y, meshes[i].color.z);

		glUniformMatrix4fv(ModelMatrixId, 1, GL_FALSE, glm::value_ptr(meshes[i].transformation));
		meshes[i].Mesh->draw();
	}
	Shaders->unbind();
}


void MyApp::scrollCallback(GLFWwindow * window, double xoffset, double yoffset) {

	if (camera1_on)
		zoom += (float)yoffset * 0.1f;
	else
		zoom2 += (float)yoffset * 0.1f;
}

void MyApp::processMouseMovement(GLFWwindow * win) {

	glfwGetCursorPos(win, &xpos, &ypos);
	double scrollX = 0; double scrollY = 0;

	if (camera1_on) {

		int state = glfwGetMouseButton(win, GLFW_MOUSE_BUTTON_LEFT);

		if (state == GLFW_PRESS) {
			accelaration_x = 0;
			accelaration_y = 0;
			double diffx = xpos - old_xpos;
			double diffy = ypos - old_ypos;

			alfa += (float)diffx * 0.1f;
			accelaration_x += (int)diffx;

			beta -= (float)diffy * 0.1f;
			accelaration_y -= (int)diffy;
		}
		// So we dont overflow it
		if (alfa >= 360.0f || alfa <= -360.0f) {
			alfa = 0.0f;
		}
		if (beta >= 360.0f || beta <= -360.0f) {
			beta = 0.0f;
		}
		old_xpos = xpos;
		old_ypos = ypos;

		alfa += (float)accelaration_x/10;
		beta += (float)accelaration_y/10;

		if (accelaration_x > 0)
			accelaration_x -= 1;
		else if (accelaration_x < 0)
			accelaration_x += 1;

		if (accelaration_y > 0)
			accelaration_y -= 1;
		else if (accelaration_y < 0)
			accelaration_y += 1;
	}
	else {
		int state = glfwGetMouseButton(win, GLFW_MOUSE_BUTTON_LEFT);

		if (state == GLFW_PRESS) {
			accelaration_x2 = 0;
			accelaration_y2 = 0;
			double diffx = xpos - old_xpos;
			double diffy = ypos - old_ypos;

			alfa2 += (float)diffx * 0.1f;
			accelaration_x2 += (int)diffx;
			beta2 -= (float)diffy * 0.1f;
			accelaration_y2 -= (int)diffy;
		}
		// So we dont overflow it
		if (alfa2 >= 360.0f || alfa2 <= -360.0f) {
			alfa2 = 0.0f;
		}
		if (beta2 >= 360.0f || beta2 <= -360.0f) {
			beta2 = 0.0f;
		}
		old_xpos = xpos;
		old_ypos = ypos;

		alfa2 += (float)accelaration_x2/10;
		beta2 += (float)accelaration_y2/10;

		if (accelaration_x2 > 0)
			accelaration_x2 -= 1;
		else if (accelaration_x2 < 0)
			accelaration_x2 += 1;

		if (accelaration_y2 > 0)
			accelaration_y2 -= 1;
		else if (accelaration_y2 < 0)
			accelaration_y2 += 1;
	}
}

////////////////////////////////////////////////////////////////////// CALLBACKS

void MyApp::initCallback(GLFWwindow * win) {
	createMeshes();
	createShaderPrograms(); // after mesh;
	createCamera();

	//SOUND
	/*SoundEngine->play2D("../assets/surrender.mp3", true);
	SoundEngine->setSoundVolume(soundVolume);*/

}

void MyApp::windowSizeCallback(GLFWwindow * win, int winx, int winy) {
	if (winy == 0) {
		winy = 1;
	}

	glViewport(0, 0, winx, winy);
	// change projection matrices to maintain aspect ratio
	float ratio = (float)winx / (float)winy;

	windowWidth = winx;
	windowHeight = winy;

	updateProjMatrices(ratio);

}

void MyApp::displayCallback(GLFWwindow * win, double elapsed) {
	update(win);
	render();
}

void MyApp::windowCloseCallback(GLFWwindow * win) {
	for (int i=0; i < meshes.size(); i++) {
		meshes[i].Mesh->destroyBufferObjects();
	}
}

void MyApp::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {

	if (key == GLFW_KEY_S && action == GLFW_PRESS) {
		snapshot(window, windowWidth, windowHeight);
	}
	if (key == GLFW_KEY_P && action == GLFW_PRESS) {
		if (camera1_on)
			projection_camera1 = !projection_camera1;
		else
			projection_camera2 = !projection_camera2;
	}
	if (key == GLFW_KEY_C && action == GLFW_PRESS) {
		camera1_on = !camera1_on;
		if (camera1_on) {
			Camera->Update(UBO_BP);
		}
		else {
			Camera2->Update(UBO_BP);
		}
	}
	if (key == GLFW_KEY_LEFT && action == GLFW_REPEAT) {
		parametric_movement += param_sensitivity;
		if (parametric_movement >= max_param) {
			parametric_movement = max_param;
		}
	}
	if (key == GLFW_KEY_RIGHT && action == GLFW_REPEAT) {
		parametric_movement -= param_sensitivity;
		if (parametric_movement <= min_param) {
			parametric_movement = min_param;
		}
	}
	
}

/////////////////////////////////////////////////////////////////////////// EXTRA
void MyApp::snapshot(GLFWwindow* win, int width, int height) {

	unsigned char* image;
	image = (unsigned char*)malloc((3 * width * height) * sizeof(char));

	glPixelStorei(GL_PACK_ALIGNMENT, 1);

	glReadBuffer(GL_BACK_LEFT);
	glReadPixels(0, 0, width, height, GL_BGR, GL_UNSIGNED_BYTE, image);

	FIBITMAP* finalImage = FreeImage_ConvertFromRawBits(image, width, height,
		3 * width, 24, 0xFF0000,
		0x00FF00, 0x0000FF, false);

	std::string s = "../Snapshots/snaphot" + std::to_string(snapNum) + ".bmp";
	const char* c = s.c_str();
	std::cout << "Snapshot taken: " << c << std::endl;
	FreeImage_Save(FIF_BMP, finalImage, c, 0);
	snapNum++;
}
/////////////////////////////////////////////////////////////////////////// MAIN

int main(int argc, char* argv[]) {
	windowWidth = 1600;
	windowHeight = 1200;

	mgl::Engine& engine = mgl::Engine::getInstance();
	engine.setApp(new MyApp());
	engine.setOpenGL(4, 6);
	engine.setWindow(windowWidth, windowHeight, "Cartoon Land", 0, 1);
	engine.init();
	engine.run();
	exit(EXIT_SUCCESS);
}

////////////////////////////////////////////////////////////////////////////////
