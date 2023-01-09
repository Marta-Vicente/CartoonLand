////////////////////////////////////////////////////////////////////////////////
//
//  Cartoon Land
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

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

////////////////////////////////////////////////////////////////////////// WINDOW

int windowWidth, windowHeight;

////////////////////////////////////////////////////////////////////////// VARIABLES

#define BUILDING_RADIUS 20
#define NUM_TEXTURES 3

int snapNum = 1;
int meshDoor;

glm::mat4 ModelMatrix(1.0f);
const glm::mat4 ChangingModelMatrix = ModelMatrix;
////////////////////////////////////////////////////////////////////////// SOUND

irrklang::ISoundEngine* SoundEngine = irrklang::createIrrKlangDevice();
irrklang::ISoundEngine* SoundEngineDoor = irrklang::createIrrKlangDevice();
const float soundVolume = 0.5f;
////////////////////////////////////////////////////////////////////////// ENUMS

enum ShadingMode {
	phong = 0, silhouette = 1, cel = 2
};

enum DoorState {
	closed, open, toOpen
};

enum TexMode {
	noTexture = 0, buildingTex = 1, doorTex = 2, bumpMap = 3
};
////////////////////////////////////////////////////////////////////////// STRUCTS

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
	TexMode texMode;
};

struct meshVectors {
	std::vector<std::string> meshesNames;
	std::vector<glm::vec3> colors;
	std::vector<glm::mat4> transformations;
	std::vector<ShadingMode> sm;
	std::vector<Material> materials;
	std::vector<TexMode> texMode;
};
////////////////////////////////////////////////////////////////////////// COORDINATES

glm::vec3 sphericalToCartesian(float alpha, float beta, float radius) {
	float x = radius * glm::sin(glm::radians(beta)) * glm::cos(glm::radians(alpha));
	float y = radius * glm::cos(glm::radians(beta));
	float z = radius * glm::sin(glm::radians(beta)) * glm::sin(glm::radians(alpha));

	return { x, y, z };
}
////////////////////////////////////////////////////////////////////////// MYAPP

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

	// COORDINATES AND AREAS -----------------------------------------
	glm::vec3 axis_x = { 1.0f, 0.0f, 0.0f };
	glm::vec3 axis_y = { 0.0f, 1.0f, 0.0f };
	glm::vec3 axis_z = { 0.0f, 0.0f, 1.0f };

	bool inDoorArea = false;
	bool insideBuilding = false;
	DoorState door = closed;

	double xpos, ypos = 0;
	double old_xpos, old_ypos = 0;

	// CAMERAS -------------------------------------------------------
	mgl::Camera* Camera = nullptr;
	GLint ModelMatrixIdCel, ModelMatrixIdPhong;

	//CAMERA TYPES
	float fovy = glm::radians(70.0f);
	glm::mat4 ProjectionMatrixPerspective = glm::perspective(fovy, 4.0f / 3.0f, 1.0f, 100.0f);
	glm::mat4 c1_ChangingViewMatrix;
	glm::mat4 c2_ChangingViewMatrix;

	glm::vec3 cameraPos;
	float lookAtCoordY = 0.f;
	float lookAtCoordYMin = 0.f;
	float lookAtCoordYMax = 12.8f;
	float alfa = 10.0f;
	float beta = 79.0f;
	float radius = 50.f;
	float max_radius = 60.f;
	float insideRadius = 19.5f;
	int accelaration_x = 0;
	int accelaration_y = 0;
	glm::vec3 initPos1 = sphericalToCartesian(alfa, beta, radius);

	//LIGHT -----------------------------------------------------------
	Light light;
	bool lightHand = false;

	//SHADERS ---------------------------------------------------------
	mgl::ShaderProgram* ShaderCel = nullptr;
	mgl::ShaderProgram* ShaderPhong = nullptr;

	//MESHES ---------------------------------------------------------
	meshVectors mv;
	std::vector<Mesh_obj> meshes;
	
	//TEXTURES ---------------------------------------------------------
	GLuint tex[NUM_TEXTURES];

	//MOVEMENT ---------------------------------------------------------
	float parametric_movement_door = 0.0f;
	float parametric_movement_camera = 0.0f; //1.16
	float parametric_movement_camera2 = 0.0f; //1.16
	const float param_sensitivity = 0.01f;
	const float max_param = 1.0f;
	const float min_param = 0.0f;
	bool doorMoving = false;
	bool cameraMoving = false;
	bool cameraMoving2 = false;
	bool cameraMovingOut = false;
	bool cameraMoving2Out = false;

	//FUNCTIONS ---------------------------------------------------------
	//init
	void deleteMeshVectors();
	void createMeshesCel(std::string meshName, glm::vec3 color, glm::mat4 transformation, Material mat);
	void createMeshBuilding(std::string meshName, glm::vec3 color, glm::mat4 transformation, ShadingMode sm, Material mat, TexMode texMode);
	void createMeshSolo(std::string meshName, glm::vec3 color, glm::mat4 transformation, ShadingMode sm, Material mat, TexMode texMode);
	void createMeshes();
	void createShaderPhongProgram();
	void createShaderCelProgram();
	void createShaderPrograms();
	void createCamera();
	void createLight();
	void loadTexture(const std::string& filename, GLuint tex);
	void createTextures();
	//update
	void checkEnterLeave();
	void update(GLFWwindow* win);
	void exitBuilding();
	void processMouseMovement(GLFWwindow* win);
	void openDoor();
	void moveCamera();
	void enterBuilding();
	void sendTexture(GLenum unit, GLuint& tex, GLuint v0, const std::string& filename);
	void sendAllTextures();
	//render
	void render();
	//window size
	void updateProjMatrices(float ratio);
	//others
	void snapshot(GLFWwindow* win, int width, int height);
};



///////////////////////////////////////////////////////////////////////// MESHES

void MyApp::deleteMeshVectors() {
	mv.meshesNames.clear();
	mv.colors.clear();
	mv.transformations.clear();
	mv.sm.clear();
	mv.materials.clear();
	mv.texMode.clear();
}

void MyApp::createMeshesCel(std::string meshName, glm::vec3 color, glm::mat4 transformation, Material mat) {
	mv.meshesNames.push_back(meshName);
	mv.colors.push_back(color);
	mv.transformations.push_back(transformation);
	mv.sm.push_back(cel);
	mv.materials.push_back(mat);
	mv.texMode.push_back(noTexture);

	mv.meshesNames.push_back(meshName);
	mv.colors.push_back({0.1f, 0.1f, 0.1f});
	mv.transformations.push_back(transformation * glm::scale(glm::vec3(1.02f, 1.02f, 1.02f)));
	mv.sm.push_back(silhouette);
	mv.materials.push_back(mat);
	mv.texMode.push_back(noTexture);
}

void MyApp::createMeshSolo(std::string meshName, glm::vec3 color, glm::mat4 transformation, ShadingMode sm, Material mat, TexMode texMode) {
	mv.meshesNames.push_back(meshName);
	mv.colors.push_back(color);
	mv.transformations.push_back(transformation);
	mv.sm.push_back(sm);
	mv.materials.push_back(mat);
	mv.texMode.push_back(texMode);
}

void MyApp::createMeshBuilding(std::string meshName, glm::vec3 color, glm::mat4 transformation, ShadingMode sm, Material mat, TexMode texMode) {
	//EXTERIOR OF THE BUILDING
	mv.meshesNames.push_back(meshName);
	mv.colors.push_back(color);
	mv.transformations.push_back(transformation);
	mv.sm.push_back(sm);
	mv.materials.push_back(mat);
	mv.texMode.push_back(texMode);
	
	//INTERIOR OF THE BUILDING
	mv.meshesNames.push_back(meshName);
	mv.colors.push_back(color);
	mv.transformations.push_back(glm::translate(glm::vec3(0.f, 0.2f, 0.f)) * glm::scale(glm::vec3(0.99f, 0.99f, 0.99f)));
	mv.sm.push_back(silhouette);
	mv.materials.push_back(mat);
	mv.texMode.push_back(noTexture);
}

void MyApp::createMeshes() {

	std::string mesh_dir = "../assets/Meshes/";
	// MESH: meshName, color, transformation, shaderMode, material, texMode
	//MATERIAL: ambientStrength, diffuseStrength, specularStrength, shineness
	
	//EXTERIOR OF THE BUILDING
	createMeshBuilding("pantheonSmooth.obj", {0.9f, 0.9f, 0.9f},
		ModelMatrix,
		phong, { 0.5f, 0.9f, 0.2f, 7.f }, noTexture);
	//--------------------------------------------------------------------------
	
	//GROUND PLANE
	createMeshSolo("ground.obj", { 0.022f, 0.64f, 0.f }, 
		glm::translate(glm::vec3(0.f, -0.1f, 0.f)) * glm::scale(glm::vec3(10.f)), 
		phong, { 0.7f, 0.7f, 0.f, 2.f }, noTexture);
	//--------------------------------------------------------------------------
	
	//LANE
	createMeshSolo("ground.obj", { 0.775f, 0.288f, 0.f }, 
		glm::translate(glm::vec3(60.f, -0.05f, 0.55f)) * glm::scale(glm::vec3(1.f, 1.f, 0.05f)), 
		phong, { 0.7f, 0.7f, 0.1f, 2.f }, bumpMap);
	//--------------------------------------------------------------------------
	
	//DOOR
	createMeshSolo("door.obj", { 0.f, 0.f, 1.f }, 
		glm::translate(glm::vec3(20.2f, 0.f, 2.f)), 
		phong, { 0.5f, 0.9f, 0.8f, 17.f }, doorTex);
	meshDoor = mv.meshesNames.size() - 1;
	//--------------------------------------------------------------------------
	
//////STATUES

	//EGYPTIAN STATUE
	createMeshesCel("/Statues/egypt.obj", {0.7, 0.56, 0.24},
		glm::translate(sphericalToCartesian(30.f, 93.f, 15.f)) * glm::scale(glm::vec3(0.18f)) * glm::rotate(glm::radians(150.f), glm::vec3(0.f, 1.f, 0.f)) * glm::rotate(glm::radians(-90.f), glm::vec3(1.f, 0.f, 0.f)),
		{ 0.5f, 0.9f, 0.1f, 3.f });
	//--------------------------------------------------------------------------

	//ROMAN STATUE
	createMeshesCel("/Statues/roman.obj", {0.9, 0.9, 0.7},
		glm::translate(sphericalToCartesian(90.f, 75.f, 15.f)), 
		{ 0.5f, 0.9f, 0.1f, 3.f });
	//--------------------------------------------------------------------------

	//BRONZE STATUE
	createMeshesCel("/Statues/bronze.obj", { 0., 0.25, 0.086 }, 
		glm::translate(sphericalToCartesian(150.f, 90.f, 15.f)) *  glm::scale(glm::vec3(0.3f)) * glm::rotate(glm::radians(210.f), glm::vec3(0.f, 1.f, 0.f)) * glm::rotate(glm::radians(-90.f), glm::vec3(1.f, 0.f, 0.f)),
		{0.5f, 0.9f, 0.1f, 3.f});
	//--------------------------------------------------------------------------
	
	//THINKER STATUE
	createMeshesCel("/Statues/thinker.obj", { 0.95, 0.89, 0.7 }, 
		glm::translate(sphericalToCartesian(210.f, 88.5f, 15.f)) * glm::rotate(glm::radians(230.f), glm::vec3(0.f, 1.f, 0.f)) * glm::scale(glm::vec3(0.8f)),
		{ 0.5f, 0.9f, 0.1f, 3.f });
	//--------------------------------------------------------------------------
	
	//MARIE THERESE STATUE
	createMeshesCel("/Statues/theresa.obj", {0.2, 0.5, 0.35},
		glm::translate(sphericalToCartesian(270.f, 90.f, 15.f)) * glm::scale(glm::vec3(0.4f)), 
		{ 0.5f, 0.9f, 0.1f, 3.f });
	//--------------------------------------------------------------------------
	
	//MARY STATUE
	createMeshesCel("/Statues/maria.obj", {0.9, 0.9, 0.9},
		glm::translate(sphericalToCartesian(330.f, 89.f, 15.f)) *  glm::scale(glm::vec3(0.2f)) * glm::rotate(glm::radians(30.f), glm::vec3(0.f, 1.f, 0.f)),
		{0.5f, 0.9f, 0.1f, 3.f});
	//--------------------------------------------------------------------------
	

	for (int i = 0; i < mv.meshesNames.size(); i++) {
		Mesh_obj meshSingle;
		meshSingle.Mesh = new mgl::Mesh();
		meshSingle.Mesh->joinIdenticalVertices();
		meshSingle.Mesh->create(mesh_dir + mv.meshesNames[i]);
		meshSingle.color = mv.colors[i];
		meshSingle.transformation = mv.transformations[i];
		meshSingle.shadingMode = mv.sm[i];
		meshSingle.material = mv.materials[i];
		meshSingle.texMode = mv.texMode[i];
		meshes.push_back(meshSingle);
	}
	
	deleteMeshVectors();
	
}


///////////////////////////////////////////////////////////////////////// SHADER

void MyApp::createShaderCelProgram() {
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
	ShaderCel->create();

	ModelMatrixIdCel = ShaderCel->Uniforms[mgl::MODEL_MATRIX].index;
}

void MyApp::createShaderPhongProgram() {
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
	ShaderPhong->addUniform("texMode");
	ShaderPhong->addUniform("tex1");
	ShaderPhong->addUniform("tex2");
	ShaderPhong->addUniform("tex3");
	ShaderPhong->create();

	ModelMatrixIdPhong = ShaderPhong->Uniforms[mgl::MODEL_MATRIX].index;
}

void MyApp::createShaderPrograms() {
	
	createShaderCelProgram();
	createShaderPhongProgram();
	
}

///////////////////////////////////////////////////////////////////////// CAMERA

void MyApp::updateProjMatrices(float ratio) {
	ProjectionMatrixPerspective = glm::perspective(fovy, ratio, 1.0f, 100.0f);
}

void MyApp::createCamera() {

	Camera = new mgl::Camera(UBO_BP);
	Camera->setViewMatrix(glm::lookAt({ initPos1.x,initPos1.y,initPos1.z },
		{ 0.f, lookAtCoordY, 0.f },
		glm::vec3(0.0f, 1.0f, 0.0f)));
	Camera->setProjectionMatrix(ProjectionMatrixPerspective);

	cameraPos = initPos1;

}

void MyApp::createLight() {
	light.lightPos = sphericalToCartesian(40.f, 50.f, 80.f);
	light.lightColor = { 0.9, 0.7, 0.9 };
}

///////////////////////////////////////////////////////////////////////// TEXTURE

void MyApp::loadTexture(const std::string& filename, GLuint tex) {
	int width, height, channels;
	unsigned char* image;

	stbi_set_flip_vertically_on_load(true);
	glBindTexture(GL_TEXTURE_2D, tex);

	image = stbi_load(filename.c_str(), &width, &height, &channels, 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(image);
}

void MyApp::createTextures() {
	glGenTextures(NUM_TEXTURES, tex);

	const std::string texFiles[NUM_TEXTURES] = { "../assets/Textures/marble.jpg",  "../assets/Textures/woodRed.jpg", 
		"../assets/Textures/Bumps/Yes/ground.png" };
	
	for (int i = 0; i < NUM_TEXTURES; i++) {
		loadTexture(texFiles[i], tex[i]);
	}
}

void MyApp::sendTexture(GLenum unit, GLuint& tex, GLuint v0, const std::string& filename) {
	glActiveTexture(unit);
	glBindTexture(GL_TEXTURE_2D, tex);
	//glUniform1i(tex, v0);
	glUniform1i(ShaderPhong->Uniforms[filename].index, v0);
}

void MyApp::sendAllTextures() {
	GLenum g = GL_TEXTURE0;
	for (int i = 0; i < NUM_TEXTURES; i++) {
		const std::string& t = "tex" + std::to_string(i + 1);
		sendTexture(g++, tex[i], i, t);
	}
}

/////////////////////////////////////////////////////////////////////////// UPDATE

void MyApp::openDoor() {
	parametric_movement_door += param_sensitivity;
	if (parametric_movement_door > 1) parametric_movement_door = 1;

	meshes[meshDoor].transformation = glm::translate(glm::vec3(20.2f, 0.f, 2.f)) *
		glm::rotate(glm::radians(-120.f * parametric_movement_door), glm::vec3(0.f, 1.f, 0.f));
}

void MyApp::moveCamera() {
	parametric_movement_camera += param_sensitivity;
	if (parametric_movement_camera > 1) parametric_movement_camera = 1;

	float oldAlfa = alfa;
	alfa = oldAlfa * (1 - parametric_movement_camera) + 1.6f * (parametric_movement_camera);

	float oldLookAtY = lookAtCoordY;
	lookAtCoordY = oldLookAtY * (1 - parametric_movement_camera) + 0 * (parametric_movement_camera);
}

void MyApp::enterBuilding() {
	parametric_movement_camera2 += param_sensitivity;
	if (parametric_movement_camera2 > 1) parametric_movement_camera2 = 1;

	float oldRadius = radius;
	radius = oldRadius * (1 - parametric_movement_camera2) + insideRadius * (parametric_movement_camera2);
}

void MyApp::exitBuilding() {
	parametric_movement_camera2 += param_sensitivity;
	if (parametric_movement_camera2 > 1) parametric_movement_camera2 = 1;

	float oldRadius = radius;
	radius = oldRadius * (1 - parametric_movement_camera2) + 25.f * (parametric_movement_camera2);
}

void MyApp::checkEnterLeave() {
	if (radius <= BUILDING_RADIUS)  insideBuilding = true;
	else insideBuilding = false;

	//OPEN DOOR
	if (parametric_movement_door < 1 && doorMoving)
		openDoor();
	else if (parametric_movement_door >= 1 && doorMoving) {
		doorMoving = false;
		door = open;
		cameraMoving = true;
	}

	//GO TO FRONT OF THE DOOR
	if (parametric_movement_camera < 1 && cameraMoving)
		moveCamera();
	else if (parametric_movement_camera >= 1 && cameraMoving) {
		cameraMoving = false;
		cameraMoving2 = true;
		parametric_movement_camera = 0.f;
	}

	//ENTER BUILDING
	if (parametric_movement_camera2 < 1 && cameraMoving2)
		enterBuilding();
	else if (parametric_movement_camera2 >= 1 && cameraMoving2) {
		cameraMoving2 = false;
		parametric_movement_camera2 = 0.f;
	}

	//GO TO FRONT OF THE DOOR
	if (parametric_movement_camera < 1 && cameraMovingOut) {
		moveCamera();
	}
	else if (parametric_movement_camera >= 1 && cameraMovingOut) {
		cameraMovingOut = false;
		cameraMoving2Out = true;
		parametric_movement_camera = 0.f;
	}

	//LEAVE BUILDING
	if (parametric_movement_camera2 < 1 && cameraMoving2Out) {
		exitBuilding();
	}
	else if (parametric_movement_camera2 >= 1 && cameraMoving2Out) {
		cameraMoving2Out = false;
		parametric_movement_camera2 = 0.f;
	}
}

void MyApp::update(GLFWwindow* win) {
	//INPUT
	processMouseMovement(win);

	checkEnterLeave();

	//CAMERAS
	glm::vec3 newPos = sphericalToCartesian(alfa, beta, radius);
	cameraPos = newPos;

	const glm::mat4 ChangingViewMatrix = glm::lookAt(newPos,
											{ 0.f, lookAtCoordY, 0.f },
											glm::vec3(0.0f, 1.0f, 0.0f));

	Camera->setViewMatrix(ChangingViewMatrix);
	Camera->setProjectionMatrix(ProjectionMatrixPerspective);


	//SOUND
	if (insideBuilding) SoundEngine->setSoundVolume(soundVolume);
	else SoundEngine->setSoundVolume(0);
}

/////////////////////////////////////////////////////////////////////////// DRAW

void MyApp::render() {
	for (int i = 0; i < meshes.size(); i++) {
		if (meshes[i].shadingMode == cel) {
			ShaderCel->bind();
			glUniform3f(ShaderCel->Uniforms[mgl::COLOR_ATTRIBUTE].index, meshes[i].color.x, meshes[i].color.y, meshes[i].color.z);
			glUniformMatrix4fv(ModelMatrixIdCel, 1, GL_FALSE, glm::value_ptr(meshes[i].transformation));
			glUniform3f(ShaderCel->Uniforms["lightPos"].index, light.lightPos.x, light.lightPos.y, light.lightPos.z);
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
			glUniform1i(ShaderPhong->Uniforms["texMode"].index, meshes[i].texMode);
			sendAllTextures();
		}
		
		meshes[i].Mesh->draw();

		if (meshes[i].shadingMode == cel)
			ShaderCel->unbind();
		else if (meshes[i].shadingMode == silhouette) {
			ShaderPhong->unbind();
			glCullFace(GL_BACK);
		}
		else if (meshes[i].shadingMode == phong) {
			glBindTexture(GL_TEXTURE_2D, 0);
			ShaderPhong->unbind();
		}
	}

}

void MyApp::processMouseMovement(GLFWwindow * win) {

	glfwGetCursorPos(win, &xpos, &ypos);

	int state = glfwGetMouseButton(win, GLFW_MOUSE_BUTTON_LEFT);

	if (state == GLFW_PRESS) {
		accelaration_x = 0;
		accelaration_y = 0;
		double diffx = xpos - old_xpos;
		double diffy = ypos - old_ypos;

		alfa += (float)diffx * 0.1f;
		accelaration_x += (int)diffx;

		lookAtCoordY -= (float)diffy * 0.1f;
		accelaration_y -= (int)diffy;
	}
	// So we dont overflow it
	if (alfa >= 360.0f || alfa <= -360.0f) {
		alfa = 0.0f;
	}

	old_xpos = xpos;
	old_ypos = ypos;

	alfa += (float)accelaration_x / 10;
	lookAtCoordY += (float)accelaration_y / 10;

	if (lookAtCoordY < lookAtCoordYMin) lookAtCoordY = lookAtCoordYMin;
	else if (lookAtCoordY > lookAtCoordYMax) lookAtCoordY = lookAtCoordYMax;

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
}

////////////////////////////////////////////////////////////////////// CALLBACKS

void MyApp::initCallback(GLFWwindow * win) {
	createLight();
	createMeshes();
	createShaderPrograms();
	createCamera();
	createTextures();

	//SOUND
	SoundEngine->play2D("../assets/Sound/PianoConcerto5.mp3", true);
	SoundEngine->setSoundVolume(0);

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
	if (key == GLFW_KEY_L && action == GLFW_PRESS) {
		lightHand = !lightHand;
	}
	if (key == GLFW_KEY_O && action == GLFW_PRESS && door == toOpen) {
		SoundEngineDoor->play2D("../assets/Sound/doorOpening.mp3", false);
		doorMoving = true;
		openDoor();
	}
	if (key == GLFW_KEY_O && action == GLFW_PRESS && door == open) {
		cameraMoving = true;
		moveCamera();
	}
	if (key == GLFW_KEY_E && action == GLFW_PRESS && insideBuilding) {
		moveCamera();
		cameraMovingOut = true;
	}
	
}

void MyApp::scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {

	if (insideBuilding) {
		radius = insideRadius;
	}
	else {
		radius -= (float)yoffset * 0.5f;
		if (radius > max_radius) radius = max_radius;

		if (!inDoorArea && radius < BUILDING_RADIUS + 3.f && !insideBuilding) //don't cross wall
			radius = BUILDING_RADIUS + 3.f;

		if (inDoorArea && radius < BUILDING_RADIUS + 3.f && door != open && !insideBuilding) //don't cross closed door
			radius = BUILDING_RADIUS + 3.f;

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
