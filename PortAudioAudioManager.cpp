#include "PortAudioAudioManager.hpp"

#include <portaudio.h>

#include <Usagi/Core/Logging.hpp>
#include <Usagi/Constants/DataFormat.hpp>

#include "PortAudioAudioDevice.hpp"

extern "C"
{
typedef void (*PaUtilLogCallback )(const char *log);
void PaUtil_SetDebugPrintFunction(PaUtilLogCallback cb);
}

namespace usagi
{
void PortAudioPreInit::debugLog(const char *log)
{
    std::string_view trim_newline = log;
    assert(!trim_newline.empty());
    trim_newline = trim_newline.substr(0, trim_newline.size() - 1);
    LOG(debug, "{}", trim_newline);
}

PortAudioPreInit::PortAudioPreInit()
{
    PaUtil_SetDebugPrintFunction(&PortAudioAudioManager::debugLog);
}

void PortAudioAudioManager::enumerateSupportedFormats(
    const portaudio::DirectionSpecificStreamParameters &input_parameters,
    const portaudio::DirectionSpecificStreamParameters &output_parameters)
{
    // https://en.wikipedia.org/wiki/Sampling_(signal_processing)#Sampling_rate
    static auto STANDARD_SAMPLE_RATES = {
        8000, 11025, 16000, 22050, 32000,
        37800, 44056, 44100, 47250, 48000,
        50000, 50400, 64000, 88200, 96000,
        176400, 192000
    };

    static auto DATA_FORMATS = {
        DataFormat::INT16,
        DataFormat::INT24,
        DataFormat::INT32
    };

    bool printed_any = false;

    for(auto &&s : STANDARD_SAMPLE_RATES)
    {
        portaudio::StreamParameters tmp = portaudio::StreamParameters(
            input_parameters, output_parameters, s, 0,
            paNoFlag);

        if(tmp.isSupported())
        {
            LOG(info, "    {}", s);
            printed_any = true;
        }
    }

    if(!printed_any)
        LOG(info, "    None.");
}

AudioManager::AcquisitionAgent PortAudioAudioManager::createDeviceAgent(
    portaudio::Device &device)
{
    AudioDeviceProperties properties;

    properties.name = device.name();
    properties.api_name = device.hostApi().name();

    properties.default_sample_rate = device.defaultSampleRate();

    properties.in.max_channels = device.maxInputChannels();
    properties.in.is_default_device = device.isHostApiDefaultInputDevice();

    properties.out.max_channels = device.maxOutputChannels();
    properties.out.is_default_device = device.isHostApiDefaultOutputDevice();

    return { this, device.index(), properties };
}

portaudio::System & PortAudioAudioManager::system()
{
    return portaudio::System::instance();
}

PortAudioAudioManager::PortAudioAudioManager()
{
    LOG(info, "Creating PortAudio audio manager");

    LOG(info, "PortAudio version number = {}", system().version());
    LOG(info, "PortAudio version text = '{}'", system().versionText());
}

const std::vector<AudioManager::AcquisitionAgent> &
PortAudioAudioManager::enumerateDevices()
{
    mDevices.clear();

    // https://app.assembla.com/spaces/portaudio/git/source/master/bindings/cpp/example/devs.cxx

    int numDevices = system().deviceCount();
    LOG(info, "Number of devices = {}", numDevices);

    for(portaudio::System::DeviceIterator i = system().devicesBegin(); i !=
        system().devicesEnd(); ++i)
    {
        mDevices.push_back(createDeviceAgent(*i));

        // print device info

        LOG(info, "--------------------------------------- device #{}",
            (*i).index());

        // Mark global and API specific default devices:
        std::string type;

        if((*i).isSystemDefaultInputDevice())
        {
            type = "Default Input";
        }
        else if((*i).isHostApiDefaultInputDevice())
        {
            type = fmt::format("Default {} Input", (*i).hostApi().name());
        }

        if((*i).isSystemDefaultOutputDevice())
        {
            if(!type.empty())
                type += ", ";
            type += "Default Output";
        }
        else if((*i).isHostApiDefaultOutputDevice())
        {
            if(!type.empty())
                type += ", ";
            type += fmt::format("Default {} Output", (*i).hostApi().name());
        }

        if(!type.empty())
            LOG(info, "[ {} ]", type);

        // Print device info:
        LOG(info, "Name                        = {}", (*i).name());
        LOG(info, "Host API                    = {}", (*i).hostApi().name());
        LOG(info, "Max inputs = {}, Max outputs = {}", (*i).maxInputChannels(),
            (*i).maxOutputChannels());

        LOG(info, "Default low input latency   = {}",
            (*i).defaultLowInputLatency()); // 8.3
        LOG(info, "Default low output latency  = {}",
            (*i).defaultLowOutputLatency()); // 8.3
        LOG(info, "Default high input latency  = {}",
            (*i).defaultHighInputLatency()); // 8.3
        LOG(info, "Default high output latency = {}",
            (*i).defaultHighOutputLatency()); // 8.3

        LOG(info, "Default sample rate         = {}",
            (*i).defaultSampleRate()); // 8.2

        // Poll for standard sample rates:
        portaudio::DirectionSpecificStreamParameters inputParameters((*i),
            (*i).maxInputChannels(), portaudio::INT16, true, 0.0, nullptr);
        portaudio::DirectionSpecificStreamParameters outputParameters((*i),
            (*i).maxOutputChannels(), portaudio::INT16, true, 0.0, nullptr);

        if(inputParameters.numChannels() > 0)
        {
            LOG(info, "Supported standard sample rates");
            LOG(info, " for half-duplex 16 bit {} channel input = ",
                inputParameters.numChannels());
            enumerateSupportedFormats(inputParameters,
                portaudio::DirectionSpecificStreamParameters::null());
        }

        if(outputParameters.numChannels() > 0)
        {
            LOG(info, "Supported standard sample rates");
            LOG(info, " for half-duplex 16 bit {} channel output = ",
                outputParameters.numChannels());
            enumerateSupportedFormats(
                portaudio::DirectionSpecificStreamParameters::null(),
                outputParameters);
        }

        if(inputParameters.numChannels() > 0 && outputParameters.numChannels() >
            0)
        {
            LOG(info, "Supported standard sample rates");
            LOG(info,
                " for full-duplex 16 bit {} channel input, {} channel output = ",
                inputParameters.numChannels(), outputParameters.numChannels());
            enumerateSupportedFormats(inputParameters,
                outputParameters);
        }
    }

    LOG(info, "----------------------------------------------");

    return mDevices;
}

AudioManager::AcquisitionAgent PortAudioAudioManager::defaultOutputDevice()
{
    return createDeviceAgent(system().defaultOutputDevice());
}

std::shared_ptr<AudioDevice> PortAudioAudioManager::acquireDevice(int index)
{
    return std::make_shared<PortAudioAudioDevice>(index);
}
}
