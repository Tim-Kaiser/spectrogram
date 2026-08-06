#include "../include/rreav_includes.h"
#include "SFML/System/Clock.hpp"
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
  Window window(cfg->getWindowName(), 2560, 1080);
  // window.setWindowedFullscreen();
  ShaderManager shaderManager;
  AudioManager audioManager("resources/audio/kalmia.flac");

  std::unique_ptr<Shader> renderShader = shaderManager.CreateShaders(
      "resources/shaders/main.vert", "resources/shaders/main.frag");

  std::unique_ptr<ComputeShader> spectrogramShader =
      shaderManager.CreateComputeShader("resources/shaders/spectrogram.comp");

  std::unique_ptr<ComputeShader> sortShader =
      shaderManager.CreateComputeShader("resources/shaders/sort.comp");

  Mesh mesh = loadObject("resources/objects/quad.obj");

  GLuint spectrogramTextureID, shiftedSpectrogramTextureID;
  createTexture(&spectrogramTextureID, window.getWidth(), window.getHeight());
  createTexture(&shiftedSpectrogramTextureID, window.getWidth(),
                window.getHeight());

  audioManager.setVolume(7.0f);
  audioManager.play();
  audioManager.bindAudioBuffer();
  audioManager.update(true);

  sf::Clock clock;
  clock.start();

  int width, height;
  int framecount = 0;
  while (window.running()) {
    audioManager.update(true);
    glClear(GL_COLOR_BUFFER_BIT);
    int t = clock.getElapsedTime().asMilliseconds();

    glUseProgram(spectrogramShader->m_shaderProgramID);

    width = window.getWidth();
    height = window.getHeight();
    shaderManager.SendUniformData("u_width", width);
    shaderManager.SendUniformData("u_height", height);
    shaderManager.SendUniformData("u_time", t);
    shaderManager.SendUniformData("u_frame", framecount);

    glBindImageTexture(2, spectrogramTextureID, 0, GL_FALSE, 0, GL_READ_WRITE,
                       GL_RGBA32F);
    glBindImageTexture(3, shiftedSpectrogramTextureID, 0, GL_FALSE, 0,
                       GL_READ_WRITE, GL_RGBA32F);
    glDispatchCompute((GLuint)width, (GLuint)height, 1);
    glMemoryBarrier(GL_ALL_BARRIER_BITS);

    // Copy shifted spectrogram back to original texture
    glCopyImageSubData(shiftedSpectrogramTextureID, GL_TEXTURE_2D, 0, 0, 0, 0,
                       spectrogramTextureID, GL_TEXTURE_2D, 0, 0, 0, 0, width,
                       height, 1);

    glUseProgram(sortShader->m_shaderProgramID);
    shaderManager.SendUniformData("u_width", width);
    shaderManager.SendUniformData("u_height", height);
    shaderManager.SendUniformData("u_time", t);
    shaderManager.SendUniformData("u_frame", framecount);

    glBindImageTexture(2, spectrogramTextureID, 0, GL_FALSE, 0, GL_READ_WRITE,
                       GL_RGBA32F);
    glDispatchCompute((GLuint)width, (GLuint)height, 1);
    glMemoryBarrier(GL_ALL_BARRIER_BITS);

    // Render shader
    glUseProgram(renderShader->m_shaderProgramID);
    glActiveTexture(GL_TEXTURE0);
    shaderManager.SendUniformData("spectrogramTex", 0);
    glBindTexture(GL_TEXTURE_2D, spectrogramTextureID);

    mesh.render();
    window.update();
    window.draw();

    framecount += 1;
  }

  return 0;
}
