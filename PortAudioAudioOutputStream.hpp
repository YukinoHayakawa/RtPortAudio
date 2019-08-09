#pragma once

#include <portaudiocpp/PortAudioCpp.hxx>

#include <Usagi/Runtime/Audio/AudioOutputStream.hpp>

namespace usagi
{
class PortAudioAudioOutputStream : public AudioOutputStream
{
    int callback(
        const void *input_buffer,
        void *output_buffer,
        unsigned long frames_per_buffer,
        const PaStreamCallbackTimeInfo *time_info,
        PaStreamCallbackFlags status_flags);

    portaudio::MemFunCallbackStream<PortAudioAudioOutputStream> mStream;

public:
    PortAudioAudioOutputStream(
        const portaudio::StreamParameters &parameters,
        AudioStreamFormat format,
        AudioOutputCallback callback);

    void start() override;
    void stop() override;
};
}
