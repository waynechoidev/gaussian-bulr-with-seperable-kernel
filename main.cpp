#include <iostream>
#include <chrono>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <GL\glew.h>
#include <glm\glm.hpp>

#include "Window.h"
#include "Program.h"
#include "StorageTexture.h"
#include "ComputeProgram.h"
#include "Quad.h"
#include "Texture.h"

int main()
{
	// Create window
	Window mainWindow = Window(1280, 768);
	mainWindow.initialise();

	// Quad to draw
	Quad quad = Quad();
	quad.initialise();	

	// Texture
	Texture texInput = Texture();
	texInput.initialise("img.jpg");
	auto texSize = texInput.getSize();
	
	StorageTexture texTemp = StorageTexture();
	texTemp.initialise(texSize);
	StorageTexture texOutput = StorageTexture();
	texOutput.initialise(texSize);

	// Create programs
	Program drawProgram = Program();
	drawProgram.createFromFiles("vertex.glsl", "fragment.glsl");

	ComputeProgram computeProgramX = ComputeProgram();
	computeProgramX.createFromFile("computeX.glsl");
	ComputeProgram computeProgramY = ComputeProgram();
	computeProgramY.createFromFile("computeY.glsl");

	// Record the start time for measuring GPU operation time
	auto start_time = std::chrono::high_resolution_clock::now();

	// Compute
	computeProgramX.use();
	texInput.use();
	texTemp.useToCompute();
	glDispatchCompute(ceil(texSize.x / 32.0f), ceil(texSize.y / 32.0f), 1);

	computeProgramY.use();
	texTemp.useToDraw();
	texOutput.useToCompute();
	glDispatchCompute(ceil(texSize.x / 32.0f), ceil(texSize.y / 32.0f), 1);

	// Make sure writing to image has finished before read
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	// Wait for the GPU to finish its operation before proceeding
	glFinish();

	// Record the end time for measuring GPU operation time
	auto end_time = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
	std::cout << "Execution time: " << duration.count() << " milliseconds" << std::endl;

	while (!mainWindow.getShouldClose()) {
		// Get + Handle user input events
		glfwPollEvents();

		// Clean window
		mainWindow.clear(0.0f, 0.0f, 0.0f, 1.0f);

		// Screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		drawProgram.use();
		texOutput.useToDraw();
		//texTemp.useToDraw();

		// Draw
		quad.draw();

		mainWindow.swapBuffers();
	}
}