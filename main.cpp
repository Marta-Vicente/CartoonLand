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

////////////////////////////////////////////////////////////////////////// VARIABLES

int snapNum = 1;
#define BUILDING_RADIUS 20
////////////////////////////////////////////////////////////////////////// SOUND

irrklang::ISoundEngine* SoundEngine = irrklang::createIrrKlangDevice();
const float soundVolume = 0.3f;

////////////////////////////////////////////////////////////////////////// MYAPP

glm::mat4 ModelMatrix(1.0f);
const glm::mat4 ChangingModelMatrix = ModelMatrix;

enum ShadingMode {
	cel, phong, silhouette
};

struct Mesh_obj
{
	mgl::Mesh* Mesh = nullptr;
	//mgl::ShaderProgram* Shaders = nullptr;
	glm::vec3 color;
	glm::mat4 transformation;
	ShadingMode shadingMode;
};


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
	glm::quat initial_postion_c1 = { 3.0f, 10.f, 5.422f, 6.928f };
	float alfa = 0.0f;
	float beta = 0.0f;
	int accelaration_x = 0;
	int accelaration_y = 0;
	float zoom = 0.1f;
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

	bool camera1_on = true;

	//Camera
	mgl::Camera* Camera = nullptr;
	mgl::Camera* Camera2 = nullptr;
	GLint ModelMatrixIdCel, ModelMatrixIdPhong;

	//CAMERA TYPES
	// Orthographic LeftRight(-2,2) BottomTop(-2,2) NearFar(1,10)
	glm::mat4 ProjectionMatrixOrtho =
		glm::ortho(-2.0f, 2.0f, -2.0f, 2.0f, 1.0f, 15.0f);

	// Perspective Fovy(30) Aspect(640/480) NearZ(1) FarZ(30)
	glm::mat4 ProjectionMatrixPerspective =
		glm::perspective(glm::radians(30.0f), 4.0f / 3.0f, 1.0f, 30.0f);

	glm::mat4 c1_ChangingViewMatrix;
	glm::mat4 c2_ChangingViewMatrix;

	glm::vec3 cameraPos;
	glm::vec3 light;

	double xpos, ypos = 0;
	double old_xpos, old_ypos = 0;

	mgl::ShaderProgram* ShaderCel = nullptr;
	mgl::ShaderProgram* ShaderPhong = nullptr;

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
	void createLight();
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
	std::vector<ShadingMode> sm;


	meshesNames.push_back("pantheon.obj");
	colors.push_back({ 0.9f, 0.5f, 0.1f });
	transformations.push_back(ModelMatrix);
	sm.push_back(phong);
	//--------------------------------------------------------------------------
	
	meshesNames.push_back("pantheon.obj");
	colors.push_back({ 0.1f, 0.1f, 0.1f });
	transformations.push_back(glm::scale(glm::vec3(1.01f, 1.01f, 1.01f)));
	sm.push_back(silhouette);
	//--------------------------------------------------------------------------

	meshesNames.push_back("lightBall.obj");
	colors.push_back({ 0.9, 0.9, 0.1 });
	transformations.push_back(glm::translate(light));
	sm.push_back(phong);
	//--------------------------------------------------------------------------

	meshesNames.push_back("ground.obj");
	colors.push_back({ 0.1f, 0.9f, 0.2f });
	transformations.push_back(glm::scale(glm::vec3(10.f)));
	sm.push_back(phong);
	//--------------------------------------------------------------------------
	meshesNames.push_back("door.obj");
	colors.push_back({ 0.9f, 0.9f, 0.2f });
	transformations.push_back(glm::translate(glm::vec3(20.2f, 0.f, 2.f)));
	sm.push_back(phong);
	//--------------------------------------------------------------------------


	for (int i = 0; i < meshesNames.size(); i++) {
		Mesh_obj meshSingle;
		meshSingle.Mesh = new mgl::Mesh();
		meshSingle.Mesh->joinIdenticalVertices();
		meshSingle.Mesh->create(mesh_dir + meshesNames[i]);
		meshSingle.color = colors[i];
		meshSingle.transformation = transformations[i];
		meshSingle.shadingMode = sm[i];
		meshes.push_back(meshSingle);
	}
	//updateTransformationMatrices();

}


///////////////////////////////////////////////////////////////////////// SHADER

void MyApp::createShaderPrograms() {
	ShaderCel = new mgl::ShaderProgram();
	ShaderCel->addShader(GL_VERTEX_SHADER, "cel-vs.glsl");
	ShaderCel->addShader(GL_FRAGMENT_SHADER, "cel-fs.glsl");

	ShaderCel->addAttribute(mgl::POSITION_ATTRIBUTE, mgl::Mesh::POSITION);
	if (meshes[0].Mesh->hasNormals()) {
		ShaderCel->addAttribute(mgl::NORMAL_ATTRIBUTE, mgl::Mesh::NORMAL);
	}
	if (meshes[0].Mesh->hasTexcoords()) {
		ShaderCel->addAttribute(mgl::TEXCOORD_ATTRIBUTE, mgl::Mesh::TEXCOORD);
	}
	if (meshes[0].Mesh->hasTangentsAndBitangents()) {
		ShaderCel->addAttribute(mgl::TANGENT_ATTRIBUTE, mgl::Mesh::TANGENT);
	}

	ShaderCel->addUniform(mgl::MODEL_MATRIX);
	ShaderCel->addUniformBlock(mgl::CAMERA_BLOCK, UBO_BP);
	ShaderCel->addUniform(mgl::COLOR_ATTRIBUTE);
	ShaderCel->addUniform("Light");
	ShaderCel->addUniform("camPos");
	ShaderCel->create();

	ModelMatrixIdCel = ShaderCel->Uniforms[mgl::MODEL_MATRIX].index;
	//----------------------------------------------------------------------------
	ShaderPhong = new mgl::ShaderProgram();
	ShaderPhong->addShader(GL_VERTEX_SHADER, "phong-vs.glsl");
	ShaderPhong->addShader(GL_FRAGMENT_SHADER, "phong-fs.glsl");

	ShaderPhong->addAttribute(mgl::POSITION_ATTRIBUTE, mgl::Mesh::POSITION);
	if (meshes[0].Mesh->hasNormals()) {
		ShaderPhong->addAttribute(mgl::NORMAL_ATTRIBUTE, mgl::Mesh::NORMAL);
	}
	if (meshes[0].Mesh->hasTexcoords()) {
		ShaderPhong->addAttribute(mgl::TEXCOORD_ATTRIBUTE, mgl::Mesh::TEXCOORD);
	}
	if (meshes[0].Mesh->hasTangentsAndBitangents()) {
		ShaderPhong->addAttribute(mgl::TANGENT_ATTRIBUTE, mgl::Mesh::TANGENT);
	}

	ShaderPhong->addUniform(mgl::MODEL_MATRIX);
	ShaderPhong->addUniformBlock(mgl::CAMERA_BLOCK, UBO_BP);
	ShaderPhong->addUniform(mgl::COLOR_ATTRIBUTE);
	ShaderPhong->addUniform("Light");
	ShaderPhong->addUniform("camPos");
	ShaderPhong->create();

	ModelMatrixIdPhong = ShaderPhong->Uniforms[mgl::MODEL_MATRIX].index;
}

///////////////////////////////////////////////////////////////////////// CAMERA

void MyApp::updateProjMatrices(float ratio) {
	ProjectionMatrixOrtho = glm::ortho(-2.f * ratio, 2.f * ratio, -2.0f * ratio, 2.0f* ratio, 1.0f, 15.0f);
	ProjectionMatrixPerspective = glm::perspective(glm::radians(30.0f), ratio, 1.0f, 30.0f);
}

void MyApp::createCamera() {

	Camera2 = new mgl::Camera(UBO_BP);
	Camera = new mgl::Camera(UBO_BP);
	Camera->setViewMatrix(glm::lookAt({ initial_postion_c1.x,initial_postion_c1.y,initial_postion_c1.z },
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f)));
	Camera->setProjectionMatrix(ProjectionMatrixPerspective);

	//cameraPos = { 30.f, 5.f, 30.f };
	cameraPos = { initial_postion_c1.x * zoom * BUILDING_RADIUS , initial_postion_c1.y * zoom * BUILDING_RADIUS, initial_postion_c1.z * zoom * BUILDING_RADIUS };

}

void MyApp::createLight() {
	light = {30.f, 40.f, 30.f};

	/*GLuint UboId;
	glGenBuffers(1, &UboId);
	glBindBuffer(GL_UNIFORM_BUFFER, UboId);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::vec3) * 2, 0, GL_STREAM_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, LIGHT_BP, UboId);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);*/
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

	/*for (int i = 0; i < TRANSF_MESHES; i++) {
		meshes[i].transformation = tTemp[i];
	}*/
}

void MyApp::update(GLFWwindow* win) {
	//INPUT
	processMouseMovement(win);

	//CAMERAS
	if (camera1_on) {

		glm::quat qy = glm::angleAxis(glm::radians(-alfa), axis_y);
		glm::quat qx = glm::angleAxis(glm::radians(beta), axis_x);
		glm::quat q1 = qx * initial_postion_c1 * glm::inverse(qx);
		glm::quat q2 = qy * q1 * glm::inverse(qy);
		glm::vec3 vf = { q2.x, q2.y, q2.z };

		cameraPos = { vf.x * zoom * BUILDING_RADIUS , vf.y * zoom * BUILDING_RADIUS, vf.z * zoom * BUILDING_RADIUS };

		const glm::mat4 ChangingViewMatrix = glm::lookAt(vf, 
												glm::vec3(0.0f, 0.0f, 0.0f),
												glm::vec3(0.0f, 1.0f, 0.0f));

		Camera->setViewMatrix(ChangingViewMatrix * glm::scale(glm::vec3(1.0f * zoom)));
		if (projection_camera1)
			Camera->setProjectionMatrix(ProjectionMatrixPerspective);
		else
			Camera->setProjectionMatrix(ProjectionMatrixOrtho);

	}
	else {

		glm::quat qy = glm::angleAxis(glm::radians(-alfa2), axis_y);
		glm::quat qx = glm::angleAxis(glm::radians(beta2), axis_x);
		glm::quat q1 = qx * initial_position_c2 * glm::inverse(qx);
		glm::quat q2 = qy * q1 * glm::inverse(qy);
		glm::vec3 vf = { q2.x, q2.y, q2.z };

		cameraPos = { vf.x * zoom * BUILDING_RADIUS , vf.y * zoom * BUILDING_RADIUS, vf.z * zoom * BUILDING_RADIUS };

		const glm::mat4 ChangingViewMatrix =
			glm::lookAt(vf, glm::vec3(0.0f, 0.0f, 0.0f),
				glm::vec3(0.0f, 1.0f, 0.0f));

		Camera2->setViewMatrix(ChangingViewMatrix * glm::scale(glm::vec3(1.0f * zoom2)));
		if (projection_camera2)
			Camera2->setProjectionMatrix(ProjectionMatrixPerspective);
		else
			Camera2->setProjectionMatrix(ProjectionMatrixOrtho);
	}
}


void MyApp::render() {

	/*ShaderCel->bind();
	for (int i = 0; i < meshes.size(); i++) {
		glUniform3f(ShaderCel->Uniforms[mgl::COLOR_ATTRIBUTE].index, meshes[i].color.x, meshes[i].color.y, meshes[i].color.z);
		glUniformMatrix4fv(ModelMatrixIdCel, 1, GL_FALSE, glm::value_ptr(meshes[i].transformation));
		meshes[i].Mesh->draw();

	}
	ShaderCel->unbind();*/

	for (int i = 0; i < meshes.size(); i++) {
		if (meshes[i].shadingMode == cel) {
			ShaderCel->bind();
			glUniform3f(ShaderCel->Uniforms[mgl::COLOR_ATTRIBUTE].index, meshes[i].color.x, meshes[i].color.y, meshes[i].color.z);
			glUniformMatrix4fv(ModelMatrixIdCel, 1, GL_FALSE, glm::value_ptr(meshes[i].transformation));
			glUniform3f(ShaderCel->Uniforms["Light"].index, light.x, light.y, light.z);
			glUniform3f(ShaderCel->Uniforms["camPos"].index, cameraPos.x, cameraPos.y, cameraPos.z);
		}
		else if (meshes[i].shadingMode == silhouette) {
			ShaderPhong->bind();
			glCullFace(GL_FRONT);
			glUniform3f(ShaderPhong->Uniforms[mgl::COLOR_ATTRIBUTE].index, meshes[i].color.x, meshes[i].color.y, meshes[i].color.z);
			glUniformMatrix4fv(ModelMatrixIdPhong, 1, GL_FALSE, glm::value_ptr(meshes[i].transformation));
			glUniform3f(ShaderPhong->Uniforms["Light"].index, light.x, light.y, light.z);
			glUniform3f(ShaderPhong->Uniforms["camPos"].index, cameraPos.x, cameraPos.y, cameraPos.z);
		}
		else if (meshes[i].shadingMode == phong) {
			ShaderPhong->bind();
			glUniform3f(ShaderPhong->Uniforms[mgl::COLOR_ATTRIBUTE].index, meshes[i].color.x, meshes[i].color.y, meshes[i].color.z);
			glUniformMatrix4fv(ModelMatrixIdPhong, 1, GL_FALSE, glm::value_ptr(meshes[i].transformation));
			glUniform3f(ShaderPhong->Uniforms["Light"].index, light.x, light.y, light.z);
			glUniform3f(ShaderPhong->Uniforms["camPos"].index, cameraPos.x, cameraPos.y, cameraPos.z);
		}

		meshes[i].Mesh->draw();

		if (meshes[i].shadingMode == cel)
			ShaderCel->unbind();
		else if (meshes[i].shadingMode == silhouette) {
			ShaderPhong->unbind();
			glCullFace(GL_BACK);
		}
		else if (meshes[i].shadingMode == phong)
			ShaderPhong->unbind();
	}

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
	createLight();
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
		//updateTransformationMatrices();
	}
	if (key == GLFW_KEY_RIGHT && action == GLFW_REPEAT) {
		parametric_movement -= param_sensitivity;
		if (parametric_movement <= min_param) {
			parametric_movement = min_param;
		}
		//updateTransformationMatrices();
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
