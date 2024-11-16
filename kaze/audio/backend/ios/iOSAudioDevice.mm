#import "iOSAudioDevice.h"
#import <kaze/core/platform/defines.h>

#if !KAZE_PLATFORM_IOS
#error kaze::audio::iOSAudioDevice can only be used on the Apple iOS platform
#endif

#import <kaze/core/debug.h>
#import <kaze/core/math/mathf.h>
#import <kaze/core/memory.h>

#import <AudioToolbox/AudioToolbox.h>
#import <AVFoundation/AVFoundation.h>

#import <atomic>
#import <condition_variable>
#import <mutex>
#import <thread>

KAUDIO_NS_BEGIN

constexpr int NumAudioBuffers = 2;

struct iOSAudioDevice::Impl {
    Impl() { }

    std::atomic<AudioUnit> audioUnit{};

//        std::atomic<AudioQueueRef> audioQueue{};
//        AudioQueueBufferRef audioBuffers[NumAudioBuffers]{};

    std::recursive_mutex mutex{};
    std::mutex bufferMutex{};
    AudioCallback callback{};
    AlignedList<Ubyte, 16> buffer{}, nextBuffer{};
    void *userdata{};
    AudioSpec spec{};
    std::atomic<int> bytesReady{};
    std::thread thread{};
    std::condition_variable_any nextBufferReady{};

    /// Audio unit callback
    static auto audioCallback(void *inRefCon,
                           AudioUnitRenderActionFlags *ioActionFlags,
                           const AudioTimeStamp *inTimeStamp,
                           UInt32 inBusNumber,
                           UInt32 inNumberFrames,
                           AudioBufferList *ioData) -> OSStatus
    {
        if (inNumberFrames == 0)
            return;

        auto dev = static_cast<Impl *>(inRefCon);

        const auto byteSize = inNumberFrames * 2 * sizeof(Float);
        if (dev->buffer.size() != byteSize)
            dev->buffer.resize(byteSize, 0);

        dev->callback(dev->userdata, &dev->buffer);
        for (Int i = 0; i < ioData->mNumberBuffers; ++i)
        {
            // Copy retrieved data to the output buffer
            auto &buffer = ioData->mBuffers[i];
            memory::copy(buffer.mData, dev->buffer.data(),
                        mathf::min<Int>((Int)byteSize, buffer.mDataByteSize));
        }



//            if (dev->bytesReady.load(std::memory_order_acquire) >= byteSize)
//            {
//                for (int i = 0; i < ioData->mNumberBuffers; ++i)
//                {
//                    // Copy retrieved data to the output buffer
//                    auto &buffer = ioData->mBuffers[i];
//                    std::memcpy(buffer.mData, dev->buffer.data(),
//                                std::min<int>((int)byteSize, buffer.mDataByteSize));
//                }
//
//                dev->bytesReady.store(0, std::memory_order_release);
//                dev->nextBufferReady.notify_all();
//            }
//            else
//            {
//                INSOUND_LOG("Buffer underrun, silence returned\n");
//                for (int i = 0; i < ioData->mNumberBuffers; ++i)
//                {
//                    // Write silence to the output buffer
//                    auto &buffer = ioData->mBuffers[i];
//                    std::memset(buffer.mData, 0,
//                                std::min<int>((int)byteSize, buffer.mDataByteSize));
//                }
//            }

        return noErr;
    }

//        static void audioQueueCallback(void *userdata, AudioQueueRef aq, AudioQueueBufferRef buffer)
//        {
//            // This function should fill the provided buffer (inBuffer) with audio data
//            // Example: Copy audio data into inBuffer->mAudioData and set inBuffer->mAudioDataByteSize
//            // ...
//            auto dev = static_cast<Impl *>(userdata);
//
//            auto audioQueue = dev->audioQueue.load(std::memory_order_acquire);
//            if (audioQueue)
//            {
//                if (dev->buffer.size() != buffer->mAudioDataBytesCapacity)
//                {
//                    dev->buffer.resize(buffer->mAudioDataBytesCapacity);
//                }
//
//                dev->callback(dev->userdata, &dev->buffer);
//
//                std::memcpy(buffer->mAudioData, dev->buffer.data(), buffer->mAudioDataBytesCapacity);
//                buffer->mAudioDataByteSize = buffer->mAudioDataBytesCapacity;
//                auto status = AudioQueueEnqueueBuffer(audioQueue, buffer, 0, NULL);
//                if (status != noErr)
//                {
//                    KAZE_PUSH_ERR(Error::RuntimeErr, "failed to enqueue audio buffer");
//                }
//            }
//        }
};

iOSAudioDevice::iOSAudioDevice() : m(new Impl())
{ }

iOSAudioDevice::~iOSAudioDevice()
{
    delete m;
}

auto iOSAudioDevice::open(const AudioDeviceOpen &config) -> Bool
{
//        m->thread = std::thread([this, audioCallback, userdata, frequency, sampleFrameBufferSize]() {
//            const auto samplerate = frequency ? frequency : getDefaultSampleRate();
//            const int bufferBytes = sampleFrameBufferSize * 2 * sizeof(Float);
//            AudioQueueRef queue;
//            AudioQueueBufferRef buffers[NumAudioBuffers];
//
//            // Initialize the audio queue
//            auto streamFormat = AudioStreamBasicDescription{0};
//            streamFormat.mSampleRate = samplerate;
//            streamFormat.mFormatID = kAudioFormatLinearPCM;
//            streamFormat.mFormatFlags = kAudioFormatFlagIsFloat | kAudioFormatFlagIsPacked;
//            streamFormat.mBytesPerPacket = sizeof(Float) * 2;
//            streamFormat.mFramesPerPacket = 1;
//            streamFormat.mBytesPerFrame = sizeof(Float) * 2;
//            streamFormat.mChannelsPerFrame = 2;
//            streamFormat.mBitsPerChannel = 32;
//            streamFormat.mReserved = 0;
//
//            auto status = AudioQueueNewOutput(&streamFormat, &Impl::audioQueueCallback, m, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode, 0, &queue);
//            if (status != noErr)
//            {
//                KAZE_PUSH_ERR(Error::RuntimeErr, "audio queue failed to create");
//                return False;
//            }
//
//            // Initialize the queue's buffers
//            for (int i = 0; i < NumAudioBuffers; ++i)
//            {
//                status = AudioQueueAllocateBuffer(queue, bufferBytes, &buffers[i]);
//                if (status != noErr)
//                {
//                    // free any allocated buffer from earlier iterations
//                    for (int j = 0; j < i; ++j)
//                    {
//                        AudioQueueFreeBuffer(queue, buffers[j]);
//                    }
//
//                    AudioQueueDispose(queue, True);
//
//                    KAZE_PUSH_ERR(Error::RuntimeErr, "failed to allocate output queue buffers");
//                    return False;
//                }
//
//                // Fill buffers with silence. No need to prime otherwise,
//                // since the engine isn't fully initialized yet.
//                std::memset(buffers[i]->mAudioData, 0, buffers[i]->mAudioDataBytesCapacity);
//                buffers[i]->mAudioDataByteSize = buffers[i]->mAudioDataBytesCapacity;
//                status = AudioQueueEnqueueBuffer(queue, buffers[i], 0, Null);
//                if (status != noErr)
//                {
//                    // free any allocated buffer from earlier iterations
//                    for (int j = 0; j < i; ++j)
//                    {
//                        AudioQueueFreeBuffer(queue, buffers[j]);
//                    }
//
//                    AudioQueueDispose(queue, True);
//
//                    KAZE_PUSH_ERR(Error::RuntimeErr, "failed to allocate output queue buffers");
//                    return False;
//
//                }
//            }
//
//            status = AudioQueueStart(queue, Null);
//            if (status != noErr)
//            {
//                for (int i = 0; i < NumAudioBuffers; ++i)
//                {
//                    AudioQueueFreeBuffer(queue, buffers[i]);
//                }
//
//                AudioQueueDispose(queue, True);
//            }
//
//            // Success! Assign values.
//            for (int i = 0; i < NumAudioBuffers; ++i)
//                m->audioBuffers[i] = buffers[i];
//            m->callback = audioCallback;
//            m->userdata = userdata;
//            m->spec.freq = samplerate;
//            m->spec.channels = 2;
//            m->spec.format = insound::SampleFormat(sizeof(Float) * CHAR_BIT, True,
//                                                   False, True);
//            m->audioQueue.store(queue, std::memory_order_release);
//
//            while(m->audioQueue.load())
//                CFRunLoopRunInMode(kCFRunLoopDefaultMode, 0.10, 1);
//        });
//
//        while(!m->audioQueue.load(std::memory_order_acquire));
//        // todo: need state to detect load failure
//
//
//        return True;
    auto desc = AudioComponentDescription{0};
    desc.componentType = kAudioUnitType_Output;
    desc.componentSubType = kAudioUnitSubType_RemoteIO;
    desc.componentManufacturer = kAudioUnitManufacturer_Apple;
    desc.componentFlags = 0;
    desc.componentFlagsMask = 0;

    const auto audioComponent = AudioComponentFindNext(Null, &desc);
    if (audioComponent == Null)
    {
        KAZE_PUSH_ERR(Error::RuntimeErr, "failed to find audio component");
        return False;
    }

    // Create and set up audio unit
    AudioUnit audioUnit;
    AudioComponentInstanceNew(audioComponent, (AudioComponentInstance *)&audioUnit);

    if (!audioUnit)
    {
        KAZE_PUSH_ERR(Error::RuntimeErr,
            "failed to create audio component instance");
        return False;
    }

    // Enable IO for recording
    Uint enableIO = 1;
    auto result = AudioUnitSetProperty(audioUnit,
                         kAudioOutputUnitProperty_EnableIO,
                         kAudioUnitScope_Input,
                         1, // input bus
                         &enableIO,
                         sizeof(enableIO));
    if (result != noErr)
    {
        KAZE_PUSH_ERR(Error::RuntimeErr,
                           "failed to enable audio unit input");
        return False;
    }

    // Enable IO for playback
    result = AudioUnitSetProperty(audioUnit,
                         kAudioOutputUnitProperty_EnableIO,
                         kAudioUnitScope_Output,
                         0, // output bus
                         &enableIO,
                         sizeof(enableIO));
    if (result != noErr)
    {
        KAZE_PUSH_ERR(Error::RuntimeErr,
                           "failed to enable audio unit output");
        return False;
    }

    // Setup stream format
    const auto frequency = (config.frequency > 0) ? config.frequency : getDefaultSampleRate();

    auto streamFormat = AudioStreamBasicDescription{0};
    streamFormat.mSampleRate = frequency;
    streamFormat.mFormatID = kAudioFormatLinearPCM;
    const auto formatFlags = kAudioFormatFlagIsFloat | kAudioFormatFlagIsPacked;
    streamFormat.mFormatFlags = formatFlags;
    streamFormat.mBytesPerPacket = sizeof(Float) * 2;
    streamFormat.mFramesPerPacket = 1;
    streamFormat.mBytesPerFrame = sizeof(Float) * 2;
    streamFormat.mChannelsPerFrame = 2;
    streamFormat.mBitsPerChannel = 32;
    streamFormat.mReserved = 0;

    result = AudioUnitSetProperty(audioUnit,
                                  kAudioUnitProperty_StreamFormat,
                                  kAudioUnitScope_Input,
                                  0,
                                  &streamFormat,
                                  sizeof(streamFormat));

    if (result != noErr)
    {
        KAZE_PUSH_ERR(Error::RuntimeErr,
                           "failed to set audio unit stream format");
        return False;
    }

    result = AudioUnitSetProperty(audioUnit,
                                  kAudioUnitProperty_StreamFormat,
                                  kAudioUnitScope_Output,
                                  1,
                                  &streamFormat,
                                  sizeof(streamFormat));
    if (result != noErr)
    {
        KAZE_PUSH_ERR(Error::RuntimeErr,
                           "failed to set audio unit stream format");
        return False;
    }

    // Set up callback
    auto callbackStruct = AURenderCallbackStruct();
    callbackStruct.inputProc = iOSAudioDevice::Impl::audioCallback;
    callbackStruct.inputProcRefCon = m;

    result = AudioUnitSetProperty(audioUnit,
                                  kAudioUnitProperty_SetRenderCallback,
                                  kAudioUnitScope_Input,
                                  0,
                                  &callbackStruct,
                                  sizeof(callbackStruct));
    if (result != noErr)
    {
        KAZE_PUSH_ERR(Error::RuntimeErr,
                           "failed to set audio unit callaback");
        return False;
    }

    // Set max frames per slice
    Uint maxFramesPerSlice = config.frameBufferSize;
    result = AudioUnitSetProperty(audioUnit,
                                  kAudioUnitProperty_MaximumFramesPerSlice,
                                  kAudioUnitScope_Global,
                                  0,
                                  &maxFramesPerSlice,
                                  sizeof(maxFramesPerSlice));
    if (result != noErr)
    {
        KAZE_PUSH_ERR(Error::RuntimeErr,
                           "failed to set audio unit max frames per slice");
        return False;
    }

    // Turn off allocation of buffer
    Uint flag = 0;
    result = AudioUnitSetProperty(audioUnit,
                                  kAudioUnitProperty_ShouldAllocateBuffer,
                                  kAudioUnitScope_Output,
                                  1,
                                  &flag,
                                  sizeof(flag));
    if (result != noErr)
    {
        KAZE_PUSH_ERR(Error::RuntimeErr,
                           "failed to set should allocate buffer flag off");
        return False;
    }

    result = AudioUnitInitialize(audioUnit);
    if (result != noErr)
    {
        KAZE_PUSH_ERR(Error::RuntimeErr,
                           "failed to initialize audio unit");
        return False;
    }

    // Retrieve the actual values back from the audio unit
    Double retrievedSampleRate;
    Uint size = sizeof(retrievedSampleRate);
    AudioUnitGetProperty(audioUnit,
                         kAudioUnitProperty_SampleRate,
                         kAudioUnitScope_Input,
                         0,
                         &retrievedSampleRate,
                         &size);
    assert(sizeof(retrievedSampleRate) == size);
    assert(retrievedSampleRate == frequency);

    AudioStreamBasicDescription retrievedStreamDesc;
    size = sizeof(retrievedStreamDesc);
    AudioUnitGetProperty(audioUnit,
                         kAudioUnitProperty_StreamFormat,
                         kAudioUnitScope_Input,
                         0,
                         &retrievedStreamDesc,
                         &size);
    assert(sizeof(retrievedStreamDesc) == size);
    assert(retrievedStreamDesc.mBitsPerChannel == sizeof(Float) * CHAR_BIT);
    assert(retrievedStreamDesc.mFormatID == kAudioFormatLinearPCM);
    assert(retrievedStreamDesc.mChannelsPerFrame == 2);
    assert((retrievedStreamDesc.mFormatFlags & formatFlags) == formatFlags);

    auto curAudioUnit = m->audioUnit.load(std::memory_order_acquire);
    if (curAudioUnit)
    {
        AudioUnitUninitialize(curAudioUnit);
        AudioComponentInstanceDispose(curAudioUnit);
    }

    // TODO: use converter for output instead of assert / fail?

    m->callback = config.audioCallback;
    m->userdata = config.userdata;
    m->spec.freq = retrievedSampleRate;
    m->spec.channels = 2;
    m->spec.format = SampleFormat(sizeof(Float) * CHAR_BIT, True,
                                           False, True);
    m->audioUnit.store(audioUnit, std::memory_order_release);
    m->buffer.resize(config.frameBufferSize * sizeof(Float) * 2, 0);
    //m->bytesReady.store(m->buffer.size());

//        m->thread = std::thread([this]() {
//            while (m->audioUnit.load(std::memory_order_acquire))
//            {
//                std::unique_lock lock(m->mutex);
//                m->nextBufferReady.wait(lock, [this]() {
//                    return m->bytesReady == 0;
//                });
//
//                if (m->nextBuffer.size() != m->buffer.size())
//                    m->nextBuffer.resize(m->buffer.size(), 0);
//
//                m->callback(m->userdata, &m->nextBuffer);
//                m->buffer.swap(m->nextBuffer);
//                m->bytesReady.store((int)m->buffer.size(), std::memory_order_release);
//            }
//        });

    return True;
}

auto iOSAudioDevice::suspend() -> void
{
//        auto status = AudioQueuePause(m->audioQueue);
//        if (status != noErr)
//        {
//            KAZE_PUSH_ERR(Error::RuntimeErr, "failed to stop audio queue");
//        }
    auto audioUnit = m->audioUnit.load(std::memory_order_acquire);
    if (audioUnit)
    {
        if (AudioOutputUnitStop(audioUnit) != noErr)
            KAZE_PUSH_ERR(Error::RuntimeErr, "failed to pause audio unit");
    }

}

auto iOSAudioDevice::resume() -> void
{
//        auto status = AudioQueueStart(m->audioQueue.load(std::memory_order_acquire), Null);
//        if (status != noErr)
//        {
//            KAZE_PUSH_ERR(Error::RuntimeErr, "failed to start audio queue");
//        }
    auto audioUnit = m->audioUnit.load(std::memory_order_acquire);
    if (audioUnit)
    {
        if (AudioOutputUnitStart(audioUnit) != noErr)
            KAZE_PUSH_ERR(Error::RuntimeErr, "failed to resume audio unit");
    }
}

auto iOSAudioDevice::close() -> void
{
    auto audioUnit = m->audioUnit.load(std::memory_order_acquire);
    if (audioUnit)
    {
        AudioUnitUninitialize(audioUnit);
        AudioComponentInstanceDispose(audioUnit);
        m->audioUnit.store(Null, std::memory_order_release);
        m->thread.join();
    }

//        const auto queue = m->audioQueue.load(std::memory_order_acquire);
//        if (queue)
//        {
//            OSStatus result;
//            for (int i = 0; i < NumAudioBuffers; ++i)
//            {
//                result = AudioQueueFreeBuffer(queue, m->audioBuffers[i]);
//                if (result != noErr)
//                    KAZE_PUSH_ERR(Error::RuntimeErr,
//                                       "failure during AudioQueueFreeBuffer, potential memory leak");
//                m->audioBuffers[i] = Null;
//            }
//
//            result = AudioQueueDispose(queue, True);
//            if (result != noErr)
//            {
//                KAZE_PUSH_ERR(Error::RuntimeErr,
//                                   "failure during AudioQueueDispose, potential memory leak");
//            }
//
//            m->audioQueue.store(Null, std::memory_order_release);
//            m->thread.join();
//        }
}

auto iOSAudioDevice::isRunning() const -> Bool
{
    Uint running = 0;
    Uint size = 0;
    const auto result = AudioUnitGetProperty(m->audioUnit.load(std::memory_order_acquire),
                         kAudioOutputUnitProperty_IsRunning,
                         kAudioUnitScope_Global,
                         0,
                         &running,
                         &size);
    if (result != noErr)
    {
        KAZE_PUSH_ERR(Error::RuntimeErr,
               "failed to query audio unit running status");
        return False;
    }
    return running;

//        Uint running = 0;
//        Uint size = 0;
//        auto result = AudioQueueGetProperty(m->audioQueue.load(std::memory_order_acquire),
//                              kAudioQueueProperty_IsRunning,
//                              &running,
//                              &size);
//        if (result != noErr)
//        {
//            KAZE_PUSH_ERR(Error::RuntimeErr,
//                               "failed to get `IsRunning` property from AudioQueue");
//            return False;
//        }
//
//        return static_cast<bool>(running);
}

auto iOSAudioDevice::isOpen() const -> Bool
{
    return static_cast<bool>(m->audioUnit.load(std::memory_order_acquire));
//        return static_cast<bool>(m->audioQueue.load(std::memory_order_acquire));
}


auto iOSAudioDevice::getDefaultSampleRate() const -> Int
{
    @autoreleasepool {
        AVAudioSession *session = [AVAudioSession sharedInstance];
        NSError *error = nil;
        [session setActive:YES error:&error];
        if (error)
        {
            KAZE_PUSH_ERR(Error::RuntimeErr, "failed to set AVAudioSession active");
            return -1;
        }

        return static_cast<int>(session.sampleRate);
    }
}

auto iOSAudioDevice::getBufferSize() const -> Int
{
    return static_cast<int>(m->buffer.size());
}


auto iOSAudioDevice::getSpec() const -> const AudioSpec &
{
    return m->spec;
}

auto iOSAudioDevice::getId() const -> Uint
{
    return m && m->audioUnit.load(std::memory_order_acquire) ? 1 : 0;
//        return m && m->audioQueue.load(std::memory_order_acquire) ? 1 : 0;
    // outputs to the current default output device, so this info isn't really important.
    // Apple devices have names not numbered ids.
}


KAUDIO_NS_END
