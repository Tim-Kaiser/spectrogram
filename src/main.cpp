#include "../include/rreav_includes.h"
#include "SFML/System/Clock.hpp"

int main() {
  //===== INIT =====

  Config *cfg = Config::getInstance();
  cfg->init("resources/config.json");
  Window window(cfg->getWindowName(), 800, 600);
  ShaderManager shaderManager;
  AudioManager audioManager("resources/audio/sine_wave_1000hz_44.1sr.wav");

  std::unique_ptr<Shader> renderShader = shaderManager.CreateShaders(
      "resources/shaders/main.vert", "resources/shaders/main.frag");
  glUseProgram(renderShader->m_shaderProgramID);

  Mesh mesh = loadObject("resources/objects/quad.obj");

  audioManager.setVolume(0.2f);
  audioManager.play();
  audioManager.bindAudioBuffer();

  sf::Clock clock;
  clock.start();
  while (window.running()) {
    int t = clock.getElapsedTime().asMilliseconds();
    shaderManager.SendUniformData("u_time", t);
    audioManager.update();

    mesh.render();
    window.update();
    window.draw();
  }

  return 0;
}
