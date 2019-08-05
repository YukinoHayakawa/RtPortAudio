#pragma once

#include <portaudiocpp/PortAudioCpp.hxx>

#include <Usagi/Runtime/Audio/AudioManager.hpp>

namespace usagi::extensions
{
class PortAudioPreInit : public AudioManager
{
protected:
    static void debugLog(const char *log);

public:
    PortAudioPreInit();
};

class PortAudioAudioManager : public PortAudioPreInit
{
    portaudio::AutoSystem mSystemInit;
    portaudio::System &mPaSystem = portaudio::System::instance();

    static void printSupportedStandardSampleRates(
        const portaudio::DirectionSpecificStreamParameters &input_parameters,
        const portaudio::DirectionSpecificStreamParameters &output_parameters);

public:
    const std::vector<AcquisitionAgent> & enumerateDevices() override;
};
}
