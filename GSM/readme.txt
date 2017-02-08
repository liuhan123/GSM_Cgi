GSM_v0.8更新：

1、删掉read_messageCenter()函数。
2、增加了get_messageCenter()函数、write_state()函数。
3、主函数增加循环，判断文件是否读到结尾，来控制循环电话号码发送文本。


能自动获取消息中心号码，并能成功发送消息。
存在的问题：


GSM_v0.92更新：

1、新增循环语音通话功能。
	（要求在10s内接通电话，等待3秒后，播放start.wav，再等3秒，播放message.wav，接着过5秒后自动挂机）
2、新增本地留言播放功能（接通电话后，先会自动播放strat.wav（需改动可在start.txt，再调用econ进行wav文件生成），再播放message内容。）
3、增加了SIM卡的初始化。
4、修复bug。
5、将cpu音频输入接在a6 SIM卡的音频输入。


GSM_v1.01更新：
1、完善代码
2、修复若干bug。



	
