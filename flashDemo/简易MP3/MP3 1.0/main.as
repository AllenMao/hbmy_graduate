package
{
	import flash.display.Sprite;
	import flash.display.MovieClip;
	import flash.display.SimpleButton;
	
	import flash.text.TextField;
	
	import flash.events.*;
	
	import flash.media.*;
	import flash.net.URLRequest;
	
	import fl.events.SliderEvent;//CHANGE事件,声道音量控制
	
	public class main extends MovieClip
	{
		var bgLayer:MovieClip = new MovieClip();
		var lrcLayer:MovieClip = new MovieClip();
		
		private var musicList:Array;//播放列表
		private var snd:Sound;//声音
		private var channel:SoundChannel;//声音通道
		private var curPosition:Number=0;//播放当前位置
		private var index:int=0;//当前歌表索引
		
		
		private var trans:SoundTransform;//改变声道音量控制
		//private var CurrentTime:TextField;
		//private var TotalsTime:TextField;
		
		//专辑图
		private var bgImage:Image;
		private var imageList:Array;
		
		//3.22歌词 3.24
		private var lrc:songLrc;
		private var lrcList:Array;
		//private var flagStop:Boolean;
		//private var flagPause:Boolean;
		
		public function main()
		{
			addChildAt(bgLayer,1);
			addChildAt(lrcLayer,2);
			
			
			bgImage=new Image();
			lrc=new songLrc();
			lrcTxt.wordWrap=true;
			//lrcTxt.autoSize=true;
			
			musicList=["music/m1.mp3","music/m2.mp3","music/m3.mp3"];
			imageList=["pic/pic1.jpg","pic/pic2.jpg","pic/pic3.jpg"];
			lrcList=["lrc/lrc1.xml","lrc/lrc2.xml","lrc/lrc3.xml"];
			//CurrentTime=this.getChildByName("currentTime") as TextField;
			//TotalsTime=this.getChildByName("totalsTime") as TextField;
			
			

			Play();
			addEventListener(Event.ENTER_FRAME,playIng);
			playbt.addEventListener(MouseEvent.CLICK,playSnd);
			pausebt.addEventListener(MouseEvent.CLICK,pauseSnd)
			stopbt.addEventListener(MouseEvent.CLICK,stopSnd);
			prebt.addEventListener(MouseEvent.CLICK,preSnd);
			nextbt.addEventListener(MouseEvent.CLICK,nextSnd);
			
			//3.21 音量控制
			volume.addEventListener(SliderEvent.CHANGE,volumeChang);
			trans=new SoundTransform(1,0);//音量最大左右平衡
			volume.value=10;//音量最大
			
			
		}
		/*3.20 基本实现*/
		private function Play()
		{
			lrcTxt.text="";
			
			if(index>=musicList.length) index=musicList.length-1;
			else if(index<0) index=0;
			
			songName.text="当前曲目:"+String(index+1)+"-";
			
			snd=new Sound();
			snd.addEventListener(Event.ID3,getID3);//注册ID3事件，接收ID3信息
			
			var  req:URLRequest=new URLRequest(musicList[index]);//确定歌曲位置信息
			
			var context:SoundLoaderContext=new SoundLoaderContext(8000,true);//设置预加载字节数
			
			snd.load(req,context);
			
			pausebt.visible=true;
			playbt.visible=false;
			
			channel=snd.play(0,0,trans);//在channel中播放当前歌曲,将trans对象传入play（）方法
			//channel.addEventListener(Event.SOUND_COMPLETE，playComplete);
			
			
			//专辑图
			bgImage.Display(imageList[index]);
			bgLayer.addChild(bgImage);
			
			// 歌词
			lrc.loadLrc(lrcList[index],snd.length,this);
			//lrcTxt.text=(lrc.lrctxt.text)as String;
			//trace(lrc.lrctxt.text);//为空？？？？？
			//lrcTxt.text="123456";
			//lrcLayer.addChild(lrc);
		}
		
		private function getID3(e:Event)
		{
			songName.text="当前曲目:"+String(index+1)+"-"+(snd.id3.songName);
		}
		
		private function playSnd(e:MouseEvent)
		{
			pausebt.visible=true;
			playbt.visible=false;
			//if(snd!=null)
			channel=snd.play(curPosition);
			lrc.timer.start();
			
		}
		
		private function pauseSnd(e:MouseEvent)
		{
			curPosition=channel.position;
			pausebt.visible=false;
			playbt.visible=true;
			channel.stop();
			
			lrc.timer.stop();
		}
		
		private function stopSnd(e:MouseEvent)
		{
			pausebt.visible=false;
			playbt.visible=true;
			curPosition=0;
			channel.stop();
			channel=snd.play(0);
			channel.stop();
			
			lrcTxt.text="";
			lrc.i=0;
			lrc.timer.stop();
		}
		
		private function preSnd(e:MouseEvent)
		{
			stopSnd(new MouseEvent(""));
			--index;
			Play();
		}
		
		private function nextSnd(e:MouseEvent)
		{
			stopSnd(new MouseEvent(""));
			++index;
			Play();
		}
		
		private function playIng(e:Event):void
		{
			//加载进度
			var laodedPercent:Number=snd.bytesLoaded/snd.bytesTotal;
			//trace(laodedPercent);//一直为1
			//计算播放进度
			var estimatedLength:uint=snd.length/laodedPercent;
			var playbackPercent:uint=100*(channel.position/estimatedLength);
			
			head.x=playLine.x+playLine.width*playbackPercent/100-5;
			per.x=head.x;
			
			per.text=String(playbackPercent)+"%";
			currentTime.text=toTime(channel.position);
			totalsTime.text=toTime(snd.length);
			//trace(channel.position);
			//trace(snd.length);
			if(playbackPercent==99) playComplete();
			
		}
		private function toTime(n:uint):String
		{
			var ms:uint=n%1000;
			var s:uint=n/1000;
			var m:uint=s/60;
			s%=60;
			return String(100+m).substr(1,2)+":"+String(100+s).substr(1,2)+"."+String(ms);
		}
		
		private function playComplete()
		{
			++index;
			Play();
		}
		
		/*3.21  添加音量控制*/
		private function volumeChang(e:SliderEvent)
		{
			trans.volume=e.value/10;
			channel.soundTransform=trans;
		}
		
	}
}