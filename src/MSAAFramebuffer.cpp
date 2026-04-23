#include "MSAAFramebuffer.h"
#include <iostream>

MSAAFramebuffer::MSAAFramebuffer(int width, int height, int samples)
    : Framebuffer(width, height),   // base allocates the FBO ID via m_RendererID
    m_Samples(samples)
{
    // The base constructor sets up texture attachments, which are not valid for
    // a multisampled FBO. We rebind and replace them with renderbuffer storage.
    // Renderbuffers are required here because glRenderbufferStorageMultisample
    // has no texture equivalent that can be cleanly resolved without texelFetch.
    glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);

    // Detach the texture attachments created by the base constructor
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, 0, 0);

    glGenRenderbuffers(1, &m_ColorRBO);
    glBindRenderbuffer(GL_RENDERBUFFER, m_ColorRBO);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, m_Samples, GL_RGBA8, m_Width, m_Height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, m_ColorRBO);

    glGenRenderbuffers(1, &m_DepthRBO);
    glBindRenderbuffer(GL_RENDERBUFFER, m_DepthRBO);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, m_Samples, GL_DEPTH24_STENCIL8, m_Width, m_Height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_DepthRBO);

    if (!CheckStatus())
        std::cerr << "MSAAFramebuffer is not complete!" << std::endl;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

MSAAFramebuffer::~MSAAFramebuffer()
{
    if (m_ColorRBO) glDeleteRenderbuffers(1, &m_ColorRBO);
    if (m_DepthRBO) glDeleteRenderbuffers(1, &m_DepthRBO);
    // Base destructor handles m_RendererID and the now-detached textures
}

void MSAAFramebuffer::Resolve(const Framebuffer& target) const
{
    glBindFramebuffer(GL_READ_FRAMEBUFFER, m_RendererID);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, target.GetID());
    glBlitFramebuffer(
        0, 0, m_Width, m_Height,
        0, 0, target.GetWidth(), target.GetHeight(),
        GL_COLOR_BUFFER_BIT, GL_NEAREST
    );
}
