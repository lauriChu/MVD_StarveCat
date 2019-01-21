//
//  Game.cpp
//
//  Copyright � 2018 Alun Evans. All rights reserved.
//

#include "Game.h"
#include "Shader.h"
#include "extern.h"
#include "Parsers.h"
#include "MovePlatformScript.h"
#include "CatMovementScript.h"
#include "FoodScript.h"
#include <functional>

Game::Game() {

}

int createFree(float aspect, ControlSystem& sys) {
	int ent_player = ECS.createEntity("Player");
	Camera& player_cam = ECS.createComponentForEntity<Camera>(ent_player);
	ECS.getComponentFromEntity<Transform>(ent_player).translate(0.0f, 3.0f, 5.0f);
	player_cam.position = lm::vec3(0.0f, 3.0f, 5.0f);
	player_cam.forward = lm::vec3(0.0f, 0.0f, 1.0f);
	player_cam.setPerspective(60.0f*DEG2RAD, aspect, 0.1f, 10000.0f);

	ECS.main_camera = ECS.getComponentID<Camera>(ent_player);

	sys.control_type = ControlTypeFree;

	return ent_player;
}

int createPlayer(float aspect, ControlSystem& sys) {
    int ent_player = ECS.createEntity("PlayerFPS");
    Camera& player_cam = ECS.createComponentForEntity<Camera>(ent_player);
    ECS.getComponentFromEntity<Transform>(ent_player).translate(0.0f, 20.0f, 45.0f);
    player_cam.position = lm::vec3(0.0f, 3.0f, 5.0f);
    player_cam.forward = lm::vec3(0.0f, 0.0f, -1.0f);
    player_cam.setPerspective(60.0f*DEG2RAD, aspect, 0.01f, 10000.0f);
    
    //FPS colliders
    int ent_down_ray = ECS.createEntity("Down Ray");
    Transform& down_ray_trans = ECS.getComponentFromEntity<Transform>(ent_down_ray);
    down_ray_trans.parent = ECS.getComponentID<Transform>(ent_player); //set parent as player entity *transform*!
    Collider& down_ray_collider = ECS.createComponentForEntity<Collider>(ent_down_ray);
    down_ray_collider.collider_type = ColliderTypeRay;
    down_ray_collider.direction = lm::vec3(0.0,-1.0,0.0);
    down_ray_collider.max_distance = 100.0f;
    
    int ent_left_ray = ECS.createEntity("Left Ray");
    Transform& left_ray_trans = ECS.getComponentFromEntity<Transform>(ent_left_ray);
    left_ray_trans.parent = ECS.getComponentID<Transform>(ent_player); //set parent as player entity *transform*!
    Collider& left_ray_collider = ECS.createComponentForEntity<Collider>(ent_left_ray);
    left_ray_collider.collider_type = ColliderTypeRay;
    left_ray_collider.direction = lm::vec3(-1.0,0.0,0.0);
    left_ray_collider.max_distance = 1.0f;
    
    int ent_right_ray = ECS.createEntity("Right Ray");
    Transform& right_ray_trans = ECS.getComponentFromEntity<Transform>(ent_right_ray);
    right_ray_trans.parent = ECS.getComponentID<Transform>(ent_player); //set parent as player entity *transform*!
    Collider& right_ray_collider = ECS.createComponentForEntity<Collider>(ent_right_ray);
    right_ray_collider.collider_type = ColliderTypeRay;
    right_ray_collider.direction = lm::vec3(1.0,0.0,0.0);
    right_ray_collider.max_distance = 1.0f;
    
    int ent_forward_ray = ECS.createEntity("Forward Ray");
    Transform& forward_ray_trans = ECS.getComponentFromEntity<Transform>(ent_forward_ray);
    forward_ray_trans.parent = ECS.getComponentID<Transform>(ent_player); //set parent as player entity *transform*!
    Collider& forward_ray_collider = ECS.createComponentForEntity<Collider>(ent_forward_ray);
    forward_ray_collider.collider_type = ColliderTypeRay;
    forward_ray_collider.direction = lm::vec3(0.0,0.0,-1.0);
    forward_ray_collider.max_distance = 1.0f;
    
    int ent_back_ray = ECS.createEntity("Back Ray");
    Transform& back_ray_trans = ECS.getComponentFromEntity<Transform>(ent_back_ray);
    back_ray_trans.parent = ECS.getComponentID<Transform>(ent_player); //set parent as player entity *transform*!
    Collider& back_ray_collider = ECS.createComponentForEntity<Collider>(ent_back_ray);
    back_ray_collider.collider_type = ColliderTypeRay;
    back_ray_collider.direction = lm::vec3(0.0,0.0,1.0);
    back_ray_collider.max_distance = 1.0f;
    
    //set all colliders for FPS control
	sys.FPS_collider_down = ECS.getComponentID<Collider>(ent_down_ray);
	sys.FPS_collider_left = ECS.getComponentID<Collider>(ent_left_ray);
	sys.FPS_collider_right = ECS.getComponentID<Collider>(ent_right_ray);
	sys.FPS_collider_forward = ECS.getComponentID<Collider>(ent_forward_ray);
	sys.FPS_collider_back = ECS.getComponentID<Collider>(ent_back_ray);
    
	ECS.main_camera = ECS.getComponentID<Camera>(ent_player);

	sys.control_type = ControlTypeFPS;

    return ent_player;
}

int createAI(ScriptSystem& script_system_) {
	// Get Cat Entity
	int ent_cat = ECS.getEntity("cat");

	// Add down raycast collider 
	int ent_down_ray = ECS.createEntity("Down Ray Cat");
	Transform& down_ray_trans = ECS.getComponentFromEntity<Transform>(ent_down_ray);
	down_ray_trans.parent = ECS.getComponentID<Transform>(ent_cat); //set parent as player entity *transform*!
	Collider& down_ray_collider = ECS.createComponentForEntity<Collider>(ent_down_ray);
	down_ray_collider.collider_type = ColliderTypeRay;
	down_ray_collider.direction = lm::vec3(0.0, -1.0, 0.0);
	down_ray_collider.max_distance = 100.0f;

	int ent_forward_ray = ECS.createEntity("Forward Ray Cat");
	Transform& forward_ray_trans = ECS.getComponentFromEntity<Transform>(ent_forward_ray);
	forward_ray_trans.parent = ECS.getComponentID<Transform>(ent_cat); //set parent as player entity *transform*!
	Collider& forward_ray_collider = ECS.createComponentForEntity<Collider>(ent_forward_ray);
	forward_ray_collider.collider_type = ColliderTypeRay;
	forward_ray_collider.direction = lm::vec3(0.0, 0.0, 1.0);
	forward_ray_collider.max_distance = 5.0f;

	int ent_box_collider = ECS.createEntity("Box Cat");
	Transform& collider_trans = ECS.createComponentForEntity<Transform>(ent_box_collider);
	collider_trans.parent = ECS.getComponentID<Transform>(ent_cat);
	Collider& collider = ECS.createComponentForEntity<Collider>(ent_box_collider);
	collider.collider_type = ColliderTypeBox;
	collider.local_center = lm::vec3(0, 0, 0);
	collider.local_halfwidth = lm::vec3(1, 1, 1);

	// Add script for Cat movement
	CatMovementScript* move_plaform_script = new CatMovementScript(ECS.getEntity("cat"));
	script_system_.registerScript(move_plaform_script);

	return ent_cat;
}
	
void printHello() {
	print("hello");
}

//Nothing here yet
void Game::init() {

	//******* INIT SYSTEMS *******

	//init systems except debug, which needs info about scene
	control_system_.init();
	graphics_system_.init(window_width_, window_height_);
	script_system_.init(&control_system_),
	gui_system_.init(window_width_, window_height_);

    //******** RESOURCES AND LEVEL **********
    
	//level + lights
    Parsers::parseJSONLevel("data/assets/level4.json", graphics_system_);

	//createFree((float)window_width_ / (float)window_height_, control_system_);
    
	// player and camera
	createPlayer((float)window_width_ / (float)window_height_, control_system_);
	
	// Create AI
	createAI(script_system_);

	// Create FoodScript
	FoodScript* food_script = new FoodScript(ECS.createEntity("foodies"));
	food_script->setGraphics(&graphics_system_);
	script_system_.registerScript(food_script);

	// Create MovePlatformScript
	MovePlatformScript* move_platform_script = new MovePlatformScript(ECS.getEntity("door"));
	move_platform_script->init();
	script_system_.registerScript(move_platform_script);

	// Create GUI
	/* LOAD A GUI IMAGE */
	/*int gui_image = ECS.createEntity("gui image");
	GUIElement& eli = ECS.createComponentForEntity<GUIElement>(gui_image);
	eli.texture = Parsers::parseTexture("data/assets/gui_test.tga");
	eli.anchor = GUIAnchorTopRight;
	eli.offset = lm::vec2(-10, -10);
	//eli.onClick = std::bind(&MovePlatformScript::toggleMove, move_plaform_script);
	int gui_image_apple = ECS.createEntity("gui image apple");
	GUIElement& eli1 = ECS.createComponentForEntity<GUIElement>(gui_image);
	eli1.texture = Parsers::parseTexture("data/assets/manzana.tga");
	eli1.anchor = GUIAnchorTopLeft;
	eli1.offset = lm::vec2(-10, -10);
	eli1.height = 20;
	eli1.width = 20;

	int gui_image_apple2 = ECS.createEntity("gui image apple 2");
	GUIElement& eli2 = ECS.createComponentForEntity<GUIElement>(gui_image);
	eli2.texture = Parsers::parseTexture("data/assets/manzana.tga");
	eli2.anchor = GUIAnchorCenterLeft;
	eli2.offset = lm::vec2(-10, -10);
	eli2.height = 20;
	eli2.width = 20;

	int gui_image_apple3 = ECS.createEntity("gui image apple 3");
	GUIElement& eli3 = ECS.createComponentForEntity<GUIElement>(gui_image);
	eli3.texture = Parsers::parseTexture("data/assets/manzana.tga");
	eli3.anchor = GUIAnchorBottomLeft;
	eli3.offset = lm::vec2(-10, -10);
	eli3.height = 20;
	eli3.width = 20;*/

    //******* INIT DEBUG SYSTEM *******
	gui_system_.lateInit();
	script_system_.lateInit();
    debug_system_.init();
    debug_system_.setActive(true);
}

//Entry point for game update code
void Game::update(float dt) {
	//update each system in turn

	//update input
	control_system_.update(dt);

	//collision
	collision_system_.update(dt);

	//render
	graphics_system_.update(dt);

    //debug
    debug_system_.update(dt);

	//gui
	gui_system_.update(dt);

	//scripts
	script_system_.update(dt);
   
}
//update game viewports
void Game::update_viewports(int window_width, int window_height) {
	window_width_ = window_width;
	window_height_ = window_height;

	auto& cameras = ECS.getAllComponents<Camera>();
	for (auto& cam : cameras) {
		cam.setPerspective(60.0f*DEG2RAD, (float)window_width_ / (float) window_height_, 0.01f, 10000.0f);
	}

	graphics_system_.updateMainViewport(window_width_, window_height_);
	gui_system_.updateViewport(window_width_, window_height_);
}


