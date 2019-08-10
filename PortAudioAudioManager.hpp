#pragma once

#include <portaudiocpp/PortAudioCpp.hxx>

#include <Usagi/Runtime/Audio/AudioManager.hpp>

namespace usagi
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

    static portaudio::System & system();

    static void enumerateSupportedFormats(
        const portaudio::DirectionSpecificStreamParameters &input_parameters,
        const portaudio::DirectionSpecificStreamParameters &output_parameters);

    AcquisitionAgent createDeviceAgent(portaudio::Device &device);

public:
    PortAudioAudioManager();

    const std::vector<AcquisitionAgent> & enumerateDevices() override;
    AcquisitionAgent defaultOutputDevice() override;
    // todo: fix recreating the same devices
    std::shared_ptr<AudioDevice> acquireDevice(int index) override;
};
}
