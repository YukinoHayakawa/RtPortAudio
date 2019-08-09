#pragma once

#include <portaudiocpp/DirectionSpecificStreamParameters.hxx>

#include <Usagi/Constants/DataFormat.hpp>

namespace usagi
{
struct AudioStreamFormat;

portaudio::SampleDataFormat translateDataFormat(DataFormat format);
portaudio::DirectionSpecificStreamParameters translateStreamFormat(
    const AudioStreamFormat &format
);
}
