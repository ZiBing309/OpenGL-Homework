// HW3.cpp: 定義主控台應用程式的進入點。
//

#include "stdafx.h"
#include"Shader.h"
#include"Camara.h"
#include"Model.h"
#include"Mesh.h"

bool fixedcamara = false;
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_C&& action == GLFW_PRESS)
		fixedcamara=!fixedcamara;
}


int width = 600, height = 600;
GLFWwindow* CreateGLWindow()
{


	///Init
	if (glfwInit() == GL_FALSE)
	{
		exit(EXIT_FAILURE);
	}


	///Auto release when exit
	atexit(glfwTerminate);


	///Set OpenGL version to 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);


	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	///Set window not resizable
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	///Create window and get handle
	GLFWwindow* window = glfwCreateWindow(width, height, u8"賽車", nullptr, nullptr);
	if (window == nullptr)
	{
		cout << "Failed to create GLFW window" << endl;
		exit(EXIT_FAILURE);
	}


	glfwMakeContextCurrent(window);

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		exit(EXIT_FAILURE);
	}
	glEnable(GL_DEPTH_TEST);
	glViewport(0, 0, width, height);

	glfwSetKeyCallback(window, key_callback);
	glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height) {glViewport(0, 0, width, height); });

	return window;

}

pair<vector<Vertex>, vector<GLuint> > CreateTrack(float width, float length, float R)
{
	vector<Vertex> vertex;
	vector<GLuint> index;
	const int detail = 32;
	Vertex v;
	for (int i = 0; i < detail; i++)///down 
	{

		v.Position = vec3(-length / 2 + i * length / detail, -(R + width), 0);
		vertex.push_back(v);

		v.Position = vec3(-length / 2 + i * length / detail, -R, 0);
		vertex.push_back(v);

		index.push_back(i * 2);
		index.push_back(i * 2 + 1);
		index.push_back(i * 2 + 2);

		index.push_back(i * 2 + 1);
		index.push_back(i * 2 + 2);
		index.push_back(i * 2 + 3);
	}
	for (int i = 0; i < detail; i++)///right
	{
		v.Position = vec3(length / 2 + (R + width)*cos(-M_PI / 2 + i * M_PI / detail), (R + width)*sin(-M_PI / 2 + i * M_PI / detail), 0);
		vertex.push_back(v);

		v.Position = vec3(length / 2 + R * cos(-M_PI / 2 + i * M_PI / detail), R * sin(-M_PI / 2 + i * M_PI / detail), 0);
		vertex.push_back(v);

		index.push_back(2 * detail + i * 2);
		index.push_back(2 * detail + i * 2 + 1);
		index.push_back(2 * detail + i * 2 + 2);

		index.push_back(2 * detail + i * 2 + 1);
		index.push_back(2 * detail + i * 2 + 2);
		index.push_back(2 * detail + i * 2 + 3);
	}

	for (int i = 0; i < detail; i++)///up
	{
		v.Position = vec3(length / 2 - i * length / detail, R + width, 0);
		vertex.push_back(v);

		v.Position = vec3(length / 2 - i * length / detail, R, 0);
		vertex.push_back(v);

		index.push_back(4 * detail + i * 2);
		index.push_back(4 * detail + i * 2 + 1);
		index.push_back(4 * detail + i * 2 + 2);

		index.push_back(4 * detail + i * 2 + 1);
		index.push_back(4 * detail + i * 2 + 2);
		index.push_back(4 * detail + i * 2 + 3);
	}

	for (int i = 0; i < detail; i++)///left
	{
		v.Position = vec3(-length / 2 + (R + width)*cos(M_PI / 2 + i * M_PI / detail), (R + width)*sin(M_PI / 2 + i * M_PI / detail),0);
		vertex.push_back(v);
		
		v.Position = vec3(-length / 2 + R * cos(M_PI / 2 + i * M_PI / detail), R * sin(M_PI / 2 + i * M_PI / detail),0);
		vertex.push_back(v);


		if (i < detail - 1)
		{
			index.push_back(6 * detail + i * 2);
			index.push_back(6 * detail + i * 2 + 1);
			index.push_back(6 * detail + i * 2 + 2);

			index.push_back(6 * detail + i * 2 + 1);
			index.push_back(6 * detail + i * 2 + 2);
			index.push_back(6 * detail + i * 2 + 3);
		}
		else
		{
			index.push_back(6 * detail + i * 2);
			index.push_back(6 * detail + i * 2 + 1);
			index.push_back(0);

			index.push_back(6 * detail + i * 2 + 1);
			index.push_back(0);
			index.push_back(1);
		}

	}


	return pair<vector<Vertex>, vector<GLuint> >(vertex, index);
}

int main()
{
	FreeConsole();

	GLFWwindow* window = CreateGLWindow();

	Shader shaderProgram("vertex.vert", "fragment.frag");

	float trackwidth = 30;
	float tracklength = 100;
	float trackradius = 30;

	pair<vector<Vertex>, vector<GLuint> > trackdata = CreateTrack(trackwidth, tracklength, trackradius);

	Mesh track(trackdata.first, trackdata.second);

	
	shaderProgram.Use();

	mat4 projection = perspective(45.0f, (float)width / height, 0.1f, 200.0f);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, value_ptr(projection));


	Camara& camara = Camara::Instance();
	camara.Up(vec3(0.0, 1.0, 0.0));

	Model car(R"(car.obj)");
	vec3 carpos(0,0, trackradius + trackwidth / 2);

	int count = 0;
	float rad = 0;
	float carspeed = 0.01;
	vector<vec3> car_color;
	for (Mesh& m : car.meshes)
	{
		car_color.emplace_back(vec3((float)rand() / RAND_MAX, (float)rand() / RAND_MAX, (float)rand() / RAND_MAX));
	}
	while (!glfwWindowShouldClose(window))
	{
		count++;
		glfwPollEvents();
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		float omega = abs(carpos.x)>=tracklength/2 ? carspeed/(trackradius+trackwidth/2) : 0;
		rad += omega;
		vec3 car_direction(cos(rad), 0, -sin(rad));
		carpos += car_direction * carspeed;

		vec3 camarapos;
		if (fixedcamara)
		{
			camarapos= vec3(0, carpos.y + 50, 0);
		}
		else
		{
			camarapos = vec3(carpos.x, carpos.y + 50, carpos.z);
			camarapos+= car_direction*50.0f;
		}
		camara.Pos(camarapos);
		camara.LookAt(carpos);
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, value_ptr(camara.View()));

		
		

		mat4 carmatrix;
		carmatrix = translate(carmatrix, carpos);
		carmatrix = rotate(carmatrix, rad, vec3(0, 1, 0));
		carmatrix = scale(carmatrix, vec3(5));
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, value_ptr(carmatrix));
		vec3 carcolor = vec3(1.0f, 0.0f, 0.0f);
		//glUniform3fv(glGetUniformLocation(shaderProgram, "color"), 1, value_ptr(carcolor));
		for(int i=0;i<car.meshes.size();i++)//i==3 car wheel
		{
			if (i == 3&& omega!=0)
			{
				mat4 wheelmatrix= rotate(carmatrix, (float)M_PI/32, vec3(0, 1, 0));
				glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, value_ptr(wheelmatrix));
			}
			else
			{
				glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, value_ptr(carmatrix));
			}
			glUniform3fv(glGetUniformLocation(shaderProgram, "color"), 1, value_ptr(car_color[i]));
			car.meshes[i].Draw();
		}
		//car.Draw();


		mat4 model;
		model = rotate(model, radians(90.0f), vec3(1, 0, 0));
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

		vec3 trackcolor = vec3(0.0f, 1.0f, 0.0f);
		glUniform3fv(glGetUniformLocation(shaderProgram, "color"), 1, value_ptr(trackcolor));
		track.Draw();



		glfwSwapBuffers(window);
	}

	return 0;

}