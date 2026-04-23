#pragma once
#include <GL/glew.h>

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
