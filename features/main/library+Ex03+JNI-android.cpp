FEATURE library.cpp/Impl
#define JNI_FUNC(FUNC_NAME) \
    JNIEXPORT void JNICALL Java_org_opengamestudio_omc03_library_ ## FUNC_NAME
#define JNI_ARG JNIEnv *env, jobject /* this */
