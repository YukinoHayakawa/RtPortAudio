#include "PortAudioAudioManager.hpp"

#include <portaudio.h>

#include <Usagi/Core/Logging.hpp>

extern "C"
{
typedef void (*PaUtilLogCallback )(const char *log);
void PaUtil_SetDebugPrintFunction(PaUtilLogCallback cb);
}

namespace usagi::extensions
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

void PortAudioAudioManager::printSupportedStandardSampleRates(
    const portaudio::DirectionSpecificStreamParameters &input_parameters,
    const portaudio::DirectionSpecificStreamParameters &output_parameters)
{
    static double STANDARD_SAMPLE_RATES[] = {
        8000.0, 9600.0, 11025.0, 12000.0, 16000.0, 22050.0, 24000.0, 32000.0,
        44100.0, 48000.0, 88200.0, 96000.0, -1
    }; // negative terminated list

    bool printed_any = false;

    for(int i = 0; STANDARD_SAMPLE_RATES[i] > 0; ++i)
    {
        portaudio::StreamParameters tmp = portaudio::StreamParameters(
            input_parameters, output_parameters, STANDARD_SAMPLE_RATES[i], 0,
            paNoFlag);

        if(tmp.isSupported())
        {
            LOG(info, "    {}", STANDARD_SAMPLE_RATES[i]);
            printed_any = true;
        }
    }
    if(!printed_any)
        LOG(info, "    None.");
}

const std::vector<AudioManager::AcquisitionAgent> &
PortAudioAudioManager::enumerateDevices()
{
    // https://app.assembla.com/spaces/portaudio/git/source/master/bindings/cpp/example/devs.cxx

    LOG(info, "PortAudio version number = {}", mPaSystem.version());
    LOG(info, "PortAudio version text = '{}'", mPaSystem.versionText());

    int numDevices = mPaSystem.deviceCount();
    LOG(info, "Number of devices = {}", numDevices);

    for(portaudio::System::DeviceIterator i = mPaSystem.devicesBegin(); i !=
        mPaSystem.devicesEnd(); ++i)
    {
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
            printSupportedStandardSampleRates(inputParameters,
                portaudio::DirectionSpecificStreamParameters::null());
        }

        if(outputParameters.numChannels() > 0)
        {
            LOG(info, "Supported standard sample rates");
            LOG(info, " for half-duplex 16 bit {} channel output = ",
                outputParameters.numChannels());
            printSupportedStandardSampleRates(
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
            printSupportedStandardSampleRates(inputParameters,
                outputParameters);
        }
    }

    LOG(info, "----------------------------------------------");

    return mDevices;
}
}
