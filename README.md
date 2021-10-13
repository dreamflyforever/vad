### This is the VAD (Voice Activity Detection) library.

### Compile
`make`

### Usage
`vad in-file out-file`

### unittest
在unittest/smartengine是包含录音，vad截断，送入云端识别，播放结果的一整套sdk。
可以连续测试vad是否有效。直接make生成fos，运行fos，会进入录音状态，如果2秒内
没有声音，就会vad截断，如果有声音就会一直录音，直到30表超时退出录音，进入识别。

### license
MIT
