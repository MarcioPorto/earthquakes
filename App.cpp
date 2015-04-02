/*
  Comp 394-01 S15 
  Assignment #3 Earthquakes Display
  Marcio Porto and Charles Park
*/

#include "App.h"
#include "config.h"
#include "EarthquakeDatabase.h"
#include <iostream>
#include <sstream>

const int PLAYBACK_WINDOW = 365 * 24 * 60 * 60;

using namespace std;

App::App(const GApp::Settings& settings) : GApp(settings) {
  renderDevice->setColorClearValue(Color3(0.1, 0.12, 0.15));
  renderDevice->setSwapBuffersAutomatically(true);

  sphereFrame = CoordinateFrame();
}


void App::onInit() {
  // Typical G3D::GApp initialization options
  createDeveloperHUD();
  debugWindow->setVisible(false);
  developerWindow->setVisible(false);
  developerWindow->cameraControlWindow->setVisible(false);
  showRenderingStats = false;
  m_cameraManipulator.reset();

  activeCamera()->setPosition(Vector3(0,0,2));
    activeCamera()->lookAt(Vector3(0,0,0), Vector3(0,1,0));
    activeCamera()->setFarPlaneZ(-100);

	shared_ptr<Light> light = Light::directional("downlight", Vector3(1,1,1), Color3(1.4, 1.3, 1.2));

	earth.reset(new Earth(activeCamera(), light));

  // Initialization for this Application
  // Note:  DATA_PATH is set in config.h
  eqd = EarthquakeDatabase(DATA_PATH);
  playbackScale = 86400;
  currentTime = eqd.getByIndex(eqd.getMinIndex()).getDate().asSeconds();
  font = GFont::fromFile("$G3D10DATA/font/arial.fnt");
  rotation = Matrix3::identity();
  playing = true;
}


void App::onUserInput(UserInput *ui) {
  // Speedup or slowdown the playback
  if (ui->keyDown(GKey::LEFT)) {
    playbackScale /= 1.3;
  }
  if (ui->keyDown(GKey::RIGHT)) {
    playbackScale *= 1.3;
  }

  // Dolly the camera closer or farther away from the earth
  if (ui->keyDown(GKey::UP)) {
      Vector3 newCamPos = activeCamera()->frame().translation + Vector3(0,0,-0.01);
      if (newCamPos[2] > 1.2) {
        activeCamera()->setPosition(newCamPos);
      }
  }
  if (ui->keyDown(GKey::DOWN)) {
      Vector3 newCamPos = activeCamera()->frame().translation + Vector3(0,0,0.01);
      activeCamera()->setPosition(newCamPos);
  }

  // Rotate the earth when the user clicks and drags the mouse
  if (ui->keyDown(GKey::LEFT_MOUSE)) {
    // TODO: Update the "rotation" matrix based on how the user has dragged the mouse
    // Note: You can get mouse movement since the last frame with:  Vector2 dxy = ui->mouseDXY();
    Vector2 dxy = ui->mouseDXY();
    Vector3 movement = Vector3(-1*dxy[1],-1*dxy[0],0);
    if(!movement.isZero()){
      rotation = Matrix3::fromAxisAngle(movement,-1*movement.magnitude()/500)*rotation;
    }
  }
}


void App::onSimulation(RealTime rdt, SimTime sdt, SimTime idt) {
  GApp::onSimulation(rdt, sdt, idt);

  // Advance the current time and loop back to the start if time is past the last earthquake
  currentTime += playbackScale * rdt;
  if (currentTime > eqd.getByIndex(eqd.getMaxIndex()).getDate().asSeconds()) {
    currentTime = eqd.getByIndex(eqd.getMinIndex()).getDate().asSeconds();
  }
  if (currentTime < eqd.getByIndex(eqd.getMinIndex()).getDate().asSeconds()) {
    currentTime = eqd.getByIndex(eqd.getMaxIndex()).getDate().asSeconds();
  }
}



void App::onGraphics3D(RenderDevice* rd, Array<shared_ptr<Surface> >& surface3D) {
  rd->clear();
  rd->setShadeMode(RenderDevice::SHADE_SMOOTH);


  // Set a rotation matrix to apply when drawing the earth and earthquakes
  rd->pushState();
  rd->setObjectToWorldMatrix(CoordinateFrame(rotation, Vector3(0,0,0)));

  // TODO: Draw the earth by implementing the draw function inside the Earth class
  earth->draw(rd);

  // Draw earthquakes
  int start = eqd.getIndexByDate(Date(currentTime - PLAYBACK_WINDOW));
  int end = eqd.getIndexByDate(Date(currentTime));

  int counter1 = 0;
  int counter2 = 0;
  int counter3 = 0;

  for (int x=start; x<end; x++) {
    Earthquake e = eqd.getByIndex(x);
    // TODO: Draw earthquake e
    double lat = e.getLatitude();
    double lon = e.getLongitude();
    
    Vector3 position = earth->getPosition(lat, lon);
    double magnitude = e.getMagnitude();
    
    if(magnitude < 6.5){
      Sphere ball(position, .01);
      Draw::sphere(ball, rd, Color3::yellow(), Color4::clear());
      counter1++;
    }
    else if(magnitude >= 6.5 && magnitude < 7.5){
      Sphere ball(position, .02);
      Draw::sphere(ball, rd, Color3::orange(), Color4::clear());
      counter2++;
    }
    else{
      Sphere ball(position, .03);
      Draw::sphere(ball, rd, Color3::red(), Color4::clear());
      counter3++;
    }

  }

  rd->popState();

  // Call to make the GApp show the output of debugDraw
  drawDebugShapes();
}


void App::onGraphics2D(RenderDevice* rd, Array<Surface2D::Ref>& posed2D) {
  Surface2D::sortAndRender(rd, posed2D);
  Date d(currentTime);
  stringstream s;
  s << "Current date: " << d.getMonth()
    << "/" << d.getDay()
    << "/" << d.getYear()
    << "  " << d.getHour()
    << ":" << d.getMinute();
  const G3D::String g3dString = s.str().c_str();
  font->draw2D(rd, g3dString, Point2(30,30), 12, Color3::white());
}




