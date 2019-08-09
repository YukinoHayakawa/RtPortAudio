#pragma once

#include <portaudiocpp/Device.hxx>
#include <portaudiocpp/StreamParameters.hxx>

#include <Usagi/Runtime/Audio/AudioDevice.hpp>

namespace usagi
{
class PortAudioAudioDevice : public AudioDevice
{
    portaudio::Device &mDevice;

private:
    portaudio::StreamParameters buildOutputStreamParameters(
        const AudioStreamFormat &format) const;

public:
    explicit PortAudioAudioDevice(PaDeviceIndex index);

    bool isOutputFormatSupported(
        const AudioStreamFormat &format) const override;
    std::unique_ptr<AudioOutputStream> createOutputStream(
        const AudioStreamFormat &format,
        AudioOutputCallback callback) override;
};
}
