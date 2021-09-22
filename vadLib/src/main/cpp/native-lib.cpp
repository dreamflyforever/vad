#include <jni.h>
#include <string>

#include "android/log.h"

#define ARRAYLEN(a) sizeof(a)/sizeof(a[0])
#define TAG "VADLib"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,TAG,__VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,TAG,__VA_ARGS__)

#include<thread>
extern "C"{
#include "vad_api.h"
}

vad_str *vadStr;
JavaVM* global_vm;
jobject vadHelper;
jmethodID feedCallbackId;



jint init(JNIEnv *env, jobject thiz, jstring params) {
    char *char_params = const_cast<char *>(env->GetStringUTFChars(params, 0));

    int ret = vad_init(&vadStr,[](void *data,int flag)->int {
        JNIEnv *env;
        jint ret = global_vm->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_6);
        bool isAttach = false;
        if(ret== JNI_EDETACHED){
            ret = global_vm->AttachCurrentThread(&env, nullptr);
            LOGE("global_vm attach current thread success");
            isAttach = true;
        }

        if (ret != JNI_OK){
            LOGE("call back fail ");
            return -1;
        }

        env->CallVoidMethod(vadHelper,feedCallbackId,flag);

        if (isAttach){

            ret = global_vm->DetachCurrentThread();
            LOGE("vm detach success-------->");
        }

        pthread_setname_np(pthread_self(),"feed_callback");

        return 0;
        }, nullptr);
    LOGD("vad init ret=%d",ret);
    env->ReleaseStringUTFChars(params,char_params);
    vadHelper = env->NewGlobalRef(thiz);
    return ret;
}

jint settime(JNIEnv *env, jobject thiz, jlong time) {
    int ret = vad_settime(vadStr, time);
    LOGD("vad setiime ret=%d",ret);
    return ret;
}

jint feed(JNIEnv *env, jobject thiz, jbyteArray buf) {
    jint len = env->GetArrayLength(buf);
    LOGD("feed len=%d",len);
    char *voiceData = reinterpret_cast<char *>(env->GetByteArrayElements(buf, 0));
    int ret = vad_feed(vadStr, voiceData, len);
    LOGD("vad feed ret=%d",ret);
    env->ReleaseByteArrayElements(buf, reinterpret_cast<jbyte *>(voiceData), 0);
    return ret;
}

jint uninit(JNIEnv *env, jobject thiz) {
    int ret = vad_delete(&vadStr);
    LOGD("vad delete ret=%d",ret);
    return ret;
}

const JNINativeMethod  methods[] = {
        {
        "init",
        "(Ljava/lang/String;)I",
        reinterpret_cast<void *>( init)
        },
        {
        "setTime",
        "(J)I",
        reinterpret_cast<void *>( settime)
        },
        {
            "feed",
            "([B)I",
                reinterpret_cast<void *>( feed)
        },
        {
                "release",
                "()I",
                reinterpret_cast<void *>( uninit)
        }

};
JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
    JNIEnv *env;
    if(vm->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_6) != JNI_OK){
        return JNI_ERR;
    }
    global_vm = vm;
    LOGE("JNI_OnLoad RegisterNatives");
    jclass j_clz = env->FindClass("com/pudutech/vad/lib/VADHelper");
    env->RegisterNatives(j_clz,methods,ARRAYLEN(methods));

    feedCallbackId = env->GetMethodID(j_clz,"feedCallback","(I)V");
    return JNI_VERSION_1_6;
}


JNIEXPORT void JNI_OnUnload(JavaVM* vm, void* reserved){
    JNIEnv *env;
    if (vm->GetEnv((void **) &env, JNI_VERSION_1_6) != JNI_OK) {
        return; // JNI version not supported.
    }
    jclass clz = env->FindClass("com/pudutech/vad/lib/VADHelper");
    env->UnregisterNatives(clz);
}