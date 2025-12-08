#include <jni.h>
#include <cstdint>
#include <memory>
#include "aec/aec.hpp"
#include "aec/config.hpp"

static std::unique_ptr<aec::AEC> aec_instance;

extern "C" {
JNIEXPORT void JNICALL
Java_com_example_aec_AecWrapper_init(JNIEnv* env, jobject obj, jint filter_length) {
    aec::AECConfig config;
    config.filter_length = static_cast<uint32_t>(filter_length);
    aec_instance = aec::create_aec(config);
}

JNIEXPORT void JNICALL
Java_com_example_aec_AecWrapper_process(JNIEnv* env, jobject obj, jshortArray far_end, jshortArray near_end, jshortArray output) {
    jsize frame_size = env->GetArrayLength(far_end);
    jshort* far_ptr = env->GetShortArrayElements(far_end, nullptr);
    jshort* near_ptr = env->GetShortArrayElements(near_end, nullptr);
    jshort* out_ptr = env->GetShortArrayElements(output, nullptr);
    if (aec_instance) {
        aec_instance->process(reinterpret_cast<int16_t*>(far_ptr), reinterpret_cast<int16_t*>(near_ptr), reinterpret_cast<int16_t*>(out_ptr), static_cast<uint32_t>(frame_size));
    }
    env->ReleaseShortArrayElements(far_end, far_ptr, 0);
    env->ReleaseShortArrayElements(near_end, near_ptr, 0);
    env->ReleaseShortArrayElements(output, out_ptr, 0);
}
}
