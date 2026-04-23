#pragma once
#include <GL/glew.h>
#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Shader.h"

#define ASSERT(x) if (!(x)) __debugbreak(); //validate a condition that checks if bool false add a breakpoint 

#define GlCall(x) glClearError();\
    x;\
    ASSERT(glLogCall(#x, __FILE__, __LINE__)) 
//a multi line macro that clears the error and executes the passed function and then if false will break at point and then 
// log call command to print out the errors in the function, the file and the line

void glClearError();
bool glLogCall(const char* function, const char* file, int line);

class Renderer {
public:
    void Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const;
    void Draw(const VertexArray& va, const IndexBuffer& ib) const;


    void Clear() const;
    void Clear(std::string colour) const;

    void ClearColour_Black() const;
    void ClearColour_White() const;
};