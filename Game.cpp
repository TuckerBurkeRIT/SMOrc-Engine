#include "Game.h"
#include "Vertex.h"
#include "Mesh.h"
#include "Entity.h"
#include "Camera.h"
#include "Material.h"
#include "SimpleShader.h"
#include <ppl.h>

using namespace Concurrency;

// Needed for a helper function to read compiled shader files from the hard drive
#pragma comment(lib, "d3dcompiler.lib")
#include <d3dcompiler.h>

// For the DirectX Math library
using namespace DirectX;

// --------------------------------------------------------
// Constructor
//
// DXCore (base class) constructor will set up underlying fields.
// DirectX itself, and our window, are not ready yet!
//
// hInstance - the application's OS-level handle (unique ID)
// --------------------------------------------------------
Game::Game(HINSTANCE hInstance)
	: DXCore(
		hInstance,		   // The application's handle
		"DirectX Game",	   // Text for the window's title bar
		1280,			   // Width of the window's client area
		720,			   // Height of the window's client area
		true)			   // Show extra stats (fps) in title bar?
{

#if defined(DEBUG) || defined(_DEBUG)
	// Do we want a console window?  Probably only in debug mode
	CreateConsoleWindow(500, 120, 32, 120);
	printf("Console window created successfully.  Feel free to printf() here.\n");
#endif

}

// --------------------------------------------------------
// Destructor - Clean up anything our game has created:
//  - Release all DirectX objects created here
//  - Delete any objects to prevent memory leaks
// --------------------------------------------------------
Game::~Game()
{

	parallel_for
	(
		size_t(0), entities.size(), [&](size_t i)
		{
			delete entities[i];
		},
		static_partitioner()
	);

	parallel_for 
	(
		size_t(0), materials.size(), [&](size_t i)
		{
			delete materials[i];
		},
		static_partitioner()
	);

	parallel_for 
	(
		size_t(0), meshes.size(), [&](size_t i) 
		{
			delete meshes[i];
		}, 
		static_partitioner()
	);


	delete playerCamera;
	delete pixelShader;
	delete vertexShader;
}

// --------------------------------------------------------
// Called once per program, after DirectX and the window
// are initialized but before the game loop.
// --------------------------------------------------------
void Game::Init()
{
	// Helper methods for loading shaders, creating some basic
	// geometry to draw and some simple camera matrices.
	//  - You'll be expanding and/or replacing these later
	LoadShaders();

	CreateBasicGeometry();

	playerCamera = new Camera(XMFLOAT3(0,0,-4.f), XMFLOAT3(0,0,0), (float)this->width / this->height);

	// Tell the input assembler stage of the pipeline what kind of
	// geometric primitives (points, lines or triangles) we want to draw.  
	// Essentially: "What kind of shape should the GPU draw with our data?"
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	dirLight.ambientColor = XMFLOAT3(0.1f, 0.1f, 0.1f);
	dirLight.diffuseColor = XMFLOAT3(.8f, .8f, .8f);
	dirLight.direction = XMFLOAT3(1, -1, 0);
	dirLight.type = 0;
	dirLight.ambientIntensity = 1.f;
	dirLight.diffuseIntensity = .5f;

	pointLight.ambientColor = XMFLOAT3(0.1f, 0.1f, 0.1f);
	pointLight.diffuseColor = XMFLOAT3(.7f, .7f, .7f);
	//dirLight2.direction = XMFLOAT3(0, 0, 1);
	pointLight.type = 1;
	pointLight.ambientIntensity = 1.f;
	pointLight.diffuseIntensity = 1.f;
	pointLight.position = XMFLOAT3(0, 0, 0);

	dirLight3.ambientColor = XMFLOAT3(0.1f, 0.1f, 0.1f);
	dirLight3.diffuseColor = XMFLOAT3(.4f, .4f, .4f);
	dirLight3.direction = XMFLOAT3(-1, -1, -1);
	dirLight3.type = 0;
	dirLight3.ambientIntensity = .5f;
	dirLight3.diffuseIntensity = .5f;

	// all the initialization for the engine has to be done prior to this. Now the game specific stuff needs to initialize
	BeginPlay();
}

// --------------------------------------------------------
// Loads shaders from compiled shader object (.cso) files
// and also created the Input Layout that describes our 
// vertex data to the rendering pipeline. 
// - Input Layout creation is done here because it must 
//    be verified against vertex shader byte code
// - We'll have that byte code already loaded below
// --------------------------------------------------------
void Game::LoadShaders()
{
	vertexShader = new SimpleVertexShader(device.Get(), context.Get(), GetFullPathTo_Wide(L"VertexShader.cso").c_str());
	pixelShader = new SimplePixelShader(device.Get(), context.Get(), GetFullPathTo_Wide(L"PixelShader.cso").c_str());
}



// --------------------------------------------------------
// Creates the geometry we're going to draw - a single triangle for now
// --------------------------------------------------------
void Game::CreateBasicGeometry()
{
	// setup models
    meshes.push_back(new Mesh(GetFullPathTo("../../Assets/Models/sphere.obj").c_str(), device.Get()));
	meshes.push_back(new Mesh(GetFullPathTo("../../Assets/Models/cube.obj").c_str(), device.Get()));
	meshes.push_back(new Mesh(GetFullPathTo("../../Assets/Models/helix.obj").c_str(), device.Get()));
	meshes.push_back(new Mesh(GetFullPathTo("../../Assets/Models/torus.obj").c_str(), device.Get()));
	meshes.push_back(new Mesh(GetFullPathTo("../../Assets/Models/cylinder.obj").c_str(), device.Get()));

	// setup materials
	materials.push_back(new Material(XMFLOAT4(.2f, .17f, .54f, 1), 1.f, vertexShader, pixelShader));
	materials.push_back(new Material(XMFLOAT4(.4f, .86f, .39f, 1), 1.f, vertexShader, pixelShader));
	materials.push_back(new Material(XMFLOAT4(.88f, 0.1f, .68f, 1), .75f, vertexShader, pixelShader));
	materials.push_back(new Material(XMFLOAT4(.15f, .1f, .5f, 1), .35f, vertexShader, pixelShader));
	materials.push_back(new Material(XMFLOAT4(0.2f, 0.8f, .28f, 1), 0, vertexShader, pixelShader));

	// setup entities
	entities.push_back(new Entity(meshes[0], materials[0]));
	entities.push_back(new Entity(meshes[1], materials[1]));
	entities.push_back(new Entity(meshes[2], materials[2]));
	entities.push_back(new Entity(meshes[3],  materials[3]));
	entities.push_back(new Entity(meshes[4],  materials[4]));
}


void Game::BeginPlay()
{
	if(entities.size() <= 0)
		return;

	
	entities[0]->GetTransform()->MoveAbsolute(3, 0, 1);

	entities[1]->GetTransform()->SetPosition(.2f, 1, .5f);

	//helix
	entities[2]->GetTransform()->SetPosition(-1.5, 0, -1);
	entities[2]->GetTransform()->SetScale(.5f, .5f, .5f);

	entities[4]->GetTransform()->SetPosition(1, -1.5, -.05f);
}

// --------------------------------------------------------
// Handle resizing DirectX "stuff" to match the new window size.
// For instance, updating our projection matrix's aspect ratio.
// --------------------------------------------------------
void Game::OnResize()
{
	// Handle base-level DX resize stuff
	DXCore::OnResize();
	if(!playerCamera)
		return;
	playerCamera->UpdateProjectionMatrix((float)this->width / this->height);
}

// --------------------------------------------------------
// Update your game here - user input, move objects, AI, etc.
// --------------------------------------------------------
void Game::Update(float deltaTime, float totalTime)
{
	// Quit if the escape key is pressed
	if (GetAsyncKeyState(VK_ESCAPE))
		Quit();

	if(entities.size() == 0) 
	{
		return;
	}

	float sinTime = (float)sin(totalTime);
	float offset = (sinTime*deltaTime);

	entities[0]->GetTransform()->MoveAbsolute(-offset/3.f, offset/5.f, 0);
	entities[0]->GetTransform()->SetPosition(
		entities[0]->GetTransform()->GetPosition().x,
		entities[0]->GetTransform()->GetPosition().y, -.01f
	);

	entities[1]->GetTransform()->MoveAbsolute(0, offset, 0);

	entities[2]->GetTransform()->Rotate(0,  1.f * deltaTime, 0);
	
	entities[3]->GetTransform()->MoveAbsolute(0,0, offset*2.f);
	entities[3]->GetTransform()->MoveAbsolute(offset/2.f, -offset/2.f, 0);
	entities[3]->GetTransform()->Rotate(-1.5f * deltaTime, 0, 0);

	entities[4]->GetTransform()->Rotate(0, 0,  offset*2.f);

	playerCamera->Update(deltaTime, hWnd);
}

// --------------------------------------------------------
// Clear the screen, redraw everything, present to the user
// --------------------------------------------------------
void Game::Draw(float deltaTime, float totalTime)
{
	// Background color (Cornflower Blue in this case) for clearing
	const float color[4] = { 0.4f, 0.6f, 0.75f, 0.0f };

	// Clear the render target and depth buffer (erases what's on the screen)
	//  - Do this ONCE PER FRAME
	//  - At the beginning of Draw (before drawing *anything*)
	context->ClearRenderTargetView(backBufferRTV.Get(), color);
	context->ClearDepthStencilView(
		depthStencilView.Get(),
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f,
		0);

	pixelShader->SetData("dirLight", &dirLight, sizeof(Light));
	pixelShader->SetData("pointLight", &pointLight, sizeof(Light));
	pixelShader->SetData("dirLight3", &dirLight3, sizeof(Light));
	pixelShader->SetFloat3("cameraPosition", playerCamera->GetTransform()->GetPosition());
	pixelShader->CopyAllBufferData();

	for (Entity* entity : entities)
	{
		entity->Draw(context.Get(), playerCamera);
	}

	// Present the back buffer to the user
	//  - Puts the final frame we're drawing into the window so the user can see it
	//  - Do this exactly ONCE PER FRAME (always at the very end of the frame)
	swapChain->Present(0, 0);

	// Due to the usage of a more sophisticated swap chain,
	// the render target must be re-bound after every call to Present()
	context->OMSetRenderTargets(1, backBufferRTV.GetAddressOf(), depthStencilView.Get());
}