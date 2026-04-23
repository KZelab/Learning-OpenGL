#pragma once
#include "Framebuffer.h"

class MSAAFramebuffer : public Framebuffer {
public:
    MSAAFramebuffer(int width, int height, int samples);
    ~MSAAFramebuffer();
    MSAAFramebuffer(const MSAAFramebuffer&) = delete;
    MSAAFramebuffer& operator=(const MSAAFramebuffer&) = delete;

    // Resolves the multisampled colour buffer into the provided single-sample
    // target framebuffer via a blit. The target must be the same dimensions.
    void Resolve(const Framebuffer& target) const;

    int GetSamples() const { return m_Samples; }

private:
    unsigned int m_ColorRBO = 0;
    unsigned int m_DepthRBO = 0;
    int m_Samples;
};
