// pages/controls/controls.js
var sendData = require('../../utils/send_prctData.js');
//页面
Page({
  //页面的初始数据
  data: {
      led_status : false,           //LED灯控制
  },
  //LED灯控制开关
ledCtrollor : function (){
     //对this指针进行保存
     var that = this;
     //改变open属性的值
     that.setData({
        led_status: !that.data.led_status
      })
      //============================================
    //定义发送的数据(申请8个字节的内存空间，即64个二进制位，此时buffer是空的)
    var buffer = new ArrayBuffer(8);
    //消息队列(32位无符号整数，长度4个字节，可以存两个数，共64个bit)
    //将buffer存成Uint32Array形式是为了给后4个字节赋值
    var socketMsgQueue = new Uint32Array(buffer);
    //第0个数和第1个数(消息队列里面是两个整数)
    //socketMsgQueue[1] = 0x1;// value占后32个字节
    socketMsgQueue[1] = that.data.led_status ? 0x1 : 0x0;// value占后32个字节
      
      //将buffer转化成Uint8Array类型的视图
      //(Uint8Array：每个元素都是8位无符号整数类型，每个数占长度1个字节)，可以存8个数字
      //给buffer存成Uint8Array形式是为了给前4个字节赋值
      var socketMsg_u8 =  new Uint8Array(buffer); 
      socketMsg_u8[0] = 0x0; //保留位(未使用)
      socketMsg_u8[1] = 0x0; //UI界面
      socketMsg_u8[2] = 0x0; //功能位CMD(可能是LED、窗、界面控制)
      socketMsg_u8[3] = 0x1; //发给谁Dst(发送给单板 0x01)
      //后4个字节是value的值(已经被赋值)
      //发送数据到Linux
      sendData.sendDataToLinux(buffer);
      //发送完毕清空消息队列
      console.log('发送的LED控制命令:');
      console.log('开关灯命令为: ' + socketMsgQueue[1]);
      console.log('目的数据0为: ' + socketMsg_u8[0]);
      console.log('目的数据1为: ' + socketMsg_u8[1]);
      console.log('目的数据2为: ' + socketMsg_u8[2]);
      console.log('目的数据3为: ' + socketMsg_u8[3]);

      socketMsgQueue = []
},

  /**
   * 生命周期函数--监听页面加载
   */
 // 页面加载的时候触发
  // 页面初始化 options为页面跳转所带来的参数
onLoad: function (options) {
},

  /**
   * 生命周期函数--监听页面初次渲染完成
   */
  onReady: function () {
  
  },

  /**
   * 生命周期函数--监听页面显示
   */
  onShow: function () {
  
  },

  /**
   * 生命周期函数--监听页面隐藏
   */
  onHide: function () {
  
  },

  /**
   * 生命周期函数--监听页面卸载
   */
  onUnload: function () {
  
  },

  /**
   * 页面相关事件处理函数--监听用户下拉动作
   */
  onPullDownRefresh: function () {
  
  },

  /**
   * 页面上拉触底事件的处理函数
   */
  onReachBottom: function () {
  
  },

  /**
   * 用户点击右上角分享
   */
  onShareAppMessage: function () {
  
  }  
})