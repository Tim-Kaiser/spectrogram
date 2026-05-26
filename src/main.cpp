#include "../include/rreav_includes.h"
#include "SFML/System/Clock.hpp"
#include <iostream>
#include <memory>

void createTexture(GLuint *textureID, int width, int height) {
  glGenTextures(1, textureID);
  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, *textureID);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA,
               GL_FLOAT, NULL);
}

int main() {
  //===== INIT =====

  Config *cfg = Config::getInstance();
  cfg->init("resources/config.json");
  Window window(cfg->getWindowName(), 800, 600);
  ShaderManager shaderManager;
  AudioManager audioManager("resources/audio/get_up.flac");

  std::unique_ptr<Shader> renderShader = shaderManager.CreateShaders(
      "resources/shaders/main.vert", "resources/shaders/main.frag");

  std::unique_ptr<ComputeShader> computeShader =
      shaderManager.CreateComputeShader("resources/shaders/spectrogram.comp");

  Mesh mesh = loadObject("resources/objects/quad.obj");

  GLuint spectrogramTextureID;
  createTexture(&spectrogramTextureID, window.getWidth(), window.getHeight());

  audioManager.setVolume(0.2f);
  audioManager.play();
  audioManager.bindAudioBuffer();

  sf::Clock clock;
  clock.start();

  while (window.running()) {
    glClear(GL_COLOR_BUFFER_BIT);
    int t = clock.getElapsedTime().asMilliseconds();

    // Compute shader dispatch
    glUseProgram(computeShader->m_shaderProgramID);
    audioManager.update();

    // without this, SendUniformData reads width and height as
    // glUniformMatrix4fv, fixed in newer RREAV version
    int width, height;
    width = window.getWidth();
    height = window.getHeight();
    shaderManager.SendUniformData("u_width", width);
    shaderManager.SendUniformData("u_height", height);
    shaderManager.SendUniformData("u_time", t);
    glBindImageTexture(2, spectrogramTextureID, 0, GL_FALSE, 0, GL_READ_WRITE,
                       GL_RGBA32F);
    glDispatchCompute((GLuint)width, (GLuint)height, 1);
    glMemoryBarrier(GL_ALL_BARRIER_BITS);

    // Render shader
    GLint texID = 0;

    glUseProgram(renderShader->m_shaderProgramID);
    glActiveTexture(GL_TEXTURE0);
    shaderManager.SendUniformData("u_spectrogramTex", texID);
    glBindTexture(GL_TEXTURE_2D, spectrogramTextureID);

    mesh.render();
    window.update();
    window.draw();
  }

  return 0;
}
