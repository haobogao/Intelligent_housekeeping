// humidity_display.js
var sendData = require('../../utils/send_prctData.js');
//页面
Page({
  //页面的初始数据
  data: {
      humidity_value: 0,
  },

  /**
   * 生命周期函数--监听页面加载
   */
 // 页面加载的时候触发
  // 页面初始化 options为页面跳转所带来的参数
onLoad: function (options) {
    var that = this;
    //定义prot_wx_cmd对象
    var proct_wx_cmd = {
      temperature: 0,
      humidity: 1,
      light: 2,
      MQ_2: 3,
      func_status: 4
    }
    // ====================================
    //监听从服务器端发送过来(到小程序)的消息
    wx.onSocketMessage( function (res) {
      //发送的buffer(即res.data)是8个字节共计64个二进制位
      //dataView_u8 是拥有8个数字的数组视图(每个数字是8个bit的无符号数)
      //var dataView_u8 = new Uint8Array(res.data);
      //console.log(dataView_u8);

      //获取到服务器发送过来的8个字节的信息
      var dv = new DataView(res.data);
      //根据约定的协议结构：
      //v0表示保留位(未使用)
      //v1表示UI界面(即温度、湿度、光照、MQ-2、功能状态等信息显示的地方)
      //v2表示功能位(即声明传递过来的信息是哪种信息，可能是温度(0)、湿度(1)、光照(1)、MQ-2(3)或者功能状态信息(4)等5种)
      //v3表示发送给谁(小程序)
      var _dst         =     dv.getUint8(0, false);
      var _cmd       =    dv.getUint8(1, false);
      var _ui           =    dv.getUint8(2, false);
      var _reserve =    dv.getUint8(3, false);
      //v4---v7表示value值：可能是温度、湿度、光照、MQ-2或者状态信息的值
      //对于温度、湿度、光照和MQ-2而言，必须一次性读4个字节，单独读一个字节没有意义
      //对于功能状态而言，数据只储存在第4个字节(可以只读第4个字节或者也可以一次性读4个字节，结果一样)
      var _value = dv.getInt32(4, false);

      // console.log('收到协议数据！');
      // console.log('目标是：' + _dst);
      // console.log('指令是：' + _cmd);
      // console.log('UI界面是：' + _ui);
      // console.log('保留位是：' + _reserve);
      // console.log('信息值是：' + _value);
        console.log('湿度监听函数：');
      //根据约定的协议结构，v1表示UI界面(即明确当前显示的是什么内容)
      //可能是显示温度、湿度、光照、MQ-2、功能状态等
      switch (_cmd) {
            case proct_wx_cmd.temperature:
              console.log('当前温度为' + _value);
              break;
            case proct_wx_cmd.humidity:
              console.log('当前湿度为' + _value);
              that.setData({
                humidity_value: _value
              })
              break;
            case proct_wx_cmd.light:
              console.log('当前光照为' + _value);
              break;
            case proct_wx_cmd.MQ_2:
              console.log('当MQ_2为' + _value);
              break;
            case proct_wx_cmd.func_status:
              console.log('当前功能状态为' + _value);
              break;
      }
    })
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
      //   wx.onSocketClose(function(res) {
      //       console.log('WebSocket 监听已关闭！')
      // })
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