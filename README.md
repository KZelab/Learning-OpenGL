# Branch: 01-window-setup

## Key Concepts

###  **Core Concepts Learned:**
- **GLFW Library**: Cross-platform library for window management and input handling
- **OpenGL Context**: The environment where OpenGL commands execute
- **GLEW**: Extension loading library that provides access to modern OpenGL functions
- **Double Buffering**: Technique to prevent flickering by drawing to a hidden buffer
- **Render Loop**: The continuous cycle of clearing, drawing, and presenting frames

###  **OpenGL Functions Introduced:**
- `glClearColor()` - Sets the background color
- `glClear()` - Clears the screen with the specified color
- `glGetString()` - Retrieves OpenGL version and hardware information

### **Technical Terms:**
- **Core Profile**: Modern OpenGL without deprecated legacy functions
- **V-Sync**: Synchronizes rendering with monitor refresh rate
- **Context**: The state machine that tracks all OpenGL settings



### Program Flow
1. **Initialize GLFW** - Set up the windowing system
2. **Configure OpenGL** - Request OpenGL 3.3 Core Profile
3. **Create Window** - 960x540 pixel window with title
4. **Initialize GLEW** - Load modern OpenGL function pointers
5. **Set Clear Color** - Choose background color (blue)
6. **Render Loop** - Continuously clear screen and swap buffers
7. **Cleanup** - Properly destroy window and terminate GLFW



### The Render Loop Explained
```cpp
while (!glfwWindowShouldClose(window)) {
    glClear(GL_COLOR_BUFFER_BIT);    // Clear with background color
    // (Future: Draw objects here)
    glfwSwapBuffers(window);         // Show the completed frame
    glfwPollEvents();                // Handle input and window events
}
```

This loop runs 60+ times per second, creating smooth animation when we start adding moving objects.

## Try These Experiments

###  **Beginner Challenges:**
1. **Change the background color** - Modify the `glClearColor()` values:
   - Try `(1.0f, 0.0f, 0.0f, 1.0f)` for red
   - Try `(0.0f, 0.0f, 0.0f, 1.0f)` for black
   - Try `(0.5f, 0.5f, 0.5f, 1.0f)` for gray

2. **Change the window size** - Modify the `glfwCreateWindow()` parameters
3. **Change the window title** - Update the title string

###  **Intermediate Challenges:**
1. **Add window resize handling**:
   ```cpp
   // Add after glfwMakeContextCurrent(window);
   glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height) {
       glViewport(0, 0, width, height);
   });
   ```

2. **Add ESC key to close window**:
   ```cpp
   // Add inside the render loop
   if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
       glfwSetWindowShouldClose(window, true);
   ```

3. **Print FPS counter**:
   ```cpp
   // Add timing variables before the loop
   double lastTime = glfwGetTime();
   int frameCount = 0;

   // Add inside the render loop
   double currentTime = glfwGetTime();
   frameCount++;
   if (currentTime - lastTime >= 1.0) {
       std::cout << "FPS: " << frameCount << std::endl;
       frameCount = 0;
       lastTime = currentTime;
   }
   ```



## Resources for Deeper Learning

### **Essential Reading:**
- [OpenGL Tutorial - Creating a Window](https://learnopengl.com/Getting-started/Creating-a-window)
- [GLFW Documentation](https://www.glfw.org/documentation.html)

###  **Video Tutorials:**
- [The Cherno OpenGL Series - Episode 2](https://www.youtube.com/watch?v=OR4fNpBjmq8)

###  **Concepts to Explore:**
- Double vs Single Buffering
- Graphics Pipeline Overview
- OpenGL State Machine Concept
