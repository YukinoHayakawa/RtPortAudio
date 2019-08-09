#include "PortAudioAudioDevice.hpp"

#include <portaudiocpp/PortAudioCpp.hxx>
#include "PortAudioCommon.hpp"
#include <Usagi/Runtime/Audio/AudioStreamFormat.hpp>
#include "PortAudioAudioOutputStream.hpp"

namespace usagi
{
portaudio::StreamParameters PortAudioAudioDevice::buildOutputStreamParameters(
    const AudioStreamFormat &format) const
{
    auto pa_output_format = translateStreamFormat(format);

    pa_output_format.setDevice(mDevice);
    pa_output_format.setSuggestedLatency(mDevice.defaultLowOutputLatency());
    pa_output_format.setHostApiSpecificStreamInfo(nullptr);

    return {
        portaudio::DirectionSpecificStreamParameters::null(),
        pa_output_format,
        format.sample_rate,
        paFramesPerBufferUnspecified,
        paClipOff
    };
}

PortAudioAudioDevice::PortAudioAudioDevice(const PaDeviceIndex index)
    : mDevice(portaudio::System::instance().deviceByIndex(index))
{
}

bool PortAudioAudioDevice::isOutputFormatSupported(
    const AudioStreamFormat &format) const
{
    return buildOutputStreamParameters(format).isSupported();
}

std::unique_ptr<AudioOutputStream> PortAudioAudioDevice::createOutputStream(
    const AudioStreamFormat &format,
    AudioOutputCallback callback)
{
    return std::make_unique<PortAudioAudioOutputStream>(
        buildOutputStreamParameters(format),
        format,
        std::move(callback)
    );
}
}
