#include "Renderer.h"

#include <iostream>

void glClearError()
{
    while (glGetError() != GL_NO_ERROR);
}
bool glLogCall(const char* function, const char* file, int line)
{
    while (GLenum error = glGetError()) //if returns an error
    {
        std::cout << "[OpenGL Error]( " << error << " ) " << function << " File: " << file << " Line:" << line << std::endl;
        return false;
    }
    return true;
}

void Renderer::Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const
{
    shader.Bind();
    va.Bind();
    ib.Bind();

    GlCall(glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr));
}

void Renderer::Draw(const VertexArray& va, const IndexBuffer& ib) const
{

    va.Bind();
    ib.Bind();

    GlCall(glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr));
}

void Renderer::Clear() const
{
    // Clear the color buffer to prepare for a new frame
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

}
void Renderer::Clear(std::string colour) const
{
    std::transform(colour.begin(), colour.end(), colour.begin(), ::towlower);
    // Clear the color buffer to prepare for a new frame
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (colour == "black")
    {
        GlCall(glClearColor(0, 0, 0, 1));
    }
    if(colour == "white")
    {
        GlCall(glClearColor(1, 1, 1, 1));
	}
    if(colour == "red")
    {
        GlCall(glClearColor(1, 0, 0, 1));
	}
    if(colour == "green")
    {
        GlCall(glClearColor(0, 1, 0, 1));
    }
    if(colour == "blue")
    {
        GlCall(glClearColor(0, 0, 1, 1));
	}
    if(colour == "yellow")
    {
        GlCall(glClearColor(1, 1, 0, 1));
    }
    if(colour == "cyan")
    {
        GlCall(glClearColor(0, 1, 1, 1));
    }
    if(colour == "magenta")
    {
        GlCall(glClearColor(1, 0, 1, 1));
	}
    

}

void Renderer::ClearColour_Black() const
{
    GlCall(glClearColor(0, 0, 0, 1));
}

void Renderer::ClearColour_White() const
{
    GlCall(glClearColor(1, 1, 1, 1));
}



