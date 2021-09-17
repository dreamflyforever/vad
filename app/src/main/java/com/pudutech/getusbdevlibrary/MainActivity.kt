package com.pudutech.getusbdevlibrary

import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.util.Log
import android.view.View
import android.widget.Toast
import com.pudutech.vad.lib.OnFeedListener

import com.pudutech.vad.lib.VADHelper
import java.io.*

class MainActivity : AppCompatActivity() {
    private var hasInit = false
    private var shouldClose = false
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

    }

    fun init(view: View) {
        val ret = VADHelper.initVAD("")
        Log.e("VADLib","ret=$ret")
        hasInit = ret == 0
        shouldClose = false
        VADHelper.addOnFeedListener("test",object :OnFeedListener{
            override fun onFeed(vadFlag: Int) {
                shouldClose = vadFlag ==0
            }

        })
    }

    fun unint(view: View) {

        VADHelper.unInit()
        hasInit = false
        shouldClose = false
        VADHelper.removeOnFeedListener("test")
    }

    fun feed(view: View) {
        if (!hasInit){
            Toast.makeText(this@MainActivity,"please init first",Toast.LENGTH_SHORT).show()
            return
        }


        VADHelper.setVADTime(80L)

       val ins = resources.assets.open("test_vad.wav")

        val fis = File("/sdcard/pudu/feed.pcm")

        if (!fis.parentFile.exists()){
            fis.mkdirs()
        }
        val bos = ByteArrayOutputStream()

        var len = -1

        val feedIns = FileOutputStream(fis)

        val buffer = ByteArray(1024)

        while (((ins.read(buffer)).also { len = it } )!=-1){
            bos.write(buffer,0,len)
        }

        ins.close()
        val result = bos.toByteArray()

        bos.close()

        var feedLen = 0

        val feedBuf = ByteArray(256)
        while (feedLen<result.size){
            if (shouldClose){
                shouldClose =false
                feedIns.flush()
                feedIns.close()
                break
            }
            if (feedLen+feedBuf.size>result.size){
                val cpLen = result.size - feedLen
                System.arraycopy(result,feedLen,feedBuf,0,cpLen)
                feedLen+=cpLen
                val ret = VADHelper.avdFeed(feedBuf)

                Log.e("VADLib","feed ret=$ret,feedLen=$feedLen")
                break
            }
            System.arraycopy(result,feedLen,feedBuf,0,feedBuf.size)
            feedLen+=feedBuf.size
            val ret = VADHelper.avdFeed(feedBuf)
            if (ret == 0){
                feedIns.write(feedBuf,0,feedBuf.size)
            }
            Log.e("VADLib","feed ret=$ret,feedLen=$feedLen")
        }




    }
}