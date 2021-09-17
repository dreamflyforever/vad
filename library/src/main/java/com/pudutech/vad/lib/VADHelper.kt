package com.pudutech.vad.lib

import android.util.Log

object VADHelper {
    init {
        System.loadLibrary("vad")
    }
    private val listeners = HashMap<String,OnFeedListener>()

    fun initVAD(params:String) = init(params)

    fun setVADTime(time:Long) = setTime(time)

    fun avdFeed(voiceData:ByteArray) = feed(voiceData)

    fun unInit() =  release()

    fun addOnFeedListener(name:String,l:OnFeedListener){
        listeners[name] = l
    }
    fun removeOnFeedListener(name: String){
        listeners.remove(name)
    }

    private fun feedCallback(flag:Int){
        Log.e("VADLib","feedCallback flag=$flag")
        for (l in listeners){
            l.value.onFeed(flag)
        }
    }

    private external fun init(params:String):Int

    private external fun setTime(time:Long):Int

    private external fun feed(voiceData:ByteArray):Int

    private external fun release():Int
}