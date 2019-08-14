#include "PortAudioAudioOutputStream.hpp"

namespace usagi
{
int PortAudioAudioOutputStream::callback(
    const void *input_buffer,
    void *output_buffer,
    const unsigned long frames_per_buffer,
    const PaStreamCallbackTimeInfo *time_info,
    PaStreamCallbackFlags status_flags)
{
    ArrayView<std::byte*> channels {
        static_cast<std::byte**>(output_buffer), mFormat.num_channels
    };
    return static_cast<int>(mCallback(
        channels,
        frames_per_buffer
    ));
}

PortAudioAudioOutputStream::PortAudioAudioOutputStream(
    const portaudio::StreamParameters &parameters,
    AudioStreamFormat format,
    AudioOutputCallback callback)
    : AudioOutputStream(std::move(format), std::move(callback))
{
    mStream.open(parameters, *this, &PortAudioAudioOutputStream::callback);
}

void PortAudioAudioOutputStream::start()
{
    mStream.start();
}

void PortAudioAudioOutputStream::stop()
{
    mStream.stop();
}
}
