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

#define BUILDING_RADIUS 20
int snapNum = 1;
int meshDoor;
////////////////////////////////////////////////////////////////////////// SOUND

irrklang::ISoundEngine* SoundEngine = irrklang::createIrrKlangDevice();
const float soundVolume = 0.3f;

////////////////////////////////////////////////////////////////////////// MYAPP

glm::mat4 ModelMatrix(1.0f);
const glm::mat4 ChangingModelMatrix = ModelMatrix;

enum ShadingMode {
	phong = 0, silhouette = 1, cel = 2
};

enum DoorState {
	closed, open, toOpen
};

struct Light {
	glm::vec3 lightPos, lightColor;
};

struct Material{
	float ambientStrength, diffuseStrength, specularStrength, shineness;
};

struct Mesh_obj
{
	mgl::Mesh* Mesh = nullptr;
	//mgl::ShaderProgram* Shaders = nullptr;
	glm::vec3 color;
	glm::mat4 transformation;
	ShadingMode shadingMode;
	Material material;
};

glm::vec3 sphericalToCartesian(float alpha, float beta, float radius) {
	float x = radius * glm::sin(glm::radians(beta)) * glm::cos(glm::radians(alpha));
	float y = radius * glm::cos(glm::radians(beta));
	float z = radius * glm::sin(glm::radians(beta)) * glm::sin(glm::radians(alpha));

	return { x, y, z };
}


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

	float minBeta = 80.f;
	float maxBeta = 88.f;
	float min_radius = 0.01f;
	float max_radius = 60.f;
	bool inDoorArea = false;
	DoorState door = closed;

	// CAMERA1
	float alfa = 10.0f;
	float beta = 81.0f;
	float radius = 50.f;
	int accelaration_x = 0;
	int accelaration_y = 0;
	bool projection_camera1 = true;
	glm::vec3 initPos1 = sphericalToCartesian(alfa, beta, radius);

	// CAMERA2
	float alfa2 = 0.0f;
	float beta2 = 0.0f;
	float radius2 = 25.f;
	int accelaration_x2 = 0;
	int accelaration_y2 = 0;
	bool projection_camera2 = true;
	glm::vec3 initPos2 = sphericalToCartesian(alfa2, beta2, radius2);

	bool camera1_on = true;

	//Camera
	mgl::Camera* Camera = nullptr;
	mgl::Camera* Camera2 = nullptr;
	GLint ModelMatrixIdCel, ModelMatrixIdPhong;

	//CAMERA TYPES
	glm::mat4 ProjectionMatrixOrtho =
		glm::ortho(-2.0f, 2.0f, -2.0f, 2.0f, 1.0f, 15.0f);

	glm::mat4 ProjectionMatrixPerspective =
		glm::perspective(glm::radians(70.0f), 4.0f / 3.0f, 1.0f, 100.0f);

	glm::mat4 c1_ChangingViewMatrix;
	glm::mat4 c2_ChangingViewMatrix;

	glm::vec3 cameraPos;
	Light light;
	bool lightHand = false;

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
	void openDoor();
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
	std::vector<Material> materials;	//ambientStrength, diffuseStrength, specularStrength, shineness;

	meshesNames.push_back("pantheon.obj");
	colors.push_back({ 0.9f, 0.5f, 0.1f });
	transformations.push_back(glm::translate(glm::vec3(0.f, 0.1f, 0.f)));
	sm.push_back(phong);
	materials.push_back({ 0.5f, 0.9f, 0.3f, 4.f });
	//--------------------------------------------------------------------------
	
	meshesNames.push_back("pantheon.obj");
	colors.push_back({ 0.9f, 0.5f, 0.1f });
	transformations.push_back(glm::translate(glm::vec3(0.f, 0.1f, 0.f)) * glm::scale(glm::vec3(0.99f, 0.99f, 0.99f)));
	sm.push_back(silhouette);
	materials.push_back({ 0.5f, 0.9f, 0.5f, 7.f });
	//--------------------------------------------------------------------------

	meshesNames.push_back("light2.obj");
	colors.push_back({ 0.9, 0.9, 0.1 });
	transformations.push_back(glm::translate(light.lightPos));
	sm.push_back(phong);
	materials.push_back({ 0.5f, 0.9f, 0.9f, 7.f });
	//--------------------------------------------------------------------------

	meshesNames.push_back("ground.obj");
	colors.push_back({ 0.1f, 0.9f, 0.2f });
	transformations.push_back(glm::scale(glm::vec3(10.f)));
	sm.push_back(phong);
	materials.push_back({ 0.9f, 0.9f, 0.1f, 2.f });
	//--------------------------------------------------------------------------
	meshesNames.push_back("door.obj");
	colors.push_back({ 0.9f, 0.9f, 0.2f });
	transformations.push_back(glm::translate(glm::vec3(20.2f, 0.f, 2.f)));
	sm.push_back(phong);
	materials.push_back({ 0.5f, 0.9f, 0.6f, 7.f });
	meshDoor = meshesNames.size() - 1;
	//--------------------------------------------------------------------------


	for (int i = 0; i < meshesNames.size(); i++) {
		Mesh_obj meshSingle;
		meshSingle.Mesh = new mgl::Mesh();
		meshSingle.Mesh->joinIdenticalVertices();
		meshSingle.Mesh->create(mesh_dir + meshesNames[i]);
		meshSingle.color = colors[i];
		meshSingle.transformation = transformations[i];
		meshSingle.shadingMode = sm[i];
		meshSingle.material = materials[i];
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
	ShaderCel->addUniform("lightPos");
	ShaderCel->addUniform("lightHand");
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
	ShaderPhong->addUniform("lightPos");
	ShaderPhong->addUniform("lightColor");
	ShaderPhong->addUniform("lightHand");
	ShaderPhong->addUniform("camPos");
	ShaderPhong->addUniform("material");
	ShaderPhong->addUniform("silhouetteMode");
	ShaderPhong->create();

	ModelMatrixIdPhong = ShaderPhong->Uniforms[mgl::MODEL_MATRIX].index;
}

///////////////////////////////////////////////////////////////////////// CAMERA

void MyApp::updateProjMatrices(float ratio) {
	ProjectionMatrixOrtho = glm::ortho(-2.f * ratio, 2.f * ratio, -2.0f * ratio, 2.0f* ratio, 1.0f, 15.0f);
	ProjectionMatrixPerspective = glm::perspective(glm::radians(70.0f), ratio, 1.0f, 100.0f);
}

void MyApp::createCamera() {

	Camera2 = new mgl::Camera(UBO_BP);
	Camera = new mgl::Camera(UBO_BP);
	Camera->setViewMatrix(glm::lookAt({ initPos1.x,initPos1.y,initPos1.z },
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f)));
	Camera->setProjectionMatrix(ProjectionMatrixPerspective);

	cameraPos = initPos1;

}

void MyApp::createLight() {
	light.lightPos = {30.f, 40.f, 30.f};
	light.lightColor = { 0.9, 0.7, 0.9 };
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

		glm::vec3 newPos = sphericalToCartesian(alfa, beta, radius);
		cameraPos = newPos;

		const glm::mat4 ChangingViewMatrix = glm::lookAt(newPos,
												glm::vec3(0.0f, 0.0f, 0.0f),
												glm::vec3(0.0f, 1.0f, 0.0f));

		Camera->setViewMatrix(ChangingViewMatrix);
		if (projection_camera1)
			Camera->setProjectionMatrix(ProjectionMatrixPerspective);
		else
			Camera->setProjectionMatrix(ProjectionMatrixOrtho);

	}
	else {

		glm::vec3 newPos = sphericalToCartesian(alfa2, beta2, radius2);
		cameraPos = newPos;

		const glm::mat4 ChangingViewMatrix =
			glm::lookAt(newPos, glm::vec3(0.0f, 0.0f, 0.0f),
				glm::vec3(0.0f, 1.0f, 0.0f));

		Camera2->setViewMatrix(ChangingViewMatrix);
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
			glUniform3f(ShaderCel->Uniforms["lightPos"].index, light.lightPos.x, light.lightPos.y, light.lightPos.z);
			glUniform1i(ShaderCel->Uniforms["lightHand"].index, lightHand);
		}
		else if (meshes[i].shadingMode == silhouette) {
			ShaderPhong->bind();
			glCullFace(GL_FRONT);
			glUniform3f(ShaderPhong->Uniforms[mgl::COLOR_ATTRIBUTE].index, meshes[i].color.x, meshes[i].color.y, meshes[i].color.z);
			glUniformMatrix4fv(ModelMatrixIdPhong, 1, GL_FALSE, glm::value_ptr(meshes[i].transformation));
			glUniform3f(ShaderPhong->Uniforms["lightPos"].index, light.lightPos.x, light.lightPos.y, light.lightPos.z);
			glUniform3f(ShaderPhong->Uniforms["lightColor"].index, light.lightColor.x, light.lightColor.y, light.lightColor.z);
			glUniform3f(ShaderPhong->Uniforms["camPos"].index, cameraPos.x, cameraPos.y, cameraPos.z);
			glUniform4f(ShaderPhong->Uniforms["material"].index, meshes[i].material.ambientStrength, meshes[i].material.diffuseStrength, meshes[i].material.specularStrength, meshes[i].material.shineness);
			glUniform1i(ShaderPhong->Uniforms["lightHand"].index, lightHand);
			glUniform1i(ShaderPhong->Uniforms["silhouetteMode"].index, silhouette);
		}
		else if (meshes[i].shadingMode == phong) {
			ShaderPhong->bind();
			glUniform3f(ShaderPhong->Uniforms[mgl::COLOR_ATTRIBUTE].index, meshes[i].color.x, meshes[i].color.y, meshes[i].color.z);
			glUniformMatrix4fv(ModelMatrixIdPhong, 1, GL_FALSE, glm::value_ptr(meshes[i].transformation));
			glUniform3f(ShaderPhong->Uniforms["lightPos"].index, light.lightPos.x, light.lightPos.y, light.lightPos.z);
			glUniform3f(ShaderPhong->Uniforms["lightColor"].index, light.lightColor.x, light.lightColor.y, light.lightColor.z);
			glUniform3f(ShaderPhong->Uniforms["camPos"].index, cameraPos.x, cameraPos.y, cameraPos.z);
			glUniform4f(ShaderPhong->Uniforms["material"].index, meshes[i].material.ambientStrength, meshes[i].material.diffuseStrength, meshes[i].material.specularStrength, meshes[i].material.shineness);
			glUniform1i(ShaderPhong->Uniforms["lightHand"].index, lightHand);
			glUniform1i(ShaderPhong->Uniforms["silhouetteMode"].index, phong);
		}
		
		meshes[i].Mesh->draw();

		if (meshes[i].shadingMode == cel)
			ShaderCel->unbind();
		else if (meshes[i].shadingMode == silhouette) {
			ShaderPhong->unbind();
			glCullFace(GL_BACK);
		}
		else if (meshes[i].shadingMode == phong) {
			ShaderPhong->unbind();
		}
	}

}


void MyApp::scrollCallback(GLFWwindow * window, double xoffset, double yoffset) {

	if (camera1_on) {
		float oldRadius;
		radius -= (float)yoffset * 0.5f;
		if (radius > max_radius) radius = max_radius;
		if (!inDoorArea && radius < BUILDING_RADIUS + 3.f)
			radius = BUILDING_RADIUS + 3.f;
	}
	else {
		radius2 -= (float)yoffset * 0.5f;
		if (radius2 < min_radius) radius2 = min_radius;
		if (radius2 > max_radius) radius2 = max_radius;
		if (!inDoorArea && radius < BUILDING_RADIUS + 3.f)
			radius = BUILDING_RADIUS + 3.f;
	}
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

		alfa += (float)accelaration_x / 10;
		beta += (float)accelaration_y/10;

		if (-6 < alfa && alfa < 7 && 18 < radius && radius < 30) { 
			inDoorArea = true;
			if (door == closed) door = toOpen;
		}
		else inDoorArea = false;	

		if (accelaration_x > 0)
			accelaration_x -= 1;
		else if (accelaration_x < 0)
			accelaration_x += 1;

		if (accelaration_y > 0)
			accelaration_y -= 1;
		else if (accelaration_y < 0)
			accelaration_y += 1;

		if (radius * glm::cos(glm::radians(beta)) < 1.7f && radius > BUILDING_RADIUS)
			beta = maxBeta;
		if (radius > BUILDING_RADIUS) beta = minBeta;
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

		if (radius2 * glm::cos(glm::radians(beta2)) < 1.7f && radius > BUILDING_RADIUS)
			beta2 = maxBeta;
	}
}

void MyApp::openDoor() {
	meshes[meshDoor].transformation = glm::translate(glm::vec3(20.2f, 0.f, 2.f)) * glm::rotate(glm::radians(-120.f), glm::vec3(0.f, 1.f, 0.f));
	door = open;
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
	if (key == GLFW_KEY_L && action == GLFW_PRESS) {
		lightHand = !lightHand;
	}
	if (key == GLFW_KEY_O && action == GLFW_PRESS && door == toOpen) {
		openDoor();
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
