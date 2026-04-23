#pragma once
#include <GL/glew.h>

// A framebuffer is an off-screen rendering target that acts as a collection of memory attachments the GPU writes into during a render pass.
// In OpenGL every application has a default framebuffer provided by the windowing system, which is what ultimately gets displayed on screen, 
// but a custom framebuffer object (FBO) lets you redirect rendering into textures or renderbuffers of your own. 
// An FBO itself holds no memory; 
// it is simply a container that references up to one depth attachment, one stencil attachment, and one or more colour attachments. 
// Each attachment is backed by either a texture, which can then be sampled in a subsequent shader pass, or a renderbuffer, 
// which is faster to write to but cannot be read from directly. Once a scene has been rendered into an FBO its colour texture can be 
// passed to another shader just like any other texture, 
// which is the foundation for almost all post-processing techniques including bloom, shadow mapping, ambient occlusion, and anti-aliasing. 

class Framebuffer {
public:
    Framebuffer(int width, int height, bool depthOnly = false);
    virtual ~Framebuffer();
    Framebuffer(const Framebuffer&) = delete;
    Framebuffer& operator=(const Framebuffer&) = delete;

    virtual void Bind()   const;
    virtual void Unbind() const;

    unsigned int GetID()           const { return m_RendererID; }
    unsigned int GetColorTexture() const { return m_ColorTexture; }
    unsigned int GetDepthTexture() const { return m_DepthTexture; }
    int  GetWidth()  const { return m_Width; }
    int  GetHeight() const { return m_Height; }
    bool CheckStatus() const;

protected:
    unsigned int m_RendererID = 0;
    int m_Width, m_Height;

private:
    unsigned int m_ColorTexture = 0;
    unsigned int m_DepthTexture = 0;
    bool m_DepthOnly;
};
