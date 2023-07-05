# RefactaMasta
Taking a flocking simulation from a uni project and refactoring the monolith classes

## TODO's
    - Move camera to another class
    - Move light related methods to another class
    - create a game state interface
    - create a mainGame state
    - create a menu state
    - create a pause state
    - execute the game loop in the mainGame state
    - execute the render loop in the AppEngine class

## Class Descriptions

### AppWindow Class
	- This class is responsible for creating the window and the context
	- It also contains the main game loop

### AppEngine Class
	- This class is responsible for the game loop
	- It contains the game state machine
	- It contains the render loop

### GameState Class
	- This is an interface for the game states

### Graphics Class
	- This class uses the DirectX 11 API to create the device, swap chain, and context
	- This class is responsible for the rendering
	- It contains the camera
	- It contains the light
	- It loads the shaders from the shader files
	- It contains the render loop

### Camera Class
	- This class is responsible for the camera
	- It contains the view matrix
	- It contains the projection matrix
	- It contains the camera position
	- It contains the camera rotation

### Light Class
	- This class is responsible for lighting
	- It contains the light position
	- It contains the light color

### 