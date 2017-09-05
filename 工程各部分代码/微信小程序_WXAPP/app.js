// var send_recvData = require('../../utils/send_prctData.js');
//app.js
App({
  onLaunch: function (options) {
    // Do something initial when launch.
    
    //创建一个WebSocket连接(微信小程序同时只能有一个WebSocket连接)
    //如果开启第2个，那么第1个将被关闭
    wx.connectSocket({
      // 连接的网络域名
      url: 'wss://gec10.rfish.xyz:12345'
    })

    var that = this;
    //监听打开WebSocket连接打开事件
    wx.onSocketOpen(function (res) {
      that.globalData.socketOpen = true;
      console.log('WebSocket连接已打开！');
    })
    // send_recvData.recieveDataFromLinux();
  },
  onShow: function (options) {
    // Do something when show.
  },
  onHide: function () {
    // Do something when hide.
  },
  onError: function (msg) {
  },
  // data: {
  //   temperature_value: 0,
  //   humidity_value: 0,
  //   temperature_warn_high_value: 100,
  //   temperature_warn_low_value: 0,
  //   led_status: false,           //LED灯控制
  // }
  globalData: {
       socketOpen : false,
        // temperature_value: 0,
        // humidity_value: 0,
        // temperature_warn_high_value: 100,
        // temperature_warn_low_value: 0,
        // led_status : false,           //LED灯控制
        // ARM_board_UI : 0,
  }
})
