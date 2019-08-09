#include "PortAudioCommon.hpp"

#include <stdexcept>

#include <Usagi/Runtime/Audio/AudioStreamFormat.hpp>

namespace usagi
{
portaudio::SampleDataFormat translateDataFormat(DataFormat format)
{
    switch(format)
    {
        case DataFormat::FLOAT32: return portaudio::FLOAT32;
        case DataFormat::INT8: return portaudio::INT8;
        case DataFormat::INT16: return portaudio::INT16;
        case DataFormat::INT24: return portaudio::INT24;
        case DataFormat::INT32: return portaudio::INT32;
        case DataFormat::UINT8: return portaudio::UINT8;
        default: throw std::runtime_error("invalid format");
    }
}

portaudio::DirectionSpecificStreamParameters translateStreamFormat(
    const AudioStreamFormat &format)
{
    portaudio::DirectionSpecificStreamParameters p;

    p.setNumChannels(format.num_channels);
    p.setSampleFormat(translateDataFormat(format.format), format.interleaved);

    return p;
}
}
