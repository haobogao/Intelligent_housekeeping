//获取到App应用对象
var globalApp = getApp()
//发送数据到Linux
function sendDataToLinux(buffer){
     //向socket发送buffer数据(一次性8个字节)
     sendSocketMessage(buffer);
   
  //自定义的sendSocketMessage函数
    function sendSocketMessage(msg) {
      //如果socket已经打开(可以发送数据，同时也可以接收数据-全双工模式)
      if (globalApp.globalData.socketOpen) {
        console.log('WebSocket打开成功！');
        //调用wx的sendSocketMessage函数向服务器端发送数据
        //发送的数据是以对象的形式发送的，接收方以res参数体现出来
        wx.sendSocketMessage({
          data: msg
        })
      } else {
        //如果socket没打开，则将msg对象压栈到消息队列中继续等待
        socketMsgQueue.push(msg);
        //并打印一个提示
        console.log('WebSocket打开失败！');
      }
    }
}

//导出sendDataToLinux
module.exports = {
  sendDataToLinux : sendDataToLinux
}