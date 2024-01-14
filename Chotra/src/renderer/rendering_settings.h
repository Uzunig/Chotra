#ifndef RENDERING_SETTINGS_H
#define RENDERING_SETTINGS_H

namespace Chotra {

    class RenderingSettings
    {
    public:
        int renderingMode = 1;

        bool enableMSAA = true;
        int samplesNumber = 4;
        float backgroundColor[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
        bool drawSkybox = true;
        bool bloom = true;
        float exposure = 1.75f;
        float backgroundExposure = 2.0f;
        bool gammaCorrection = true;
        float brightness = 0.0f;
        float contrast = 0.6f;

        bool showShadows = true;
        float shadowBiasMin = 0.0009f;
        float shadowBiasMax = 0.0006f;
        float shadowOpacity = 0.5f;

        int kernelSizeSSAO = 64;
        float radiusSSAO = 0.5;
        float biasSSAO = 0.025;

        float biasSSR = 0.3f; // 20,0
        float rayStep = 0.014f; // 0,014f
        int iterationCount = 6; // 1400
        float accuracySSR = 0.05f; // 0.05f
    };

} //namespace Chotra
#endif
