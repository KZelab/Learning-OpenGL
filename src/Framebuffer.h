#pragma once
#include <GL/glew.h>

class Framebuffer {
public:
    Framebuffer(int width, int height, bool depthOnly = false, int samples = 1);
    ~Framebuffer();
    Framebuffer(const Framebuffer&) = delete;
    Framebuffer& operator=(const Framebuffer&) = delete;

    void Bind() const;
    void Unbind() const;
    unsigned int GetID()           const { return m_RendererID; }
    unsigned int GetDepthTexture() const { return m_DepthTexture; }
    unsigned int GetColorTexture() const { return m_ColorTexture; }
    int  GetWidth()   const { return m_Width; }
    int  GetHeight()  const { return m_Height; }
    int  GetSamples() const { return m_Samples; }
    bool IsMultisampled() const { return m_Samples > 1; }
    bool CheckStatus() const;

private:
    unsigned int m_RendererID = 0;
    unsigned int m_DepthTexture = 0;
    unsigned int m_ColorTexture = 0;
    // Renderbuffer objects used for multisampled attachments.
    // Regular (single-sample) framebuffers use textures instead.
    unsigned int m_ColorRBO = 0;
    unsigned int m_DepthRBO = 0;
    int  m_Width, m_Height;
    int  m_Samples;
    bool m_DepthOnly;
};