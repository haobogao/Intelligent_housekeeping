Page({
  data: {
    imgUrls: [
      './image/main_UI.jpg',
      './image/musicplayerBGP_pause.jpg',
      './image/voicerecorgnize_BGP.jpg',
      './image/g10.jpg',
      './image/g13.jpg',
      './image/g15.jpg',
      './image/g17.jpg',
      './image/g20.jpg',
      './image/2.jpg',
    ],
    // 是否显示面板指示点
    indicatorDots: true,
    // 是否自动播放
    autoplay: true,
    // 自动切换时长
    interval: 1000,
    // 滑动动画时长
    duration: 1000,
    // 按钮是否被禁用
    disabled: false,
    // 按钮背景色是否透明
    plain: false,
    // 名称前是否带loading图标
    loading: false,
  },
  
  // 页面跳转函数
  jumpTo_LEDcontrol: function(e){
    //保留当前页面，跳转到应用中的另一个页面用navigateTo
    wx.switchTab({
      // 这里必须是相对路径，不能是绝对路径，否则不能跳转成功
      url: '../ARM_controlor/ARM_controlor'
    })
  },
 jumpTo_humidity_display: function(e){
    //保留当前页面，跳转到应用中的另一个页面用navigateTo
    wx.switchTab({
      // 这里必须是相对路径，不能是绝对路径，否则不能跳转成功
      url: '../humidity_display/humidity_display'
    })
  },
 jumpTo_temperature_display: function(e){
    //保留当前页面，跳转到应用中的另一个页面用navigateTo
    wx.switchTab({
      // 这里必须是相对路径，不能是绝对路径，否则不能跳转成功
      url: '../sensorND/sensorND'
    })
  },

  onLoad: function (options) {
      //监听从服务器端发送过来(到小程序)的消息
    // wx.onSocketMessage(  function (res) {} );
  }
})


